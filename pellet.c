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
int writeToFile(FileInfo *file_array, int index, char *content);
void closeAndFree(FileInfo *file_array, int num_files);

int main(int argc, char *argv[]) 
{
    printf("Usage: filename, nodecount, username, path, hosts1,..,hostsn\n");
    FILE *file;
    long file_size;
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
    if (fseek(file, 0, SEEK_SET) != 0) { //rewind file
        fprintf(stderr, "Error: Could not seek to the beginning of the input file.\n");
        fclose(file);
        return 1;
    }

    FileInfo *fileArray = NULL;
    fileArray = fileArrayINIT(nodecount, filename);

    char* buffer = (char*)malloc(pellet_size+finalPelletOffset);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for the buffer.\n");
        return 1;
    }

    if (fileArray != NULL)
    {
        long readSize = 0,
             bytesRead = 0;

        printf("Writing to Pellets...\n");
        for(int fileNum = 0; fileNum < nodecount; fileNum++)
        {
            char partBuffer[16];
            sprintf(partBuffer, "filepart_%d", fileNum);
            writeToFile(fileArray, fileNum+1, partBuffer);

            if (fileNum != nodecount+1)
            {
                readSize = pellet_size;
            } else {
                readSize = pellet_size+finalPelletOffset;
            }

            bytesRead = fread(buffer, 1, readSize, file);
            writeToFile(fileArray, fileNum+1, buffer);
        }
    } else {
        fprintf(stderr, "fileArray Empty.");
        return 1;
    }

    fclose(file);
    closeAndFree(fileArray, nodecount);

    for(int cmdNum = 0; cmdNum < nodecount; cmdNum++)
    {
        char command[1024];
        char currentFilePath[32];

        snprintf(currentFilePath, sizeof(currentFilePath), "%s_%d.pel", filename, cmdNum);

        printf("Current file path: %s\n",currentFilePath);
        printf("Being sent to- %s@%s:%s \n",
            argv[3], //username
            argv[5+cmdNum], //host
            argv[4]); //destination

        snprintf(command, sizeof(command),
             "scp %s %s@%s:%s",
             currentFilePath, //sourcefile
             argv[3], //username
             argv[5+cmdNum], //host
             argv[4]); //destination
        
            int result = system(command);
            if (result == -1) {
                fprintf(stderr, "SCP for file %s failed", currentFilePath);
            }
    }


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
        snprintf(file_array[i].filename, MAX_FILENAME_LENGTH, "%s_%d.pel",filename, i);

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

int writeToFile(FileInfo *file_array, int index, char *content) 
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