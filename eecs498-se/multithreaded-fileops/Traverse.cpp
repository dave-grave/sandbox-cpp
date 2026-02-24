// Traverse.cpp
//
// Create and print a sorted list of all the paths specified
// on the command line, traversing any directories to add
// their contents using a pool of worker threads.  Ignore
// any non-existent paths.
//
// Compile with g++ --std=c++17 Traverse.cpp -pthread -o Traverse

#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

pthread_mutex_t queue_lock;
pthread_mutex_t paths_lock;
pthread_cond_t work_cv;
pthread_cond_t stop_cv;

int thread_count;
size_t working_count = 0;
vector<string> paths_found;
queue<string> work_queue;

// Get the next path to be traversed from the work queue
// shared among all the threads, sleeping if the queue is
// empty. If this is the last thread going to sleep, signal
// that all the work is done.
string GetWork()
{
    // Lock the working mutex
    pthread_mutex_lock(&queue_lock);

    // Thread will only call this if it has finished its previous task
    if (working_count > 0)
    {
        working_count--;
    }

    while (work_queue.empty() && working_count > 0)
    {
        pthread_cond_wait(&work_cv, &queue_lock);
    }

    // Check if we broke out of the loop because there's no more work
    if (work_queue.empty() && working_count == 0)
    {
        pthread_cond_broadcast(&work_cv);
        pthread_mutex_unlock(&queue_lock);
        return "";
    }

    // Process directory
    working_count++;

    string path = work_queue.front();
    work_queue.pop();

    pthread_mutex_unlock(&queue_lock);

    return path;
}

// Add work to the queue and signal that there's work available.
void AddWork(string path)
{
    // Lock the queue mutex
    pthread_mutex_lock(&queue_lock);

    work_queue.push(path);

    // Wake up another thread
    pthread_cond_signal(&work_cv);

    pthread_mutex_unlock(&queue_lock);
}

// Add a new path to the list all those that have been found
void AddPath(string path)
{
    pthread_mutex_lock(&paths_lock);
    paths_found.push_back(path);
    pthread_mutex_unlock(&paths_lock);
}

bool DotName(const char *name)
{
    return name[0] == '.' && (name[1] == 0 || name[1] == '.' && name[2] == 0);
}

// Traverse a path.  If it exists, add it to the list of paths that
// have been found.  If it's a directory, add any children other than
// . and .. to the work queue.  If it doesn't exist, ignore it.
void *Traverse(string pathname)
{
    // Try to open the directory
    DIR *dirp = opendir(pathname.c_str());
    if (!dirp)
        return NULL;

    // Add directory name
    AddPath(pathname);

    dirent *entry;
    struct stat buf;

    while ((entry = readdir(dirp)) != NULL)
    {
        // Ignore ./ and ../ directories
        if (DotName(entry->d_name))
        {
            continue;
        }

        // Process directory
        string fullPath = (pathname.back() == '/')
                              ? pathname + entry->d_name
                              : pathname + "/" + entry->d_name;

        if (stat(fullPath.c_str(), &buf) == -1)
        {
            return NULL;
        }

        if (S_ISDIR(buf.st_mode))
        {
            // Add directories to queue
            AddWork(fullPath);
        }
        else
        {
            AddPath(fullPath);
        }
    }

    closedir(dirp);

    return NULL;
}

// Each worker thread simply loops, grabbing the next item
// on the work queue and traversing it.
void *WorkerThread(void *arg)
{
    while (true)
    {
        string path = GetWork();

        if (path == "")
            break;

        Traverse(path);
    }

    // Never reached.
    return nullptr;
}

int main(int argc, char **argv)
{

    if (argc < 3 || (thread_count = atoi(argv[1])) == 0)
    {
        cerr << "Usage: Traverse <number of workers> <list of pathnames>"
             << endl
             << "Number of workers must be greater than 0." << endl
             << "Invalid paths are ignored." << endl;
        return 1;
    }

    // 1. Initialize the locks.
    pthread_mutex_init(&queue_lock, NULL);
    pthread_mutex_init(&paths_lock, NULL);
    pthread_cond_init(&work_cv, NULL);
    pthread_cond_init(&stop_cv, NULL);

    // 2. Iterate over the argv pathnames, adding them to the work queue.
    for (int i = 2; i < argc; i++)
    {
        work_queue.push(argv[i]);
    }

    // 3. Create the specified number of workers.
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++)
    {
        pthread_create(threads + i, NULL, WorkerThread, NULL);
    }

    // 4. Sleep until the work has finished.

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // 5. Sort the paths found vector.
    sort(paths_found.begin(), paths_found.end());

    // 6  Print the list of paths.
    for (auto &path : paths_found)
    {
        cout << path << '\n';
    }

    pthread_mutex_destroy(&queue_lock);
    pthread_mutex_destroy(&paths_lock);
    pthread_cond_destroy(&work_cv);
    pthread_cond_destroy(&stop_cv);

    return 0;
}
