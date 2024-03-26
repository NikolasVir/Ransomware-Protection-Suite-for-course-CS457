all:
	gcc antivirus.c -o antivirus.out -g -lcrypto -lcurl -std=c99
	gcc ransomware.c -o ransomware.out
	gcc unransomware.c -o unransomware.out

test_scan: all
	./antivirus.out scan testdirectory

test_inspect: all
	./antivirus.out inspect testdirectory

test_monitor: all
	@echo "\nIn this test after "monitor" is run, test it with make test_ransom from a different terminal in the same directory.\n README for more info\n"
	./antivirus.out monitor ransomtestdirectory

test_ransom: all
	./ransomware.out ransomtestdirectory/myfile.txt
	./unransomware.out ransomtestdirectory/myfile.txt.locked

test_slice: all
	@echo "\n Slice 5\n"
	./antivirus.out slice 5

test_unlock: all
	@echo "\n\nUnlock with Points: (4.986645,16108.776301) (2.006358,3506.000661) (-0.473057,-236.259475)\n Important Note: test_unlock does not take as input the random points from test_slice. Please use these points for verification. This is just an example for a different set of points.\n"
	./antivirus.out unlock 4.986645 16108.776301 2.006358 3506.000661 -0.473057 -236.259475