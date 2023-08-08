/*
 * Program 2
 * Name: Schuyler Wang
 * cssc1766@edoras.sdsu.edu
 * July 23, 2023
 */

Please answer the following questions regarding your programming assignment#2.

1) Please describe your design of your implementation.

In my implementation of alphabetcountmulthreads:

I first created a struct named ThreadInfo containing the necessary information for the program to function such as:
    thread ID, the path to the directory, the local alphabetfreq array, a filelist array to hold the names of files assigned to the specific thread,
    and the number of files each thread will process.

The function processFile processes a single file, updates the local alphabetfreq array for the file,
    and counts then the frequency of each alphabet letter in that file. This function acts as a helper function
    for the processFiles function which acts as the entry point for each thread.
    It takes a pointer to a ThreadInfo struct as an argument. Each thread is responsible for processing a specific set of files from the fileList array
    and then updating its local alphabetfreq array based on the files it processes.

Finally the alphabetcountmulthreads function serves as the main function and performs the following:
    1. Opens the specified directory and creates a list of files with the .txt extension.
    2. Sorts the list of files alphabetically using bubbblesort algorithm.
    3. Divides the files equally among the threads, ensuring each thread processes approximately the same number of files.
    4. Creates multiple threads, each assigned a portion of the files to process.
    5. After all threads finish their work, the function combines the local alphabetfreq arrays of each thread into the
        shared alphabetfreq array using a mutex to protect the critical region.
        The mutex assures that When threads update their local alphabetfreq arrays,
        they need to acquire the mutex so to prevent multiple threads from updating
        the shared array simultaneously.
    6. Writes the final letter frequency results to the output file.

2) Please execute your program by varying thread quantities: {3, 4, 6, 8, 10, 12}, measure the execution times and document your measurement below.

time ./testmulthreads 3 takes 0.681 seconds
time ./testmulthreads 4 takes 0.571 seconds
time ./testmulthreads 6 takes 0.441 seconds
time ./testmulthreads 8 takes 0.365 seconds
time ./testmulthreads 10 takes 0.351 seconds
time ./testmulthreads 12 takes 0.283 seconds

3) Do the observed trends in the measured execution times align with your initial expectations? Please provide a justification for your response.

As the number of threads used increases, the time it takes to run the program decreases due to higher parallel computing happening.
    The negative correlation between thread and runtime align with initial expectations.

4) Is it possible to improve your design to achieve a greater speedup for your program?  If so, what are the possible improvements you can make to your design? 

 If not, why you think your current design is optimal?

A few methods of achieving lower runtime include:

1. File Reading Optimization: By utilizing buffered I/O to read larger chunks of data from files, I/O overhead can be reduced and improve the overall performance.
2. Thread Pooling: Rather than creating and destroying threads for each file batch, a thread pooling mechanism can be implemented by creating a fixed number of
    threads at the start of the program and then assigning work (files) to these threads as they become available, reducing the overhead of creating and
    destroying threads.