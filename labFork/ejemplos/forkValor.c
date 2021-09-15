#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argcv[]) {
	int a = 4;
	int i = fork();
	if (i < 0) {
		printf("Error en fork! %d\n", i);
		_exit(-1);

	}
	
	if (i == 0) {
		printf("Soy el proceso hijo y mi PID es: %d\n", getpid());
		printf("[hijo] a=%d\n", a);
	} else{
		a = 5;
		printf("Soy el proceso padre y mi PID es: %d\n", getpid());			printf("[padre] a=%d\n", a);
		
	}
	
	printf("Terminando\n");
	_exit(0);


}
