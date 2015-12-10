#include "election.h"

int main(int argc, char * argv[]) {
    nombreProcessus = atoi(argv[1]);
    pid_t* tableauPID = (pid_t*) calloc(nombreProcessus, sizeof(pid_t));
    int i;
    int PvF[nombreProcessus][2];

    /* Allocation des tubes */
    int** tube = (int**) malloc(nombreProcessus * sizeof(int*));
    for (i = 0; i < nombreProcessus+1; i++)
        tube[i] = (int*) malloc(2 * sizeof(int));

    /* Creation des tubes */
    for (i = 0; i < nombreProcessus; i++) {
        pipe(tube[i]);
        pipe(PvF[i]);
        write(PvF[i][WRITE], &i, sizeof(int));
      }

    /* i processus fils */
    for (i = 0; i < (nombreProcessus-1); i++) {
        tableauPID[i] = fork();
        /* Enfants */
        if (tableauPID[i] == 0) {
            read(PvF[i][READ], &i, sizeof(int));
            break;
          }
    }

    if(tableauPID[i] == 0) {
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
      int etat = CANDIDAT;

      /* Un message par candidat */
      Message message;

      int numeroTour = 1;

      /* On ferme les côtés inutiles des tubes
      Le fils écrit sur le tube i (numéro du processus)
      Il lit le tube i-1
      */
      close(tube[(i+1) % nombreProcessus][READ]);
      close(tube[(i) % nombreProcessus][WRITE]);

      /* Premier tour */
      message.hop = 1;
      message.level = 1;
      message.id = idCandidat;
      message.boole = 1;

      printf("ID du candidat n°%d : %d\n", i, idCandidat);

      write(tube[(i+1) % nombreProcessus][WRITE], &message, sizeof(Message));

      /* Tant que le processus n'est pas gagnant, il n'envoie pas de signal */
      while(1) {
          /* On récupère le message */
          //sleep(1);
          if(read(tube[(i) % nombreProcessus][READ], &message, sizeof(Message)) > 0) {
              /* Si le tour est fini, le processus reçoit son propre message */
              if (message.hop == nombreProcessus) {
                switch(etat) {

                  case CANDIDAT:
                    if (message.boole == 0) {
                      etat = ELIMINE;
                      printf("Processus n°%d éliminé\n", i);
                      idCandidat = VAINCU;  // VAINCU = define pour être sur que l'id sera supérieur aux autres
                    }
                    else if (message.boole == 1 && message.level == 1) {
                      etat = VAINQUEUR;
                      printf("Processus n°%d vainqueur\n", i);
                      raise(SIGUSR1);
                      break;
                    }
                    else {
                      /* On met à jour l'id */
                      idCandidat = rand() % nombreProcessus + 1;
                      message.id = idCandidat;
                      numeroTour++;
                      printf("ID du candidat n°%d au tour n°%d : %d\n", i, numeroTour, idCandidat);
                    }
                    message.level = 1;
                    message.hop = 0;
                    break;

                  case ELIMINE:
                    message.level = 1;
                    message.hop = 0;
                    break;
                }
              }
              else {
                  /* Si le processus n'est pas déjà éliminé */
                  if (etat != ELIMINE) {
                      /* Si l'identifiant reçu est inférieur à celui du processus, celui-ci est éliminé */
                      if (idCandidat < message.id)
                          message.boole = 0;
                      /* Sinon il est toujours en course donc il incrémente le level */
                      else
                          message.level++;
                  }
              }
              message.hop++; // S'incrémente à chaque saut
              write(tube[(i+1) % nombreProcessus][WRITE], &message, sizeof(Message));
          }
      }
    }
    else {
      for (i = 0; i < nombreProcessus; i++)
        wait(0);
    }
    free(*tube);
    exit(0);
}

void afficherMessage(Message m) {
    printf("Message :\n\tlevel : %d\n\tid : %d\n\thop : %d\n\tboole : %d\n\t", m.level, m.id, m.hop, m.boole);
}

void SIGhandler(int sig) {
    if (sig == SIGUSR1) {
        for (int i = 0; i < nombreProcessus-1; i++)
            kill(0, SIGUSR2);
    }
}
