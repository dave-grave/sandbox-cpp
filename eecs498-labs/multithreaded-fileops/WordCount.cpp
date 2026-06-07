// WordCount.cpp
//
// Word count a set of files using a separate thread for each file.
//
// Compile with g++ --std=c++17 WordCount.cpp -pthread -o WordCount

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <vector>

using namespace std;

// WordCount runs in a separate thread, counting words
// in an single file.
//
// If a file can't be opened, ignore it, adding zero words to
// the count.
//
// You are free to use either a memory-mapped implemenation
// or one that reads into a buffer.
//
// When you create a thread, you get to pass a single
// argument as a void *, which the child thread can cast to
// anything meaningful it likes, e.g., a char *pathname.
//
// When WordCount finishes counting a file, the result needs
// to be added to the total.  There are two ways to do this.
//
// (a) Add the count to a shared running total, locking
//     the resource with a pthread mutex while changing it.
//     (Most common.)
//
// (b) Caste the count to a void * and return it as its exit
//     status, which can be retrieved in main with pthread_join
//     and added there to the total.

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int total_words = 0;

// YOUR CODE HERE

void *WordCount(void *arg)
{
    // YOUR CODE HERE
    ifstream file((char *)arg);

    if (!file.is_open())
    {
        return NULL;
    }

    int local_count = 0;
    std::string word;
    while (file >> word)
    {
        local_count++;
    }

    // Unlock total_words and update it
    pthread_mutex_lock(&mut);
    total_words += local_count;
    pthread_mutex_unlock(&mut);

    return NULL;
}

// main() should iterate over the list of filenames given as
// arguments, creating a new thread running WordCount for each of the
// files.  Do not wait for a thread to finish before creating the
// next one.  Once all the threads complete, print total words.

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cerr << "Usage: WordCount <filenames>" << endl
             << "Count the total number of word in the files." << endl
             << "Invalid paths are ignored." << endl;
        return 1;
    }

    // YOUR CODE HERE

    int s;
    int N = argc - 1;

    pthread_t threads[N];
    //  pthread_mutex_init(&mut, NULL);
    for (int i = 0; i < N; i++)
    {
        s = pthread_create(&threads[i], NULL, WordCount, argv[i + 1]);
        if (s != 0)
        {
            cerr << "Error creating threads." << endl;
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    cout << "Total words = " << total_words << endl;
    //  pthread_mutex_destroy(&mut);
    return 0;
}
