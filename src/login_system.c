#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/sha256.h"

#define MAX_USER 50
#define LOGIN_FILE "../data/login.dat"

void register_user()
{
    char username[MAX_USER], password[MAX_USER];
    printf("Create username: ");
    scanf("%49s", username);
    printf("Create password: ");
    scanf("%49s", password);

    // Hash password
    unsigned char hash[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE *)password, strlen(password));
    sha256_final(&ctx, hash);

    // Save to binary file
    FILE *file = fopen(LOGIN_FILE, "wb");
    if (!file)
    {
        perror("Error saving login");
        return;
    }

    int name_len = strlen(username);
    fwrite(&name_len, sizeof(int), 1, file);
    fwrite(username, sizeof(char), name_len, file);
    fwrite(hash, sizeof(unsigned char), 32, file);
    fclose(file);

    printf("Registration successful!\n");
}

void login_user()
{
    char username[MAX_USER], password[MAX_USER];
    printf("Enter username: ");
    scanf("%49s", username);
    printf("Enter password: ");
    scanf("%49s", password);

    FILE *file = fopen(LOGIN_FILE, "rb");
    if (!file)
    {
        printf("No account found. Please register first.\n");
        return;
    }

    int name_len;
    char stored_user[MAX_USER] = {0};
    unsigned char stored_hash[32];

    fread(&name_len, sizeof(int), 1, file);
    fread(stored_user, sizeof(char), name_len, file);
    stored_user[name_len] = '\0';
    fread(stored_hash, sizeof(unsigned char), 32, file);
    fclose(file);

    // Hash input password
    unsigned char input_hash[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE *)password, strlen(password));
    sha256_final(&ctx, input_hash);

    if (strcmp(username, stored_user) == 0 &&
        memcmp(input_hash, stored_hash, 32) == 0)
    {
        printf("✅ Login successful!\n");
    }
    else
    {
        printf("❌ Login failed. Incorrect username or password.\n");
    }
}

int main(void)
{
    int choice;
    printf("====== LOGIN SYSTEM ======\n");
    printf("1. Register\n2. Login\nChoose an option: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
        register_user();
        break;
    case 2:
        login_user();
        break;
    default:
        printf("Invalid choice.\n");
        break;
    }
    system("pause");

    return 0;
}
