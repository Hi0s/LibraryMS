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
void viewBooks(void);
void editBookMenu(int bookID);
void membersMenu(void);
void clearInput(void);
int isValidEmail(const char *email);
int isDigitsOnly(const char *s);
int isValidBookID(int bookID);
char *strptime(const char *buf, const char *format, struct tm *tm);

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

// Main function to start the program
int main()
{
    login_user();

    return 0;
}

// ultils functions
void clearInput(void)
{
    int c;
    while (c = getchar() != '\n' && c != EOF)
    {
    }
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

// Check if bookID is valid, to use for adding or editing books
int isValidBookID(int bookID)
{
    if (bookID <= 0)
    {
        return 0; // Invalid book ID
    }

    FILE *file = fopen(BOOKS_FILE, "rb");
    if (!file)
    {
        return 1; // No books file, so any ID is valid
    }

    Book book;
    while (fread(&book, sizeof(Book), 1, file) == 1)
    {
        if (book.bookID == bookID)
        {
            fclose(file);
            return 0; // Book ID found, so it is invalid
        }
    }
    fclose(file);
    return 1; // Book ID not found, so it is valid
}

// Check if the string is a valid date in the format YYYY-MM-DD and within the range 1970-01-01 to current date
int isValidDate(const char *dateStr)
{
    struct tm tm = {0};
    char *result = strptime(dateStr, "%Y-%m-%d", &tm);
    if (result == NULL || *result != '\0')
    {
        return 0; // Invalid format or extra characters
    }

    // Save original input
    int inputYear, inputMonth, inputDay;
    if (sscanf(dateStr, "%d-%d-%d", &inputYear, &inputMonth, &inputDay) != 3)
    {
        return 0;
    }

    // Normalize date with mktime (adjusts out-of-range values)
    time_t epoch = mktime(&tm);
    if (epoch == -1)
        return 0; // mktime failed

    // Validate that mktime didn’t adjust the values
    if (tm.tm_year != inputYear - 1900 || tm.tm_mon != inputMonth - 1 || tm.tm_mday != inputDay)
    {
        return 0; // mktime adjusted the values → invalid date
    }

    // Optional: prevent future dates
    time_t now = time(NULL);
    if (difftime(epoch, now) > 0)
    {
        return 0; // Date is in the future
    }

    return 1; // ✅ Valid date
}

#ifdef _WIN32
// Fallback strptime() for Windows (supports only "%Y-%m-%d")
char *strptime(const char *buf, const char *format, struct tm *tm)
{
    if (strcmp(format, "%Y-%m-%d") == 0)
    {
        int y, m, d;
        if (sscanf(buf, "%d-%d-%d", &y, &m, &d) == 3)
        {
            tm->tm_year = y - 1900;
            tm->tm_mon = m - 1;
            tm->tm_mday = d;
            tm->tm_hour = tm->tm_min = tm->tm_sec = 0;
            return (char *)buf + strlen(buf);
        }
    }
    return NULL;
}
#endif

// Function to login user

void login_user()
{
    int failed_attempts = 0;
    char username[MAX_USER], password[MAX_USER];
    printf("Enter username: ");
    scanf("%49s", username);
    clearInput(); // Clear the newline character from the input buffer
    printf("Enter password: ");
    scanf("%49s", password);
    clearInput(); // Clear the newline character from the input buffer

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
        viewBooks();
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

void addBook(void)
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
    while (scanf("%d", &newBook.bookID) != 1 || isValidBookID(newBook.bookID) == 0)
    {
        // If the book ID is not valid, prompt the user to enter a valid one
        puts("Invalid book ID. Please enter a positive integer that is not already used.");
        // Clear the input buffer
        // This will clear the input buffer until a newline is found
        clearInput();
        printf("Enter book ID: ");
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
    struct tm tm = {0};
    // Check if the input is valid
    while (1)
    {
        fgets(dateStr, sizeof(dateStr), stdin);
        dateStr[strcspn(dateStr, "\n")] = '\0'; // Remove trailing newline
        if (isValidDate(dateStr))
            break;
        else
            printf("Invalid date format. Please enter a valid date (YYYY-MM-DD): ");
    }

    // Parse the date string into a tm structure
    strptime(dateStr, "%Y-%m-%d", &tm);
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
    system("pause");
    booksMenu();
}

void viewBooks(void)
{
    system("cls"); // Clear the console screen
    puts("===== LIST OF BOOKS =====");
    FILE *file = fopen(BOOKS_FILE, "rb");
    if (!file)
    {
        puts("No books found.");
        system("pause");
        booksMenu();
    }
    else
    {
        Book book;
        int count = 0;
        while (fread(&book, sizeof(Book), 1, file) == 1)
        {
            printf("Book ID: %d\n", book.bookID);
            printf("Title: %s\n", book.title);
            printf("Author: %s\n", book.author);
            char dateStr[11];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&book.publicationDate));
            printf("Publication Date: %s\n", dateStr);
            printf("Quantity: %d\n", book.quantity);
            puts("-------------------------");
            count++;
        }
        fclose(file);
        if (count == 0)
        {
            puts("No books found.");
        }
        else
        {
            puts("End of book list.");
            puts("===========================");
            printf("Total books: %d\n", count);
            puts("===========================");
            puts("Type Book ID to edit or delete a book, or 0 to return to the books menu.");
            printf("> ");
            int bookID;
            scanf("%d", &bookID);
            clearInput(); // Clear the newline character from the input buffer
            if (bookID > 0)
            {
                editBookMenu(bookID);
            }
            else if (bookID == 0)
            {
                booksMenu();
            }
            else
            {
                puts("Invalid book ID. System will return to the main menu.");
                system("pause");
                printMainMenu();
            }
        }
    }
}
void editBookMenu(int bookID)
{
    system("cls"); // Clear the console screen
    puts("===== EDIT BOOK =====");
    FILE *file = fopen(BOOKS_FILE, "rb+");
    Book book;
    int found = 0;
    while (fread(&book, sizeof(Book), 1, file) == 1)
    {
        if (book.bookID == bookID)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        puts("Book not found.");
        fclose(file);
        puts("Returning to the books menu...");
        system("pause");
        // Return to the books menu
        fclose(file);
        clearInput(); // Clear the input buffer
        booksMenu();
        return;
    }
    char dateStr[11];
    struct tm tm = {0};
    printf("Book ID: %d\n", book.bookID);
    printf("Current Title: %s\n", book.title);
    printf("Current Author: %s\n", book.author);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&book.publicationDate));
    printf("Current Publication Date: %s\n", dateStr);
    printf("Current Quantity: %d\n", book.quantity);
    puts("-------------------------");

    puts("Choose an option to edit:");
    puts("1. Edit Title");
    puts("2. Edit Author");
    puts("3. Edit Publication Date");
    puts("4. Edit Quantity");
    puts("5. Edit All");
    puts("6. Delete Book");
    puts("7. Cancel and return to the books menu");
    printf("Select > ");
    int choice;

    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 7)
    {
        clearInput();
        printf("Invalid input. Please select a valid option: ");
    }
    clearInput(); // Clear the newline character from the input buffer
    switch (choice)
    {
    case 1:
        printf("Enter new title: ");
        fgets(book.title, sizeof(book.title), stdin);
        book.title[strcspn(book.title, "\n")] = '\0'; // Remove trailing newline
        puts("✅ Title updated successfully.");
        break;
    case 2:
        printf("Enter new author: ");
        fgets(book.author, sizeof(book.author), stdin);
        book.author[strcspn(book.author, "\n")] = '\0'; // Remove trailing newline
        puts("✅ Author updated successfully.");
        break;
    case 3:
        printf("Enter new publication date (YYYY-MM-DD): ");
        while (1)
        {
            printf("> ");
            if (!fgets(dateStr, sizeof(dateStr), stdin))
            {
                printf("Input error.\n");
                continue;
            }

            // Remove newline
            size_t len = strlen(dateStr);
            if (len > 0 && dateStr[len - 1] == '\n')
            {
                dateStr[len - 1] = '\0';
            }
            else
            {
                // If there's no newline, the input was too long — flush buffer
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
            }

            // Skip empty/whitespace-only inputs
            if (strlen(dateStr) == 0 || strspn(dateStr, " \t\r") == strlen(dateStr))
            {
                printf("No input detected. Please enter a date.\n");
                continue;
            }

            if (isValidDate(dateStr))
            {
                break;
            }
            else
            {
                printf("Invalid date format. Please enter a valid date (YYYY-MM-DD):\n");
            }
        }

        strptime(dateStr, "%Y-%m-%d", &tm);
        book.publicationDate = mktime(&tm);
        puts("✅ Publication date updated successfully.");
        break;

    case 4:
        printf("Enter new quantity: ");
        while (scanf("%d", &book.quantity) != 1 || book.quantity < 0)
        {
            clearInput();
            printf("Invalid input. Please enter a non-negative integer for quantity: ");
        }
        break;
    case 5:
        printf("Enter new title: ");
        fgets(book.title, sizeof(book.title), stdin);
        book.title[strcspn(book.title, "\n")] = '\0'; // Remove trailing newline

        printf("Enter new author: ");
        fgets(book.author, sizeof(book.author), stdin);
        book.author[strcspn(book.author, "\n")] = '\0'; // Remove trailing newline

        printf("Enter new publication date (YYYY-MM-DD): ");
        while (1)
        {
            printf("> ");
            if (!fgets(dateStr, sizeof(dateStr), stdin))
            {
                printf("Input error.\n");
                continue;
            }

            // Remove newline
            size_t len = strlen(dateStr);
            if (len > 0 && dateStr[len - 1] == '\n')
            {
                dateStr[len - 1] = '\0';
            }
            else
            {
                // If there's no newline, the input was too long — flush buffer
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF)
                    ;
            }

            // Skip empty/whitespace-only inputs
            if (strlen(dateStr) == 0 || strspn(dateStr, " \t\r") == strlen(dateStr))
            {
                printf("No input detected. Please enter a date.\n");
                continue;
            }

            if (isValidDate(dateStr))
            {
                break;
            }
            else
            {
                printf("Invalid date format. Please enter a valid date (YYYY-MM-DD):\n");
            }
        }
        strptime(dateStr, "%Y-%m-%d", &tm);
        book.publicationDate = mktime(&tm);

        printf("Enter new quantity: ");
        while (scanf("%d", &book.quantity) != 1 || book.quantity < 0)
        {
            clearInput();
            printf("Invalid input. Please enter a non-negative integer for quantity: ");
        }
        puts("✅ Book updated successfully!");
        break;
    case 6:

        // Delete the book
        FILE *tempFile = fopen("data/temp_books.dat", "wb+");
        if (!tempFile)
        {
            perror("Failed to open temporary file");
            fclose(file);
            return;
        }
        Book tempBook;
        rewind(file); // Reset file pointer to the beginning
        while (fread(&tempBook, sizeof(Book), 1, file) == 1)
        {
            if (tempBook.bookID != bookID)
            {
                fwrite(&tempBook, sizeof(Book), 1, tempFile);
            }
        }
        fclose(file);
        fclose(tempFile);
        remove(BOOKS_FILE);
        rename("data/temp_books.dat", BOOKS_FILE);
        puts("✅ Book deleted successfully!");
        break;
    case 7:
        puts("Cancelled. Returning to the books menu...");
        fclose(file);
        system("pause");
        booksMenu();
        return;
    }
    // Write the updated book back to the file
    fseek(file, -(long)sizeof(Book), SEEK_CUR); // Move the file pointer back to the position of the book
    fwrite(&book, sizeof(Book), 1, file);
    fclose(file);
    system("pause");
    booksMenu();
}
