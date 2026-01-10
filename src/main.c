#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.c"
#include "file.c"
#include "parse.c"

void print_usage(char *argv[]) {
    printf("Usage: -n -f <database file path>\n");
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");

    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    bool newfile = false;

    int db_fd = -1;
    struct dbheader_t* db_header = NULL;

    int c;
    while ((c = getopt(argc, argv, "nf:")) != -1) {
        switch (c) {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case '?':
            print_usage(argv);
            return 0;
        default:
            printf("Unknown option - %c", c);
            return -1;
        }
    }

    if (!filepath) {
        printf("File path is required.\n");
        print_usage(argv);

        return 0;
    }

    if (newfile) {
        db_fd = create_db_file(filepath);
        if (db_fd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }

        if (create_db_header(db_fd, &db_header) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            return -1;
        }

        printf("Successfully created the db\n");
    } else {
        db_fd = open_db_file(filepath);
        if (db_fd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }

        if (validate_db_header(db_fd, &db_header) == STATUS_ERROR) {
            printf("Failed to validate database header");
            return -1;
        }

        printf("Successfully opened the db\n");
    }

    output_file(db_fd, db_header);

    free(db_header);

    return 0;
}
