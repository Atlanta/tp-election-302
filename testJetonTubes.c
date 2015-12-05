// TP3 INFO0302
// 30 novembre 2015

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

typedef struct {
    int id;     // id du tour
    int level;  // id du processus actuel
    int hop;    // nombre de sauts du message
    int boole;
    int t;
} Message;

void afficherMessage(Message m) {
    printf("Message :\nid : %d\nlevel : %d\nhop : %d\nboole : %d\nt : %d\n", m.id, m.level, m.hop, m.boole, m.t);
}

/*
 La fonction vérifierTableau vérifie que des nombres générés aléatoirement n'existent pas déjà dans un tableau.
 */
int verifierTableau(int* t, int taille, int id) {
    int i;
    int dejaAttribue = 0;
    for(i = 0; i < taille; i++) {
        if(id == t[i])
            dejaAttribue = 1;
    }
    
    return dejaAttribue;
}

// SIGhandler est le gestionnaire des signaux
void SIGhandler(int signum) {
    kill(getpid(), SIGUSR2);
}

/*void premierfils(int nombreProcessus, int** tube) {
    
    close(tube[0][READ]);
    close(tube[nombreProcessus-1][WRITE]);
    
    int jeton = 0;
    
    while (jeton < 50){
        write (tube[0][WRITE],&jeton,sizeof(int));
        printf ("Ecriture de fils 0 jeton %d \n",jeton);
        read (tube[nombreProcessus-1][READ],&jeton,sizeof(int));
        jeton++;
    }
    jeton = -1;
    write (tube[0][WRITE],&jeton,sizeof(int));
    exit(EXIT_SUCCESS);
    
}

void autrefils(int numero, int** tube){
    
    close(tube[numero][READ]);
    close(tube[numero-1][WRITE]);
    
    int jeton = 0;
    
    while (jeton != -1) {
        read(tube[numero-1][READ],&jeton,sizeof(int));
        jeton++; // incremente a chaque fils
        write(tube[numero][WRITE],&jeton,sizeof(int));
        printf ("Ecriture de fils %d jeton %d \n",numero,jeton);
    }
    
    exit(EXIT_SUCCESS);
    
}*/


int main(int argc, char * argv[]){
    int pid;
    int nombreProcessus = 7;
    int max = nombreProcessus;
    int i;
    
    /* Allocation des tubes */
    int** tube = (int**) malloc(nombreProcessus * sizeof(int*));
    for (i = 0; i < nombreProcessus; i++)
        tube[i] = (int*) malloc(2 * sizeof(int));
    
    /* Creation des tubes */
    for (i = 0; i < nombreProcessus; i++)
        pipe(tube[i]);
    
    /* i processus fils */
    for (i = 0; i < nombreProcessus; i++) {
        pid = fork();
        
        /* Enfants */
        if (pid == 0) {
            
            /* Premier fils */
            if (i == 0) {
                /* On ferme les côtés inutiles des tubes */
                close(tube[0][READ]);
                close(tube[nombreProcessus-1][WRITE]);
                
                /* On instancie le jeton */
                int jeton = 0;
                
                while (jeton < max) {
                    write(tube[0][WRITE], &jeton, sizeof(int));
                    printf("Ecriture du fils n°0. Jeton = %d\n", jeton);
                    read(tube[nombreProcessus-1][READ], &jeton, sizeof(int));
                    jeton++;
                }
                
                jeton = -1;
                
                write(tube[0][WRITE], &jeton, sizeof(int));
                
                printf("Fin du fils n°0\n");
                exit(EXIT_SUCCESS);
            }
            
            /* Autres fils */
            else {
                /* On ferme les côtés inutiles des tubes */
                close(tube[i][READ]);
                close(tube[i-1][WRITE]);
                
                int jeton = 0;
                
                while (jeton != -1) {
                    read(tube[i-1][READ], &jeton, sizeof(int));
                    if (jeton != -1) {
                        jeton++; // incremente a chaque fils
                        printf("Ecriture du fils n°%d. Jeton = %d\n", i, jeton);
                    }
                    write(tube[i][WRITE], &jeton, sizeof(int));
                }
                
                printf("Fin du fils n°%d\n", i);
                exit(EXIT_SUCCESS);
            }
        }
    }
    
    for (i = 0; i < nombreProcessus; i++)
        wait(0);
    
    exit(0);
    
}