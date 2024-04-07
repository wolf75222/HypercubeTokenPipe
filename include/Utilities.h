#ifndef UTILITIES_H
#define UTILITIES_H

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void delete_previous_files();
int difference1bit(int a, int b, int *bitDiffPos);
int isInTab(int val, int* tab, int size);
int hasToBeUnlink(char file_name[256]);

#endif

