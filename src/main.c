#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/sha256.h"

#define MAX_USER 50
#define LOGIN_FILE "../data/login.dat"

void printMainMenu(void);
void handleMainMenu(void);
void login_user(void);

typedef struct
{
    int bookID;
    char title[100];
    char author[100];
    time_t publicationDate;
    int quantity;
} Book;
typedef struct
{
    int memberID;
    char name[100];
    char email[100];
    char phone[10];
} Member;

const char *BOOKS_FILE = "data/books.txt";

const char *MEMBERS_FILE = "data/members.txt";

int main()
{
    login_user();

    return 0;
}

void login_user()
{
    int failed_attempts = 0;
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
        system("pause");
        printMainMenu();
        handleMainMenu();
    }
    else
    {
        printf("❌ Login failed. Incorrect username or password.\n");
        failed_attempts++;
        if (failed_attempts >= 3)
        {
            printf("Too many failed attempts. Exiting...\n");
            exit(1);
        }
        else
        {
            printf("Please try again.\n");
            login_user();
        }
    }
}

void printMainMenu(void)
{
    puts("===== LIBRARY MANAGEMENT SYSTEM =====");
    puts("1. Books");
    puts("2. Members");
    puts("3. Exit");
    printf("Select > ");
}

void handleMainMenu(void)
{
    int choice;
    while (scanf("%d,&choice") != 1)
    {
        clearInput();
    }
    switch (choice)
    {
    case 1:
        booksMenu();
        break;
    case 2:
        membersMenu();
        break;
    case 3:
        puts("Exiting the system.");
        exit(0);
    default:
        puts("Invalid choice. Please try again.");
        break;
    }
}