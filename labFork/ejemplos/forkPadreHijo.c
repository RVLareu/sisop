#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argcv[]) {
	printf("Mi PID es: %d\n", getpid());

	int i = fork();

	if (i < 0) {
		printf("Error en fork! %d\n", i);
		_exit(-1);

	}
	
	if (i == 0) {
		printf("Soy el procesi hijo y mi PID es: %d\n", getpid());
	} else{
		printf("Soy el proceso padre y mi PID es: %d\n", getpid());		
	}
	
	printf("Terminando\n");
	_exit(0);


}
