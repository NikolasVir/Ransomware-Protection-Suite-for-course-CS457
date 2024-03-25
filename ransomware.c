#include <stdio.h>
#include <stdlib.h>

void copy_and_lock_file(const char *filename)
{
    FILE *original_file, *locked_file;
    char ch;
    char new_filename[1024];

    original_file = fopen(filename, "r");
    if (original_file == NULL)
    {
        printf("Error: Unable to open file %s.\n", filename);
        exit(1);
    }

    snprintf(new_filename, sizeof(new_filename), "%s.locked", filename);

    locked_file = fopen(new_filename, "w");
    if (locked_file == NULL)
    {
        printf("Error: Unable to create locked file %s.\n", new_filename);
        fclose(original_file);
        exit(1);
    }

    while ((ch = fgetc(original_file)) != EOF)
    {
        fputc(ch, locked_file);
    }

    fclose(original_file);
    fclose(locked_file);

    if (remove(filename) != 0)
    {
        printf("Error: Unable to delete file %s.\n", filename);
        exit(1);
    }
    else
    {
        printf("File %s copied to %s and deleted successfully.\n", filename, new_filename);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    copy_and_lock_file(argv[1]);

    return 0;
}
