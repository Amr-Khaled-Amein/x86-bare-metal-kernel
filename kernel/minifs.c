#include "minifs.h"
#include "vga.h"

#define MAX_FILES 10
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_CONTENT_LENGTH 256

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    char content[MAX_FILE_CONTENT_LENGTH];
    int used;
    int size;
} minifs_file_t;

static minifs_file_t files[MAX_FILES];

int fs_string_equals(const char *a, const char *b) {
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}

void fs_copy_string(char *destination, const char *source, int max_size) {
    int i = 0;

    while (source[i] != '\0' && i < max_size - 1) {
        destination[i] = source[i];
        i++;
    }

    destination[i] = '\0';
}

void fs_clear_string(char *text, int size) {
    for (int i = 0; i < size; i++) {
        text[i] = '\0';
    }
}

void fs_print_number(int value) {
    char buffer[16];
    int i = 0;

    if (value == 0) {
        print_char('0');
        return;
    }

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

int fs_find_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && fs_string_equals(files[i].name, name)) {
            return i;
        }
    }

    return -1;
}

int fs_find_free_slot() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            return i;
        }
    }

    return -1;
}

void minifs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
        files[i].size = 0;
        fs_clear_string(files[i].name, MAX_FILENAME_LENGTH);
        fs_clear_string(files[i].content, MAX_FILE_CONTENT_LENGTH);
    }
}

void fs_list_files() {
    int found = 0;

    print("MiniFS files:\n");

    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print("- ");
            print(files[i].name);
            print(" (");
            fs_print_number(files[i].size);
            print(" bytes)\n");
            found = 1;
        }
    }

    if (!found) {
        print("No files found.\n");
    }
}

void fs_create_file(const char *name) {
    if (name[0] == '\0') {
        print("Usage: touch filename\n");
        return;
    }

    if (fs_find_file(name) != -1) {
        print("File already exists: ");
        print(name);
        print("\n");
        return;
    }

    int slot = fs_find_free_slot();

    if (slot == -1) {
        print("MiniFS error: maximum number of files reached.\n");
        return;
    }

    files[slot].used = 1;
    files[slot].size = 0;

    fs_copy_string(files[slot].name, name, MAX_FILENAME_LENGTH);
    fs_clear_string(files[slot].content, MAX_FILE_CONTENT_LENGTH);

    print("Created file: ");
    print(name);
    print("\n");
}

void fs_write_file(const char *name, const char *content) {
    if (name[0] == '\0' || content[0] == '\0') {
        print("Usage: write filename text\n");
        return;
    }

    int index = fs_find_file(name);

    if (index == -1) {
        print("File not found: ");
        print(name);
        print("\n");
        return;
    }

    fs_copy_string(files[index].content, content, MAX_FILE_CONTENT_LENGTH);

    int length = 0;
    while (files[index].content[length] != '\0') {
        length++;
    }

    files[index].size = length;

    print("Written to file: ");
    print(name);
    print("\n");
}

void fs_read_file(const char *name) {
    if (name[0] == '\0') {
        print("Usage: cat filename\n");
        return;
    }

    int index = fs_find_file(name);

    if (index == -1) {
        print("File not found: ");
        print(name);
        print("\n");
        return;
    }

    print("File: ");
    print(files[index].name);
    print("\n");

    if (files[index].size == 0) {
        print("(empty file)\n");
    } else {
        print(files[index].content);
        print("\n");
    }
}

void fs_delete_file(const char *name) {
    if (name[0] == '\0') {
        print("Usage: rm filename\n");
        return;
    }

    int index = fs_find_file(name);

    if (index == -1) {
        print("File not found: ");
        print(name);
        print("\n");
        return;
    }

    files[index].used = 0;
    files[index].size = 0;
    fs_clear_string(files[index].name, MAX_FILENAME_LENGTH);
    fs_clear_string(files[index].content, MAX_FILE_CONTENT_LENGTH);

    print("Deleted file: ");
    print(name);
    print("\n");
}

void fs_info() {
    int used_files = 0;
    int used_bytes = 0;

    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            used_files++;
            used_bytes += files[i].size;
        }
    }

    print("MiniFS information:\n");
    print("Type: in-memory file system\n");
    print("Maximum files: ");
    fs_print_number(MAX_FILES);
    print("\n");

    print("Used files: ");
    fs_print_number(used_files);
    print("\n");

    print("Free file slots: ");
    fs_print_number(MAX_FILES - used_files);
    print("\n");

    print("Maximum file size: ");
    fs_print_number(MAX_FILE_CONTENT_LENGTH);
    print(" bytes\n");

    print("Used content bytes: ");
    fs_print_number(used_bytes);
    print(" bytes\n");
}