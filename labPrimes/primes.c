#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argcv[]) {


    int n =atoi(argcv[1]);
    int primo = 2;
    int arrn[n];
    for(int i = 0; i < n-1; i++){
        arrn[i] = i+2;
        printf("ARREGLO DE NATURALES: %d\n", arrn[i]);
    }

    int fds[2];
    int r = pipe(fds);

    r = write(fds[1], &arrn[0], sizeof(arrn));

    
    
    int i = fork();
    


    if (i == 0) {

        int recibido[n];
        int err = read(fds[0], &recibido[0], sizeof(recibido));

        for(int i = 0; i < n-1; i++){
            if (recibido[i] % primo == 0) {
                printf("%d\n", recibido[i]);
            }
        }

        




    } else {

    }


}
