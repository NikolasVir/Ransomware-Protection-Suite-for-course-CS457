#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <time.h>
#include <ctype.h>
#include <regex.h>
#include <curl/curl.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/stat.h>

char **file_table;
int file_table_size;

#define MD5 "85578cd4404c6d586cd0ae1b36c98aca"
#define SHA256 "d56d67f2c43411d966525b3250bfaa1a85db34bf371468df1b6a9882fee78849"
#define BITCOIN "bc1qa5wkgaew2dkv56kfvj49j0av5nml45x9ek9hz6"
uint8_t SIGNATURE[] = {0x98, 0x1d, 0x00, 0x00, 0xec, 0x33, 0xff, 0xff, 0xfb, 0x06, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10};

#include "scanner.c"
#include "inspector.c"
#include "monitor.c"
#include "slicer.c"
#include "unlocker.c"

typedef enum
{
    SCAN,
    INSPECT,
    MONITOR,
    SLICE,
    UNLOCK
} Mode;

Mode mode;

// If arguments are correct, it sets the correct mode, else it exits and prints an appropriate error message
void check_arguments(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid argument.\n");
        exit(1);
    }
    if (strcmp(argv[1], "scan") == 0)
    {
        mode = SCAN;
    }
    if (strcmp(argv[1], "inspect") == 0)
    {
        mode = INSPECT;
    }
    if (strcmp(argv[1], "monitor") == 0)
    {
        mode = MONITOR;
    }
    if (strcmp(argv[1], "slice") == 0)
    {
        mode = SLICE;
    }
    if (strcmp(argv[1], "unlock") == 0)
    {
        mode = UNLOCK;
    }
}

int count_files(const char *dir_path)
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    int count = 0;

    if (dir == NULL)
    {
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        char path[2048];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            count += count_files(path);
        }
        else
        {
            count++;
        }
    }
    closedir(dir);
    return count;
}

int get_file_names(const char *dir_path, int start_index)
{
    struct dirent *entry;
    DIR *dir = opendir(dir_path);
    int i = start_index;

    if (dir == NULL)
    {
        return i;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            i = get_file_names(path, i);
        }
        else
        {
            file_table[i] = strdup(path);
            i++;
        }
    }
    closedir(dir);
    return i;
}

// Initializes file_table's values
void init_file_table(const char *dir_path)
{
    file_table_size = count_files(dir_path);
    file_table = (char **)malloc(file_table_size * sizeof(char *));
    get_file_names(dir_path, 0);
}

int main(int argc, char *argv[])
{
    double c;
    double ca, cb, cc;
    double x1, y1, x2, y2, x3, y3;
    check_arguments(argc, argv);
    switch (mode)
    {
    case SCAN:
        init_file_table(argv[2]);
        check_files();
        break;
    case INSPECT:
        init_file_table(argv[2]);
        printf("Suspected Websites Links:\n");
        extract_from_files();
        break;
    case MONITOR:
        init_directory_table(argv[2]);
        init_monitoring(directory_table_size, directory_table);
        break;
    case SLICE:
        sscanf(argv[2], "%lf", &c);
        generate_random_polynomial_and_points(c);
        break;
    case UNLOCK:
        sscanf(argv[2], "%lf", &x1);
        sscanf(argv[3], "%lf", &y1);
        sscanf(argv[4], "%lf", &x2);
        sscanf(argv[5], "%lf", &y2);
        sscanf(argv[6], "%lf", &x3);
        sscanf(argv[7], "%lf", &y3);
        calculate_coefficients(x1, y1, x2, y2, x3, y3, &ca, &cb, &cc);
        printf("Key is: %lf\n", cc);
        break;
    }
}