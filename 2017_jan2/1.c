#define _XOPEN_SOURCE 500
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \


int main(int argc, char **argv){

    checkError(argc==2,"");
    int numdays = atoi(argv[1]);
    time_t now = time(NULL);
    checkError(now!=-1,"");
    now += numdays;
    struct tm* formated_time = localtime(&now);
    switch(formated_time->tm_mday){
        case 0:
			printf("Nedelja %d\n",formated_time->tm_mday);
			break;
		case 1:
			printf("Ponedeljak %d\n",formated_time->tm_mday);
			break;
		case 2:
			printf("Utorak %d\n",formated_time->tm_mday);
			break;
		case 3:
			printf("Sreda %d\n",formated_time->tm_mday);
			break;
		case 4:
			printf("Cetvrtak %d\n",formated_time->tm_mday);
			break;
		case 5:
			printf("Petak %d\n",formated_time->tm_mday);
			break;
		case 6:
			printf("Subota %d\n",formated_time->tm_mday);
			break;
		default:
			break;
	}
    
    exit(EXIT_SUCCESS);
}