#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//si no la declaro el compilador no la toma, a diferencia de strstr
char * strcasestr(const char*, const char*);


void recursiveFind (DIR *parent, char* pathToPrint, char* subString, char* (*locateSubString)(const char*,const char*))
{
    struct dirent *entry;
    DIR *directory;
    int fd;

    while ((entry = readdir(parent)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if ((strstr(entry->d_name, ".") != NULL) || (strstr(entry->d_name, "..")) != NULL) {
                continue;
            }

            char newPath[PATH_MAX];

            if (locateSubString(entry->d_name, subString) != NULL) {
                printf("%s/%s\n", pathToPrint, entry->d_name);
            }
            snprintf(newPath, PATH_MAX, "%s/%s", pathToPrint, entry->d_name);

            fd = openat(dirfd(parent), entry->d_name, O_DIRECTORY);
            if (fd > 0) {
                directory = fdopendir(fd);
                recursiveFind(directory, newPath, subString, locateSubString);
                closedir(directory);

            } else {
                perror("open");
            }
        } else if (locateSubString(entry->d_name, subString) != NULL) {
            printf("%s/%s\n", pathToPrint, entry->d_name);
        }
    }
}

int main (int argc, char *argv[]) {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("error con opendir");
        exit(-1);
    }

    if (argc > 2) {
        recursiveFind(dir,".", argv[2], strcasestr);
    } else {
        recursiveFind(dir,".", argv[1], strstr);
    }

    return 0;
}