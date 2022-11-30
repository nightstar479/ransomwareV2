

---
title: "IR209 - Développement"
subtitle: "Rapport de Laboratoire 3: Parcours récursif (suite & fin), Gestion des arguments & Allocation dynamique"
author: [Maljean Thimothé, Tasiaux Victor]
date: "19-10-2022"
geometry: "left=2.5cm,right=2.5cm,top=2cm,bottom=2cm"
pdf_document: null
titlepage: true
book: false
toc-own-page: true
toc-title: "Table des Matières"
lof-title: "Table des Figures"
lof: false
toc: true

...
# Parcours récursif suite et fin
```c
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void dirContent(const char rootpath);


int main(void){

    dirContent("/home/kali/Documents/Developpement/Lab2/exercice2/test");

    return 0;
}

void dirContent(const charrootpath){
    puts(rootpath);
    DIR d = opendir(rootpath);
    struct direntsd = readdir(d);

    while (sd != NULL){

        if(sd -> d_type == DT_DIR){
         ### TO DO ###
        }

        else{
            if(sd -> d_type == DT_REG){
                printf("Nom du fichier: %s",sd->d_name);
            }
        }

    }




}
```
Ce code avait déjà été validé au cours précédent, il reste à créer un parcours de dossier récursif.

\newpage

# Gestion des arguments

## Version 1
```c
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
        }

        else{
            if(sd -> d_type == DT_REG){
                printf("Nom du fichier: %s",sd->d_name);
            }
        }

    }
    
}
```
Dans cette version, on a vidé la partie lecture de directory pour se concentrer sur l'`argv`. On appelle la fonction `dirContent`, et on passe le string entré dans le terminal  en paramètre de la fonction.

## Version 2

```c
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

       if(sd -> d_type == DT_DIR){

        }


        else{
            if( sd -> d_type == DT_REG){
                printf("Nom du fichier: %s\n",sd->d_name);
            }
        }
       sd = readdir(d);
    }
}
```
Dans cette version on ajoute un contrôle du nombre d'arguments grâce à `argc`.

\newpage

# Allocation dynamique du Path

## Version 1: tableau à taille définie

```c
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

        if(sd -> d_type == DT_DIR && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){
            char new_path[4096] = "";
            strncat(new_path,rootpath,strlen(rootpath));
            strncat(new_path,"/",1);
            strncat(new_path,sd ->d_name,strlen(sd->d_name));
            printf("Path:%s\n",new_path);

            dirContent(new_path);
        }

        else{
            if( sd -> d_type == DT_REG){
                printf("Nom du fichier: %s\n",sd->d_name);
            }
        }
       sd = readdir(d);
    }
}
```
Dans cette version, on utilise un tableau à taille fixe et `strncat`, version sécurisée de `strcat` pour créer le nouveau répertoire.

On vérifie que le fichier est un dossier et qu'il n'est pas le dossier courant(`.`) ou le dossier parent (`..`).

## Version 2: Avec malloc
```c
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

        if(sd -> d_type == DT_DIR && strcmp(sd -> d_name,".") !=0 && strcmp(sd -> d_name,"..") !=0 ){ 
            char* new_path = (char*)malloc(strlen(rootpath)+strlen(sd->d_name)+2);
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

```
On emploie ici la fonction `malloc` pour créer un tableau de taille variable.

On alloue de la mémoire pour le nouveau chemin 
&rarr; c'est la longueur du `rootpath` + du `d_name` + `2`. 
Le 2 correspond au "`/`" + le `\0` qui marque la fin du tableau. Comme `malloc` est à la base un `void`, on caste le malloc en tableau de char.