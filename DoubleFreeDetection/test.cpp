#include <iostream>
#include <cstdlib>
#include <cstring>

// Main function to demonstrate the use of processData
int main() {
    char *ch = (char*) malloc(16);

    strcpy(ch, "12345");

    int a = 1;

    char* ch2 = ch;

    free(ch);

    if (a < 10) {
        free(ch2);
    }

    return 0;
}
