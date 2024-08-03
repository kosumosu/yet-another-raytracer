#include <windows.h>

int wmain(int argc, const wchar_t** argv);

int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    int argCount;

    LPWSTR *szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
    if (szArgList == nullptr)
    {
        MessageBoxW(nullptr, L"Unable to parse command line", L"Error", MB_OK);
        return 10;
    }

    wmain(argCount, const_cast<LPCWSTR*>(szArgList));

    LocalFree(szArgList);

    return 0;
}