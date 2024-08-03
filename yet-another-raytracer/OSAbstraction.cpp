#include "OSAbstraction.h"

#if defined(_WIN32)
#define NOGDI
#include <Windows.h>

void openImageFileForDisplay(const std::filesystem::path& imageFileName)
{
	ShellExecuteW(nullptr, L"Open", imageFileName.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

#elif defined(__linux__)

#include <codecvt>
#include <cstdlib>
#include <sstream>

void openImageFileForDisplay(const std::filesystem::path& imageFileName)
{
	std::stringstream ss;
	ss << "xdg-open " << imageFileName;
	std::system(ss.str().c_str());
}

#endif