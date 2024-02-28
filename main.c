#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>


/*
    |----< A FAIRE >----|

-> Tests d'erreurs (open(f), ...)
-> Continuer le TP (jeton, signaux)


    |----< INFOS >----|

-> Pour un hypercude de dimension n:
    - 2 puissance n sommets
    - (2 puissance n-1) * n arrêtes
    - (2 * (2 puissance n-1) * n) == ((2 puissance n) * n) tubes

-> (1 << n) == (2 puissance n)

-> Opérateurs binaires : 
    - i & j : ET
    - i ^ j : OU EXCLUSIF (mets les bits différents à 1, les autres à 0)
    - i | j : OU INCLUSIF

    - Dans notre cas, comme deux id de sommets adjacents on un seul bit de différent, 
      le ET binaire donne le min des deux id et le OU INCLUSIF donne le max des deux id
*/


int difference1bit(int a, int b, int *bitDiffPos);
void libererTubes(int ***pipeTab, int n);


int main(int argc, char *argv[])
{
    int id_processus; // id du processus courant
    int n; // dimension de l'hypercube
    int nb_processus; // nombre de processus fils = nombre de sommets de l'hypercube = puissance(2, n)
    int nb_tubes; // nombre de tubes = nombre d'arretes de l'hypercube * 2 = puissance(2, n-1) * n * 2
    int i, j;
    int f;
    int bitDiff, bitDiffPos;
    int k = 1;
    int *allPID;
    int ***pipeTab;
    char etiquette[100];
    int *readfds;

    if(argc != 2)
    {
        printf("Ce programme prend obligatoirement un seul paramètre.\n");
        exit(0);
    }
    else if(atoi(argv[1]) < 1)
    {
        printf("Le parametre de ce programme doit être un nombre strictement possitif.\n");
        exit(0);
    }
    
    n = atoi(argv[1]);
    nb_processus = 1 << n; // 2 puissance n
    nb_tubes = (1 << n) * n;

    allPID = (int *) malloc(nb_processus * sizeof(int));

    pipeTab = (int ***) malloc((1 << n) * sizeof(int**));
    for(i = 0; i < (1 << n); i++)
    {
        pipeTab[i] = (int **) malloc(n * sizeof(int*));
        for(j = 0; j < n; j++)
        {
            pipeTab[i][j] = (int *) malloc(2 * sizeof(int));
        }
    }


    for(id_processus = 0; id_processus < nb_processus; ++id_processus)
    {
        if((allPID[id_processus] = fork()) == 0)
        {
            // processus fils ayant l'id : id_processus


            // parcours des id des autres processus pour fermer les tubes inutilisés
            for(i = 0; i < nb_processus; i++)
            {
                if(i != id_processus)
                {
                    if(difference1bit(i, id_processus, &bitDiffPos)) // si les 2 id ont 1 seul bit de différence, alors bitDiff == 1
                    {
                        if(i < id_processus)
                        {
                            // celui avec l'id le plus grand lit dans i & j et écrit dans id_processus i | j
                            close(pipeTab[id_processus & i][bitDiffPos][1]); // lire uniquement
                            close(pipeTab[id_processus | i][bitDiffPos][0]);
                        }
                        else
                        {
                            // celui avec l'id le plus petit lit dans i | j et écrit dans id_processus i & j
                            close(pipeTab[id_processus & i][bitDiffPos][0]); // ecrire uniquement
                            close(pipeTab[id_processus | i][bitDiffPos][1]);
                        }
                    }
                    else
                    {
                        close(pipeTab[id_processus & i][bitDiffPos][0]); // close read
                        close(pipeTab[id_processus & i][bitDiffPos][1]); // close write
                        close(pipeTab[id_processus | i][bitDiffPos][0]); // close read
                        close(pipeTab[id_processus | i][bitDiffPos][1]); // close write
                    }
                }
            }

            // faire ...

            sprintf(etiquette, "%d.txt", id_processus);
            f = open(etiquette, O_WRONLY | O_CREAT, 0644);

            // select(1, &readfds, NULL, NULL, 10000); // ????

            close(f);

            // close tout ce qui reste
            for(i = 0; i < n; i++)
            {
                j = nb_processus ^ (1 << i); // changer valeur de 1 bit

                bitDiff = i^j; // ou exclusif
                bitDiffPos = 0;

                // recherche de la position du premier bit 1 (en partant de la droite)
                while(bitDiff % 2 != 1)
                {
                    bitDiff >>= 1;
                    bitDiffPos++;
                }

                if(j < nb_processus)
                {
                    close(pipeTab[id_processus & i][bitDiffPos][0]);
                    close(pipeTab[id_processus | i][bitDiffPos][1]);
                }
                else
                {
                    close(pipeTab[id_processus & i][bitDiffPos][1]);
                    close(pipeTab[id_processus | i][bitDiffPos][0]);
                }
            }


            // libérer la mémoire allouée !!!
            libererTubes(pipeTab, n);
            pipeTab = NULL;
            free(allPID);
            allPID = NULL;

            exit(0);
        }
    }

    // processus père

    // close tout
    for(i = 0; i < (1 << n); i++)
    {
        for(j = 0; j < n; j++)
        {
            close(pipeTab[i][j][0]);
            close(pipeTab[i][j][1]);
        }
    }

    // faire ...


    libererTubes(pipeTab, n);
    pipeTab = NULL;
    free(allPID);
    allPID = NULL;
    exit(0);
}


// renvoie 1 si les valeurs a et b diffèrent seulement d'un bit, 0 sinon ; stock la position de ce bit dans bitDiffPos
int difference1bit(int a, int b, int *bitDiffPos) 
{
    int bitDiff = a ^ b; // ou exclusif
    *bitDiffPos = 0;

    // recherche de la position du premier bit 1 (en partant de la droite)
    while(bitDiff % 2 != 1)
    {
        bitDiff >>= 1;
        *bitDiffPos += 1;
    }
    return (bitDiff == 1); // si seul le bit à la position bitDiffPos est égale à 1, return 1 (0 sinon)
}


void libererTubes(int ***pipeTab, int n)
{
    int i, j;
    for(i = 0; i < (1 << n); i++)
    {
        for(j = 0; j < n; j++)
        {
            free(pipeTab[i][j]);
            pipeTab[i][j] = NULL;
        }
        free(pipeTab[i]);
        pipeTab[i] = NULL;
    }
    free(pipeTab);
}


