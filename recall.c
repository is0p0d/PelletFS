#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 20
#define BUFFER_SIZE 4196

int main(int argc, char *argv[]) 
{

    FILE *file;
    long file_size;
    char *filename = argv[1];
    char pelletname[MAX_FILENAME_LENGTH] = "\0";
    char numBuffer[8] = "\0";
    int currentPart = 0;
    int totalParts = 0;
    char filebuffer[BUFFER_SIZE] = "\0";

    // Open the file in read binary mode.
    file = fopen(filename, "rb");

    if (file == NULL) 
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        perror("fopen");
        return 1;
    }
    //get first line, filename
    fgets(pelletname, sizeof(pelletname), file);

    //second line
    fgets(numBuffer, sizeof(numBuffer), file);
    totalParts = atoi(numBuffer);

    //third line
    fgets(numBuffer, sizeof(numBuffer), file);
    currentPart = atoi(numBuffer);

    printf("Current pellet name is: %s, part %d of %d\n", pelletname, currentPart, totalParts);

    for(int part = 0; part < totalParts-1; part++)
    {
        char command[1024];
        char currentFilePath[32];

        snprintf(currentFilePath, sizeof(currentFilePath), "%s_%d.pel", filename, part+1);

        printf("Current file path: %s\n",currentFilePath);
        printf("Being brought from - %s@%s:%s \n",
            argv[2], //username
            argv[4+part], //host
            argv[3]); //destination

        snprintf(command, sizeof(command),
             "scp %s@%s:%s %s",
             argv[2], //username
             argv[4+part], //host
             argv[3],
             currentFilePath); //sourcefile); //destination
        
        int result = system(command);
        if (result == -1) {
            fprintf(stderr, "SCP for file %s failed", currentFilePath);
        }
    }


    fclose(file);
}