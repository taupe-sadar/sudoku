#include "cnp.h"
Tabperm * perm(int n, int p){
  return internal_perm(n,p,p);
}

Tabperm * internal_perm(int n, int p, int malloc_size){
  if(p>n || n < 1){
    return NULL;
  }else if(p == 0){
    Tabperm * pTab = malloc(sizeof(Tabperm));
    pTab->nb_perm = 0 ;
    return pTab;
  }else if(p == n){
    Tabperm * pTab = malloc(sizeof(Tabperm));
    int nb;
    pTab->nb_perm = 1;
    pTab->tab=malloc(sizeof(int*));
    pTab->tab[0]=malloc(sizeof(int)*malloc_size);
    for(nb=0;nb<n;nb++){
      pTab->tab[0][nb]=nb;
    }
    return pTab;
  }else{
    int i;
    
    Tabperm * pTab1 = internal_perm(n-1,p-1,malloc_size);
    Tabperm * pTab2 = internal_perm(n-1,p,malloc_size);
    Tabperm * pTab = malloc(sizeof(Tabperm));
    int size1 = pTab1->nb_perm?pTab1->nb_perm:1;
    int size2 = pTab2->nb_perm;
    pTab->nb_perm= size1 + size2;
    pTab->tab=malloc(sizeof(int*)*pTab->nb_perm);
    for(i=0;i<pTab1->nb_perm;i++){
      pTab->tab[i]=pTab1->tab[i];
      pTab->tab[i][p-1]=n-1;
    }
    if(pTab1->nb_perm == 0){
      pTab->tab[0]=malloc(sizeof(int)*malloc_size);
      pTab->tab[0][0]=n-1;
    }
    for(i=0;i<size2;i++){
      pTab->tab[i+size1]=pTab2->tab[i];
    }
    
    if(pTab1->nb_perm>0)
      free(pTab1->tab);
    if(pTab2->nb_perm>0)
      free(pTab2->tab);
    free(pTab1);
    free(pTab2);
    return pTab;
    
  }
}

void free_perm(Tabperm * pTab){
  int i;
  for(i=0;i<pTab->nb_perm;i++){
    free(pTab->tab[i]);
  }
  if(pTab->nb_perm>0)
    free(pTab->tab);
  free(pTab);
}
