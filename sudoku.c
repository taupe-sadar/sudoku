#include "cnp.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum{
  FILE_GRILLE = 0,
  STD_INPUT = 1
} eInputType;

typedef enum{
  PRETTY_STD_OUTPUT = 0,
  PRETTY_STD_OUTPUT_WITH_DEBUG_CANDIDATES = 1,
  STD_OUTPUT_CANDIDATES = 2,
} eOutputType;

typedef enum{
  SOLVED,
  IMPOSSIBLE,
  TOOHARD,
} eEndType;


#define INPUT 0
#define OUTPUT 1
  
typedef struct Contrainte Contrainte;
struct Contrainte{
  int * cases;
  int exploitee;
} ;

typedef struct Case Case;
struct Case{
  int * contraintes;
} ;

typedef struct File_19 File_19;
struct File_19{
  int * chiffres;
  int longueur;
  int ok;
} ;


static int* Grille= NULL;
static FILE* input = NULL;
static File_19** Possible = NULL;


Contrainte* lignes = NULL;
Contrainte* colonnes = NULL;
Contrainte* zones = NULL;

Case* all_cases = NULL;
int ETAPE=1;

int * difficulte;

void lire_grille(){
  char caractere='n';
  int indice=0;
  int a=0;
  Grille=malloc(sizeof(int)*81);
  for(a=0;a<81;a++){Grille[a]=0;}
  

  if( INPUT == FILE_GRILLE ){
    input=fopen("grille.txt", "r");
    
    while((caractere != EOF)&&(indice!=81)){
      
      caractere = fgetc(input);
      if(caractere!='\n'){
	Grille[indice]=(int)(caractere - '0');
	indice++;
      }
    }
    fclose(input);
  }
  else if ( INPUT == STD_INPUT ){
    while(indice!=81){
      
      caractere = fgetc(stdin);
      if(caractere!='\n'){
	Grille[indice]=(int)(caractere - '0');
	indice++;
      }
    }
  }
}

void def_difficulte(int difficulte_max){
  difficulte=malloc(difficulte_max*sizeof(int));
  int i;
  for(i=0;i<difficulte_max;i++){
    difficulte[i]=0;
  }
}

void affiche_difficulte(int difficulte_max)
{
  printf("%d singletons nus\n",difficulte[0]);
  printf("%d singletons caches\n",difficulte[1]);
  printf("%d eliminations indirectes\n",difficulte[2]);
  int i;
  for(i=3;i<difficulte_max;i++){
    if(difficulte[i]!=0){
      if(i%2){
	printf("%d %d-uplets nus\n",difficulte[i],i/2+1);
      }
      else{
	printf("%d %d-uplets caches\n",difficulte[i],i/2);
      }
    }
  }
}

void affiche_grille(){
  printf("/-----------------\\\n");
  int indice=0;
  int chiffre=-1;
  while(indice<81){
    
    chiffre = Grille[indice];
    
      switch(indice%9){
      case 0:
      case 3:
      case 6:
	printf("|");
	break;
      default:
	printf(" ");
      }
      
      if(chiffre!=0){
	printf("%d",chiffre);
      }
      else{
	printf(" ");
      }
      indice++;
      
    
      if(indice%9==0){
	printf("|\n");
	
	switch((indice/9)%9){
	case 3:
	case 6:
	  printf("|-----------------|\n");
      break;
	case 0:
	  printf("\\-----------------/\n");
	  break;
	default:
	  printf("|                 |\n");
	}
      }
      
  }
}

void debug_possible(){
  int i=0;
  for(i=0;i<81;i++){
    int j=0;
    printf("%d: %d | %d |",i,Possible[i]->ok,Possible[i]->longueur);
    for(j=0;j<Possible[i]->longueur;j++){
      printf(" %d",Possible[i]->chiffres[j]);
    }
    printf("\n");
    
  }
}

void debug_possible_plus(){
  int i,j,k;
  for(i=0;i<81;i+=9){
    if(i%27)
      printf("|.................|.................|.................|\n");
    else
      printf("-------------------------------------------------------\n");
    
    for(j=0;j<9;j+=3){
      for(k=0;k<9;k++){
	if(k%3)
	  printf(".");
	else
	  printf("|");
	  
	if(Possible[i+k]->ok)
	  printf("     ");
	else{
	  int length=(Possible[i+k]->longueur-j);
	  if(length<=0){
	    printf("     ");
	  }
	  else if(length==1){
	    printf(" %d   ",Possible[i+k]->chiffres[j]);
	  }
	  else if(length==2){
	    printf(" %d%d  ",Possible[i+k]->chiffres[j],Possible[i+k]->chiffres[j+1]);
	  }
	  else if(length>=3){
	    printf(" %d%d%d ",Possible[i+k]->chiffres[j],Possible[i+k]->chiffres[j+1],Possible[i+k]->chiffres[j+2]);
	  }
	  
	}

      }
      printf("|\n");
    }
    
  }
  printf("-------------------------------------------------------\n");
}

void def_contraintes(){
  int a,b;
  all_cases=malloc(sizeof(Case)*81);
  for(a=0;a<81;a++){
    all_cases[a].contraintes=malloc(sizeof(int)*3);
  }

  lignes = malloc(sizeof(Contrainte)*9);
  for(a=0;a<9;a++){
    lignes[a].cases=malloc(sizeof(int)*9);
    for(b=0;b<9;b++){
      lignes[a].cases[b]=9*a+b;
      all_cases[9*a+b].contraintes[0]=a;
    }
    lignes[a].exploitee=0;
  }
  
  colonnes = malloc(sizeof(Contrainte)*9);
  for(a=0;a<9;a++){
    colonnes[a].cases=malloc(sizeof(int)*9);
    for(b=0;b<9;b++){
      colonnes[a].cases[b]=9*b+a;
      all_cases[9*b+a].contraintes[1]=a;
    }
    colonnes[a].exploitee=0;
  }
  
  zones = malloc(sizeof(Contrainte)*9);
  for(a=0;a<9;a++){
    zones[a].cases=malloc(sizeof(int)*9);
    for(b=0;b<9;b++){
      int c=(a/3)*27 + (a%3)*3 + (b/3)*9 + (b%3);
      zones[a].cases[b]=c;
      all_cases[c].contraintes[2]=a;
    }
    zones[a].exploitee=0;
  }

}

void free_all_globals(){

  int a;
  //contraintes
  for(a=0;a<9;a++){
    free(zones[a].cases);
    free(lignes[a].cases);
    free(colonnes[a].cases);    
  }
  free(zones);
  free(lignes);
  free(colonnes);
  
  //all_cases
  for(a=0;a<81;a++){
    free(all_cases[a].contraintes);
  }
  free(all_cases);
  
  //Possible
  for(a=0;a<81;a++){
    free(Possible[a]->chiffres);
    free(Possible[a]);
  }
  free(Possible);

  //...
  free(Grille);
  free(difficulte);
  
}

void terminate_with_summary( eEndType endType, int difficulte_max, char* message )
{
  int exit_code = 3;
  if( endType == SOLVED )
    exit_code = 0;
  else if ( endType == IMPOSSIBLE )
    exit_code = 1;
  else if ( endType == TOOHARD )
    exit_code = 2;

  if( OUTPUT == STD_OUTPUT_CANDIDATES ){
    debug_possible();
  }
  else{
    switch( endType ){
    case SOLVED:
      printf("*** SUDOKU FINI ! ***\n");
      affiche_difficulte(difficulte_max);
      
      break;
    case IMPOSSIBLE:
      printf("*** CONTRACDICTION ***\n%s",message);
      
    break;
    case TOOHARD:
      if( PRETTY_STD_OUTPUT_WITH_DEBUG_CANDIDATES ){
	debug_possible_plus();
      }
      printf("*** JE SAIS PAS FAIRE MIEUX ! ***\n");
      
      break;
    }
  }
  
  free_all_globals();
  exit( exit_code );
} 

void remplir_Possible(){
  int i=0;
  int j=0;
  Possible=malloc(sizeof(File_19*)*81);
  for(i=0;i<81;i++){
    if(Grille[i]!=0){
      
      Possible[i]=malloc(sizeof(File_19));
      Possible[i]->longueur=1;
      Possible[i]->chiffres=malloc(sizeof(int)*9);
      Possible[i]->chiffres[0]=Grille[i];
      Possible[i]->ok=1;
    }
    else{
      Possible[i]=malloc(sizeof(File_19));
      Possible[i]->longueur=9;
      Possible[i]->chiffres=malloc(sizeof(int)*9);
      for(j=1;j<=9;j++){
	Possible[i]->chiffres[j-1]=j;
      }
      Possible[i]->ok=0;
    }
  }
}



int traite_contrainte(Contrainte* cont,char* nom){
  //Le retour renvoie si il y a eu une modification
  int jairienfait=0;
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  for(i=0;i<9;i++){
    if(!cont[i].exploitee){
	
      int* vec=malloc(sizeof(int)*9);
      //mise a zero de vec
      for(j=0;j<9;j++){
	vec[j]=0;
      }
      //on remplit vec des chiffres de la contrainte
      for(j=0;j<9;j++){
	  int no=cont[i].cases[j];
	  if(Possible[no]->ok){
	    vec[Possible[no]->chiffres[0]-1]++;
	  }
	}
      //On traite vec: 0->rien 1->action 2+->erreur
      for(j=0;j<9;j++){
	switch(vec[j]){
	case 0:
	  break;
	case 1:
	  for(k=0;k<9;k++){
	    int no2=cont[i].cases[k];
	    if(!Possible[no2]->ok){
	      int indice=-1;
	      for(l=0;l<Possible[no2]->longueur;l++){
		if(Possible[no2]->chiffres[l]==(j+1)){
		  indice=l;
		  break;
		}
	      }
	      if(indice!=-1){
		for(l=indice;l<Possible[no2]->longueur-1;l++){
		  Possible[no2]->chiffres[l]=Possible[no2]->chiffres[l+1];
		}
		Possible[no2]->longueur--;
		jairienfait++;
	      }
	    }
	  }
	  
	  break;
	default:
	  {
	    char message[256];
	    sprintf (message,"%d chiffres %d dans la %s %d\n",vec[j],j+1,nom,i+1 );
	    terminate_with_summary( IMPOSSIBLE, 0, message);
	  }
	  break;
	}
      }
      free(vec);
      cont[i].exploitee=1;
    }
  }
  return jairienfait;
}

void ecrire_chiffres(int difficulte_max){
  int i=0;
  int ecrits=0;
  int compteur=0;
  for(i=0;i<81;i++){
    if(!Possible[i]->ok){
      switch(Possible[i]->longueur){
      case 0:
	{
	  char message[256];
	  sprintf (message,"ligne %d, colonne %d, zone %d\n",all_cases[i].contraintes[0]+1,all_cases[i].contraintes[1]+1,all_cases[i].contraintes[2]+1 );
	  terminate_with_summary( IMPOSSIBLE, 0, message);
	}
	break;
      case 1:
	//On reveille les contraintes concernees
	lignes[all_cases[i].contraintes[0]].exploitee=0;
	colonnes[all_cases[i].contraintes[1]].exploitee=0;
	zones[all_cases[i].contraintes[2]].exploitee=0;
	//On bloque dans possible
	Possible[i]->ok=1;
	//On inscrit le chiffre dans la Grille
	//(On peut eventuellement l'afficher apres)
	Grille[i]=Possible[i]->chiffres[0];
	ecrits=1;
	compteur++;
	break;
      }
      
    }
    else{
      compteur++;
    }
  }
  if(ecrits && OUTPUT != STD_OUTPUT_CANDIDATES){
    printf("*** ETAPE %d ***\n",ETAPE);
    affiche_grille();
    ETAPE++;
  }
  if(compteur==81){
    terminate_with_summary( SOLVED, difficulte_max, "");
  }
  
}

int traite_contrainte_plus(Contrainte* cont){
  int jairienfait=0;
  int i=0;
  int j=0;
  int k=0;
  int l=0;
  for(i=0;i<9;i++){
    int*vec;
    vec=malloc(sizeof(int)*9);
    
    //on remplit vec avec des zeros
    for(j=0;j<9;j++){
      vec[j]=0;
    }
    //on regarde le nombre d'occurences dans la zone
    for(j=0;j<9;j++){
      int no=cont[i].cases[j];
      if(!Possible[no]->ok){
	for(k=0;k<Possible[no]->longueur;k++){
	  vec[Possible[no]->chiffres[k]-1]++;
	}
      }
    }
    //Si on a qu'une occurence on remplace.
    for(j=0;j<9;j++){
      if(vec[j]==1){
	for(k=0;k<9;k++){
	  int no=cont[i].cases[k];
	  if(!Possible[no]->ok){
	    for(l=0;l<Possible[no]->longueur;l++){
	      if(Possible[no]->chiffres[l]==(j+1)){
		
		Possible[no]->chiffres[0]=Possible[no]->chiffres[l];
		Possible[no]->longueur=1;
		
		jairienfait++;
		break;
	      }
	    }
	  }
	}
      }
    }
    free(vec);
  }
  
  return jairienfait;
}

int resolution_cretine(int difficulte_max){
  int jairienfait=0;
  
  ecrire_chiffres(difficulte_max);
  jairienfait+=traite_contrainte(zones,"zone");
  jairienfait+=traite_contrainte(lignes,"ligne");
  jairienfait+=traite_contrainte(colonnes,"colonne");
    
  return jairienfait;
}

int resolution_cretine_plus(){
  int jairienfait=0;
  
  jairienfait+=traite_contrainte_plus(lignes);
  jairienfait+=traite_contrainte_plus(colonnes);
  jairienfait+=traite_contrainte_plus(zones);
  return jairienfait;
}

int nuplet_cache(Contrainte * cont, int n_card){
  
  int i,j,k,l,m;
  int jairienfait = 0;
  for(i=0;i<9;i++){
    int nb_chiffres = 0;
    int * temp = malloc(9*sizeof(int));
    int * tab_candidats = malloc(9*sizeof(int));
    for(j=0;j<9;j++)
      temp[j]=1; //0: fixe, 1: Possible
    for(j=0;j<9;j++){
      int no=cont[i].cases[j];
      if(Possible[no]->ok){
	temp[Possible[no]->chiffres[0]-1]=0;
      }
    }
    for(j=0;j<9;j++){
      if(temp[j]==1){
	tab_candidats[nb_chiffres]=j+1 ; 	
	nb_chiffres++;
      }
    }
    if(nb_chiffres<n_card){
      free(temp);
      free(tab_candidats);
      continue;
    }
    Tabperm * permutation = perm(nb_chiffres,n_card);
    for(j=0;j<permutation->nb_perm;j++){
      int * tab_cases = malloc(9*sizeof(int));
      int size_tab_cases=0;
      for(k=0;k<9;k++){
	int no2=cont[i].cases[k];
	if(!Possible[no2]->ok){
	  int length=Possible[no2]->longueur;
	  for(l=0;l<length;l++){
	    int stopit=0;
	    for(m=0;m<n_card;m++){
	      if(Possible[no2]->chiffres[l] == tab_candidats[permutation->tab[j][m]]){
		tab_cases[size_tab_cases] = no2;
		size_tab_cases++;
		stopit=1;
		break;
	      }
	    }
	    if(stopit)
	      break;
	  }
	}
      }
      if(size_tab_cases <= n_card ){ // On a trouve une permutation de la contrainte contenant un nuplet cache
	for(k=0;k<size_tab_cases;k++){
	  int no=tab_cases[k];
	  int length=Possible[no]->longueur;
	  int* new_chiffres = malloc(sizeof(int)*9);
	  int size_new_chiffres=0;
	  for(l=0;l<length;l++){
	    for(m=0;m<n_card;m++){
	      if(Possible[no]->chiffres[l] == tab_candidats[permutation->tab[j][m]]){
		new_chiffres[size_new_chiffres]=Possible[no]->chiffres[l];
		size_new_chiffres++;
		break;
	      }
	    }
	    
	  }
	  //La, on peut supprimer un(des) candidat(s). On remplace le tableau.
	  if(size_new_chiffres<length ){
	    free(Possible[no]->chiffres);
	    Possible[no]->chiffres = new_chiffres;
	    Possible[no]->longueur = size_new_chiffres;
	    jairienfait++;
	  }
	  else{
	    free(new_chiffres);
	  }
	}
	
      }
      
      
      free(tab_cases);
      if(jairienfait)
	break;
    }
    free_perm(permutation);
    free(temp);
    free(tab_candidats);
    if(jairienfait)
	break;
  }
  return jairienfait;
}

int nuplet_nu(Contrainte * cont, int n_card){
  int i,j,k,l,m;
  int jairienfait = 0;
  for(i=0;i<9;i++){
    int nb_cases = 0;
    int * tab_cases = malloc(9*sizeof(int));
    
    for(j=0;j<9;j++){
      if(!Possible[cont[i].cases[j]]->ok){
	tab_cases[nb_cases]=j ; 	
	nb_cases++;
      }
    }
    if(nb_cases<n_card){
      free(tab_cases);
      continue;
    }
    Tabperm * permutation = perm(nb_cases,n_card);
    for(j=0;j<permutation->nb_perm;j++){
      int * candidats = malloc(9*sizeof(int));
      int nb_candidats=0;
      for(k=0;k<n_card;k++){
	int no=cont[i].cases[tab_cases[permutation->tab[j][k]]];
	int length=Possible[no]->longueur;
	for(l=0;l<length;l++){
	  int found=0;
	  for(m=0;m<nb_candidats;m++){
	    if(Possible[no]->chiffres[l] == candidats[m]){
	      found=1;
	      break;
	    }
	  }
	  if(found==0){
	    candidats[nb_candidats]=Possible[no]->chiffres[l];
	    nb_candidats++;
	  }
	}
	
      }
      if(nb_candidats <= n_card ){ // On a trouve une permutation de la contrainte contenant un nuplet nu
	for(k=0;k<nb_cases;k++){
	  int case_in_perm = 0;
	  for(l=0;l<n_card;l++){
	    if(permutation->tab[j][l] == k){
	      case_in_perm=1;
	      break;
	    }
	  }
	  if(case_in_perm)
	    continue;
	  
	  //La on a une case qui n'est pas dans la permutation, on supprime les candidats
	  int no=cont[i].cases[tab_cases[k]];
	  int length=Possible[no]->longueur;
	  int* new_chiffres = malloc(sizeof(int)*9);
	  int size_new_chiffres=0;
	  for(l=0;l<length;l++){
	    int found=0;
	    for(m=0;m<nb_candidats;m++){
	      if(Possible[no]->chiffres[l] == candidats[m]){
		found=1;
		break;
	      }
	    }
	    if(found==0){
	      new_chiffres[size_new_chiffres]=Possible[no]->chiffres[l];
	      size_new_chiffres++;
	    }
	    
	  }
	  //La, on peut supprimer un(des) candidat(s). On remplace le tableau.
	  if(size_new_chiffres<length ){
	    free(Possible[no]->chiffres);
	    Possible[no]->chiffres = new_chiffres;
	    Possible[no]->longueur = size_new_chiffres;
	    jairienfait++;
	  }
	  else{
	    free(new_chiffres);
	  }
	}
	
      }
      
      
      free(candidats);
      if(jairienfait)
	break;
    }
    free_perm(permutation);
    free(tab_cases);
    if(jairienfait)
	break;
  }
  return jairienfait;
}


int resolution_nuplet(int n_card, int nu_cache){//nu=1 cache=0
  int jairienfait = 0;  
  if(n_card>4)
    return 0;
  if(jairienfait += nu_cache?nuplet_nu(lignes,n_card):nuplet_cache(lignes,n_card))
    return jairienfait;
  else if(jairienfait += nu_cache?nuplet_nu(colonnes,n_card):nuplet_cache(colonnes,n_card))
    return jairienfait;
  else
  return (jairienfait += nu_cache?nuplet_nu(zones,n_card):nuplet_cache(zones,n_card));
}

int indirect(Contrainte * cont1,Contrainte * cont2){
  int jairienfait = 0;
  int i,j,k;
  int * nocommuns = malloc(sizeof(int)*9);
  int nb_communs = 0;
  int * nocont1 = malloc(sizeof(int)*9);
  int nbcont1 = 0;
  int * nocont2 = malloc(sizeof(int)*9);
  int nbcont2 = 0;
  for(i=0;i<9;i++){
    for(j=0;j<9;j++){
      if(cont1->cases[i]==cont2->cases[j]){
	if(!Possible[cont1->cases[i]]->ok){
	  nocommuns[nb_communs]=cont1->cases[i];
	  nb_communs++;
	}
      }
    }
  }
  if(nb_communs==0){
    free(nocommuns);
    free(nocont1);
    free(nocont2);
    return jairienfait;
  }
  for(i=0;i<9;i++){
    int found=0;
    for(j=0;j<nb_communs;j++){
      if(cont1->cases[i]==nocommuns[j]){
	found=1;
	break;
      }
    }
    if(!found){
      if(!Possible[cont1->cases[i]]->ok){
	nocont1[nbcont1]=cont1->cases[i];
	nbcont1++;
      }
    }
  }
  for(i=0;i<9;i++){
    int found=0;
    for(j=0;j<nb_communs;j++){
      if(cont2->cases[i]==nocommuns[j]){
	found=1;
	break;
      }
    }
    if(!found){
      if(!Possible[cont2->cases[i]]->ok){
	nocont2[nbcont2]=cont2->cases[i];
	nbcont2++;
      }
    }
  }
  int* vec = malloc(sizeof(int)*9);
  for(i=0;i<9;i++){
    vec[i]=0;
  }
  for(i=0;i<nb_communs;i++){
    for(j=0;j<Possible[nocommuns[i]]->longueur;j++){
      vec[Possible[nocommuns[i]]->chiffres[j]-1]=1;
    }
  }
  for(i=0;i<9;i++){
    if(vec[i]==1){
      int in1=0;
      int in2=0;
      for(j=0;j<nbcont1;j++){
	int stop=0;
	for(k=0;k<Possible[nocont1[j]]->longueur;k++){
	  if(Possible[nocont1[j]]->chiffres[k]==i+1){
	    in1=1;
	    stop=1;
	    break;
	  }
	}
	if(stop){
	  break;
	}
      }
      for(j=0;j<nbcont2;j++){
	int stop=0;
	for(k=0;k<Possible[nocont2[j]]->longueur;k++){
	  if(Possible[nocont2[j]]->chiffres[k]==(i+1)){
	    in2=1;
	    stop=1;
	    break;
	  }
	}
	if(stop){
	  break;
	}
      }
      if( in1 && !in2){
	for(j=0;j<nbcont1;j++){
	  int * new_chiffres = malloc(sizeof(int)*9);
	  int nb_chiffres = 0;
	  for(k=0;k<Possible[nocont1[j]]->longueur;k++){
	    if(Possible[nocont1[j]]->chiffres[k]!=(i+1)){
	      new_chiffres[nb_chiffres]=Possible[nocont1[j]]->chiffres[k];
	      nb_chiffres++;
	    }
	  }
	  if(nb_chiffres<Possible[nocont1[j]]->longueur){
	    free(Possible[nocont1[j]]->chiffres);
	    Possible[nocont1[j]]->chiffres=new_chiffres;
	    Possible[nocont1[j]]->longueur=nb_chiffres;
	    jairienfait++;
	  }
	  else{
	    free(new_chiffres);
	  }
	  
	}
      }
      if(jairienfait)
	break;
      if( !in1 && in2){
	for(j=0;j<nbcont2;j++){
	  int * new_chiffres = malloc(sizeof(int)*9);
	  int nb_chiffres = 0;
	  for(k=0;k<Possible[nocont2[j]]->longueur;k++){
	    if(Possible[nocont2[j]]->chiffres[k]!=(i+1)){
	      new_chiffres[nb_chiffres]=Possible[nocont2[j]]->chiffres[k];
	      nb_chiffres++;
	    }
	  }
	  if(nb_chiffres<Possible[nocont2[j]]->longueur){
	    free(Possible[nocont2[j]]->chiffres);
	    Possible[nocont2[j]]->chiffres=new_chiffres;
	    Possible[nocont2[j]]->longueur=nb_chiffres;
	    jairienfait++;
	  }
	  else{
	    free(new_chiffres);
	  }
	  
	}
      }
      if(jairienfait)
	break;
    }
  }
  free(vec);
  free(nocommuns);
  free(nocont1);
  free(nocont2);
  return jairienfait;
}

int resolution_indirecte(){
  int i,j;
  int jairienfait = 0;
  for(i=0;i<9;i++){
    for(j=0;j<3;j++){
      if(jairienfait += indirect(&zones[i],&lignes[3*(i/3)+j])){
	break;
      }
    }
    if(jairienfait){
      break;
    }
    for(j=0;j<3;j++){
      if(jairienfait += indirect(&zones[i],&colonnes[3*(i%3)+j])){
	break;
      }
    }
    if(jairienfait){
      break;
    }
  }
  return jairienfait;
}

int main(int argc,char **argv){
  
  int bloque=0;
  int const highest_difficulty = 11;
  int difficulte_max= highest_difficulty;
  
  if(argc==1){
    difficulte_max = highest_difficulty;
  }else if(argc==2){
     difficulte_max = atoi(argv[1]);
  }
  
  if(argc > 2 || difficulte_max >highest_difficulty || difficulte_max <=0  ){
    printf("Usage : %s [difficulte_max]\n",argv[0]);
    exit(1);
  }

  
  def_difficulte(difficulte_max);
  lire_grille();
  if( OUTPUT != STD_OUTPUT_CANDIDATES ){
    affiche_grille();
  }
  def_contraintes();
  remplir_Possible();
  while(bloque<difficulte_max){
    switch(bloque){
    case 0: //Singleton nu (termes selon Wikipedia)
      if(resolution_cretine(difficulte_max)){
	difficulte[bloque]++;
	bloque=0;
      }else{
	bloque++;
      }
      break;
    case 1:  //Singleton cache
      if(resolution_cretine_plus()){
	difficulte[bloque]++;
	bloque=0;
      }else{
	bloque++;
      }
      break;
    case 2:
      if(resolution_indirecte()){
	difficulte[bloque]++;
	bloque=0;
      }else{
	bloque++;
      }
      break;
    default : //Nuplet cache puis nu
      if(resolution_nuplet((bloque+1)/2,1-(bloque%2))){
	difficulte[bloque]++;
	bloque=0;
      }else{
	bloque++;
      }
      
    }
    
  }
  terminate_with_summary( TOOHARD, 0 , "");
  return 3; //Should never happen
}
