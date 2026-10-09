#include "platform/Process.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <fstream>
#include <sstream>

namespace rt::process
{
    namespace
    {
        std::wstring widen(const std::string &s)
        {
            if (s.empty())
                return {};
            const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
            std::wstring w(static_cast<size_t>(n), L'\0');
            MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), n);
            return w;
        }

        // One argument as CreateProcess wants it (the rules CommandLineToArgvW undoes).
        void appendQuoted(std::wstring &out, const std::wstring &a)
        {
            if (!a.empty() && a.find_first_of(L" \t\n\v\"") == std::wstring::npos)
            {
                out += a;
                return;
            }
            out += L'"';
            for (size_t i = 0;; ++i)
            {
                size_t backslashes = 0;
                while (i < a.size() && a[i] == L'\\')
                {
                    ++i;
                    ++backslashes;
                }
                if (i == a.size())
                {
                    out.append(backslashes * 2, L'\\');
                    break;
                }
                if (a[i] == L'"')
                    out.append(backslashes * 2 + 1, L'\\');
                else
                    out.append(backslashes, L'\\');
                out += a[i];
            }
            out += L'"';
        }
    }

    int run(const std::vector<std::string> &argv, const std::filesystem::path &log, const std::filesystem::path &cwd)
    {
        if (argv.empty())
            return -1;

        SECURITY_ATTRIBUTES inherit{sizeof(inherit), nullptr, TRUE};
        HANDLE out = CreateFileW(log.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                                 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (out == INVALID_HANDLE_VALUE)
            return -1;

        std::wstring cmd;
        for (size_t i = 0; i < argv.size(); ++i)
        {
            if (i)
                cmd += L' ';
            appendQuoted(cmd, widen(argv[i]));
        }

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = out;
        si.hStdError = out;
        PROCESS_INFORMATION pi{};
        const std::wstring dir = cwd.empty() ? std::wstring() : cwd.wstring();
        const BOOL ok = CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr,
                                       dir.empty() ? nullptr : dir.c_str(), &si, &pi);
        CloseHandle(out);
        if (!ok)
            return -1;
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 0;
        const BOOL got = GetExitCodeProcess(pi.hProcess, &code);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return got ? static_cast<int>(code) : -1;
    }

    std::string capture(const std::vector<std::string> &argv)
    {
        const std::filesystem::path tmp =
            std::filesystem::temp_directory_path() / ("rt_capture_" + std::to_string(GetCurrentProcessId()));
        std::filesystem::remove(tmp);
        const int rc = run(argv, tmp);
        std::ifstream in(tmp, std::ios::binary);
        std::stringstream ss;
        ss << in.rdbuf();
        in.close();
        std::filesystem::remove(tmp);
        std::string s = ss.str();
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' '))
            s.pop_back();
        return rc == 0 ? s : std::string{};
    }
}
