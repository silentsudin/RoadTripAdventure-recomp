package io.github.roadtrip;

import android.app.Presentation;
import android.content.Context;
import android.hardware.display.DisplayManager;
import android.os.Bundle;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowInsets;
import android.view.WindowInsetsController;

import org.libsdl.app.SDLActivity;

/** Road Trip: SDL's activity with SDL3 linked statically into libmain.so (the whole app). */
public class RoadTripActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] { "main" };
    }

    /**
     * Immersive: no status or navigation bar over the game. Applied again whenever the window gets
     * focus back (after the document picker, home, a notification), since Android restores the bars
     * when another activity has been in front. A swipe from the edge shows them briefly.
     */
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            hideSystemBars();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        hideSystemBars();
        showSecondScreen();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        DisplayManager displays = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        displays.registerDisplayListener(new DisplayManager.DisplayListener() {
            @Override public void onDisplayAdded(int id) { showSecondScreen(); }
            @Override public void onDisplayRemoved(int id) {
                if (secondScreen != null && secondScreen.getDisplay().getDisplayId() == id) {
                    secondScreen.dismiss();
                    secondScreen = null;
                }
                nativeSecondDisplay(displays.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION).length > 0);
            }
            @Override public void onDisplayChanged(int id) {}
        }, null);
    }

    // ---- The second screen (the AYN Thor's lower display) ----
    // A Presentation on the first presentation display: a SurfaceView whose surface the native
    // presenter draws into (its own swapchain on the game's Vulkan device), and whose touches go to
    // the app's lower-screen UI.

    private static native void nativeSecondSurface(Surface surface);
    private static native void nativeSecondTouch(int action, int pointer, float x, float y);
    private static native void nativeSecondDisplay(boolean present);

    private Presentation secondScreen;
    private volatile boolean secondScreenEnabled = true; // Options → Second screen

    /** From native code (any thread): Options → Second screen. Off gives the display back to Android. */
    public void setSecondScreenEnabled(boolean on) {
        secondScreenEnabled = on;
        runOnUiThread(() -> {
            if (on) {
                showSecondScreen();
            } else if (secondScreen != null) {
                secondScreen.dismiss();
                secondScreen = null;
            }
        });
    }

    private void showSecondScreen() {
        if (secondScreen != null) {
            nativeSecondDisplay(true);
            return;
        }
        DisplayManager displays = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        Display[] found = displays.getDisplays(DisplayManager.DISPLAY_CATEGORY_PRESENTATION);
        nativeSecondDisplay(found.length > 0);
        if (found.length == 0 || !secondScreenEnabled) {
            return;
        }
        Presentation presentation = new Presentation(this, found[0]);
        SurfaceView view = new SurfaceView(presentation.getContext());
        view.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override public void surfaceCreated(SurfaceHolder holder) {}
            @Override public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                nativeSecondSurface(holder.getSurface());
            }
            @Override public void surfaceDestroyed(SurfaceHolder holder) {
                nativeSecondSurface(null);
            }
        });
        view.setOnTouchListener((v, e) -> {
            int action = e.getActionMasked();
            int index = e.getActionIndex();
            if (action == MotionEvent.ACTION_MOVE) {
                for (int i = 0; i < e.getPointerCount(); i++) {
                    nativeSecondTouch(action, e.getPointerId(i), e.getX(i), e.getY(i));
                }
            } else {
                nativeSecondTouch(action, e.getPointerId(index), e.getX(index), e.getY(index));
            }
            return true;
        });
        presentation.setContentView(view);
        presentation.setOnDismissListener(d -> {
            if (secondScreen == presentation) {
                secondScreen = null;
            }
        });
        try {
            presentation.show();
            secondScreen = presentation;
        } catch (RuntimeException e) {
            // The display went away (or can't take a window): no second screen.
        }
    }

    private void hideSystemBars() {
        WindowInsetsController controller = getWindow().getInsetsController();
        if (controller != null) {
            controller.hide(WindowInsets.Type.systemBars());
            controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        }
    }
}
