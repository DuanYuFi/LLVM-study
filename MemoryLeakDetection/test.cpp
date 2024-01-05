#include <iostream>
#include <cstdlib>
#include <cstring>

// Main function to demonstrate the use of processData
int main() {
    char *ch = (char*) malloc(16);

    strcpy(ch, "12345");

    int a = 1;

    char* ch2 = (char*) malloc(16);

    free(ch);

    return 0;
}
