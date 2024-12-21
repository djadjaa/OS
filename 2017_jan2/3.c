//roditelj ucitava rec i komandu, prosledjuje detetu rec i komandu, dete obradjuje komandu i izmeni rec, salje obradjenu rec roditelju

#define _XOPEN_SOURCE 700
#include <sys/types.h> //sistemski tipovi
#include <sys/stat.h> //informacije o fajlu
#include <unistd.h> //spisak cesto koriscenih fja za pristup POSIX sistemskih poziva
#include <fcntl.h> //file control
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#define checkError(expr,userMsg) \
    do{ \
        if(!(expr)){ \
            perror(userMsg); \
            exit(EXIT_FAILURE); \
        }\
    }while(0); \

#define RD 0
#define WR 1
#define MAX 64

int main(int argc, char **argv){

    
    /*
    Prvi memset u roditeljskom procesu služi da se inicijalizuje niz dataToSend na 0 (tj. da se svi bajtovi postave na nulu). 
    Ovo je korisno zato što želite osigurati da niz nema neinicijalizovanih podataka pre nego što ga koristite za slanje podataka detetu preko cevi. 
    Takođe, to garantuje da, u slučaju da niz ne bude potpuno popunjen, ostatak niza ne sadrži smeće (neinicijalizovane podatke) 
    koje bi moglo dovesti do problema ili nepredviđenih ponašanja kada dete prima podatke.

    Drugi memset u detetovom procesu služi da se niz dataToRec takođe postavi na 0 pre nego što dete počne da čita podatke od roditelja. 
    Ovo osigurava da je niz prazno inicijalizovan pre nego što dete primi bilo kakve podatke. 
    Ako roditelj pošalje manje od 2*MAX_SIZE bajtova, ostatak niza će biti nule, što smanjuje rizik od neispravnih podataka u ostatku niza.
    Takođe, u detetovom procesu, postoji još jedan memset za niz dataToSend, koji se koristi za inicijalizaciju pre slanja obrađenih podataka nazad roditelju.
    Ovaj memset služi istoj svrsi kao i prethodni, kako bi se niz pripremio za upis obrađenih podataka i izbeglo da sadrži neispravne ili stare podatke.
    
     */
    
    while(1){
        int par2cld[2];
        int cld2par[2];
        checkError(pipe(par2cld)!=-1,"");
        checkError(pipe(cld2par)!=-1,"");
        pid_t childPid = fork();
        checkError(childPid!=-1,"");

        if(childPid>0){ //parent
            char wrd[MAX];
            char command[MAX];
            char dataToSend[2*MAX];

            close(par2cld[RD]);
            close(cld2par[WR]);

            int numOfPairs = fscanf(stdin,"%s%s",wrd,command); //roditelj ucitava rec i komandu sa standardnog ulaza
            if(numOfPairs!=2)
                break;
            memset(dataToSend, 0, 2*MAX); //inicijalizacija
            if(strlen(command)>1){
                fprintf(stderr,"wrong");
                continue;
            }
            if(command[0]=='l' || command[0]=='r' || command[0]=='u'){
                sprintf(dataToSend,"%s %s",wrd,command);
            }else{
                fprintf(stderr,"wrong command\n");
                continue;
            }
            checkError(write(par2cld[WR],dataToSend,2*MAX)!=-1,"write par2cld");//roditelj prosledjuje detetu rec i komandu preko dataToSend 
            memset(dataToSend, 0, 2*MAX); //flushing
            checkError(read(cld2par[RD],dataToSend,2*MAX)!=-1,"read cld2par");//roditelj cita ono sto mu je dete modifikovalo
            printf("%s\n",dataToSend); //roditelj ispisuje konacan rezultat
            close(par2cld[WR]);
            close(cld2par[RD]);
        }else{ //child 
            close(par2cld[WR]);
            close(cld2par[RD]);
            
            //inicijalizacija
            char dataToWrd[2*MAX];
            memset(dataToWrd,0,2*MAX);
            char dataToSend[2*MAX];
            memset(dataToSend,0,2*MAX);

            checkError(read(par2cld[RD],dataToWrd,2*MAX)!=-1,"read par2cld"); //dete cita ono sto mu je prosledjeno od roditelja preko dataToSend
            //izvucicemo komandu iz stringa dataToWrd preko pozicija
            int pos = (int)(strchr(dataToWrd,' ')-dataToWrd);
            if(dataToWrd[pos+1]=='l'){
                int i=0;
                for(i=0; i<pos; i++)
                    dataToSend[i] = tolower(dataToWrd[i]);
            }else if(dataToWrd[pos+1]=='u'){
                int i=0;
                for(int i=0; i<pos; i++)
                    dataToSend[i] = toupper(dataToWrd[i]);
            }else if(dataToWrd[pos+1]=='r'){
                int i=0;
                for(int i=0; i<pos; i++)
                    dataToSend[i] = tolower(dataToWrd[pos-i-1]); //svako drugo slovo umanjujemo
            }
            checkError(write(cld2par[WR],dataToSend,2*MAX)!=-1,"write cld2par"); //dete salje roditelju ono sto je modifikovalo

            close(par2cld[RD]);
            close(cld2par[WR]);
            exit(EXIT_SUCCESS);
        }
        checkError(wait(NULL)!=-1,"");
    }

    exit(EXIT_SUCCESS);
}