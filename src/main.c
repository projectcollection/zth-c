#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// #include "constants.c"
// #include "file.c"
// #include "parse.c"

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

#include "file.c"
#include "parse.c"

void print_usage() {
    printf("Usage: -n -f <database file path>\n");
    printf("\t -n - create new database file\n");
    printf("\t -f - (required) path to database file\n");
    printf("\t -a - add via CSV list of (name, address, salary)\n");

    return;
}

int main(int argc, char* argv[]) {
    char* filepath = NULL;
    bool newfile = false;
    bool list = false;
    char* add_string = NULL;

    int db_fd = -1;
    struct dbheader_t* db_header = NULL;
    struct employee_t* employees = NULL;

    int c;
    while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
        switch (c) {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            add_string = optarg;
            break;

        case 'l':
            list = true;
            break;
        case '?':
            print_usage();
            return 0;
        default:
            printf("Unknown option - %c", c);
            return -1;
        }
    }

    if (!filepath) {
        printf("File path is required.\n");
        print_usage();

        return 0;
    }

    if (newfile) {
        db_fd = create_db_file(filepath);
        if (db_fd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }

        if (create_db_header(&db_header) == STATUS_ERROR) {
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
    }

    if (read_employees(db_fd, db_header, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees");
        return 0;
    }

    if (add_string) {
        add_employee(db_header, &employees, add_string);
    }

    if (list) {
        list_employees(db_header, employees);
    }

    if(newfile || add_string) {
        output_file(db_fd, db_header, employees);
    }

    free(db_header);
    // free(employees);

    return 0;
}
