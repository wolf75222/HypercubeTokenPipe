#include "Utilities.h"

int difference1bit(int a, int b, int *bitDiffPos) {
    int x = a ^ b; // XOR pour trouver les bits différents
    // Vérifier si x est une puissance de 2, ce qui signifie un seul bit différent
    if (x && !(x & (x - 1))) {
        // Trouver la position du bit différent
        *bitDiffPos = 0; // Initialiser la position du bit à 0
        while (x > 1) {
            x >>= 1; // Décaler x de 1 bit vers la droite
            (*bitDiffPos)++; // Incrémenter la position du bit
        }
        return 1; // Retourner vrai si un seul bit diffère
    }
    return 0; // Retourner faux si plus d'un bit diffère
}

