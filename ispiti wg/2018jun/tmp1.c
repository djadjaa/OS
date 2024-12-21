#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *reverse(char *s){
    int n=strlen(s);
    for(int i=0; i<n/2; i++){
        s[i]=s[n-i+1];
    }
    return s;
}

int main(){

    char s[6];
    scanf("%s",s);
    printf("%s\n",reverse(s));

    return 0;
}
