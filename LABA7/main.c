#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>

#define FILENAME "records.bin"
#define RECORD_SIZE sizeof(struct Record)


struct Record {
    char name[80];
    char address[80];
    uint8_t semester;
};

void printError(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void lockRecord(int fd, int recNo, int type) {
    struct flock fl;
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = recNo * RECORD_SIZE;
    fl.l_len = RECORD_SIZE;
    fl.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        printError("fcntl");
    }
}

void unlockRecord(int fd, int recNo) {
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = recNo * RECORD_SIZE;
    fl.l_len = RECORD_SIZE;
    fl.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        printError("fcntl");
    }
}

void listRecords(int fd) {
    struct Record rec;
    lseek(fd, 0, SEEK_SET);
    printf("List of records:\n");
    for (int i = 0; read(fd, &rec, RECORD_SIZE) == RECORD_SIZE; i++) {
        printf("Record %d: Name: %s, Address: %s, Semester: %d\n", i, rec.name, rec.address, rec.semester);
    }
}

void getRecord(int fd, int recNo, struct Record *rec) {
    lseek(fd, recNo * RECORD_SIZE, SEEK_SET);
    if (read(fd, rec, RECORD_SIZE) != RECORD_SIZE) {
        printError("read");
    }
}

void putRecord(int fd, int recNo, const struct Record *rec) {
    lseek(fd, recNo * RECORD_SIZE, SEEK_SET);
    if (write(fd, rec, RECORD_SIZE) != RECORD_SIZE) {
        printError("write");
    }
}

void modifyRecord(struct Record *rec) {
    printf("Enter new name: ");
    fgets(rec->name, sizeof(rec->name), stdin);
    strtok(rec->name, "\n");
    printf("Enter new address: ");
    fgets(rec->address, sizeof(rec->address), stdin);
    strtok(rec->address, "\n");
    printf("Enter new semester: ");
    scanf("%hhu", &rec->semester);
    getchar();
}

off_t getFileSize(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        printError("fstat");
    }
    return st.st_size;
}

int main() {
    int fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        printError("open");
    }

    off_t fileSize = getFileSize(fd);
    if (fileSize == 0) {
        printf("File is empty. Please initialize the file with records.\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int totalRecords = fileSize / RECORD_SIZE;

    struct Record currentRecord;
    int currentRecNo = -1;
    char command[20];
    int recNo;
    int aborted = 0;

    while (1) {
        printf("Enter command (lst, get RecNo, put, q): ");
        fgets(command, sizeof(command), stdin);

        if (strncmp(command, "lst", 3) == 0) {
            listRecords(fd);
        } else if (sscanf(command, "get %d", &recNo) == 1) {
            if (recNo < 0 || recNo >= totalRecords) {
                printf("Record number out of range. Valid range is 0 to %d.\n", totalRecords - 1);
                continue;
            }

            struct flock fl;
            fl.l_type = F_RDLCK;
            fl.l_whence = SEEK_SET;
            fl.l_start = recNo * RECORD_SIZE;
            fl.l_len = RECORD_SIZE;
            fl.l_pid = getpid();

            if (fcntl(fd, F_GETLK, &fl) == -1) {
                printError("fcntl");
            }

            if (fl.l_type != F_UNLCK) {
                printf("Record %d is currently locked by another process. Try again later.\n", recNo);
                continue;
            }

            lockRecord(fd, recNo, F_RDLCK);
            getRecord(fd, recNo, &currentRecord);
            unlockRecord(fd, recNo);
            currentRecNo = recNo;
            printf("Fetched record %d: Name: %s, Address: %s, Semester: %d\n", recNo,
                   currentRecord.name,
                   currentRecord.address,
                   currentRecord.semester);
        } else if (strncmp(command, "put", 3) == 0) {
            if (currentRecNo == -1) {
                printf("Use GET command first.\n");
                continue;
            }

            struct flock fl;
            fl.l_type = F_WRLCK;
            fl.l_whence = SEEK_SET;
            fl.l_start = currentRecNo * RECORD_SIZE;
            fl.l_len = RECORD_SIZE;
            fl.l_pid = getpid();

            if (fcntl(fd, F_GETLK, &fl) == -1) {
                printError("fcntl");
            }

            if (fl.l_type != F_UNLCK) {
                printf("Record %d is currently locked by another process. Cannot write.\n", currentRecNo);
                aborted = 1;
                continue;
            }

            lockRecord(fd, currentRecNo, F_WRLCK);
            if (aborted) {
     
                printf("Record was previously modified by another process. Refetching...\n");
                getRecord(fd, currentRecNo, &currentRecord);
                printf("Refetched record %d: Name: %s, Address: %s, Semester: %d\n", currentRecNo,
                       currentRecord.name,
                       currentRecord.address,
                       currentRecord.semester);
                aborted = 0;
            }

            modifyRecord(&currentRecord);

            putRecord(fd, currentRecNo, &currentRecord);
            unlockRecord(fd, currentRecNo);
            printf("Record %d has been updated.\n", currentRecNo);
        } else if (strncmp(command, "q", 1) == 0) {
            break;
        } else {
            printf("Unknown command.\n");
        }
    }
    close(fd);
    return 0;
}