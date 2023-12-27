#include <iostream>
#include <cstdlib>
#include <cstring>

// Main function to demonstrate the use of processData
int main() {
    char *ch = (char*) malloc(16);

    strcpy(ch, "12345");
    free(ch);

    int a = 1;

    if (a < 10) {
        free(ch);
    }

    return 0;
}
