package io.github.roadtrip;

import org.libsdl.app.SDLActivity;

/** Road Trip: SDL's activity with SDL3 linked statically into libmain.so (the whole app). */
public class RoadTripActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] { "main" };
    }
}
