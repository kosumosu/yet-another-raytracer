#include "OSAbstraction.h"

#if defined(_WIN32)
#define NOGDI
#include <Windows.h>

void openImageFileForDisplay(const wchar_t* imageFileName)
{
	ShellExecute(nullptr, L"Open", imageFileName, nullptr, nullptr, SW_SHOWNORMAL);
}

#elif defined(__linux__)

#include <codecvt>
#include <cstdlib>
#include <sstream>

void openImageFileForDisplay(const wchar_t* imageFileName)
{
	std::wstringstream ss;
	ss << "xdg-open " << imageFileName;
	auto converter = std::wstring_convert<std::codecvt_utf8<wchar_t>>{};
	std::system(converter.to_bytes(ss.str()).c_str());
}

#endif