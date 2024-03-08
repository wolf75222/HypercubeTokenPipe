#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>


/*
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

int main()
{
    int n = 3;
    int nb_sommets = 1 << n;
    int i, j;
    int bitDiffPos;
    

    for(i = 0; i < nb_sommets; ++i)
    {
        for(j = i+1; j < nb_sommets; ++j)
        {
            if(difference1bit(i, j, &bitDiffPos)) // si les 2 id ont 1 seul bit de différence
            {
                // celui avec l'id le plus grand lit dans i | j et écrit dans id_processus i & j
                // ici, on a toujours j > i

                // printf("%d --> %d : %d - %d\n", i, j, j | i, bitDiffPos);
                // printf("%d <-- %d : %d - %d\n", i, j, j & i, bitDiffPos);
                
                printf("%d --> %d : %d - %d\n", i, j, j | i, bitDiffPos);
                printf("%d <-- %d : %d - %d\n", i, j, j & i, bitDiffPos);
                
                // id1 <--> id2 : [...][...]
                // (lire) <-- (écrire)
            }
        }
    }

    return 0;
}


int difference1bit(int a, int b, int *bitDiffPos) // renvoie 1 si les valeur a et b différent seulement d'un bit, 0 sinon ; stock la position de ce bit
{
    int bitDiff = a ^ b; // ou exclusif
    *bitDiffPos = 0;

    // recherche de la position du premier bit 1 (en partant de la droite)
    while(bitDiff % 2 != 1)
    {
        bitDiff >>= 1;
        *bitDiffPos += 1;
    }
    return (bitDiff == 1);
}


