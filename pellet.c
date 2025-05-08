#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 20

typedef struct {
    FILE *file_ptr;
    char filename[MAX_FILENAME_LENGTH];
} FileInfo;

long findFileSize(FILE *file);
FileInfo* fileArrayINIT(int num_files, char* filename);
int writeToFile(FileInfo *file_array, int index, const char *content);
void closeAndFree(FileInfo *file_array, int num_files);

int main(int argc, char *argv[]) 
{
    FILE *file;
    long file_size;

    // Check if the filename is provided as a command-line argument.
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <filename> <nodecount>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    int nodecount = atoi(argv[2]);

    // Open the file in read binary mode.
    file = fopen(filename, "rb");
    if (file == NULL) 
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        perror("fopen");
        return 1;
    }
    file_size = findFileSize(file);
    long pellet_size = file_size / nodecount;
    int finalPelletOffset = file_size % nodecount;
    // Print the file size.
    printf("File size of %s: %ld bytes\n", filename, file_size);
    printf("Individual pellet sizes: %ld bytes. Final pellet offset: %d bytes.\n", pellet_size, finalPelletOffset);
    // Close the file.
    fclose(file);

    FileInfo *fileArray = NULL;
    fileArray = fileArrayINIT(nodecount, filename);

    if (fileArray != NULL)
    {
        printf("Writing to Pellets...\n");
        for(int fileNum = 0; fileNum < nodecount; fileNum++)
        {
            writeToFile(fileArray, fileNum+1, "Test!");
        }
    } else {
        fprintf(stderr, "fileArray Empty.");
        return 1;
    }

    closeAndFree(fileArray, nodecount);
    return 0;
}

long findFileSize(FILE *file)
{
    // Seek to the end of the file.
    if (fseek(file, 0, SEEK_END) != 0) 
    {
        fprintf(stderr, "Error: fseek failed.\n");
        perror("fseek");
        fclose(file);
        return 1;
    }

    // Get the current file pointer position, which is the file size.
    long file_size = ftell(file);
    if (file_size == -1) 
    {
        fprintf(stderr, "Error: ftell failed.\n");
        perror("ftell");
        fclose(file);
        return 1;
    }

    return file_size;
}

FileInfo* fileArrayINIT(int num_files, char* filename)
{
    if (num_files <= 0) 
    {
        fprintf(stderr, "Error: Number of files must be positive.\n");
        return NULL;
    }

    // Allocate memory for the array of FileInfo structures
    FileInfo *file_array = (FileInfo *)malloc(sizeof(FileInfo) * num_files);
    if (file_array == NULL) 
    {
        perror("Error allocating memory for file array");
        return NULL;
    }

    for (int i = 0; i < num_files; i++) 
    {
        // Create a filename based on the loop index
        snprintf(file_array[i].filename, MAX_FILENAME_LENGTH, "%s_%d.txt",filename, i + 1);

        // Open the file in write mode ("w")
        file_array[i].file_ptr = fopen(file_array[i].filename, "w");

        // Check if the file was opened successfully
        if (file_array[i].file_ptr == NULL) 
        {
            fprintf(stderr, "Error: Could not create file %s\n", file_array[i].filename);
            // Clean up any files that were created and opened before the error
            for (int j = 0; j < i; j++) {
                fclose(file_array[j].file_ptr);
                remove(file_array[j].filename);
            }
            free(file_array);
            return NULL; // Indicate an error during file creation
        }

        printf("Created and opened file: %s\n", file_array[i].filename);
    }

    printf("Successfully created and opened %d files.\n", num_files);
    return file_array; // Return the pointer to the array of file information
}
int writeToFile(FileInfo *file_array, int index, const char *content) 
{
    if (file_array == NULL) 
    {
        fprintf(stderr, "Error: File array is NULL.\n");
        return -1;
    }
    if (index < 0) 
    {
        fprintf(stderr, "Error: Index cannot be negative.\n");
        return -1;
    }
    // Calculate the actual index within the allocated array
    // Assuming the files are indexed from 1 in their names
    int actual_index = index - 1;
    // Check if the index is within the bounds of the array
    // Assuming the files are named file_1.txt to file_n.txt
    if (actual_index >= 0) 
    {
        fprintf(file_array[actual_index].file_ptr, "%s\n", content);
        return 0; // Success
    } else {
        fprintf(stderr, "Error: Invalid file index: %d\n", index);
        return -1; // Error: Invalid index
    }
}
void closeAndFree(FileInfo *file_array, int num_files)
{
    if (file_array != NULL) 
    {
        for (int i = 0; i < num_files; i++) 
        {
            if (file_array[i].file_ptr != NULL) 
            {
                fclose(file_array[i].file_ptr);
                printf("Closed file: %s\n", file_array[i].filename);
            }
        }
        free(file_array);
        printf("Memory for file array freed.\n");
    }
}