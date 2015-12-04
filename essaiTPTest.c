#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include "participant.h"

int main(int argc, char const *argv[])
{
	int PvF[2];
	int FvP[2];
	int filePipe[2];
	int handle;
	char data[256];
	int numId = 1, hop = 0;
	int x, x2, pidInfo;
	pid_t pid;

	

	srand(time(NULL));
	x = rand()%100;

	char* fileName = "data.txt";
	handle = open(fileName, O_TRUNC | O_TRUNC, 7777);
	close(handle);


	participant_t* p = creerParticipant(numId, x, hop);

	sauvegarderParticipant(fileName, p);
	participant_t* decoy;
	decoy = chargerParticipant(fileName);






	printf("père - tirage : %d\n", x);

	pipe(PvF);
	pipe(FvP);


	pipe(filePipe);


	if(pid = (fork() == 0))
	{
		int n, i;
		close(PvF[1]);
		close(FvP[0]);
		close(filePipe[1]);
		char c;
		read(PvF[1], (void*)&x, sizeof(int));
		read(filePipe[1], (void*)&handle, sizeof(int));
		printf("x : %d\n", x);
		sleep(1);
		srand(time(NULL));
		n = rand()%10;

		printf("fils - tirage : %d\n", n);








		


		// pos = lseek(handle, k, SEEK_END);
		// while(read(handle, &c, sizeof(char)) != 0)
		// {
		// 	//printf("%c", c);
		// 	data[i] = c;
		// 	i++;
		// }

		

		//snprintf(data, sizeof(data), "%d%c%d%c%d%c%c", numId, ',', x, ',', hop, '\n', '\0');

		//printf("data (fils) ; %s \n", data);



		write(FvP[1], (void*)&n, sizeof(int));

		printf("\n");
		
		close(PvF[0]);
		close(FvP[1]);

		exit(0);
	}
	else
	{
		wait(NULL);
		close(PvF[0]);
		close(FvP[1]);
		read(FvP[0], (void*)&x2, sizeof(int));

		
		if(x > x2)
		{
			printf("le PERE a gagné , %d contre %d\n",x,x2);
		}
		else
			printf("le FILS a gagné, %d contre %d\n",x2,x);

		//int reversedFile = open("/home/anic-001/Bureau/rep_perso/302/tube/reversed.txt", O_WRONLY);
		//write(reversedFile, data, sizeof(char)*13);



		close(PvF[1]);
		close(FvP[0]);
	}

	close(handle);
	
	return 0;
}