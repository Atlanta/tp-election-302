// TP3 INFO0302
// 30 novembre 2015

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

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

int main(int argc, char* argv[])
{
    srand(time(NULL));

    // n est le nombre de processus
    int n;
    if(argc == 2)
      n = atoi(argv[1]);
    else
      n = 5;
    // n pipes de 2 cases
    int tubes[n][2];

    int tableauIDProcessus[n], tableauPid[n], pid, idGagnant;
    int i;
    int continuer = 1;

    // Création d'un message
    Message m;
    m.id = 1;
    m.t = 1;

    // Remplissage du tableau d'id par des 0
    memset(tableauIDProcessus, 0, n*sizeof(int));

    // Attribution des IDs aléatoirement
    int random;
    for (i = 0; i < n; i++)
    {
        do {
            random = rand() % n;
            // On vérifie que l'id n'est pas déjà attribué
        } while (verifierTableau(tableauIDProcessus, n, random));
        tableauIDProcessus[i] = random;
    }

    for (i = 0; i < n; i++) { // Création des tubes
        printf("Création du tube pour le processus n°%d\n", tableauIDProcessus[i]);
        pipe(tubes[i]);
    }

    for(i = 0; i < n; i++) { // Compteur de tours

        if( (tableauPid[i] = fork()) == 0) {
          /*
          Processus fils
          */
            int idProcessus = tableauIDProcessus[i];

            /*
             L'état est un entier correspondant à l'état du processus pendant le tour :
                0 = Candidat
                1 = Elu
                2 = Battu
            */
            int etat = 0;

            // "Profondeur" du message
            int level = 0;

            //int pidProcessus = getpid();

            if (i == 0) // Initialisation au premier tour
            {
                close(tubes[n][1]);
                close(tubes[i][0]);

                m.hop = 1;
                m.id = idProcessus;
                m.boole = 1;
                m.level = idProcessus;

                write(tubes[i][1], &m, sizeof(Message));
            }
            else
            {
                Message buffer;
                close(tubes[i-1][1]);
                close(tubes[i][0]);

                while (continuer) { //début algo election
                    read(tubes[i-1][0], &buffer, sizeof(Message));
                    if (m.t == 1) {
                        if ((m.hop == n) && (etat == 0)) {
                            if(m.boole == 1) {
                                etat = 2;
                                m.t = 0;
                                idGagnant = getpid();
                                write(tubes[i][1], &m, sizeof(Message));
                                raise(SIGUSR1);
                            }
                            else {
                                level += 1;

                                do {
                                    random = rand() % n;
                                } while (verifierTableau(tableauIDProcessus, n, random));

                                m.hop = 1;
                                write(tubes[i][1], &m, sizeof(Message));
                            }

                        }
                        else {
                            if ((m.level > level) || ((m.level=level) && (m.id<idProcessus))) {
                                level = m.level;
                                etat = 1;
                                m.hop += 1;
                                write(tubes[i][1], &m, sizeof(Message));
                            }
                            else {
                                if ((m.level == level) && (m.id == idProcessus)) {
                                    m.hop++;
                                    m.boole = 0;
                                    write(tubes[i][1], &m, sizeof(Message));
                                }
                            }
                        }
                    }
                    else {
                        m.t = 1;
                        write(tubes[i][1], &m, sizeof(Message));
                    }
                }
            }

            // Fin du fils
            exit(0);
        }
        else {
          /*
          Processus père
          */
            signal(SIGUSR1, SIGhandler);
        }
    }

    afficherMessage(m);

    // Fin du père (main)
    exit(0);
}
