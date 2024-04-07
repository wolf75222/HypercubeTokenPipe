#include "Utilities.h"



// Delete files created during previous execution
void delete_previous_files() 
{
    DIR *directory;
    struct dirent *de;

    directory = opendir("./");
    if(directory == NULL)
    {
      fprintf(stderr, "Unable to open current directory\n");
      exit(EXIT_FAILURE);
    }
    while((de = readdir(directory)) != NULL)
    {
        if(hasToBeUnlink(de->d_name))
        {
            unlink(de->d_name);
        }
    }
    closedir(directory);
}



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


int isInTab(int val, int* tab, int size) {
    for(int i = 0; i < size; i++)
    {
        if(val == tab[i])
            return 1;
    }
    return 0;
}


int hasToBeUnlink(char file_name[256])
{
    int c;
    int hasTo = 0;
    if(file_name[0] != '.') // permet d'éviter la supression d'un fichier nommé uniquement : ".txt"
    {
        while((c = file_name[0]) != 0 && c >= '0' && c <= '9')
        {
            file_name++;
        }
        if(c == '.' && file_name[1] == 't' && file_name[2] == 'x' && file_name[3] == 't' && file_name[4] == 0)
        {
            hasTo = 1;
        }
    }
    return hasTo;
}


    