#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<dirent.h>
#include<errno.h>
#include<time.h>
#include<math.h>
 
#define checkError(cond, msg)\
	do{\
		if(!(cond)){\
			perror(msg);\
			exit(EXIT_FAILURE);\
		}\
	}while(0)
	
time_t now;
	
void obidji(char* path){
	struct stat fInfo;
	checkError(-1 != lstat(path, &fInfo),"");
	if(S_ISREG(fInfo.st_mode)){ //Ako je regularan fajl izmenjen <= 5 dana ispisujemo ime
		int brDana = ceil((double)(now-fInfo.st_mtime)/(60*60*24));
		if(brDana <= 5){
			char* ime = strrchr(path,'/');
			printf("%s\n", ime+1);
		}
	}
	else if(S_ISDIR(fInfo.st_mode)){  //Ako je direktorijum (bez . i ..), obilazimo rekurzivno
		DIR* stream = opendir(path);
		checkError(stream != NULL, "");
		
		struct dirent* dirEntry = NULL;
		errno=0;
		
		while((dirEntry = readdir(stream))!=NULL){
			if((strcmp(dirEntry->d_name,".")==0) || (strcmp(dirEntry->d_name,"..")==0))
				continue;
			
			//Sad pravimo putanju
			char* putanja = malloc(strlen(path) + strlen(dirEntry->d_name)+2);
			strcpy(putanja, path);
			strcat(putanja,"/");
			strcat(putanja, dirEntry->d_name);
			
			obidji(putanja);
			free(putanja);
		}
		
		checkError(errno != EBADF,"");
		errno=0;
		
		checkError(-1 != closedir(stream),"");
	}
}	

int main(int argc, char** argv){
	checkError(argc == 2, "");
	
	struct stat fInfo;
	checkError(-1 != lstat(argv[1],&fInfo),"");
	
	checkError(S_ISDIR(fInfo.st_mode),"");
	
	checkError(-1 != time(&now),"");
	obidji(argv[1]);
	
	exit(EXIT_SUCCESS);
}
