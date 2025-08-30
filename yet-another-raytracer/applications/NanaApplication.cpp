#if defined(_WIN32)
#include <vector>
#include <windows.h>

int main(int argc, const char* argv[]);

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    int argCount;

    LPWSTR *szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
    if (szArgList == nullptr)
    {
        MessageBoxW(nullptr, L"Unable to parse command line", L"Error", MB_OK);
        return 10;
    }

    std::vector<char> utf8_buf;
    std::vector<const char*> utf8_pointers(argCount, nullptr);

    for (int i = 0; i < argCount; ++i) {
        const auto arg_len = wcstombs(nullptr, szArgList[i], -1);
        utf8_buf.resize(utf8_buf.size() + arg_len + 1);
        auto buf_ptr = &utf8_buf.back() - arg_len - 1;
        wcstombs(buf_ptr, szArgList[i], arg_len + 1);
        utf8_pointers[i] = buf_ptr;
    }

    main(argCount, utf8_pointers.data());

    LocalFree(szArgList);

    return 0;
}

#endif