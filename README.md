Nikolas Virnitzakis, csd4606

"testdirectory" includes files and subdirectories to test "scan" and "inspect". Can also be used for "monitor"

Notes:
    1. Max path size is 2048
    2. 
    3. Max character size for string in URL inspection is 1024
    4. I needed to install curl with:
        sudo apt-get install libcurl4-openssl-dev
    5. Inspector inspects every string of format:
        something.somehing_else
    6. Monitor implementation is an adjusted version of the man example.