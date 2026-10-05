// Android: stdout and stderr go nowhere by default. Forward them to logcat (tag RoadTrip), line by
// line, so the runtime's [gs], [fps] and [presenter] lines show up in `adb logcat -s RoadTrip`.

#include <android/log.h>
#include <cstdio>
#include <string>
#include <thread>
#include <unistd.h>

namespace
{
    struct LogRedirect
    {
        LogRedirect()
        {
            int fds[2];
            if (pipe(fds) != 0)
                return;
            setvbuf(stdout, nullptr, _IOLBF, 0);
            setvbuf(stderr, nullptr, _IONBF, 0);
            dup2(fds[1], STDOUT_FILENO);
            dup2(fds[1], STDERR_FILENO);
            std::thread([fd = fds[0]] {
                char buf[4096];
                std::string line;
                for (;;)
                {
                    const ssize_t n = read(fd, buf, sizeof(buf));
                    if (n <= 0)
                        break;
                    for (ssize_t i = 0; i < n; ++i)
                    {
                        if (buf[i] == '\n')
                        {
                            __android_log_write(ANDROID_LOG_INFO, "RoadTrip", line.c_str());
                            line.clear();
                        }
                        else
                            line += buf[i];
                    }
                }
            }).detach();
        }
    } g_logRedirect;
}
