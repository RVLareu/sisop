#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>


//si no la declaro el compilador no la toma, a diferencia de strstr
char * strcasestr(const char*, const char*);

void recursiveFind(char *path, char* subString, char* (*locateSubString)(const char*,const char*));


int main(int argc, char* argv[]) {
    if (argc > 2) {
        recursiveFind(".", argv[2], strcasestr);
    } else {
        recursiveFind(".", argv[1], strstr);
    }
    return 0;
}



void recursiveFind(char *path, char* subString, char* (*locateSubString)(const char*,const char*)) {

    DIR *directory = opendir(path);
    if (directory == NULL) {
        perror("error con opendir");
        exit(-1);
    }

    struct dirent *entry;
    while (entry = readdir(directory)) {
        char newPath[FILENAME_MAX];
        if (entry->d_type == DT_DIR) {
            if (strstr(entry->d_name, ".") != NULL) {
                continue;
            }
            //Concateno de donde vengo con "/" y el nombre del dir que sigue. El tamaño es para que no se pase del tamaño del buffer
            snprintf(newPath, FILENAME_MAX, "%s/%s", path, entry->d_name);
            if (locateSubString(entry->d_name, subString) != NULL) {
                printf("%s\n", newPath);
            }
            recursiveFind(newPath, subString, locateSubString);
        //Asumo que es DT_DIR o DT_REG, no tengo en cuenta el desconocido    
        } else if (locateSubString(entry->d_name, subString) != NULL) {
            printf("%s/%s\n", path, entry->d_name);
        }
    }
}



