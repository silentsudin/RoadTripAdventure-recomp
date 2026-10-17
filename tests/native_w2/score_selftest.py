#!/usr/bin/env python3
"""Self-check W2 score metrics and v13 continuity baselines."""
from pathlib import Path
import importlib.util, sys, tempfile
import numpy as np
from PIL import Image

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts'))
import w2_score
from w2_score import YMAP_REGISTRY

def main():
    required={'sky','ground','hud_dial','hud_timer','hud_speed_digits','minimap','menu_panel','title_logo'}
    assert set(YMAP_REGISTRY)==required
    for name, entry in YMAP_REGISTRY.items():
        assert entry['status'] in {'FIT','ELEMENT','GAP'}
        if entry['status']=='FIT':
            assert len(entry['pairs'])>=3 and entry['residual_px']<=1.0
            assert all(pair.get('citation') and pair.get('oracle_y') is not None and pair.get('native_y') is not None for pair in entry['pairs'])
        elif entry['status']=='ELEMENT':
            assert entry.get('scale') is not None and entry.get('offset') is not None and entry.get('citation')
        else:
            assert entry.get('reason')
    base=ROOT/'build/scratch'
    pairs=[('title','title',.872),('menu','main_menu',.460),('town','town_driving',.532),('race','race_hud_driving',.374)]
    old=ROOT/'scripts/frame_compare.py'
    spec=importlib.util.spec_from_file_location('frame_compare',old); mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
    actual=[]
    for native,golden,target in pairs:
        op=base/'oracle-golden'/f'{golden}.png';npth=base/'gamelook'/f'{native}-v13.ppm'
        if not op.exists() or not npth.exists():
            print('NO_GOLDEN',file=sys.stderr);return 2
        value=mod.compare_frames(op,npth)['histogram_intersection'];actual.append(value)
        if abs(value-target)>.001: raise AssertionError(f'{native} histogram {value:.6f} != {target:.3f}')
    rgb=np.random.default_rng(14).integers(0,256,(64,64,3),dtype=np.uint8)
    identical=w2_score.roi_metric(rgb,rgb,(0,0,64,64))
    assert identical['mad']==0 and identical['histogram_intersection']==1 and abs(identical['block_ssim_8']-1)<1e-12 and identical['edge_ratio']==1
    shifted=np.roll(rgb,1,axis=1); assert w2_score.ssim8(rgb,shifted)<1
    with tempfile.TemporaryDirectory() as td:
        p=Path(td)/'alpha.png'; rgba=np.zeros((8,8,4),np.uint8);rgba[:,:,:3]=[255,0,128];rgba[:,:,3]=0;Image.fromarray(rgba).save(p)
        assert not w2_score.load(p).any()
    # Independent repeated call must have no metric noise.
    assert w2_score.ssim8(rgb,shifted)==w2_score.ssim8(rgb,shifted)
    regions=sum(len(w2_score.ROIS[s]) for s in w2_score.SCENES)
    self_noise=abs(w2_score.ssim8(rgb,shifted)-w2_score.ssim8(rgb,shifted))
    ymap_counts={kind:sum(item['status']==kind for item in YMAP_REGISTRY.values()) for kind in ('FIT','ELEMENT','GAP')}
    classified=all(item['status']=='GAP' and item.get('reason') or item['status']=='ELEMENT' and item.get('citation') and item.get('scale') is not None and item.get('offset') is not None or item['status']=='FIT' and len(item.get('pairs',[]))>=3 and item.get('residual_px',float('inf'))<=1.0 for item in YMAP_REGISTRY.values())
    passed=regions>0 and len(actual)==4 and self_noise<=.01 and classified and all(abs(v-t)<=.001 for v,(_,_,t) in zip(actual,pairs)) and identical['block_ssim_8']==1.0 and w2_score.ssim8(rgb,shifted)<1.0
    print(f"W2-14 SCORE baseline_hist={actual[0]:.3f}/{actual[1]:.3f}/{actual[2]:.3f}/{actual[3]:.3f} region_scores={regions} ymap_status={w2_score.ymap_status()} self_noise={self_noise:.3f} oracle_vs_oracle=1.000 {'PASS' if passed else 'FAIL'}")
    return 0 if passed else 1
if __name__=='__main__': sys.exit(main())
