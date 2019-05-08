#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings

char *globalcontents;

// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    // TODO: FILL THIS IN
    char * password;
    char * hash;
};

// returns the length of a file
int file_length(char *filename)
{
    struct stat info;
    int res = stat(filename, &info);
    if (res == -1) return -1;
    else return info.st_size;
}

int cbcomp(const void *target, const void *elem)
{
    char *target_str = (char *)target;
    struct entry *celem = (struct entry *)elem;
    return strcmp(target_str, (*celem).hash);
}

//what qsort needs to sort the array
int cccomp(const void *a, const void *b)
{
    struct entry *ca = (struct entry *)a;
    struct entry *cb = (struct entry *)b;
    return strcmp(ca->hash, (*cb).hash);
}

// TODO
// Read in the dictionary file and return an array of structs.
// Each entry should contain both the hash and the dictionary
// word.
struct entry *read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    FILE *c = fopen(filename, "r");
    if (!c)
    {
        printf("Can't open %s for reading\n", filename);
        exit(1);
    }
    printf("FILE LENGTH: %d\n", filelength);
    
    char *contents = malloc(filelength);
    globalcontents = contents;
    fread(contents, 1, filelength, c);
    fclose(c);
    
    // Loop through contents, replace \n with \0
    int lines = 0;
    for (int i = 0; i < filelength; i++)
    {
        if (contents[i] == '\n') {
            contents[i] = '\0';
            lines++;
        }
    }
    printf("lines %d\n", lines);
    printf("contents: %s", contents);
    // Allocate array of structs
    struct entry *cc = malloc(lines * sizeof(struct entry));
    printf("are we getting there okay\n");
    printf("cc thing %s\n", cc->password);
    // Copy the first name into the cc array
    cc->password = contents;
    printf("cc thing %s\n", cc->password);
    printf("are we getting there now\n");
    cc[0].hash = md5(cc[0].password, strlen(cc[0].password));
    
    printf("are we getting there okay\n");
    printf("contents element 6: %d\n", contents[6]);
    int count = 1;
    for (int i = 0; i < filelength-1; i++)
    {
        if (contents[i] == '\0')
        {
            char *nextcc = &contents[i] + 1;

            cc[count].password = nextcc;
            cc[count].hash = md5(cc[count].password, strlen(cc[count].password));
            count++;
        }
    }
    printf("are we getting there okay??\n");
    *size = lines;
    printf("cc = %p\n", cc);
    return cc;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // TODO: Read the dictionary file into an array of entry structures
    int numlines;
    struct entry *dict = read_dictionary(argv[2], &numlines);
    
    // TODO: Sort the hashed dictionary using qsort.
    // You will need to provide a comparison function.
    qsort(dict, numlines, sizeof(struct entry), cccomp);

    // TODO
    // Open the hash file for reading.
    FILE *hashes = fopen(argv[1], "r");
    
    // TODO
    // For each hash, search for it in the dictionary using
    // binary search.
    // If you find it, get the corresponding plaintext dictionary word.
    // Print out both the hash and word.
    // Need only one loop. (Yay!)
    //bsearch(name, chars, clen, sizeof(struct cartoon), cbcomp);
    char hashchar[33];
    while (fgets(hashchar, 33, hashes) != NULL)
    {
        struct entry *found = bsearch(hashchar, dict, numlines, sizeof(struct entry), cbcomp);
        if (found != NULL)
        {
            printf("%s %s\n", found->password, found->hash);
        }
    }
    printf("dict = %p\n", dict);
    for (int i = 0; i < numlines; i++)
    {
        free(dict[i].hash);
    }
    fclose(hashes);
    free(globalcontents);
    free(dict);
}
