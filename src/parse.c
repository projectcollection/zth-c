#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "constants.c"

#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

#endif

struct dbheader_t {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct employee_t {
    char name[256];
    char address[256];
    unsigned int hours;
};

// void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees)
// {}

// int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
//                  char *addstring) {}

// int read_employees(int fd, struct dbheader_t *dbhdr,
//                    struct employee_t **employeesOut) {}

int output_file(int fd, struct dbheader_t* db_header) {
// ,
//                 struct employee_t *employees) {
    if (fd < 0) {
        printf("Got a bad FD from the user\n");
        return STATUS_ERROR;
    }

    db_header->magic = htonl(db_header->magic);
    db_header->filesize = htonl(db_header->filesize);
    db_header->count = htons(db_header->count);
    db_header->version = htons(db_header->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, db_header, sizeof(struct dbheader_t));

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) {
        printf("Got a bad fd from the user\n");
        return STATUS_ERROR;
    }

    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

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

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

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
