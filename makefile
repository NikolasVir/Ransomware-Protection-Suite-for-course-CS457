all:
	gcc antivirus.c -o antivirus -g -lcrypto -lcurl -std=c99