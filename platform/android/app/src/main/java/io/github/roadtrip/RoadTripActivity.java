package io.github.roadtrip;

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
    }

    private void hideSystemBars() {
        WindowInsetsController controller = getWindow().getInsetsController();
        if (controller != null) {
            controller.hide(WindowInsets.Type.systemBars());
            controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
        }
    }
}
