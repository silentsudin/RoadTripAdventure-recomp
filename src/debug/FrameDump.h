#pragma once

namespace rt::debug
{
    // Call once per rendered frame on the render thread. If RT_FRAME_DUMP=<dir> is set, saves the
    // window contents to <dir>/frame_NNNN.png every RT_FRAME_DUMP_SECONDS (default 2) seconds.
    void maybeDumpFrame();
}
