#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void unlock_file(const char *locked_filename) {
    FILE *locked_file, *unlocked_file;
    char ch;
    char original_filename[1024];
    char *extension_position;

    if((extension_position = strstr(locked_filename, ".locked")) == NULL) {
        printf("Error: The specified file is not in locked format.\n");
        exit(1);
    }
    
    strncpy(original_filename, locked_filename, extension_position - locked_filename);
    original_filename[extension_position - locked_filename] = '\0';
    
    locked_file = fopen(locked_filename, "r");
    if(locked_file == NULL) {
        printf("Error: Unable to open locked file %s.\n", locked_filename);
        exit(1);
    }
    
    char newFilename[strlen(original_filename) + 10];
    snprintf(newFilename, sizeof(newFilename), "%s.unlocked", original_filename);
    
    unlocked_file = fopen(newFilename, "w");
    if(unlocked_file == NULL) {
        printf("Error: Unable to create unlocked file %s.\n", newFilename);
        fclose(locked_file);
        exit(1);
    }
    
    while((ch = fgetc(locked_file)) != EOF) {
        fputc(ch, unlocked_file);
    }
    
    fclose(locked_file);
    fclose(unlocked_file);
    
    if(remove(locked_filename) != 0) {
        printf("Error: Unable to delete locked file %s.\n", locked_filename);
        exit(1);
    } else {
        printf("File %s unlocked to %s and deleted successfully.\n", locked_filename, newFilename);
    }
}


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <locked_filename>\n", argv[0]);
        return 1;
    }
    
    unlock_file(argv[1]);
    
    return 0;
}