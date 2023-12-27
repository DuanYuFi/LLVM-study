#include <iostream>

// A function with potential taint source as argument
void processData(int data) {
    int localVariable = data; // Tainted as it receives data from the argument

    int anotherVariable = 10;
    anotherVariable = localVariable; // Tainted as it receives value from a tainted variable

    std::cout << "Processed data: " << anotherVariable << std::endl; // Use of tainted data
}

// Main function to demonstrate the use of processData
int main() {
    int inputData = 42; // Tainted as it may come from an external source
    processData(inputData); // Passing tainted data to a function

    return 0;
}
