Nikolas Virintzakis, csd4606

"testdirectory" includes files and subdirectories to test "scan" and "inspect". 
"ransomtestdiretory" is for ransom testing (in order to not damage "malware files" above)

Make:
    make all: generated antivirus, dummy ransomware and unransomware
    antivirus usage:
        as described in the assignment (loot at "Notes" below)
    ransomware usage:
        ./ransomware.out <filepath>: deletes original file and replaces it with .locked
    unransomware usage:
        ./unransomware.out <filepath>: deletes .locked file and replaces it with original one

    Note: no encryption/decryption is involved for ransomware and unransomware. 

Notes:
    1. Max path size is 2048
    2. Could not make files with corresponding md5 and sha256 hashes
    3. Max character size for string in URL inspection is 1024
    4. I needed to install curl with:
        sudo apt-get install libcurl4-openssl-dev
    5. Inspector inspects every string of format:
        something.somehing_else
    6. Monitor implementation is an adjusted version of the "man" example.
    7. Ransomware assumed behaviour: (this is when the program will flag behaviour as a ransomware attack)
        open(original)->access(original)->create(.locked)->modify(.locked)->close(.locked)->delete(original)
    8. Unlocker: ./antivirus.out unlock x1 y1 x2 y2 x3 y3
    9. Unlocker is not 100% precise. For example: if c = 5, after unlocking, it might result 5.004262 etc.
    10. YARA.txt contains the yara rules requested.
    11. Complete Example for using slice and unlock
    nikolasvir@nikolasvir-SATELLITE-C50-B:~/cs457_series2_4606$ ./antivirus.out slice 5
    Point 1: (8.028591, 62972.137655)
    Point 2: (1.171944, 1887.311596)
    Point 3: (-9.194094, 71927.387724)
    Point 4: (-5.862553, 28100.379621)
    Point 5: (-8.830747, 66166.515124)
    Point 6: (4.098479, 17497.652103)
    Point 7: (4.503350, 20884.099958)
    Point 8: (3.490555, 12972.845841)
    Point 8: (3.490555, 12972.845841)
    Point 9: (8.888696, 76671.807622)
    Point 10: (-4.580294, 16613.226016)
    nikolasvir@nikolasvir-SATELLITE-C50-B:~/cs457_series2_4606$ ./antivirus.out unlock 4.503350 20884.099958 -9.194094 71927.387724 3.490555 12972.845841
    Key is: 5.014912
    nikolasvir@nikolasvir-SATELLITE-C50-B:~/cs457_series2_4606$
