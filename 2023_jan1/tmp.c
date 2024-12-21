#include <stdio.h>
#include <string.h>

int main() {

    char str[] = "abvgf.txt";
    char *tacka = strrchr(str,'.');
    printf("%s\n",tacka);

    return 0;
}