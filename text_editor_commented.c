// stdio  = printf, scanf, fgets, fopen, fclose, fprintf  (all input output stuff)
// stdlib = malloc, free, exit  (memory allocation wala)
// string = strcpy, strcspn  (string copy and find character position)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// max characters one line can hold.. like a limit
#define MAX_LINE 1024

// this is our NODE structure.. each node = one line of text
// text[] holds the actual line content
// *next is a pointer that points to the next node in the chain
struct Line {
    char text[MAX_LINE];
    struct Line *next;
};

// head = pointer to the FIRST node.. if NULL means list is empty
// total_lines = just a counter, we ++ on insert and -- on delete
struct Line *head = NULL;
int total_lines = 0;

// -- this function makes a new node --
// malloc asks OS for memory (sizeof struct Line = enough bytes for 1 node)
// if malloc returns NULL it means system has no memory left so we exit
// strcpy copies the text into the node
// next = NULL bcz this node isnt connected to anything yet
// returns the pointer so whoever called this can use it
struct Line *create_line(char *text) {
    struct Line *new_line = (struct Line *)malloc(sizeof(struct Line));
    if (new_line == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(new_line->text, text);
    new_line->next = NULL;
    return new_line;
}

// -- adds a new line at the END of the list --
// first creates the node using create_line()
// if head is NULL = list is empty so new node becomes the head
// otherwise we WALK from head to the last node (the one whose next is NULL)
// then we attach our new node at the end
// total_lines++ to update the count
void insert_at_end(char *text) {
    struct Line *new_line = create_line(text);
    if (head == NULL) {
        head = new_line;
    } else {
        struct Line *current = head;
        while (current->next != NULL) {  // keep walking till last node
            current = current->next;
        }
        current->next = new_line;  // attach at end
    }
    total_lines++;
    printf("Line added at position %d.\n", total_lines);
}

// -- inserts a line at a SPECIFIC position (1-indexed) --
// first check if position is valid (between 1 and total+1)
// if position is 1 = new node becomes head, its next points to old head
// otherwise walk to the node JUST BEFORE the target position
//   then do the pointer rewiring trick:
//   new_node->next = what was after current
//   current->next = new_node
//   basically squeeze the new node in between
void insert_at_position(int position, char *text) {
    if (position < 1 || position > total_lines + 1) {
        printf("Invalid position! Valid range: 1 to %d\n", total_lines + 1);
        return;
    }
    struct Line *new_line = create_line(text);
    if (position == 1) {
        new_line->next = head;  // new node points to old head
        head = new_line;        // new node IS the head now
    } else {
        struct Line *current = head;
        for (int i = 1; i < position - 1; i++) {  // walk to node BEFORE target
            current = current->next;
        }
        new_line->next = current->next;  // new node grabs whats after current
        current->next = new_line;        // current now points to new node
    }
    total_lines++;
    printf("Line inserted at position %d.\n", position);
}

// -- deletes a line at given position --
// first check if list is empty or position is out of range
// if deleting position 1 = save head in to_delete, move head forward
// otherwise walk to the node BEFORE the target
//   save the target in to_delete
//   make previous node skip over target (current->next = to_delete->next)
// then free(to_delete) = give that memory back to OS
//   THIS is the matching free() for the malloc() we did in create_line
//   without this = memory leak = valgrind will complain
void delete_line(int position) {
    if (head == NULL) {
        printf("No lines to delete!\n");
        return;
    }
    if (position < 1 || position > total_lines) {
        printf("Invalid position! Valid range: 1 to %d\n", total_lines);
        return;
    }
    struct Line *to_delete;
    if (position == 1) {
        to_delete = head;      // save current head
        head = head->next;     // head moves to 2nd node
    } else {
        struct Line *current = head;
        for (int i = 1; i < position - 1; i++) {  // walk to node BEFORE target
            current = current->next;
        }
        to_delete = current->next;          // this is the one we want gone
        current->next = to_delete->next;    // skip over it
    }
    printf("Deleted line %d: %s\n", position, to_delete->text);
    free(to_delete);   // give memory back!! very important
    total_lines--;
}

// -- just prints all lines with line numbers --
// starts at head, walks through each node printing its text
// current = current->next moves to the next node
// loop stops when current becomes NULL (end of list)
void display_lines() {
    if (head == NULL) {
        printf("No lines in the editor.\n");
        return;
    }
    printf("\n--- Editor Contents ---\n");
    struct Line *current = head;
    int line_number = 1;
    while (current != NULL) {
        printf("%3d | %s\n", line_number, current->text);
        current = current->next;  // move to next node
        line_number++;
    }
    printf("--- Total: %d lines ---\n", total_lines);
}

// -- writes all lines to a file on disk --
// fopen with "w" = open for Writing (creates file if doesnt exist, overwrites if it does)
// if fopen returns NULL = couldnt open the file (maybe permission issue)
// walk the list, fprintf writes each line to the file (like printf but to a file)
// fclose = ALWAYS close the file when done, or data can be lost
void save_to_file(char *filename) {
    FILE *file = fopen(filename, "w");  // open in write mode
    if (file == NULL) {
        printf("Error: Could not open file '%s' for writing.\n", filename);
        return;
    }
    struct Line *current = head;
    while (current != NULL) {
        fprintf(file, "%s\n", current->text);  // write each line to file
        current = current->next;
    }
    fclose(file);  // close it!!
    printf("Saved %d lines to '%s'.\n", total_lines, filename);
}

// -- frees ALL nodes in the list (memory cleanup) --
// we cant just do free(current) then current->next bcz after free
//   that memory is GONE, we cant access ->next anymore
// so we save current in temp, move current forward, THEN free temp
// at the end head = NULL and total_lines = 0 (list is now empty)
// this is what makes our program valgrind-clean = 0 memory leaks
void free_all_lines() {
    struct Line *current = head;
    while (current != NULL) {
        struct Line *temp = current;   // save it before moving
        current = current->next;       // move to next
        free(temp);                    // NOW free the saved one
    }
    head = NULL;       // list is empty now
    total_lines = 0;   // reset counter
}

// -- reads lines from a file and loads them into our list --
// fopen with "r" = open for Reading
// first call free_all_lines() to clear whatever was in the list before
// fgets reads one line at a time from the file (up to MAX_LINE chars)
// strcspn(buffer, "\n") finds position of the \n character
//   we replace it with \0 (null terminator) to remove the newline
//   bcz fgets includes the \n and we dont want it
// then insert_at_end adds each line to our linked list
// fclose when done
void load_from_file(char *filename) {
    FILE *file = fopen(filename, "r");  // open in read mode
    if (file == NULL) {
        printf("Error: Could not open file '%s' for reading.\n", filename);
        return;
    }
    free_all_lines();  // clear old data first
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file) != NULL) {   // read line by line
        buffer[strcspn(buffer, "\n")] = '\0';  // remove the \n at end
        insert_at_end(buffer);                  // add to our list
    }
    fclose(file);
    printf("Loaded %d lines from '%s'.\n", total_lines, filename);
}

// -- lets user change text of a specific line --
// walk to the node at given position
// show whats currently there
// read new text from user with fgets
// strcspn trick again to remove \n
// strcpy overwrites the old text with new text
// no malloc/free needed here bcz we reuse the same node
void edit_line(int position) {
    if (position < 1 || position > total_lines) {
        printf("Invalid position! Valid range: 1 to %d\n", total_lines);
        return;
    }
    struct Line *current = head;
    for (int i = 1; i < position; i++) {   // walk to the target node
        current = current->next;
    }
    printf("Current text: %s\n", current->text);
    printf("Enter new text: ");
    char buffer[MAX_LINE];
    fgets(buffer, MAX_LINE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';   // remove \n
    strcpy(current->text, buffer);          // overwrite old text
    printf("Line %d updated.\n", position);
}

// -- prints the menu options.. nothing fancy --
void show_menu() {
    printf("\n===== TEXT EDITOR =====\n");
    printf("1. Insert line at end\n");
    printf("2. Insert line at position\n");
    printf("3. Delete a line\n");
    printf("4. Edit a line\n");
    printf("5. Display all lines\n");
    printf("6. Save to file\n");
    printf("7. Load from file\n");
    printf("8. Exit\n");
    printf("Choice: ");
}

// -- MAIN function, this is where everything starts --
// while(1) = infinite loop, keeps running forever until we break out
// show_menu prints the 8 options
// scanf reads the number user typed
// getchar() eats the leftover \n that Enter key leaves behind
//   without this the next fgets would read that \n as empty input
// switch-case sends control to the right function based on choice
// case 8 = free all memory and exit cleanly
// strcspn trick used everywhere to strip \n from fgets input
int main() {
    int choice, position;
    char buffer[MAX_LINE];
    char filename[256];

    printf("Welcome to the Text Editor!\n");

    while (1) {                        // infinite loop
        show_menu();
        scanf("%d", &choice);          // read user's number
        getchar();                     // eat the leftover \n from Enter

        switch (choice) {
            case 1:                    // insert at end
                printf("Enter text: ");
                fgets(buffer, MAX_LINE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';  // strip \n
                insert_at_end(buffer);
                break;

            case 2:                    // insert at position
                printf("Enter position (1 to %d): ", total_lines + 1);
                scanf("%d", &position);
                getchar();             // eat \n again after scanf
                printf("Enter text: ");
                fgets(buffer, MAX_LINE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                insert_at_position(position, buffer);
                break;

            case 3:                    // delete
                display_lines();
                printf("Enter line number to delete: ");
                scanf("%d", &position);
                getchar();
                delete_line(position);
                break;

            case 4:                    // edit
                display_lines();
                printf("Enter line number to edit: ");
                scanf("%d", &position);
                getchar();
                edit_line(position);
                break;

            case 5:                    // display
                display_lines();
                break;

            case 6:                    // save
                printf("Enter filename: ");
                fgets(filename, 256, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                save_to_file(filename);
                break;

            case 7:                    // load
                printf("Enter filename to load: ");
                fgets(filename, 256, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                load_from_file(filename);
                break;

            case 8:                    // EXIT
                free_all_lines();      // clean up ALL memory before leaving
                printf("Goodbye!\n");
                return 0;              // program ends here

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}
