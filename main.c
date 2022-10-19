#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void dirContent(const char *rootpath);


int main(int argc, char* argv[]){

    dirContent(argv[1]);

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