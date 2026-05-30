#include <iostream>

static int static_global;
int non_static_global;

class Thing {
//    private: 
	public: 
		static int num_things;	
		Thing() { num_things++; };
		~Thing() { num_things--; };
};

int Thing::num_things = 0;

int main() {

	// Part 1: static vs. non-static memory
    static int static_local; 
    int non_static_local; 
    
    std::cout << "static global lives at:      " << &static_global << '\n';
    std::cout << "static local  lives at:      " << &static_local << '\n';
    std::cout << "non-static global lives at:  " << &non_static_global << '\n';
    std::cout << "non-static local lives at:   " << &non_static_local << '\n';
	
	std::cout << '\n';

	// Part 2: static member variables of a class
	std::cout << "There exist " << Thing::num_things << " things.\n";
	Thing t1;
	Thing t2;

	std::cout << "There exist " << Thing::num_things << " things.\n";
	t1.~Thing();
	t2.~Thing();


	std::cout << "There exist " << Thing::num_things << " things.\n";
	return 0;

}
