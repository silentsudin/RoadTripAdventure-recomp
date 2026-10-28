package io.github.silentsudin.roadtrip.dev;

import android.app.Presentation;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.hardware.display.DisplayManager;
import android.view.Display;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

import java.nio.ByteBuffer;

import org.libsdl.app.SDLActivity;

/**
 * SDL3 launcher for the authored native scene, plus the second screen (the AYN Thor's lower
 * display): a Presentation holding a SurfaceView. Native code renders the Map / Notebook / Radio
 * page offscreen and hands the RGBA frame to drawSecondFrame(); touches go back as 0..1 fractions.
 * The page is shown only while Options -> Second screen is On (setSecondScreen).
 */
public final class RoadTripNativeActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] { "main" };
    }

    /** Surface size changed (0, 0 = gone). */
    private static native void nativeSecondSurface(int width, int height);
    /** A finger went down on the second screen; x and y are fractions of its size. */
    private static native void nativeSecondTouch(float x, float y);

    private Presentation secondScreen; // UI thread only
    private volatile boolean secondScreenEnabled;
    private volatile SurfaceHolder secondHolder;
    private Bitmap secondBitmap; // drawSecondFrame's thread only

    /** From native code (any thread): is a presentation display attached? */
    public boolean hasSecondDisplay() {
        DisplayManager displays = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        return displays.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION).length > 0;
    }

    /** From native code (any thread): Options -> Second screen. Off gives the display back to Android. */
    public void setSecondScreen(boolean on) {
        secondScreenEnabled = on;
        runOnUiThread(() -> {
            if (on) {
                showSecondScreen();
            } else if (secondScreen != null) {
                Presentation closing = secondScreen;
                secondScreen = null; // first, so its dismiss listener doesn't reopen it
                closing.dismiss();
            }
        });
    }

    /** From native code: one tightly packed RGBA frame, the surface's size. */
    public void drawSecondFrame(ByteBuffer rgba, int width, int height) {
        SurfaceHolder holder = secondHolder;
        if (holder == null) {
            return;
        }
        if (secondBitmap == null || secondBitmap.getWidth() != width || secondBitmap.getHeight() != height) {
            secondBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        }
        rgba.rewind();
        secondBitmap.copyPixelsFromBuffer(rgba);
        Canvas canvas = holder.lockCanvas();
        if (canvas == null) {
            return;
        }
        try {
            canvas.drawBitmap(secondBitmap, 0, 0, null);
        } finally {
            holder.unlockCanvasAndPost(canvas);
        }
    }

    private void showSecondScreen() {
        if (secondScreen != null || !secondScreenEnabled) {
            return;
        }
        DisplayManager displays = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        Display[] found = displays.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION);
        if (found.length == 0) {
            return;
        }
        Presentation presentation = new Presentation(this, found[0]);
        SurfaceView view = new SurfaceView(presentation.getContext());
        view.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override public void surfaceCreated(SurfaceHolder holder) {}
            @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                secondHolder = holder;
                nativeSecondSurface(width, height);
            }
            @Override public void surfaceDestroyed(SurfaceHolder holder) {
                secondHolder = null;
                nativeSecondSurface(0, 0);
            }
        });
        view.setOnTouchListener((v, e) -> {
            if (e.getActionMasked() == MotionEvent.ACTION_DOWN && v.getWidth() > 0 && v.getHeight() > 0) {
                nativeSecondTouch(e.getX() / v.getWidth(), e.getY() / v.getHeight());
            }
            return true;
        });
        presentation.setContentView(view);
        // Not focusable and not cancelable: a focused Presentation would take Back (and pad
        // buttons) from SDL, and as a dialog close itself on Back. It still gets touches.
        presentation.setCancelable(false);
        presentation.getWindow().addFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE);
        presentation.setOnDismissListener(d -> {
            if (secondScreen == presentation) {
                secondScreen = null;
                // Closed by anything but the option (or the display going away): open it again.
                if (secondScreenEnabled) {
                    getWindow().getDecorView().post(this::showSecondScreen);
                }
            }
        });
        try {
            presentation.show();
            secondScreen = presentation;
        } catch (RuntimeException e) {
            // The display went away or can't take a window yet; native code asks again.
        }
    }
}
