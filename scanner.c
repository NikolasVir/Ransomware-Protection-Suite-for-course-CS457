// Computes the SHA256 hash of file_name
char *sha256(const char *file_name)
{
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    char buffer[1024];
    size_t bytes_read;
    FILE *file;
    EVP_MD_CTX *mdctx;

    file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", file_name);
        return NULL;
    }

    // Create the hash context
    mdctx = EVP_MD_CTX_new();

    // Initialize the hash context with the SHA-256 algorithm
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

    // Update the hash with the file data
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) != 0)
    {
        EVP_DigestUpdate(mdctx, buffer, bytes_read);
    }

    // Finalize the hash
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);

    // Allocate memory for the hash string
    char *hash_string = malloc(hash_len * 2 + 1);

    // Convert the hash to a string
    for (unsigned int i = 0; i < hash_len; i++)
    {
        sprintf(hash_string + i * 2, "%02x", hash[i]);
    }

    // Clean up
    EVP_MD_CTX_free(mdctx);
    fclose(file);

    return hash_string;
}

// Computes the MD5 hash of file_name
char *md5(const char *file_name)
{
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    char buffer[1024];
    size_t bytes_read;
    FILE *file;
    EVP_MD_CTX *mdctx;

    file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", file_name);
        return NULL;
    }

    // Create the hash context
    mdctx = EVP_MD_CTX_new();

    // Initialize the hash context with the MD5 algorithm
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
    // Update the hash with the file data

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) != 0)
    {
        EVP_DigestUpdate(mdctx, buffer, bytes_read);
    }

    // Finalize the hash
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);

    // Allocate memory for the hash string
    char *hash_string = malloc(hash_len * 2 + 1);

    // Convert the hash to a string
    for (unsigned int i = 0; i < hash_len; i++)
    {
        sprintf(hash_string + i * 2, "%02x", hash[i]);
    }

    // Clean up
    EVP_MD_CTX_free(mdctx);
    fclose(file);

    return hash_string;
}

// Checks if the file contains the bitcoin address. Returns 1 if it does, else 0
int check_for_bitcoin(const char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", file_name);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, file);
    fclose(file);

    string[fsize] = 0;

    int result = (strstr(string, BITCOIN) != NULL) ? 1 : 0;

    free(string);

    return result;
}

// Checks if the file contains the signature. Returns 1 if it does, else 0
int check_signature(const char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open file %s\n", file_name);
        return -1;
    }

    size_t sig_size = sizeof(SIGNATURE) / sizeof(SIGNATURE[0]);

    uint8_t buffer[sig_size];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sig_size, file)) == sig_size)
    {
        if (memcmp(buffer, SIGNATURE, sig_size) == 0)
        {
            fclose(file);
            return 1;
        }
        fseek(file, 1 - sig_size, SEEK_CUR); // Move back sig_size - 1 positions to check overlapping signatures
    }

    fclose(file);
    return 0;
}

void print_threat_found(const char *file_name, const char *threat)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *time_str = asctime(tm);
    time_str[strlen(time_str) - 1] = '\0';
    printf("[%s] ENCOUNTERED THREAT: %s:%s\n", time_str, file_name, threat);
}

void check_files()
{
    for (int i = 0; i < file_table_size; i++)
    {
        if (strcmp(SHA256, sha256(file_table[i])) == 0)
        {
            print_threat_found(file_table[i], "SHA256");
        }
        if (strcmp(MD5, md5(file_table[i])) == 0)
        {
            print_threat_found(file_table[i], "MD5");
        }
        if (check_for_bitcoin(file_table[i]))
        {
            print_threat_found(file_table[i], "BITCOIN");
        }
        if (check_signature(file_table[i]))
        {
            print_threat_found(file_table[i], "SIGNATURE");
        }
    }
}
