
#include <iostream>

using namespace std;

class Test {
    // Member variable
    int x;
public:

    Test copy() {
        return *this;
    }

    // Method
    void print() {
        cout << "Print: " << x << endl;
    }

    // Basic constructor
    Test(int x) : x(x) {
        cout << "Test constructor: " << x << endl;
    }

    // Copy constructor
    Test(const Test& other) : x(other.x + 1) {
        cout << "Test copy constructor: " << other.x << " -> " << x << endl;
    }

    // Destructor
    ~Test() {
        cout << "Test destructor: " << x << endl;
    }

    // Assignment operator (overloading the = operator)
    Test& operator=(const Test& other) {
        cout << "Test assignment operator: " << x << " -> " << other.x+1 << endl;
        x = other.x+1;
        return *this;
    }

};

int main() {
    // Construct an object a
    Test a {1};

    // Call a method of a
    a.print();

    // Construct b using the copy constructor from a
    Test b = a;
    // equivalent to `Test b(a);`

    // Construct an object c
    Test c(3);

    // Assign b to c (calls = operator)
    b = c;

    return 0;
}

