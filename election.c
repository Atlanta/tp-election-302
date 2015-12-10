#include "election.h"

int main(int argc, char * argv[]) {

    /* hi i iz sum variabel */
    nombreProcessus = 4;
    tableauPID = (pid_t*) calloc(nombreProcessus, sizeof(pid_t));
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

        /* Enfants */
        if ((tableauPID[i] = fork()) == 0) {
            /* Just putting some random... */
            srand((unsigned int)time(NULL) * getpid());

            /* Gestion du signal */
            signal(SIGUSR1, SIGhandler);

            /* Pick a number */
            int idCandidat = rand() % nombreProcessus + 1;
            /*
             L'état est un entier correspondant à l'état du processus pendant le tour :
             0 = Candidat
             1 = Battu
             2 = Elu
             */
            int etat = 0;

            /* Un message par candidat */
            Message message;

            int numeroTour = 1;

            /* On ferme les côtés inutiles des tubes
            Le fils écrit sur le tube i (numéro du processus)
            Il lit le tube i-1
            */
            close(tube[i % nombreProcessus][READ]);
            close(tube[(i - 1) % nombreProcessus][WRITE]);

            /* Premier tour */
            message.hop = 0;
            message.level = 1;
            message.id = idCandidat;
            message.boole = 1;

            printf("ID du candidat n°%d : %d\n", i, idCandidat);

            write(tube[i % nombreProcessus][WRITE], &message, sizeof(Message));

            /* Tant que le processus n'est pas gagnant, il n'envoie pas de signal */
            while (numeroTour < 5 && etat != 2) {
                /* On récupère le message */
                //sleep(1);
                if(read(tube[(i-1) % nombreProcessus][READ], &message, sizeof(Message)) > 0) {
                    /* Si le tour est fini, le processus reçoit son propre message */
                    if (message.hop == nombreProcessus - 1) {

                        if (message.boole == 0) {
                            if (etat != 1) {
                                printf("Processus n°%d éliminé\n", i);
                            }
                            etat = 1;
                        }
                        /* Si le level n'a pas été incrémenté, le processus est le seul restant, il a gagné */
                        else if (message.level == 1) {
                            etat = 2;
                            printf("Processus n°%d vainqueur\n", i);
                            raise(SIGUSR1);
                            //exit(2);
                        }
                        else {
                            idCandidat = rand() % nombreProcessus + 1;
                            message.level = 1;
                            message.id = idCandidat;
                            numeroTour++;
                            printf("ID du candidat n°%d au tour n°%d : %d\n", i, numeroTour, idCandidat);
                            printf("Tour n°%d\n", numeroTour);
                        }
                        message.hop = 0;
                    }
                    else {
                        /* Si le processus n'est pas déjà éliminé */
                        if (etat != 1) {
                            /* Si l'identifiant reçu est inférieur à celui du processus, celui-ci est éliminé */
                            //printf("Message envoyé par p%d reçu par p%d, id message = %d\n", (i-message.hop)%nombreProcessus, i, message.id);
                            if (idCandidat < message.id && message.boole != 0) {
                                message.boole = 0;
                            }
                            message.level++;
                        }
                    }
                    message.hop++; // S'incrémente à chaque saut
                    write(tube[i % nombreProcessus][WRITE], &message, sizeof(Message));
                    //printf("Message écrit par p%d dans le tube %d\n", i, (i - 1) % nombreProcessus);
                }
            }
            printf("Fin du fils n°%d\n", i);
            exit(EXIT_SUCCESS);
        }
    }


    /* pls
     w8 me m8
     pls stahp */
    for (i = 0; i < nombreProcessus; i++)
        wait(0);

    /* ya free man */
    free(*tube);

    /* c ya */
    exit(0);
}

void afficherMessage(Message m) {
    printf("Message :\n\tlevel : %d\n\tid : %d\n\thop : %d\n\tboole : %d\n\t", m.level, m.id, m.hop, m.boole);
}

void SIGhandler(int sig) {
    if (sig == SIGUSR1) {
        for (int i = 0; i < nombreProcessus-1; i++) {
            printf("Signal reçu\n");
            kill(tableauPID[i], SIGUSR2);
        }
    }
    exit(1);
}
