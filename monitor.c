char **directory_table;
int directory_table_size;

void count_directories(const char *dir_path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    dir = opendir(dir_path);
    if (dir == NULL)
    {
        fprintf(stderr, "Cannot open directory '%s'\n", dir_path);
        return;
    }

    // Increment directory_table_size
    directory_table_size++;
    // Traverse directory
    while ((entry = readdir(dir)) != NULL)
    {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_path, entry->d_name);

        // Ignore current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file status
        if (lstat(fullpath, &fileStat) < 0)
        {
            fprintf(stderr, "Cannot stat file '%s'\n", fullpath);
            continue;
        }

        // Check if it's a directory
        if (S_ISDIR(fileStat.st_mode))
        {
            // Recursively list directories
            count_directories(fullpath);
        }
    }

    closedir(dir);
}

void add_directories(const char *dir_path, int index)
{
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;

    // Open directory
    dir = opendir(dir_path);
    if (dir == NULL)
    {
        fprintf(stderr, "Cannot open directory '%s'\n", dir_path);
        return;
    }

    // Add the current directory
    directory_table[index] = strdup(dir_path);

    // Traverse directory
    while ((entry = readdir(dir)) != NULL)
    {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_path, entry->d_name);

        // Ignore current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file status
        if (lstat(fullpath, &fileStat) < 0)
        {
            fprintf(stderr, "Cannot stat file '%s'\n", fullpath);
            continue;
        }

        // Check if it's a directory
        if (S_ISDIR(fileStat.st_mode))
        {
            // Recursively list directories
            add_directories(fullpath, index + 1);
        }
    }

    closedir(dir);
}

void init_directory_table(const char *dir_path)
{
    count_directories(dir_path);
    directory_table = (char **)malloc(directory_table_size * sizeof(char *));
    add_directories(dir_path, 0);
}

void print_directory_table()
{
    for (int i = 0; i < directory_table_size; i++)
    {
        printf("%d. %s\n", i, directory_table[i]);
    }
}

// The program will use a logfile to log all captured events.
//  This ill be used as storage to check if there has been a ransomware attack.
void append_log_file(const char *event, const char *file_name)
{
    FILE *log = fopen("activity.log", "a");
    if (!log)
    {
        fprintf(stderr, "Could not open file %s\n", "activity.log");
        exit(-5);
    }

    fprintf(log, "%s,%s\n", event, file_name);

    fclose(log);
}

// This function will check the log file for suspected behaviour (see README)
void check_log_file(const char *file_name)
{
    char *locked_name = (char *)malloc((strlen(file_name) * sizeof(char)) + 10);
    strcpy(locked_name, file_name);
    strcat(locked_name, ".locked");

    int original_opened = 0;
    int original_accessed = 0;
    int locked_created = 0;
    int locked_modified = 0;
    int locked_closed = 0;
    int original_deleted = 0;

    FILE *log = fopen("activity.log", "r");
    if (!log)
    {
        fprintf(stderr, "Could not open file %s\n", "activity.log");
        exit(-5);
    }

    char line[1128];
    while (fgets(line, sizeof(line), log))
    {
        char *event_buffer = strtok(line, ",");
        char *file_name_buffer = strtok(NULL, ",");
        file_name_buffer[strlen(file_name_buffer) - 1] = '\0';
        if (strcmp(file_name_buffer, file_name) == 0)
        {
            if (strcmp(event_buffer, "OPENED") == 0)
            {
                original_opened = 1;
            }
            if (strcmp(event_buffer, "ACCESSED") == 0)
            {
                original_accessed = 1;
            }
            if (strcmp(event_buffer, "DELETED") == 0)
            {
                original_deleted = 1;
            }
        }
        if (strcmp(file_name_buffer, locked_name) == 0)
        {
            if (strcmp(event_buffer, "CREATED") == 0)
            {
                locked_created = 1;
            }
            if (strcmp(event_buffer, "MODIFIED") == 0)
            {
                locked_modified = 1;
            }
            if (strcmp(event_buffer, "CLOSED_WRITTEN") == 0)
            {
                locked_closed = 1;
            }
        }
    }

    if (original_opened &&
        original_accessed &&
        locked_created &&
        locked_modified &&
        locked_closed &&
        original_deleted)
    {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char *time_str = asctime(tm);
        time_str[strlen(time_str) - 1] = '\0';
        printf("\033[1;31m[%s][WARNING] Ransomware attack detected on file: %s\n", time_str, file_name);
        printf("\033[0m");
    }
}

static void handle_events(int fd, int *wd, int argc, char *argv[])
{
    /* Some systems cannot read integer variables if they are not
       prlinuxoperly aligned. On other systems, incorrect alignment may
       decrease performance. Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event. */

    char buf[4096]
        __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;)
    {

        /* Read some events. */

        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */

        if (len <= 0)
            break;

        /* Loop over all events in the buffer. */

        for (char *ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len)
        {

            event = (const struct inotify_event *)ptr;

            /* Print event type. */
            if (!(event->mask & IN_ISDIR))
            {
                if (event->mask & IN_OPEN)
                {
                    printf("OPENED: ");
                    printf("%s\n", event->name);
                    append_log_file("OPENED", event->name);
                }
                if (event->mask & IN_ACCESS)
                {
                    printf("ACCESSED: ");
                    printf("%s\n", event->name);
                    append_log_file("ACCESSED", event->name);
                }
                if (event->mask & IN_CREATE)
                {
                    printf("CREATED: ");
                    printf("%s\n", event->name);
                    append_log_file("CREATED", event->name);
                }
                if (event->mask & IN_MODIFY)
                {
                    printf("MODIFIED: ");
                    printf("%s\n", event->name);
                    append_log_file("MODIFIED", event->name);
                }
                if (event->mask & IN_CLOSE_WRITE)
                {
                    printf("CLOSED_WRITTEN: ");
                    printf("%s\n", event->name);
                    append_log_file("CLOSED_WRITTEN", event->name);
                }
                if (event->mask & IN_CLOSE_NOWRITE)
                {
                    printf("CLOSED_NOWRITTEN: ");
                    printf("%s\n", event->name);
                    append_log_file("CLOSED_NOWRITTEN", event->name);
                }
                if (event->mask & IN_DELETE)
                {
                    printf("DELETED: ");
                    printf("%s\n", event->name);
                    append_log_file("DELETED", event->name);
                    check_log_file(strdup(event->name));
                }
            }
            /* Print the name of the watched directory. */

            for (size_t i = 1; i < argc; ++i)
            {
                if (wd[i] == event->wd)
                {
                    printf("%s/", argv[i]);
                    break;
                }
            }
        }
    }
}

int init_monitoring(int argc, char *argv[])
{
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    remove("activity.log"); // If already exists (from previous run), delete it

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API. */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }

    /* Allocate memory for watch descriptors. */

    wd = calloc(argc, sizeof(int));
    if (wd == NULL)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* Mark directories for events
       - file was opened
       - file was closed */

    for (i = 0; i < argc; i++)
    {
        wd[i] = inotify_add_watch(fd, argv[i],
                                  IN_OPEN | IN_ACCESS | IN_CREATE | IN_MODIFY | IN_CLOSE | IN_DELETE);
        if (wd[i] == -1)
        {
            fprintf(stderr, "Cannot watch '%s': %s\n",
                    argv[i], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Prepare for polling. */

    nfds = 2;

    fds[0].fd = STDIN_FILENO; /* Console input */
    fds[0].events = POLLIN;

    fds[1].fd = fd; /* Inotify input */
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input. */
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *time_str = asctime(tm);
    time_str[strlen(time_str) - 1] = '\0';
    printf("[%s] Listening for events:\n", time_str);

    while (1)
    {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1)
        {
            if (errno == EINTR)
                continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num > 0)
        {

            if (fds[0].revents & POLLIN)
            {

                /* Console input is available. Empty stdin and quit. */

                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN)
            {

                /* Inotify events are available. */

                handle_events(fd, wd, argc, argv);
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor. */

    close(fd);

    free(wd);
    exit(EXIT_SUCCESS);
}