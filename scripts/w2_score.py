#!/usr/bin/env python3
"""Region-aware visual comparison for Road Trip oracle/native frames."""
from __future__ import annotations
import argparse, json
from pathlib import Path
import numpy as np
from PIL import Image

CANVAS=(320,180)
SCENES=('title','menu','town','race')
# Named-ROI calibration inventory. ELEMENT is a documented per-element map,
# not a claim that every pixel in the ROI shares a scene-wide transform.
YMAP_REGISTRY={
 'sky': {'status':'GAP','reason':'No ROI-specific native/oracle landmark pairs establish sky y alignment; sky color profiles alone are not native correspondence.'},
 'ground': {'status':'ELEMENT','scale':447.0/448.0,'offset':0.0,'citation':'docs/native-reference/camera-constants.md; .qoder/wave-board.md W2-04r3 (static_px_err_max=0.011824 PASS)','basis':'W2-04r3 screen law verified in static poses: oracle shot row = UI row x 447/448 (447-line presenter over the 448-line drawing space); ground/horizon rows registered to <=0.012 px.'},
 'hud_dial': {'status':'GAP','reason':'Dial needle/dial pixels lack a validated native-to-oracle y transform.'},
 'hud_timer': {'status':'GAP','reason':'Timer oracle readings are documented but no native timer landmarks establish its y transform.'},
 'hud_speed_digits': {'status':'ELEMENT','scale':1.0,'offset':0.0,'citation':'docs/native-reference/gauge-calibration.md:3-8; .qoder/wave2-plan-sonnet.md:23-27','basis':'448-line HUD element; observed shot y approximately equals 448-space y (scale 1, offset 0).'},
 'minimap': {'status':'GAP','reason':'No grounded minimap y landmark correspondences are recorded; a car-shift measurement is not yet a y-map calibration.'},
 'menu_panel': {'status':'ELEMENT','scale':1.0/0.93125,'offset':-16.0,'citation':'docs/native-reference/menu-calibration.md:15-24','basis':'placement_y = scan_y / 0.93125 - 16.'},
 'title_logo': {'status':'GAP','reason':'No title-logo oracle/native landmark correspondences are documented.'},
}
def ymap_status():
    counts={k:sum(entry['status']==k for entry in YMAP_REGISTRY.values()) for k in ('FIT','ELEMENT','GAP')}
    return f"{counts['FIT']}+{counts['ELEMENT']}+{counts['GAP']}/{len(YMAP_REGISTRY)}"
# Named ROIs use source-image coordinates (x,y,w,h); scene-specific omissions
# are explicit. Full-resolution measurements use the oracle's coordinate space.
ROIS={
 'title': {'sky':(0,0,1280,180),'text':(0,180,1280,267)},
 'menu': {'sky':(0,0,1280,100),'menu_panel':(250,60,780,330),'text':(250,60,780,330)},
 'town': {'sky':(0,0,1280,190),'ground':(0,190,1280,257),'minimap':(60,300,180,130),'text':(0,0,1280,447)},
 'race': {'sky':(0,0,1280,190),'ground':(0,190,1280,257),'hud_dial':(1040,350,180,80),'hud_timer':(500,350,280,50),'minimap':(0,300,200,147),'text':(0,0,1280,447)},
}

def load(path):
    im=Image.open(path)
    # Composite alpha over black; PNG RGB bytes below transparent pixels are garbage.
    if 'A' in im.getbands():
        rgba=im.convert('RGBA'); bg=Image.new('RGBA',rgba.size,(0,0,0,255)); im=Image.alpha_composite(bg,rgba).convert('RGB')
    else: im=im.convert('RGB')
    return np.asarray(im,dtype=np.uint8)

def fit(a,size):
    w,h=size; ih,iw=a.shape[:2]; s=min(w/iw,h/ih)
    rw,rh=max(1,round(iw*s)),max(1,round(ih*s))
    b=np.asarray(Image.fromarray(a).resize((rw,rh),Image.Resampling.BILINEAR))
    out=np.zeros((h,w,3),np.uint8); x=(w-rw)//2;y=(h-rh)//2;out[y:y+rh,x:x+rw]=b
    return out

def hist(a,b):
    return float(np.mean([np.minimum(np.histogram(a[...,c],32,(0,256))[0],np.histogram(b[...,c],32,(0,256))[0]).sum()/a[...,c].size for c in range(3)]))

def edges(a):
    g=a.astype(float)@np.array([.299,.587,.114]); p=np.pad(g,1,mode='edge')
    gx=p[:-2,2:]+2*p[1:-1,2:]+p[2:,2:]-p[:-2,:-2]-2*p[1:-1,:-2]-p[2:,:-2]
    gy=p[2:,:-2]+2*p[2:,1:-1]+p[2:,2:]-p[:-2,:-2]-2*p[:-2,1:-1]-p[:-2,2:]
    return float(np.hypot(gx,gy).mean())

def ssim8(a,b):
    h=(a.shape[0]//8)*8;w=(a.shape[1]//8)*8
    x=a[:h,:w].reshape(h//8,8,w//8,8,3).mean((1,3)); y=b[:h,:w].reshape(h//8,8,w//8,8,3).mean((1,3))
    c1=6.5025;c2=58.5225; mx=x.mean(2);my=y.mean(2)
    vx=((x-mx[:,:,None])**2).mean(2);vy=((y-my[:,:,None])**2).mean(2);cov=((x-mx[:,:,None])*(y-my[:,:,None])).mean(2)
    return float(np.mean(((2*mx*my+c1)*(2*cov+c2))/((mx*mx+my*my+c1)*(vx+vy+c2))))

def roi_metric(a,b,rect):
    x,y,w,h=rect; x2=min(a.shape[1],x+w);y2=min(a.shape[0],y+h)
    aa,bb=a[y:y2,x:x2],b[y:y2,x:x2]
    if aa.size==0 or aa.shape!=bb.shape: raise ValueError('ROI outside or mismatched image dimensions')
    diff=np.abs(aa.astype(np.int16)-bb.astype(np.int16))
    ea,eb=edges(aa),edges(bb)
    return {'mad':float(diff.mean()),'mad_rgb':[float(v) for v in diff.mean((0,1))],'edge_ratio':eb/ea if ea else None,'histogram_intersection':hist(aa,bb),'block_ssim_8':ssim8(aa,bb)}

def compare(scene,oracle_path,native_path):
    a,b=load(oracle_path),load(native_path); an,bn=fit(a,CANVAS),fit(b,CANVAS)
    out={'scene':scene,'histogram_intersection':hist(an,bn),'canvas':roi_metric(an,bn,(0,0,*CANVAS)), 'full_resolution':{},'regions':{}}
    for name,rect in ROIS[scene].items():
        # Full resolution oracle ROI, native mapped proportionally to its dimensions.
        x,y,w,h=rect; nr=(round(x*b.shape[1]/a.shape[1]),round(y*b.shape[0]/a.shape[0]),round(w*b.shape[1]/a.shape[1]),round(h*b.shape[0]/a.shape[0]))
        if a.shape==b.shape:
            out['full_resolution'][name]=roi_metric(a,b,rect)
        else:
            oracle_roi=a[y:y+h,x:x+w]
            native_roi=b[nr[1]:nr[1]+nr[3],nr[0]:nr[0]+nr[2]]
            native_roi=np.asarray(Image.fromarray(native_roi).resize((oracle_roi.shape[1],oracle_roi.shape[0]),Image.Resampling.BILINEAR))
            out['full_resolution'][name]=roi_metric(oracle_roi,native_roi,(0,0,oracle_roi.shape[1],oracle_roi.shape[0]))
        out['regions'][name]=roi_metric(an,bn,(round(x*320/a.shape[1]),round(y*180/a.shape[0]),max(8,round(w*320/a.shape[1])),max(8,round(h*180/a.shape[0]))))
    out['channel_mean_delta']=[float(v) for v in (bn.astype(float).mean((0,1))-an.astype(float).mean((0,1)))]
    return out

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--scene',choices=SCENES,required=True);p.add_argument('--oracle',required=True);p.add_argument('--native',required=True);p.add_argument('--json')
    q=p.parse_args(); result=compare(q.scene,q.oracle,q.native); text=json.dumps(result,indent=2,sort_keys=True)
    if q.json: Path(q.json).write_text(text+'\n')
    print(text)
if __name__=='__main__': main()
