#ifndef __ELECTION_H__
#define __ELECTION_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define VAINCU 32196
#define CANDIDAT 0
#define ELIMINE 1
#define VAINQUEUR 2

typedef struct {
    int level;  // Nombre de processus encore en course
    int id;     // id du processus qui envoie le message
    int hop;    // nombre de sauts du message
    int boole;  // état du processus (participant = 1 ou non = 0)
} Message;

//pid_t* tableauPID;
int nombreProcessus;

void afficherMessage(Message m);
void SIGhandler(int n);

#endif
