#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "../include/sha256.h"

#define MAX_USER 50
#define LOGIN_FILE "data/login.dat"

void printMainMenu(void);
void handleMainMenu(void);
void login_user(void);
void booksMenu(void);
void addBook(void);
void membersMenu(void);
void clearInput(void);
int isValidEmail(const char *email);
int isDigitsOnly(const char *s);

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

const char *BOOKS_FILE = "data/books.dat";

const char *MEMBERS_FILE = "data/members.dat";

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
        system("pause");
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

// ultils functions
void clearInput(void)
{
    int c;
    while (c = getchar() != '\n' && c != EOF)
        ;
}

int isDigitsOnly(const char *s)
{
    while (*s)
    {
        if (!isdigit(*s))
            return 0;
        s++;
    }
}

// Check email that:
// - both are found
// - '@' comes before the last '.'
// - '@' is not the first char
// - '.' is not the last char
int isValidEmail(const char *email)
{
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    return at && dot && at > email && dot > at && dot[1] != '\0';
}

// UI functions
void printMainMenu(void)
{
    system("cls"); // Clear the console screen
    puts("===== LIBRARY MANAGEMENT SYSTEM =====");
    puts("1. Books");
    puts("2. Members");
    puts("3. Exit");
    printf("Select > ");
}

void handleMainMenu(void)
{
    int choice;
    while (scanf("%d", &choice) != 1)
    {
        clearInput();
    }
    switch (choice)
    {
    case 1:
        booksMenu();
        break;
    case 2:
        // membersMenu();
        break;
    case 3:
        puts("Exiting the system.");
        exit(0);
    default:
        puts("Invalid choice. Please try again.");
        system("pause");
        clearInput(); // Clear the input buffer
        printMainMenu();
        break;
    }
}

// Function to display the books menu
void booksMenu(void)
{
    system("cls"); // Clear the console screen
    puts("===== BOOKS MENU =====");
    puts("1. Add Book");
    puts("2. View Books");
    puts("3. Back to Main Menu");
    printf("Select > ");

    int choice;
    while (scanf("%d", &choice) != 1)
    {
        clearInput();
        printf("Invalid input. Please try again: ");
    }

    switch (choice)
    {
    case 1:
        addBook();
        break;
    case 2:
        // viewBooks();
        break;
    case 3:
        printMainMenu();
        handleMainMenu();
        break;
    default:
        puts("Invalid choice. Please try again.");
        booksMenu();
        break;
    }
}

void addBook()
{
    system("cls"); // Clear the console screen
    Book newBook;
    FILE *file = fopen(BOOKS_FILE, "ab+");
    if (!file)
    {
        perror("Failed to open books file");
        return;
    }

    printf("Enter book ID: ");
    while (scanf("%d", &newBook.bookID) != 1 || newBook.bookID <= 0)
    {
        clearInput();
        printf("Invalid input. Please enter a positive integer for book ID: ");
    }

    clearInput(); // Clear the newline character from the input buffer

    printf("Enter book title: ");
    fgets(newBook.title, sizeof(newBook.title), stdin);
    newBook.title[strcspn(newBook.title, "\n")] = '\0'; // Remove trailing newline

    printf("Enter book author: ");
    fgets(newBook.author, sizeof(newBook.author), stdin);
    newBook.author[strcspn(newBook.author, "\n")] = '\0'; // Remove trailing newline

    printf("Enter publication date (YYYY-MM-DD): ");
    char dateStr[11];
    fgets(dateStr, sizeof(dateStr), stdin);
    // set all fields of tm to 0
    dateStr[strcspn(dateStr, "\n")] = '\0'; // Remove trailing newline
    struct tm tm = {0};
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &tm);
    newBook.publicationDate = mktime(&tm);

    printf("Enter quantity: ");
    while (scanf("%d", &newBook.quantity) != 1 || newBook.quantity < 0)
    {
        clearInput();
        printf("Invalid input. Please enter a non-negative integer for quantity: ");
    }

    fwrite(&newBook, sizeof(Book), 1, file);
    fclose(file);

    puts("✅ Book added successfully!");
}