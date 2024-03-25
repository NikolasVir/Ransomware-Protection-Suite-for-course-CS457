#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void unlockFile(const char *lockedFilename);

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <locked_filename>\n", argv[0]);
        return 1;
    }
    
    unlockFile(argv[1]);
    
    return 0;
}

void unlockFile(const char *lockedFilename) {
    FILE *lockedFile, *unlockedFile;
    char ch;
    char originalFilename[100]; // Assuming the original filename won't exceed 100 characters
    char *extensionPosition;
    
    // Check if the filename ends with ".locked"
    if((extensionPosition = strstr(lockedFilename, ".locked")) == NULL) {
        printf("Error: The specified file is not in locked format.\n");
        exit(1);
    }
    
    // Extract original filename without ".locked"
    strncpy(originalFilename, lockedFilename, extensionPosition - lockedFilename);
    originalFilename[extensionPosition - lockedFilename] = '\0';
    
    // Open the locked file
    lockedFile = fopen(lockedFilename, "r");
    if(lockedFile == NULL) {
        printf("Error: Unable to open locked file %s.\n", lockedFilename);
        exit(1);
    }
    
    // Construct the new filename with ".unlocked" appended
    char newFilename[strlen(originalFilename) + 10]; // ".unlocked" = 9 characters + '\0'
    snprintf(newFilename, sizeof(newFilename), "%s.unlocked", originalFilename);
    
    // Open the new unlocked file
    unlockedFile = fopen(newFilename, "w");
    if(unlockedFile == NULL) {
        printf("Error: Unable to create unlocked file %s.\n", newFilename);
        fclose(lockedFile);
        exit(1);
    }
    
    // Copy contents of locked file to unlocked file
    while((ch = fgetc(lockedFile)) != EOF) {
        fputc(ch, unlockedFile);
    }
    
    // Close files
    fclose(lockedFile);
    fclose(unlockedFile);
    
    // Delete the locked file
    if(remove(lockedFilename) != 0) {
        printf("Error: Unable to delete locked file %s.\n", lockedFilename);
        exit(1);
    } else {
        printf("File %s unlocked to %s and deleted successfully.\n", lockedFilename, newFilename);
    }
}
