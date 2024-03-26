Nikolas Virnitzakis, csd4606

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
    2. 
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