all:
	gcc antivirus.c -o antivirus -g -lcrypto -lcurl -std=c99
	gcc ransomware.c -o ransomware.out
	gcc unransomware.c -o unransomware.out 