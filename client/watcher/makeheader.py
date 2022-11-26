with open("main.exe", "rb") as file:
 bytez = file.read()
header = "char watcher_code[] = \""
i = len(bytez)
for bytec in bytez:
 header += "\\x{:02x}".format(bytec)
header += "\";"
print(header)