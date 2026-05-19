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
void displayAllRecords(const char *filename); // Helper to see results

int main() {
    const char *filename = "students.bin";
    
    // 1. Store N records
    int n = 4;
    printf("--- Storing %d Records ---\n", n);
    storeRecords(filename, n);
    
    printf("\n--- Current File Content ---\n");
    displayAllRecords(filename);

    // 2. Display the m-th record (0-indexed)
    int m = 2; 
    printf("\n--- Fetching Record at Index %d via fseek ---\n", m);
    displayMthRecord(filename, m);

    // 3. Delete a record (Let's delete index 1)
    int delIdx = 1;
    printf("\n--- Deleting Record at Index %d ---\n", delIdx);
    deleteRecord(filename, delIdx);

    // 4. Display file content again to verify
    printf("\n--- File Content After Deletion ---\n");
    displayAllRecords(filename);

    return 0;
}

// Function to store N records into a binary file
void storeRecords(const char *filename, int n) {
    FILE *file = fopen(filename, "wb"); // Write Binary
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Hardcoded sample data for demonstration
    struct Student data[] = {
        {101, "Alice", 3.8},
        {102, "Bob", 3.5},
        {103, "Charlie", 3.9},
        {104, "David", 3.2}
    };

    // Write the array of structures to the file in one go
    fwrite(data, sizeof(struct Student), n, file);
    
    fclose(file);
    printf("Successfully stored %d records.\n", n);
}

// Function to fetch and display the m-th record using fseek
void displayMthRecord(const char *filename, int m) {
    FILE *file = fopen(filename, "rb"); // Read Binary
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    struct Student student;
    
    // Move file pointer to the m-th record position
    // Offset = m * size of one structure
    int seekStatus = fseek(file, m * sizeof(struct Student), SEEK_SET);
    
    if (seekStatus != 0) {
        printf("Error: Record index out of bounds or seek failed.\n");
        fclose(file);
        return;
    }

    // Read exactly one structure from that position
    if (fread(&student, sizeof(struct Student), 1, file) == 1) {
        printf("Record %d -> ID: %d, Name: %s, GPA: %.2f\n", m, student.id, student.name, student.gpa);
    } else {
        printf("Record at index %d does not exist.\n", m);
    }

    fclose(file);
}

// Function to delete a record using the Temp File method
void deleteRecord(const char *filename, int recordIndex) {
    FILE *file = fopen(filename, "rb");
    FILE *tempFile = fopen("temp.bin", "wb");

    if (file == NULL || tempFile == NULL) {
        perror("Error opening files during deletion");
        return;
    }

    struct Student student;
    int currentIndex = 0;
    int deleted = 0;

    // Read record by record from original file
    while (fread(&student, sizeof(struct Student), 1, file) == 1) {
        // If it's NOT the record we want to delete, copy it to temp file
        if (currentIndex != recordIndex) {
            fwrite(&student, sizeof(struct Student), 1, tempFile);
        } else {
            deleted = 1; // Skip writing this record
        }
        currentIndex++;
    }

    fclose(file);
    fclose(tempFile);

    if (deleted) {
        remove(filename);             // Delete original file
        rename("temp.bin", filename); // Rename temp file to original name
        printf("Record %d successfully deleted.\n", recordIndex);
    } else {
        remove("temp.bin"); // Clean up temp file if index wasn't found
        printf("Record index %d not found. No changes made.\n", recordIndex);
    }
}

// Helper function to print the entire file
void displayAllRecords(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return;

    struct Student student;
    int idx = 0;
    while (fread(&student, sizeof(struct Student), 1, file) == 1) {
        printf("Index %d | ID: %d | Name: %s | GPA: %.2f\n", idx++, student.id, student.name, student.gpa);
    }
    fclose(file);
}