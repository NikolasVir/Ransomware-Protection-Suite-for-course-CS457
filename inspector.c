// Removes the https:// and / parts of a URL (if they exist)
void extract_domain(char *url)
{
    const char *prefix = "https://";
    size_t len = strlen(prefix);

    if (strncmp(url, prefix, len) == 0)
    {
        memmove(url, url + len, strlen(url) - len + 1);
    }

    for (int i = 0; i < strlen(url); i++)
    {
        if (url[i] == '/')
        {
            url[i] = '\0';
        }
    }
}

size_t print_response(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    char *response = (char *)contents;

    if (strstr(response, "0.0.0.0"))
    {
        printf("UNSAFE\n");
    }
    else
    {
        printf("SAFE\n");
    }

    return realsize;
}

void check_domain(const char *file_name, char *to_check)
{
    extract_domain(to_check);
    printf("%s:%s:", file_name, to_check);
    char *to_request = (char *)malloc((strlen("https://family.cloudflare-dns.com/dns-query?name=") + 1024) * sizeof(char));
    strcpy(to_request, "https://family.cloudflare-dns.com/dns-query?name=");

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = NULL;

        headers = curl_slist_append(headers, "accept: application/dns-json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_URL, strcat(to_request, to_check));

        /* Set the write callback */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, print_response);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

// Returns 1 if a match is found, else 0
void match_regex_and_check_domain(const char *file_name, char *to_match)
{
    regex_t regex;
    regmatch_t matches[1];

    // Compile the regular expression
    if (regcomp(&regex, "^((http|https)://)?(www.)?[a-zA-Z0-9-]+\\.[a-zA-Z]{2,}(/)?", REG_EXTENDED))
    {
        printf("Could not compile regex\n");
        return;
    }

    // Execute the regular expression
    int match = regexec(&regex, to_match, 1, matches, 0);
    regfree(&regex);

    if (!match) // is a URL ==>> check domain
    {
        char *url = strndup(to_match + matches[0].rm_so, matches[0].rm_eo - matches[0].rm_so);
        check_domain(file_name, url);
    }
    else if (match == REG_NOMATCH) // is not a URL
    {
    }
    else
    {
        printf("An error occurred.\n");
    }
}

void extract_strings(const char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", file_name);
        return;
    }

    char buffer[1024];
    int index = 0;

    while (1)
    {
        int c = fgetc(file);
        if (c == EOF)
        {
            break;
        }

        if (isprint(c))
        {
            buffer[index++] = c;
            if (index == sizeof(buffer) - 1)
            {
                buffer[index] = '\0';
                match_regex_and_check_domain(file_name, buffer);
                index = 0;
            }
        }
        else
        {
            if (index > 0)
            {
                buffer[index] = '\0';
                match_regex_and_check_domain(file_name, buffer);
                index = 0;
            }
        }
    }

    if (index > 0)
    {
        buffer[index] = '\0';
        match_regex_and_check_domain(file_name, buffer);
    }

    fclose(file);
}

void extract_from_files()
{
    for (int i = 0; i < file_table_size; i++)
    {
        extract_strings(file_table[i]);
    }
}