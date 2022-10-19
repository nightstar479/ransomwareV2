
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
            return 0;
        }

        else {
            puts("ERROR: too few arguments");
            return 0;
        }
    }

    return 0;
}


void dirContent(const char *rootpath){
    DIR *d = opendir(rootpath);

    if (d == NULL){
        perror("ERROR: ");
    }
    struct dirent *sd = readdir(d);



    while (sd!= NULL){

        if(sd -> d_type == DT_DIR && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ // On vérifie que le fichier est un dossier et qu'il n'est pas le dossier courant ou le dossier parent
            char* new_path = (char*)malloc(strlen(rootpath)+strlen(sd->d_name)+2); // On alloue de la mémoire pour le nouveau chemin-> c'est la longueur du rootpath + du d_name + 2, 2 correspond à "/" + le \0 qui marque la fin du tableau, on caste le malloc en tableau de char, 
            strcpy(new_path,"");
            strncat(new_path,rootpath,strlen(rootpath));
            strcat(new_path,"/");
            strncat(new_path,sd ->d_name,strlen(sd->d_name));
            printf("Path:%s\n",new_path);

            
            dirContent(new_path);
            free(new_path);
        }

        else{
            if( sd -> d_type == DT_REG){
                printf("Nom du fichier: %s\n",sd->d_name);
            }
        }
       sd = readdir(d);
    }
    

  

}