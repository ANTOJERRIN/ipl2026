#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME 50
#define NUM_RECORDS 4

// Define our structure
typedef struct {
    int id;
    char name[MAX_NAME];
    float gpa;
} Student;

// Function Prototypes
void saveRecordsASCII(const char *filename, Student students[], int n);
long* createSeekMap(const char *filename, int n);
void displayRecordAtPosition(const char *filename, long seekPosition);

int main() {
    const char *filename = "students.txt";

    // 1. Create sample data (Notice names have different lengths)
    Student students[NUM_RECORDS] = {
        {101, "Alice Smith", 3.82},
        {102, "Bob", 3.45},
        {103, "Alexander Wright", 3.91},
        {104, "Li", 2.98}
    };

    // 2. Save data to file in ASCII format
    printf("Saving records to %s...\n", filename);
    saveRecordsASCII(filename, students, NUM_RECORDS);

    // 3. Generate the seek positions map
    printf("Building the seek positions index map...\n\n");
    long *seekMap = createSeekMap(filename, NUM_RECORDS);

    if (seekMap == NULL) {
        printf("Error creating seek map.\n");
        return 1;
    }

    // Print out the map so we can see the byte offsets
    for(int i = 0; i < NUM_RECORDS; i++) {
        printf("Record %d starts at byte position: %ld\n", i, seekMap[i]);
    }
    printf("\n");

    // 4. Test displaying a specific record using our map
    // Let's look up Record #2 (Alexander Wright) directly
    int targetRecord = 2; 
    printf("--- Direct Lookup of Record %d ---\n", targetRecord);
    displayRecordAtPosition(filename, seekMap[targetRecord]);

    // Free the dynamically allocated map
    free(seekMap);
    return 0;
}

/**
 * Stores an array of structures in ASCII format.
 * Each record is written on a new line.
 */
void saveRecordsASCII(const char *filename, Student students[], int n) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < n; i++) {
        // Writing as plain readable text (ASCII)
        fprintf(file, "%d,%s,%.2f\n", students[i].id, students[i].name, students[i].gpa);
    }

    fclose(file);
}

/**
 * Reads the ASCII file and builds an array containing the starting 
 * byte position (seek position) of each record.
 */
long* createSeekMap(const char *filename, int n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading map");
        return NULL;
    }

    // Allocate memory for the array of seek positions
    long *map = (long *)malloc(n * sizeof(long));
    if (!map) return NULL;

    char buffer[256];
    int recordCounter = 0;

    // The first record always starts at the very beginning (byte 0)
    map[recordCounter] = ftell(file); 

    // Read line by line. fgets reads until it hits a newline character.
    while (fgets(buffer, sizeof(buffer), file) != NULL && recordCounter < n - 1) {
        recordCounter++;
        // The NEXT record starts exactly where we are standing right now
        map[recordCounter] = ftell(file);
    }

    fclose(file);
    return map;
}

/**
 * Jumps directly to a given byte offset and prints the record found there.
 */
void displayRecordAtPosition(const char *filename, long seekPosition) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for random access");
        return;
    }

    // Jump directly to the saved position
    if (fseek(file, seekPosition, SEEK_SET) != 0) {
        perror("Error seeking to position");
        fclose(file);
        return;
    }

    // Read the line at this position
    int id;
    char name[MAX_NAME];
    float gpa;

    // fscanf parses the comma-separated ASCII format back into variables
    if (fscanf(file, "%d,%[^,],%f", &id, name, &gpa) == 3) {
        printf("ID:   %d\n", id);
        printf("Name: %s\n", name);
        printf("GPA:  %.2f\n", gpa);
    } else {
        printf("Error: Could not parse record at position %ld\n", seekPosition);
    }

    fclose(file);
}