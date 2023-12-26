#include <iostream>
#include <cstdlib>

using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {

    int a = 1, b = 2;
    int c = add(a, b);

    cout << "c = " << c << endl;

    return 0;
}