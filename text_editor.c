#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

struct Line {
    char text[MAX_LINE];
    struct Line *next;
};

struct Line *head = NULL;
int total_lines = 0;

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

void insert_at_end(char *text) {
    struct Line *new_line = create_line(text);
    if (head == NULL) {
        head = new_line;
    } else {
        struct Line *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_line;
    }
    total_lines++;
    printf("Line added at position %d.\n", total_lines);
}

void insert_at_position(int position, char *text) {
    if (position < 1 || position > total_lines + 1) {
        printf("Invalid position! Valid range: 1 to %d\n", total_lines + 1);
        return;
    }
    struct Line *new_line = create_line(text);
    if (position == 1) {
        new_line->next = head;
        head = new_line;
    } else {
        struct Line *current = head;
        for (int i = 1; i < position - 1; i++) {
            current = current->next;
        }
        new_line->next = current->next;
        current->next = new_line;
    }
    total_lines++;
    printf("Line inserted at position %d.\n", position);
}

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
        to_delete = head;
        head = head->next;
    } else {
        struct Line *current = head;
        for (int i = 1; i < position - 1; i++) {
            current = current->next;
        }
        to_delete = current->next;
        current->next = to_delete->next;
    }
    printf("Deleted line %d: %s\n", position, to_delete->text);
    free(to_delete);
    total_lines--;
}

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
        current = current->next;
        line_number++;
    }
    printf("--- Total: %d lines ---\n", total_lines);
}

void save_to_file(char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file '%s' for writing.\n", filename);
        return;
    }
    struct Line *current = head;
    while (current != NULL) {
        fprintf(file, "%s\n", current->text);
        current = current->next;
    }
    fclose(file);
    printf("Saved %d lines to '%s'.\n", total_lines, filename);
}

void free_all_lines() {
    struct Line *current = head;
    while (current != NULL) {
        struct Line *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    total_lines = 0;
}

void load_from_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s' for reading.\n", filename);
        return;
    }
    free_all_lines();
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        insert_at_end(buffer);
    }
    fclose(file);
    printf("Loaded %d lines from '%s'.\n", total_lines, filename);
}

void edit_line(int position) {
    if (position < 1 || position > total_lines) {
        printf("Invalid position! Valid range: 1 to %d\n", total_lines);
        return;
    }
    struct Line *current = head;
    for (int i = 1; i < position; i++) {
        current = current->next;
    }
    printf("Current text: %s\n", current->text);
    printf("Enter new text: ");
    char buffer[MAX_LINE];
    fgets(buffer, MAX_LINE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    strcpy(current->text, buffer);
    printf("Line %d updated.\n", position);
}

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

int main() {
    int choice, position;
    char buffer[MAX_LINE];
    char filename[256];

    printf("Welcome to the Text Editor!\n");

    while (1) {
        show_menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter text: ");
                fgets(buffer, MAX_LINE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                insert_at_end(buffer);
                break;

            case 2:
                printf("Enter position (1 to %d): ", total_lines + 1);
                scanf("%d", &position);
                getchar();
                printf("Enter text: ");
                fgets(buffer, MAX_LINE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                insert_at_position(position, buffer);
                break;

            case 3:
                display_lines();
                printf("Enter line number to delete: ");
                scanf("%d", &position);
                getchar();
                delete_line(position);
                break;

            case 4:
                display_lines();
                printf("Enter line number to edit: ");
                scanf("%d", &position);
                getchar();
                edit_line(position);
                break;

            case 5:
                display_lines();
                break;

            case 6:
                printf("Enter filename: ");
                fgets(filename, 256, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                save_to_file(filename);
                break;

            case 7:
                printf("Enter filename to load: ");
                fgets(filename, 256, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                load_from_file(filename);
                break;

            case 8:
                free_all_lines();
                printf("Goodbye!\n");
                return 0;

            default:
                printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}
