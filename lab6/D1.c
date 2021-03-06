#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>

#define MAX 50

union semun { 
	int val; 
	struct semid_ds *buf; 
	ushort *array; 
};

int setSemValue(int semId) { 
	union semun semUnion; 
	semUnion.val = 1; 
	return semctl(semId, 0, SETVAL, semUnion); 
}

void delSemValue(int semId) { 
	union semun semUnion; 
	semctl(semId, 0, IPC_RMID, semUnion); 
}

//perguntar sobre esses troço loko aqui em baixo

int semaforoP(int semId) { 
	struct sembuf semB; 
	semB.sem_num = 0; 
	semB.sem_op = -1; 
	semB.sem_flg = SEM_UNDO; 
	semop(semId, &semB, 1); 
	return 0; 
}

int semaforoV(int semId) { 
	struct sembuf semB; 
	semB.sem_num = 0; 
	semB.sem_op = 1; 
	semB.sem_flg = SEM_UNDO; 
	semop(semId, &semB, 1); 
	return 0; 
}

int main(void)
{
	//id do segmento de memória
	int segmento;
	//controle do semáforo
	int semId;
	//o buffer
	char *buffer;
	//cria segmento de memória compartilhada. CRIA
	
	
	segmento  = shmget (9300, MAX*sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
	// associa segmento de memória ao buffer
	buffer = (char*) shmat(segmento, 0,0);
	
	
	// O semáforo. CRIA
	semId = semget (8600, 1, 0666 | IPC_CREAT); 
	
	setSemValue(semId);
	
	buffer[0] = '\0';
	
	
	while(1)
	{
		semaforoP(semId);
		printf("USR2: %s\n", buffer);
		scanf("%s", buffer);
		printf("EU: %s \n", buffer);
		semaforoV(semId);
	}
	
	// libera a memória compartilhada do processo
	shmdt (buffer); 
	// libera a memória compartilhada
	shmctl (segmento, IPC_RMID, 0); 
	delSemValue(semId); 
	
	delSemValue(semId);
	
	
	return 0;
}
	
	
