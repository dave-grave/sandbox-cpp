// ======================================================
// =                                                    =
// =                                                    =
// =            David's homemade c++ labs               =
// =          lab1: constructors w/ gemini              =
// =                      2/3/26                        =
// =                                                    =
// =                                                    =
// ======================================================

#include <iostream>
#include <string.h>
#include <string>

class Widget
{
    std::string *data;

  public:
    // constructor that creates a new dynamically allocated string
    Widget(std::string value) : data(new std::string(value))
    {
        std::cout << "Constructing [" << *data << "]\n";
    }

    // copy ctor
    Widget(const Widget &other) : data(new std::string(*other.data))
    {
        std::cout << "COPYING [" << *data << "]\n";
    }

    // move ctor (PARAMETER SHOULD NOT BE CONST SINCE WE ARE UPDATING IT)
    Widget(Widget &&other) noexcept : data(other.data)
    {
        other.data = nullptr; // nullify other
        std::cout << "MOVING data out \n";
    }

    // copy assignment operator
    Widget &operator=(const Widget &other)
    {
        delete data;
        data = new std::string(*other.data);
        return *this;
    }

    ~Widget()
    {
        if (data)
        {
            std::cout << "deleting data\n";
            delete data;
        }
        else
        {
            std::cout << "data is empty but is deleted anyways\n";
        }
    }
};

int main()
{
    std::cout << "--- Creating w1 ---\n";
    Widget w1("HeavyResource");

    std::cout << "\n--- Creating w2 (Copying w1) ---\n";
    Widget w2 = w1;

    std::cout << "\n--- Creating w3 (Moving w1) ---\n";
    Widget w3 = std::move(w1);

    std::cout << "\n--- part 4 ---\n";
    w1 = Widget("New Data");

    std::cout << "\n--- End of Main ---\n";
    return 0;
}