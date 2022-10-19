#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void dirContent(const char *rootpath);


int main(int argc, char* argv[]){
    if(argc == 2){
        dirContent(argv[1]);
    }
    else{

        if(argc > 2){
        puts("ERROR: too many arguments");
        }

        else {
            puts("ERROR: too few arguments");
        }
    }

    return 0;
}


void dirContent(const char *rootpath){
    puts(rootpath);
    DIR *d = opendir(rootpath);
    struct dirent *sd = readdir(d);

    if (d == NULL){
        perror("ERROR: ");
    }

    while (sd != NULL){

        if(sd -> d_type == DT_DIR){
        /*    char *rp,*newPath;
            rp = malloc(sizeof (char) * BUFSIZ);
            newPath = malloc(sizeof (char) * BUFSIZ);
            strcpy(rp,d);
            strcpy(newPath,sd->d_name);
            strcat(newPath,d);
            dirContent(newPath);*/
        }

        else{
            if( sd -> d_type == DT_REG){
                printf("Nom du fichier: %s",sd->d_name);
            }
        }
       sd = readdir(d);
    }
    

  

}