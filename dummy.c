#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void create_test_file(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Failed to open file %s\n", filename);
        return;
    }

    // Write some strings
    const char *str1 = "Hello, World!\n";
    const char *str2 = "This is a test file.\n";
    const char *str3 = "End of test file.\n";
    fwrite(str1, 1, strlen(str1), file);
    fwrite(str2, 1, strlen(str2), file);

    // Write some bytes
    unsigned char bytes[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};
    fwrite(bytes, 1, sizeof(bytes), file);

    // Write some non-printable characters
    unsigned char non_printable_chars[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    fwrite(non_printable_chars, 1, sizeof(non_printable_chars), file);

    // Write another string
    fwrite(str3, 1, strlen(str3), file);

    fclose(file);
}

int main()
{
    /*
     FILE *file = fopen("test.bin", "wb");
     if (!file)
     {
         printf("Unable to open file!\n");
         return 1;
     }

     uint8_t signature[] = {0x98, 0x1d, 0x00, 0x00, 0xec, 0x33, 0xff, 0xff, 0xfb, 0x06, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10};
     size_t sig_size = sizeof(signature) / sizeof(signature[0]);

     fwrite(signature, 1, sig_size, file);

     fclose(file);
     printf("File created successfully.\n");
     return 0;*/
    create_test_file("testfile.bin");
    return 0;
}