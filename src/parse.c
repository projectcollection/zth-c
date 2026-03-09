#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/parse.h"

// #include "constants.c"

// #ifndef PARSE_H
// #define PARSE_H

// #define HEADER_MAGIC 0x4c4c4144

// #endif

// struct dbheader_t {
//     unsigned int magic;
//     unsigned short version;
//     unsigned short count;
//     unsigned int filesize;
// };

// struct employee_t {
//     char name[256];
//     char address[256];
//     unsigned int hours;
// };

int list_employees(struct dbheader_t *db_header, struct employee_t *employees) {
    if (!db_header) {
        printf("Error: db_header is empty");
        return STATUS_ERROR;
    }

    for (int i = 0; i < db_header->count; i++ ) {
        struct employee_t employee = employees[i];
        printf("Employee %d\n", i);
        printf("\tName: %s\n", employee.name);
        printf("\tAddress: %s\n", employee.address);
        printf("\tHours: %d\n", employee.hours);
    }

    return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t* db_header, struct employee_t** employees,
                 char* add_string) {
    if (!db_header || !employees || !*employees || !add_string) {
        return STATUS_ERROR;
    }

    char* name = strtok(add_string, ",");
    if (!name)
        return STATUS_ERROR;

    char* address = strtok(NULL, ",");
    if (!address)
        return STATUS_ERROR;

    char* hours = strtok(NULL, ",");
    if (!hours)
        return STATUS_ERROR;

    struct employee_t* new_employees = *employees;

    new_employees =
        realloc(new_employees, (db_header->count + 1) * (sizeof(struct employee_t)));
    if (!new_employees) {
        return STATUS_ERROR;
    }

    db_header->count++;

    strncpy(new_employees[db_header->count - 1].name, name,
            sizeof(new_employees[db_header->count - 1].name) - 1);
    strncpy(new_employees[db_header->count - 1].address, address,
            sizeof(new_employees[db_header->count - 1].address) - 1);

    new_employees[db_header->count - 1].hours = atoi(hours);

    *employees = new_employees;

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t* db_header,
                   struct employee_t** employees_out) {
    int count = db_header->count;

    struct employee_t* employees = calloc(count, sizeof(struct employee_t));
    if (!employees) {
        printf("Malloc failed\n");
        return STATUS_ERROR;
    }

    read(fd, employees, count * sizeof(struct employee_t));

    int i = 0;
    for (i = 0; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }
    *employees_out = employees;

    return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t* db_header,
                struct employee_t* employees) {
    printf("%p", employees);
    if (fd < 0) {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    int employee_count = db_header->count;

    db_header->magic = htonl(db_header->magic);
    db_header->filesize = htonl(sizeof(struct dbheader_t) +
                                sizeof(struct employee_t) * employee_count);
    db_header->count = htons(db_header->count);
    db_header->version = htons(db_header->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, db_header, sizeof(struct dbheader_t));

    for (int i = 0; i < employee_count; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t** headerOut) {
    if (fd < 0) {
        printf("Got a bad fd from the user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));

    if (!header) {
        printf("Malloc failed to create a db header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) !=
        sizeof(struct dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->magic != HEADER_MAGIC) {
        printf("Improper header magic\n");
        free(header);
        return STATUS_ERROR;
    }

    if (header->version != 1) {
        printf("Improper header version\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("Corrupted database\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;

    return STATUS_SUCCESS;
}

int create_db_header(struct dbheader_t** headerOut) {
    struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));

    if (!header) {
        printf("Failed to allocate header");
        return STATUS_ERROR;
    }

    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;

    return STATUS_SUCCESS;
}
