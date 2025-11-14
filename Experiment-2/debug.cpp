// ...existing code...
#include "debug.h"
#include <windows.h>
#include <string>
#include <cstdlib>

// ...existing code...
#ifdef DEBUG
void debug(const std::string &str)
{
    AllocConsole();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written = 0;
    WriteConsoleA(hConsole, str.c_str(), (DWORD)str.length(), &written, NULL);
    system("pause");
    FreeConsole();
}
#endif
// ...existing code...
std::string to_string(POINT P)
{
    return std::string("(") + std::to_string(P.x) + "," + std::to_string(P.y) + ")";
}
// ...existing code...