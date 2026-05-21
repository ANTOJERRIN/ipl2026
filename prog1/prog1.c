#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a sample structure
struct Student {
    int id;
    char name[50];
    float gpa;
};

// Function prototypes
void storeRecords(const char *filename, int n);
void displayMthRecord(const char *filename, int m);
void deleteRecord(const char *filename, int recordIndex);
void displayAllRecords(const char *filename);

int main(void) {
    const char *filename = "students.bin";
    int n = 4;

    printf("--- Storing %d Records ---\n", n);
    storeRecords(filename, n);

    printf("\n--- Current File Content ---\n");
    displayAllRecords(filename);

    int m = 2;
    printf("\n--- Fetching Record at Index %d via fseek ---\n", m);
    displayMthRecord(filename, m);

    int delIdx = 1;
    printf("\n--- Deleting Record at Index %d ---\n", delIdx);
    deleteRecord(filename, delIdx);

    printf("\n--- File Content After Deletion ---\n");
    displayAllRecords(filename);

    return 0;
}

// Function to store N records into a binary file
void storeRecords(const char *filename, int n) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    struct Student data[] = {
        {101, "Alice", 3.8f},
        {102, "Bob", 3.5f},
        {103, "Charlie", 3.9f},
        {104, "David", 3.2f}
    };
    const int availableRecords = sizeof(data) / sizeof(data[0]);
    int writeCount = n;

    if (writeCount < 0) {
        writeCount = 0;
    } else if (writeCount > availableRecords) {
        writeCount = availableRecords;
        printf("Warning: requested %d records, but only %d sample records are available. Writing %d records.\n",
               n, availableRecords, writeCount);
    }

    if (writeCount > 0) {
        if (fwrite(data, sizeof(struct Student), writeCount, file) != (size_t)writeCount) {
            perror("Error writing student records");
        } else {
            printf("Successfully stored %d records.\n", writeCount);
        }
    } else {
        printf("No records were written to file.\n");
    }

    fclose(file);
}

// Function to fetch and display the m-th record using fseek
void displayMthRecord(const char *filename, int m) {
    if (m < 0) {
        printf("Error: invalid record index %d. Index must be non-negative.\n", m);
        return;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    struct Student student;
    long offset = (long)m * (long)sizeof(struct Student);
    if (fseek(file, offset, SEEK_SET) != 0) {
        printf("Error: seek failed for index %d.\n", m);
        fclose(file);
        return;
    }

    if (fread(&student, sizeof(struct Student), 1, file) == 1) {
        printf("Record %d -> ID: %d, Name: %s, GPA: %.2f\n", m, student.id, student.name, student.gpa);
    } else {
        printf("Record at index %d does not exist.\n", m);
    }

    fclose(file);
}

// Function to delete a record using the Temp File method
void deleteRecord(const char *filename, int recordIndex) {
    if (recordIndex < 0) {
        printf("Error: invalid record index %d. Index must be non-negative.\n", recordIndex);
        return;
    }

    const char *tempFilename = "temp.bin";
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening source file for deletion");
        return;
    }

    FILE *tempFile = fopen(tempFilename, "wb");
    if (tempFile == NULL) {
        perror("Error opening temporary file for deletion");
        fclose(file);
        return;
    }

    struct Student student;
    int currentIndex = 0;
    int deleted = 0;

    // Read record by record from original file
    while (fread(&student, sizeof(struct Student), 1, file) == 1) {
        if (currentIndex != recordIndex) {
            if (fwrite(&student, sizeof(struct Student), 1, tempFile) != 1) {
                perror("Error writing to temporary file");
                break;
            }
        } else {
            deleted = 1;
        }
        currentIndex++;
    }

    fclose(file);
    fclose(tempFile);

    if (deleted) {
        if (remove(filename) != 0) {
            perror("Error removing original file");
            remove(tempFilename);
            return;
        }
        if (rename(tempFilename, filename) != 0) {
            perror("Error renaming temporary file");
            return;
        }
        printf("Record %d successfully deleted.\n", recordIndex);
    } else {
        remove(tempFilename);
        printf("Record index %d not found. No changes made.\n", recordIndex);
    }
}

// Helper function to print the entire file
void displayAllRecords(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file for display");
        return;
    }

    struct Student student;
    int idx = 0;
    while (fread(&student, sizeof(struct Student), 1, file) == 1) {
        printf("Index %d | ID: %d | Name: %s | GPA: %.2f\n", idx++, student.id, student.name, student.gpa);
    }
    fclose(file);
}