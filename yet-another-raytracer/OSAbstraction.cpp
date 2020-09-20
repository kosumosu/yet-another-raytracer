#include "OSAbstraction.h"

#define NOGDI
#include <Windows.h>

void openImageFileForDisplay(const wchar_t* imageFileName)
{
	ShellExecute(nullptr, L"Open", imageFileName, nullptr, nullptr, SW_SHOWNORMAL);
}
