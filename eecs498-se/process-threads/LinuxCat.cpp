#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// word structure
struct Buffer
{
    char block[1024];
    ssize_t length;
};

// templated linked list structure
template <typename T> struct Node
{
    Node *next;
    T data;

    Node() : next(nullptr) {}
};

template <typename T> class SharedList
{
  private:
    Node<T> *top, *bottom;
    pthread_cond_t available;
    pthread_mutex_t lock;

  public:
    SharedList() : top(nullptr), bottom(nullptr)
    {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&available, NULL);
    }

    // dtor
    ~SharedList()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&available);

        // delete LL
        Node<T> *p;
        while (top)
        {
            p = top;
            top = p->next;
            delete p;
        }
    }

    // Reads one Node of the LL, moving top to the next Node.
    // This function waits for the condition variable to be freed so that it can
    // access the LL.
    Node<T> *Get()
    {
        Node<T> *a;
        pthread_mutex_lock(&lock);

        // Put the thread to sleep if SharedList is empty
        while (!(a = top))
        {
            pthread_cond_wait(&available, &lock);
        }

        // Check if we've reached the end of the linked list.
        if ((top = a->next) == nullptr)
        {
            bottom = nullptr;
        }
        a->next = nullptr;
        pthread_mutex_unlock(&lock);
        return a;
    }

    // Push a Node to the back of the LL.
    // This function signals the condition variable when it's done accessing the
    // SharedList, which is a shared resource.
    void Put(Node<T> *a)
    {
        pthread_mutex_lock(&lock);

        if (bottom)
        {
            bottom = bottom->next = a;
        }
        else
        {
            top = bottom = a;
        }
        // When we are done accessing the list, we can signal the condition
        // variable so that we can allow a singular waiting thread to continue
        // execution.
        pthread_cond_signal(&available);
        pthread_mutex_unlock(&lock);
    }
};

SharedList<Buffer> Empty, Full;

// Read from the buffer. This will be our child thread.
void *Reader(void *p)
{
    Node<Buffer> *e;
    ssize_t length;

    do
    {
        e = Empty.Get();
        e->data.length = read(0, e->data.block, sizeof(e->data.block));
        length = e->data.length;
        Full.Put(e);
    } while (length > 0); // check if we've finished reading the datastream

    return nullptr;
}

// Write to stdout via the main thread
void Writer(void)
{
    Node<Buffer> *f;
    ssize_t length;

    while (f = Full.Get(), f->data.length > 0)
    {
        write(1, f->data.block, f->data.length);
        Empty.Put(f);
    }
}

int main()
{
    SharedList<Buffer> empty, full;

    for (int i = 0; i < 5; i++)
    {
        Empty.Put(new Node<Buffer>);
    }

    pthread_t child;

    // Spawn the reader child thread
    pthread_create(&child, NULL, Reader, NULL);

    // Perform writing in the main thread.
    Writer();

    pthread_join(child, NULL);

    return 0;
}