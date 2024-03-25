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
void append_log_file(const char *event)
{
    FILE *log = fopen("activity.log", "a");
    if (!log)
    {
        fprintf(stderr, "Could not open file %s\n", "activity.log");
        exit(-5);
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

            if (event->mask & IN_OPEN)
            {
                printf("IN_OPEN: ");
            }
            if (event->mask & IN_ACCESS)
            {
                printf("IN_ACCESS: ");
            }
            if (event->mask & IN_CREATE)
            {
                printf("IN_CREATE: ");
            }
            if (event->mask & IN_MODIFY)
            {
                printf("IN_MODIFY: ");
            }
            if (event->mask & IN_CLOSE_WRITE)
            {
                printf("IN_CLOSE_WRITE: ");
            }
            if (event->mask & IN_CLOSE_NOWRITE)
            {
                printf("IN_CLOSE_NOWRITE: ");
            }
            if (event->mask & IN_DELETE_SELF)
            {
                printf("IN_DELETE_SELF: ");
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

            /* Print the name of the file. */

            if (event->len)
                printf("%s", event->name);

            /* Print type of filesystem object. */

            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");
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
                                  IN_OPEN | IN_ACCESS | IN_CREATE | IN_MODIFY | IN_CLOSE | IN_DELETE_SELF);
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

    printf("Listening for events.\n");
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