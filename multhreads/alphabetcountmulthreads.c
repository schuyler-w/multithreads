/*
 * alphabetcountmulthreads.c - this file implements the alphabetcountmulthreads function.
 */

/*
 * Program 2
 * Name: Schuyler Wang
 * cssc1766@edoras.sdsu.edu
 * July 23, 2023
 */


#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include "count.h"

/**
  The alphabetcountmulthreads function counts the frequency of each alphabet letter (a-z, case insensitive) in all the .txt files under
  directory of the given path and write the results to a file named as filetowrite. Different with programming assignment#0, you need to implement the program using mutithreading.

  Input:
      path - a pointer to a char string [a character array] specifying the path of the directory; and
      filetowrite - a pointer to a char string [a character array] specifying the file where results should be written in.
      alphabetfreq - a pointer to a long array storing the frequency of each alphabet letter from a - z, which should be already up-to-date;
      num_threads - number of the threads running in parallel to process the files


  Output: a new file named as filetowrite with the frequency of each alphabet letter written in

  Requirements:
1)	Multiple threads are expected to run in parallel to share the workload, i.e. suppose 3 threads to process 30 files, then each thread should process 10 files;
2)	When a thread is created, a message should be print out showing which files this thread will process, for example:
Thread id = 274237184 starts processing files with index from 0 to 10!
3)	When a file is being processed, a message should be print out showing which thread (thread_id = xxx) is processing this file, for example:
Thread id = 265844480 is processing file input_11.txt
4)	When a thread is done with its workload, a message should be print out showing which files this thread has done with work, for example:
      Thread id = 274237184 is done !
5)	The array: long alphabetfreq[ ] should always be up-to-date, i.e. it always has the result of all the threads counted so far.  [You may need to use mutexes to protect this critical region.]


You should have the screen printing should be similar as follows:

 Thread id = 274237184 starts processing files with index from 0 to 10!
 Thread id = 265844480 starts processing files with index from 11 to 22!
 Thread id = 257451776 starts processing files with index from 23 to 31!

 Thread id = 265844480 is processing file input_11.txt
 Thread id = 257451776 is processing file input_22.txt
 Thread id = 274237184 is processing file input_00.txt


 Thread id = 274237184 is done !
 Thread id = 265844480 is done !
 Thread id = 257451776 is done !

 The results are counted as follows:
 a -> 2973036
 b -> 556908
 c -> 765864
 ... ...
*/

// Mutex to protect the critical section (updating alphabetfreq)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Define a struct to hold thread information
struct ThreadInfo {
    pthread_t thread_id;
    char *path;
    long alphabetfreq[ALPHABETSIZE]; // Array of 26 elements for each letter
    char filelist[50][30]; // Filelist for each thread
    int num_files;         // Number of files for this thread
};

// Function to process a single file and update local alphabetfreq
void processFile(char *path, char *filename, long local_alphabetfreq[]) {
    char filepath[256];
    sprintf(filepath, "%s/%s", path, filename);

    // Open the file and update local alphabetfreq
    FILE *textfile = fopen(filepath, "r");
    if (textfile == NULL) {
        printf("Cannot open file: %s\n", filepath);
        return;
    }

    char c = fgetc(textfile);
    while (c != EOF) {
        if (isalpha(c)) {
            if (c >= 'A' && c <= 'Z') {
                local_alphabetfreq[c - 'A']++;
            } else {
                local_alphabetfreq[c - 'a']++;
            }
        }
        c = fgetc(textfile);
    }
    fclose(textfile);
}

void *processFiles(void *arg) {
    struct ThreadInfo *info = (struct ThreadInfo *)arg;

    int start_idx;
    int end_idx;

    // find the numbers of the first and last files. Since ordered, this returns first and last indices of files
    sscanf(info->filelist[0], "input_%d.txt", &start_idx);
    sscanf(info->filelist[info->num_files-1], "input_%d.txt", &end_idx);

    printf("Thread id = %ld starts processing files with index from %d to %d!\n", info->thread_id, start_idx, end_idx);

    /* debugging, prints list of files in fileList

    printf("Thread id = %ld filelist:\n", info->thread_id);
    for (int i = 0; i < info->num_files; i++) {
        printf("    %s\n", info->filelist[i]);
    }

    */

    // print statement for main()
    for (int i = 0; i < info->num_files; i++) {
        processFile(info->path, info->filelist[i], info->alphabetfreq);
        printf("Thread id = %ld is processing file %s\n", info->thread_id, info->filelist[i]);
    }

    // prints thread is done
    printf("Thread id = %ld is done!\n", info->thread_id);

    // terminates thread
    pthread_exit(NULL);
}

void alphabetcountmulthreads(char *path, char *filetowrite, long alphabetfreq[], int num_threads) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("No directory found\n");
        return;
    }

    // Initialize filelist array to empty strings
    char filelist[50][30] = {0};

    // initializing a temporary string that can hold 30 characters for sorting later
    char tmp[30];

    // init idx to 0 for iterating
    int idx = 0;

    // if statement if no directory could be found using the in param
    if (dir == NULL) {
        printf("No directory found");
    }

    // init a dirent struct pointer named currfile
    struct dirent *currfile;

    // reads file from directory
    currfile = readdir(dir);

    // Checks and adds all files with .txt extension in filelist array
    while (currfile != NULL) {
        int len = strlen(currfile->d_name);
        if (len > 3){
            const char *extension = &currfile->d_name[len-4];
            if (strcmp(extension,".txt") == 0) {
                strcpy(filelist[idx], currfile->d_name);
                idx++;
            }
        }
        currfile = readdir(dir);
    }

    // Defines the size of the nonempty portion of string array
    int N = 0;
    for (int i = 0; i < 50; i++) {
        if (strcmp(filelist[i], "\0") > 0)
            N++;
    }

    // Sorts filelist via bubblesort
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 31 -1-i; j++) {
            if (strcmp(filelist[j], filelist[j+1]) > 0) {
                strcpy(tmp, filelist[j]);
                strcpy(filelist[j], filelist[j+1]);
                strcpy(filelist[j+1], tmp);
            }
        }
    }
    closedir(dir);

    // Initialize and create threads
    struct ThreadInfo thread_info[num_threads];

    int files_per_thread = N / num_threads;
    int remaining_files = N % num_threads;

    int file_idx = 0;

    for (int i = 0; i < num_threads; i++) {
        thread_info[i].path = path;
        memset(thread_info[i].alphabetfreq, 0, sizeof(thread_info[i].alphabetfreq)); // Initialize the local alphabetfreq array

        // Calculate the number of files to be processed by this thread
        thread_info[i].num_files = files_per_thread + (i < remaining_files ? 1 : 0);

        // Copy the filelist into each thread's info struct
        for (int j = 0; j < thread_info[i].num_files; j++) {
            strcpy(thread_info[i].filelist[j], filelist[file_idx]);
            file_idx++;
        }

        pthread_create(&thread_info[i].thread_id, NULL, processFiles, &thread_info[i]);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(thread_info[i].thread_id, NULL);
    }

    // Combine the local alphabetfreq arrays into the shared alphabetfreq array with mutex protection
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_threads; i++) {
        for (int j = 0; j < ALPHABETSIZE; j++) {
            alphabetfreq[j] += thread_info[i].alphabetfreq[j];
        }
    }
    pthread_mutex_unlock(&mutex);

    // Write the results to the output file
    FILE *outputFile = fopen(filetowrite, "w");
    if (outputFile == NULL) {
        printf("Error opening file: %s\n", filetowrite);
        return;
    }

    for (int i = 0; i < 26; i++) {
        fprintf(outputFile, "%c -> %ld\n", 'a' + i, alphabetfreq[i]);
    }

    fclose(outputFile);
}