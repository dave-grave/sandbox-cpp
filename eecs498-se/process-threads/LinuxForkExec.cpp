#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv)
{
    // Usage notes
    if (--argc == 0)
    {
        cerr << "Usage: LinuxForkExec command args" << endl;
        return 1;
    }

    // Fork the current process
    pid_t processId = fork();

    if (processId)
    {
        // this is the parent!
        cout << "[parent] is waiting for child" << endl;
        int waitStatus;
        waitpid(processId, &waitStatus, 0);
        cout << "[parent] child has exited with status "
             << WEXITSTATUS(waitStatus) << endl;
    }
    else
    {
        // this is the child!
        argv++;
        cout << "[child] starting " << *argv << endl;
        execvp(*argv, argv);
        cout << "this won't print lmao";
    }
    return 0;
};