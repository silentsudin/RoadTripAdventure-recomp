#pragma once

class PS2Runtime;

namespace rt::debug
{
    // If RT_THREAD_DUMP=<seconds> is set, prints the EE kernel snapshot (guest threads,
    // what each is waiting on, semaphores, event flags) to stderr at that interval.
    void startThreadDumpIfRequested(PS2Runtime &runtime);
}
