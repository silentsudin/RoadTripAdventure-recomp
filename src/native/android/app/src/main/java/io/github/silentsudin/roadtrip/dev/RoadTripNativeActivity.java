package io.github.silentsudin.roadtrip.dev;

import org.libsdl.app.SDLActivity;

/** Minimal SDL3 launcher for the authored native scene. */
public final class RoadTripNativeActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] { "main" };
    }
}
