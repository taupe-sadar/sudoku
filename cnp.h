#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct tabperm Tabperm;
struct tabperm{
  int nb_perm;
  int ** tab;
};

Tabperm * perm(int n, int p);
void free_perm(Tabperm * pTab);
Tabperm * internal_perm(int n, int p, int malloc_size);
