//#########################################################
//#
//# Titre : 	Utilitaires CVS LINUX Automne 20
//#		SIF-1015 - Système d'exploitation
//#		Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//# Date :	Septembre 2020
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "gestionListeChaineeCVS.h"

//Pointeur de tête de la liste de versions 
extern struct noeudV* debutV;
//Pointeur de queue de liste de versions
extern struct noeudV* finV;

//#######################################
//#
//# Affiche une série de retour de ligne pour "nettoyer" la console
//#
void cls(void){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}

//#######################################
//#
//# Affiche un message et quitte le programme
//#
void error(const int exitcode, const char * message){
	printf("\n-------------------------\n%s\n",message);
	exit(exitcode);
	}

//#######################################
//#
//# Chargement d'une version Vi ou i est le numero de version
//#
void loadVersions(){
	//Initialisation des pointeurs sur la liste de versions
	debutV = NULL;
	finV   = NULL;

	char version[100];
	int numVersion=1;

	//Ouverture du fichier contenant la liste des version en mode "rt" : [r]ead [t]ext
	FILE *f;
	f = fopen("ListeVersions.txt", "rt");
	
	if (f==NULL) // au dbut le fichier de version n'existe pas ,..
	{
		struct paramAV *ptr = (struct paramAV*) malloc(sizeof(struct paramAV));
		ptr->new = VRAI;
		ptr->nv = 1;
		strcpy(ptr->tligne,"V1");
	  	addItemV(ptr);
	}
	else 	// fichier existe donc des versions sont disponibles
	{
  //Ajout des éléments extraits du fichier de versions
	  while(!feof(f)){
	    fgets(version, 100, f);
		struct paramAV *ptr = (struct paramAV*) malloc(sizeof(struct paramAV));
		ptr->new = FAUX;
		ptr->nv = numVersion++;
		strcpy(ptr->tligne,(const char *)version);
	    addItemV(ptr);
	    loadVersion(numVersion-1); // chargement du code de la version
	  }
	  fclose(f);

	}

	

 }
 
//#######################################
//#
//# Chargement d'une version Vi ou i est le numero de version
//#
void loadVersion(const int noV){

	char tligne[100];
	int noligne=0;
	
	struct noeudV * ptr = findItemV(noV);
	
	if(ptr == NULL) // Verifier si la version existe
	  return;
	
	ptr->debutL = NULL;
	ptr->finL = NULL;
	ptr->oldVersion = VRAI;
	ptr->commited = FAUX;

	//Ouverture du fichier source en mode "rt" : [r]ead [t]ext
	FILE *f;
	f = fopen(ptr->ptrNoVersion, "rt"); // ouverture du fichier de code c de la version
	if (f==NULL)
	  return;

	//Ajout des éléments extraits du fichier source
	
	while(!feof(f)){
	  fgets(tligne, 100, f);
	  //addItemL(noV, noligne++, tligne);

	  struct paramAL *ptrAddItemL = (struct paramAL*) malloc(sizeof(struct paramAL));
	  ptrAddItemL->noVersion = noV;
	  ptrAddItemL->noligne = noligne;
	  strcpy(ptrAddItemL->tligne,(const char *)tligne);
	  addItemL(ptrAddItemL);

	}
	fclose(f);

 }

 
//#######################################
//#
//# Enregistre le contenu de la liste chaînée de versions  dans le  fichier texte ListeVersion.txt
//#
void saveItemsV(){

	struct noeudV * ptr = debutV;	// premier element de la liste des versions
	FILE *f;

	//Ouverture du fichier en mode "wt" : [w]rite [t]ext

 	f= fopen("ListeVersions.txt", "wt");

	
	if (f==NULL)
		error(2, "saveItemsV: Erreur lors de l'ouverture du fichier des versions pour écriture en mode texte.");

	while (ptr!=NULL){
		if((ptr->oldVersion == VRAI) || (ptr->commited == VRAI)){ 
		  //Écriture des données
		  fprintf(f,"%s\n",ptr->ptrNoVersion);
		}
		//Déplacement du pointeur
		ptr = ptr->suivant;
	}
	
	//Fermeture du fichier
	fclose(f);
	
	}
	
//#######################################
//#
//# Enregistre le contenu de la liste chaînée de lignes de codes dans un fichier texte
//#
//void saveItemsL(const int noVersion){
void saveItemsL(struct paramSL* param){
	int noVersion;
	noVersion = param ->noVersion;
	free(param);

	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;  

	ptrV->commited = VRAI;
	struct noeudL * ptr = ptrV->debutL;	// premier element de la liste de code
	FILE *f;
	char nomC[200];
	char nomTXT[200];
	
	strcpy(nomTXT,ptrV->ptrNoVersion); // copier le nom de version
	sprintf(nomC,"%s.c",nomTXT);


	//Ouverture du fichier en mode "wt" : [w]rite [t]ext

 	f= fopen(nomC, "wt");

	
	if (f==NULL)
		error(2, "saveItemsL: Erreur lors de l'ouverture du fichier pour écriture en mode texte.");

	while (ptr!=NULL){

		//Écriture des données
		fprintf(f,"%s\n",ptr->ligne.ptrligne);
		
		//Déplacement du pointeur
		ptr = ptr->suivant;
	}
	
	//Fermeture du fichier
	fclose(f);
	
	}

//#######################################
//#
//# Execute le fichier source .c 
//#
//void executeFile(char* sourcefname){
void executeFile(struct paramX* param){
	char* sourcefname;
	strcpy(sourcefname,(const char*)param ->sourcefname);
	free(param);
	char command[100];
	char *nomFichier, *sp;
	
	nomFichier = strtok_r(sourcefname, ".", &sp); // extraction du nom de fichier sans ext

	sprintf(command,"gcc -o fichCVSEXE%s %s.c\n",nomFichier,nomFichier);
	system(command);

	// execution du fichier fichCVSEXE
	sprintf(command, "./fichCVSEXE%s",nomFichier);
	system(command);

}

//#######################################
//#
//# Lister  les fichiers sources .c du repertoire courant
//#
void listFileC(){
	char command[100];

	// listage des fichiers .c du dossier courant 
	sprintf(command, "ls -l V*.c");
	// Verouillage de l'affichage
	sem_wait(&semAffichage);
	system(command);
	sem_post(&semAffichage);

}


//#######################################
//#
//# fonction utilisée par les threads de transactions
//#
void* readTrans(char* nomFichier){
	FILE *f;
	char buffer[100];
	char *tok, *sp;

	pthread_t tid[1000];
	int nbThread = 0;

	//Ouverture du fichier en mode "r" (equiv. "rt") : [r]ead [t]ext
	f = fopen(nomFichier, "rt");
	if (f==NULL)
		error(2, "readTrans: Erreur lors de l'ouverture du fichier.");

	//Lecture (tentative) d'une ligne de texte
	fgets(buffer, 100, f);
	
	//Pour chacune des lignes lues
	while(!feof(f)){

		//Extraction du type de transaction 2 cas: commande avec arguments et sans argument
		tok = strtok_r(buffer, " .", &sp);


		//Branchement selon le type de transac	
		if(strcmp(tok, "AL") == 0){
				//Extraction des paramètres
				int noVersion = atoi(strtok_r(NULL, " ", &sp));
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				char *tligne = strtok_r(NULL, "\n", &sp);
				
				//passage des paramètres dans une structure afin de threader la fonction
				struct paramAL *ptr = (struct paramAL*) malloc(sizeof(struct paramAL));
				ptr->noVersion = noVersion;
				ptr->noligne = noligne;
				strcpy(ptr->tligne,(const char *)tligne);
				//Appel de la fonction associée tout en la threadant
				pthread_create(&tid[nbThread++], NULL, addItemL, ptr);


				
				
		}
		else if(strcmp(tok, "CV") == 0){
			    //Extraction des paramètres
				int noVersion = atoi(strtok_r(NULL, "\n ", &sp));
				//Appel de la fonction associée
				struct paramCV *ptr = (struct paramCV*) malloc(sizeof(struct paramCV));
				ptr->new = VRAI;
				ptr->nv = noVersion;
				pthread_create(&tid[nbThread++], NULL, copyItemV, ptr);
				//copyItemV(VRAI, noVersion);
				
		}
		else if(strcmp(tok, "EL") == 0){
				//Extraction du paramètre
				int noversion = atoi(strtok_r(NULL, " ", &sp));
				int noligne = atoi(strtok_r(NULL, "\n", &sp));
				//Appel de la fonction associée
				struct paramEL *ptr = (struct paramEL*) malloc(sizeof(struct paramEL));
				ptr->noline = noligne;
				ptr->noVersion = noversion;
				pthread_create(&tid[nbThread++], NULL, removeItemL, ptr);
				
				}
		else if(strcmp(tok, "EV") == 0){
				//Extraction du paramètre
				int noversion = atoi(strtok_r(NULL, "\n", &sp));
				//Appel de la fonction associée
				struct paramEV *ptr = (struct paramEV*) malloc(sizeof(struct paramEV));
				ptr->nv = noversion;
				pthread_create(&tid[nbThread++], NULL, removeItemV, ptr);
				
		}
		else if(strcmp(tok, "ML") == 0){
				//Extraction des paramètres
				int noversion = atoi(strtok_r(NULL, " ", &sp));
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				char *tligne = strtok_r(NULL, "\n", &sp);
				//Appel de la fonction associée
				struct paramML *ptr = (struct paramML*) malloc(sizeof(struct paramML));
				ptr->noline = noligne;
				ptr->noVersion = noversion;
				strcpy(ptr->tline,(const char *)tligne);
				pthread_create(&tid[nbThread++], NULL, modifyItemL, ptr);	
				
		}
		else if(strcmp(tok, "LL") == 0){
				//Extraction des paramètres
				int noversion = atoi(strtok_r(NULL, " ", &sp));
				int nstart = atoi(strtok_r(NULL, "-", &sp));
				int nend = atoi(strtok_r(NULL, "\n", &sp));
				struct paramLL *ptr = (struct paramLL*) malloc(sizeof(struct paramLL));
				ptr->end = nend;
				ptr->start = nstart;
				ptr->noVersion = noversion;
				pthread_create(&tid[nbThread++], NULL, listItemsL, ptr);
				
		}
		else if(strcmp(tok, "LV") == 0){
				//Extraction des paramètres
				int nstart = atoi(strtok_r(NULL, "-", &sp));
				int nend = atoi(strtok_r(NULL, "\n", &sp));
				//Appel de la fonction associée
				struct paramLV *ptr = (struct paramLV*) malloc(sizeof(struct paramLV));
				ptr->end = nend;
				ptr->start = nstart;
				pthread_create(&tid[nbThread++], NULL, listItemsV, ptr);
				
		}
		else if(strcmp(tok, "LC") == 0){ // lister les fichier c 
				//Appel de la fonction associée
				pthread_create(&tid[nbThread++], NULL, listFileC, NULL);
				
		}
		else if(strcmp(tok, "SV") == 0){ // sauvegarde de la liste des versions
				//Appel de la fonction associée
				//saveItemsV();
				pthread_create(&tid[nbThread++], NULL, saveItemsV, NULL);
				
		}
		else if(strcmp(tok, "SL") == 0){
				//Appel de la fonction associée
				int noversion = atoi(strtok_r(NULL, "\n", &sp));
				//saveItemsL(noversion);
				struct paramSL *ptr = (struct paramSL*) malloc(sizeof(struct paramSL));
				ptr->noVersion = noversion;
				pthread_create(&tid[nbThread++], NULL, saveItemsL, ptr);
				
		}
		else if(strcmp(tok, "X") == 0){
				char *nomfich = strtok_r(NULL, "\n", &sp); // nom du fichier source .c
				struct paramX *ptr = (struct paramX*) malloc(sizeof(struct paramX));
				strcpy(ptr->sourcefname,(const char *)nomfich);
				pthread_create(&tid[nbThread++], NULL, executeFile, ptr);
				
		}

		//Lecture (tentative) de la prochaine ligne de texte
		fgets(buffer, 100, f);
	}
	//Fermeture du fichier
	fclose(f);

	// on s'assure que tous les threads sont terminés avant de quitter la fonction ReadTrans
	for(int i=0; i<nbThread;i++){
		pthread_join(&tid[i], NULL);
	}
	//Retour
	return NULL;
}


