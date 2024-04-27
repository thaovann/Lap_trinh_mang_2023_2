#include <stdio.h>

int main() {
    FILE *file;
    char line[100]; // Assuming each line in the file is no longer than 100 characters

    // Open the file in read mode
    file = fopen("users.txt", "r");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    // Read and display each line until the end of the file
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line); // Print the line
    }

    // Close the file
    fclose(file);

    return 0;
}
