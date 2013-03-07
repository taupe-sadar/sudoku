#include "cnp.h"

int main (int argc , char ** argv){
  Tabperm* tab_perm = NULL;
  int n;
  int p;
  if(argc<3){
    printf("Usage test n p\n");
    return 1;
  }
  n=atoi(argv[1]);
  p=atoi(argv[2]);

  if((tab_perm=perm(n,p)) != NULL){
    int i;
    int j;
    int Cnp=1;
    
    for(j=0;j<p;j++){
      Cnp*=(n-j);
      Cnp/=(j+1);
    }
    

    printf("Nombres de permutations %d, Cnp = %d\n",tab_perm->nb_perm,Cnp);
    for(i=0;i<tab_perm->nb_perm;i++){
      int j;
      for(j=0;j<p;j++){
	printf("%d ",tab_perm->tab[i][j]);
      }
      printf("\n");
    }
  }else{
    printf("Bad call to perm : %s %s\n",argv[0],argv[1]);
    return 1;
  }
  free_perm(tab_perm);
  return 0;
}
