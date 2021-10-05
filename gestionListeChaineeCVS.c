//#########################################################
//#
//# Titre : 	Utilitaires Liste Chainee et CVS LINUX Automne 20
//#				SIF-1015 - Système d'exploitation
//#				Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//#	Date :		Septembre 2020
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "gestionListeChaineeCVS.h"

//Pointeur de tête de liste
extern struct noeudV* debutV;
//Pointeur de queue de liste pour ajout rapide
extern struct noeudV* finV;

//#######################################
//#
//# Recherche un item dans la liste chaînée de VERSIONS
//#
//# RETOUR:	Un pointeur vers l'item recherché
//# 		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#
struct noeudV * findItemV(const int no){
	//La liste est vide 
	if ((debutV==NULL)&&(finV==NULL)) return NULL;
	

	//Pointeur de navigation
	struct noeudV * ptr = debutV;

	if(ptr->noVersion==no) // premier noeud
	    return ptr;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;

		//Est-ce l'item recherché?
		if (ptr->noVersion==no){
			//On retourne un pointeur sur l'item
			return ptr;
			}
		}

	//On retourne un pointeur NULL
	return NULL;
	}


//#######################################
//#
//# Recherche un item dans la liste chaînée de code d'une version
//#
//# RETOUR:	Un pointeur vers l'item recherché
//# 		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#
struct noeudL * findItemL(const int noV, const int no){
  
  	struct noeudV * ptrV;
	
	ptrV = findItemV(noV);
	// Verifier si la version existe
	if (ptrV==NULL)
	  return NULL;


	//La liste  de codes est vide 
	if ((ptrV->debutL==NULL)&&(ptrV->finL==NULL)) return NULL;

	//Pointeur du debut de la liste de codes
	struct noeudL * ptr = ptrV->debutL;

	if(ptr->ligne.noligne==no) // premier noeud
		return ptr;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;

		//Est-ce l'item recherché?
		if (ptr->ligne.noligne==no){
			//On retourne un pointeur sur l'item (toujours vérrouillé)
			return ptr;
			}
		}

	//On retourne un pointeur NULL
	return NULL;
	}

//#######################################
//#
//# Recherche le PRÉDÉCESSEUR d'un item dans la liste chaînée de versions
//#
//# RETOUR:	Le pointeur vers le prédécesseur est retourné
//# 		
//#			
//# 		Retourne NULL dans le cas où l'item est introuvable
//#
struct noeudV * findPrevV(const int no){

		//La liste  de versions est vide 
	if ((debutV==NULL)&&(finV==NULL)) return NULL;

	//Pointeur du debut de la liste de versions
	struct noeudV * ptr = debutV;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
		if (ptr->suivant->noVersion==no){
			//On retourne un pointeur sur l'item précédent
			return ptr;
		}

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;
		}

	//On retourne un pointeur NULL
	return NULL;
	}
	
//#######################################
//#
//# Recherche le PRÉDÉCESSEUR d'un item dans la liste chaînée de codes
//#
//# RETOUR:	Le pointeur vers le prédécesseur est retourné
//# 		
//#			
//# 		Retourne NULL dans le cas où l'item est introuvable
//#
struct noeudL * findPrevL(const int noV, const int no){
  
  	struct noeudV * ptrV;
	
	ptrV = findItemV(noV);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return NULL;

		//La liste  de codes est vide 
	if ((ptrV->debutL==NULL)&&(ptrV->finL==NULL)) return NULL;

	//Pointeur du debut de la liste de codes
	struct noeudL * ptr = ptrV->debutL;

	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
		if (ptr->suivant->ligne.noligne==no){
			//On retourne un pointeur sur l'item précédent
			return ptr;
		}

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;
		}

	//On retourne un pointeur NULL
	return NULL;
	}

//#######################################
//#
//# Ajoute un noeud (ligne de code) dans la liste chaînée de code d'une version
//#
void addItemL(struct paramAL* param){
	char tl[100]; 
	int noVersion, nl;
	
	noVersion = param ->noVersion;
	nl = param->noligne;
	strcpy(tl,(const char*)param->tligne);
	free(param);

	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);

	// Verifier si la version existe
	if (ptrV==NULL)
		return;

	//Création de l'enregistrement en mémoire
	struct noeudL* ni = (struct noeudL*)malloc(sizeof(struct noeudL));

	struct noeudL* ptrINS = findItemL(noVersion, nl);


	//Affectation des valeurs des champs
	ni->ligne.noligne	= nl;
	strcpy(ni->ligne.ptrligne, tl);
	sem_init(&ni->semL, 0, 1);


	if((ptrINS == NULL) && (nl == 1)) // ajout au debut de la liste vide
	{
		// premiere ligne premier noeud 
		ni->suivant= NULL;
		sem_wait(&ptrV->semDebutL);
		sem_wait(&ptrV->semFinL);
		ptrV->finL = ptrV->debutL = ni;
		sem_post(&ptrV->semFinL);
		sem_post(&ptrV->semDebutL);

	}
	else if ((ptrINS == NULL) && (nl > 1)) // ajout a la fin de la liste
	{
		sem_wait(&ptrV->semFinL); 
		sem_wait(&ptrV->finL->semL);
		struct noeudL* tptr = ptrV->finL;
		ni->suivant= NULL;
		ptrV->finL = ni;
		tptr->suivant = ni;
		sem_post(&tptr->semL); 
		sem_post(&ptrV->semFinL);
	}
	else
	{
		struct noeudL* tptr = ptrINS;
		sem_wait(&ptrV->semDebutL);
		if(tptr == ptrV->debutL){
			ptrV->debutL = ni;
			sem_wait(&ni->semL);
			sem_post(&ptrV->semDebutL);
		} 
		else
		{
			struct noeudL* ptrPREV = findPrevL(noVersion, nl);
			ptrPREV->suivant = ni;
			sem_wait(&ni->semL);
			sem_post(&ptrPREV->semL);
		}
		ni->suivant = tptr;
		sem_post(&ni->semL);
		
		while (tptr!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
			tptr->ligne.noligne++;
			//On retourne un pointeur sur l'item précédent
			
			struct noeudL* otptr = tptr;
			if(tptr->suivant != NULL)
			{
				sem_wait(&tptr->suivant->semL); // verrouiller le noeud suivant dans la liste de code
				//Déplacement du pointeur de navigation
				tptr=tptr->suivant;	
				sem_post(&otptr->semL); // deverrouiller le noeud courant dans la liste de code
			}
			else // tptr->suivant == NULL
			{
				//Déplacement du pointeur de navigation
				tptr=NULL;	
				sem_post(&otptr->semL);
			}
		}
	}

}

//#######################################
//#
//# Ajoute un item (noeud version) a la fin de la liste chaînée de VERSIONS
//#
void addItemV(struct paramAV* param){
char vl[100]; 
	int new, nv;
	nv = param ->nv;
	new = param->new;
	strcpy(vl,(const char*)param->tligne);
	free(param);


	//Création de l'enregistrement en mémoire
	struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));


	//Affectation des valeurs des champs de la struct noeudV
	ni->noVersion	= nv;
	strcpy(ni->ptrNoVersion, vl);
	ni->debutL	= NULL;
	ni->finL	= NULL;

	sem_init(&ni->semV, 0, 1);
	sem_init(&ni->semDebutL, 0, 1);
	sem_init(&ni->semFinL, 0, 1);
	
	if(new)
	{
	    ni->oldVersion	= FAUX; // 0 old 1 new
	    ni->commited	= FAUX; // never commited	  
	}
	else // old version
	{
	    ni->oldVersion	= VRAI; // 0 old 1 new
	    ni->commited	= FAUX; // never commited	  
	}
	  
	
	
	if(finV == NULL) // ajout au debut de la liste vide
	{
		sem_wait(&finV->semV);
		sem_wait(&debutV->semV);
		// premiere version au  premier noeud 
		ni->suivant= NULL;
		finV = debutV = ni;
		sem_post(&finV->semV);
		sem_post(&debutV->semV);

	}
	else  // ajout a la fin de la liste
	{
	// ajout a la fin de la liste de versions
	    struct noeudV* tptr = finV;
		sem_wait(&finV->semV);
	    ni->suivant= NULL;
	    finV = ni;
	    tptr->suivant = ni;	
		sem_post(&finV->semV);
	}

}

//#######################################
//#
//# Copier un item (noeud version) a la fin de la liste chaînée de VERSIONS (BRANCH)
//#
//void copyItemV(const int new, const int nv){
void copyItemV(struct paramCV* param){

	int new, nv;
	new = param ->new;
	nv = param ->nv;
	free(param);
  
	struct noeudV * ptrV = findItemV(nv); // ptr sur la version a copier 
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;

	//Création de l'enregistrement en mémoire
	struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));


	//Affectation des valeurs des champs de la struct noeudV
	ni->noVersion	= finV->noVersion+1;

	char nomVersion[200];
	sprintf(nomVersion,"V%d",ni->noVersion);
	strcpy(ni->ptrNoVersion, nomVersion); // copier le nouveau nom de version
	ni->debutL	= NULL;
	ni->finL	= NULL;
	
	if(new)
	{
	    ni->oldVersion	= FAUX; // 0 old 1 new
	    ni->commited	= FAUX; // never commited	  
	}
	else // old version
	{
	    ni->oldVersion	= VRAI; // 0 old 1 new
	    ni->commited	= FAUX; // never commited	  
	}

	
	// ajout a la fin de la liste de versions
	struct noeudV* tptr = finV;
	ni->suivant= NULL;
	finV = ni;
	tptr->suivant = ni;
	
	struct noeudL * ptrL = ptrV->debutL; // copier la liste de code de la version a la fin
	while (ptrL!=NULL){ 
	  //Affectation des valeurs des champs
	  //addItemL(finV->noVersion, ptrL->ligne.noligne, ptrL->ligne.ptrligne);
	  struct paramAL *addItemLptr = (struct paramAL*) malloc(sizeof(struct paramAL));
	  addItemLptr->noVersion = finV->noVersion;
	  addItemLptr->noligne = ptrL->ligne.noligne;
	  strcpy(addItemLptr->tligne,(const char *)ptrL->ligne.ptrligne);
	  addItemL(addItemLptr);

	  ptrL = ptrL->suivant;
        }

}


//#######################################
//#
//# Retire un item de la liste chaînée de versions
//#

//void removeItemV(const int noV){
void removeItemV(struct paramEV* param){
	int noV = param->nv;
	free(param);

	struct noeudV * ptrV;
	struct noeudV * tptrV;
	struct noeudV * optrV;
	struct noeudL * ptrL;
	struct noeudL * ptrLSuivant;
	
	char texteVersion[100];


	//Vérification sommaire (no>0 et liste non vide)
	
	if ((noV<1)||((debutV==NULL)&&(finV==NULL)))
		return;

	//Pointeur de recherche
	if(noV==1){
		ptrV = debutV; // suppression du premier element de la liste de versions
	}
	else{
		ptrV = findPrevV(noV);
	}
	//L'item a été trouvé
	if (ptrV!=NULL){
		// destruction des semafores; Une fois détruites, plus besoin de se soucis du problèmes d'exclusion
		sem_destroy(&ptrV->semV);
		sem_destroy(&ptrV->semDebutL);
		sem_destroy(&ptrV->semFinL);
		// Memorisation du pointeur de l'item en cours de suppression
		// Ajustement des pointeurs
		if((debutV == ptrV) && (noV==1)) // suppression de l'element de tete
		{
			if(debutV==finV) // un seul element dans la liste des version
			{
			    ptrL = ptrV->debutL; // supprimer la liste de code de la version
			    while (ptrL!=NULL){ 
				 ptrLSuivant = ptrL->suivant;
				 free(ptrL); // suppression du noeud de la liste de code 
				 ptrL=ptrLSuivant;
			    }
				
			    free(ptrV);
			    finV = debutV = NULL;
			    return;
			}
			tptrV = ptrV->suivant;
			debutV = tptrV;
			ptrL = ptrV->debutL; // supprimer la liste de code de la version
			while (ptrL!=NULL){ 
			    ptrLSuivant = ptrL->suivant;
			    free(ptrL); // suppression du noeud de la liste de code 
			    ptrL=ptrLSuivant;
			 }
			free(ptrV);
		}
		else if (finV==ptrV->suivant) // suppression de l'element de queue
		{
			finV=ptrV;
			ptrL = ptrV->suivant->debutL; // supprimer la liste de code de la version a la fin
			while (ptrL!=NULL){ 
				 ptrLSuivant = ptrL->suivant;
				 free(ptrL); // suppression du noeud de la liste de code 
				 ptrL=ptrLSuivant;
			 }
			free(ptrV->suivant);
			ptrV->suivant=NULL;
			return;
		}
		else // suppression d'un element dans la liste
		{
			optrV = ptrV->suivant;
			ptrL = ptrV->suivant->debutL; // supprimer la liste de code de la version a la fin
			while (ptrL!=NULL){ 
				 ptrLSuivant = ptrL->suivant;
				 free(ptrL); // suppression du noeud de la liste de code 
				 ptrL=ptrLSuivant;
			 }
			ptrV->suivant = ptrV->suivant->suivant;
			tptrV = ptrV->suivant;
			free(optrV);
		}
		
		
		while (tptrV!=NULL){ // ajautement des numeros de version

		//Est-ce le prédécesseur de l'item recherché?
			tptrV->noVersion--;
			//On retourne un pointeur sur l'item précédent
			
			sprintf(texteVersion, "V%d",tptrV->noVersion);
			strcpy(tptrV->ptrNoVersion, texteVersion);
			
		

		//Déplacement du pointeur de navigation
			tptrV=tptrV->suivant;
		}
	}
}



//#######################################
//#
//# Retire un item de la liste chaînée de code d'une version noVersion
//#
//void removeItemL(const int noVersion, const int noline){
void removeItemL(struct paramEL* param){
	int noVersion, noline;
	noVersion = param ->noVersion;
	noline = param ->noline;
	free(param);
	struct noeudL * ptrL;
	struct noeudL * tptrL;
	struct noeudL * optrL;
	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;

	//Vérification sommaire (no>0 et liste non vide)
	
	if ((noline<1)||((ptrV->debutL==NULL)&&(ptrV->finL==NULL)))
		return;

	//Pointeur de recherche
	if(noline==1){
		ptrL = ptrV->debutL; // suppression du premier element de la liste
	}
	else{
		ptrL = findPrevL(noVersion, noline);
	}
	//L'item a été trouvé
	if (ptrL!=NULL){
		// destruction du sémafore. Une fois détruite, plus besoin de se soucier du problème d'exclusion mutuelle
		sem_destroy(&ptrL->semL);

		// Memorisation du pointeur de l'item en cours de suppression
		// Ajustement des pointeurs
		if((ptrV->debutL  == ptrL) && (noline==1)) // suppression de l'element de tete
		{
			if(ptrV->debutL == ptrV->finL) // un seul element dans la liste de code
			{
				free(ptrL);
				ptrV->debutL = ptrV->finL = NULL;
				return;
			}
			tptrL = ptrL->suivant;
			ptrV->debutL = tptrL;
			free(ptrL);
		}
		else if (ptrV->finL == ptrL->suivant) // suppression de l'element de queue
		{
			ptrV->finL = ptrL;
			free(ptrL->suivant);
			ptrL->suivant=NULL;
			return;
		}
		else // suppression d'un element dans la liste de code
		{
			optrL = ptrL->suivant;	
			ptrL->suivant = ptrL->suivant->suivant;
			tptrL = ptrL->suivant;
			free(optrL);
		}
		
		
		while (tptrL!=NULL){ // ajautement des numeros de ligne

		//Est-ce le prédécesseur de l'item recherché?
			tptrL->ligne.noligne--;
			//On retourne un pointeur sur l'item précédent	

		//Déplacement du pointeur de navigation
			tptrL=tptrL->suivant;
		}
	}
}

//#######################################
//#
//# Modifie un item de la liste chaînée
//#
//void modifyItemL(const int noVersion, const int noline, const char* tline){
void modifyItemL(struct paramML* param){
	int noVersion, noline;
	char* tline;
	noVersion = param ->noVersion;
	noline = param ->noline;
	strcpy(tline,(const char*)param ->tline);
	free(param);
	struct noeudV * ptrV;

	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;
	//Vérification sommaire (no>0 et liste non vide)

	if ((noline<1)||((ptrV->debutL==NULL)&&(ptrV->finL==NULL)))
		return;


	//Recherche de l'élément à modifier
	struct noeudL * ptrL = findItemL(noVersion, noline);

	//L'élément à été trouvé
	if (ptrL!=NULL){

		//Modification des champs de l'élément trouvé en s'assurant d'avoir mutuellement exclu les threads
		sem_wait(&ptrL->semL);
		strcpy(ptrL->ligne.ptrligne, tline);
		sem_post(&ptrL->semL);

	}
}

//#######################################
//#
//# Affiche les items dont le numéro séquentiel est compris dans une plage de numero de version
//#
//void listItemsV(const int start, const int end){
void listItemsV(struct paramLV* param){
	int start, end;
	start = param -> start;
	end = param ->end;
	free(param);
	sem_wait(&semAffichage);
	//Affichage des entêtes de colonnes
	printf("noVersion  Nom Version                                  \n");
	printf("======= ================================================\n");
	sem_post(&semAffichage);

	struct noeudV * ptr = debutV;			//premier element de la liste des version


	while (ptr!=NULL){

		//L'item a un numéro séquentiel dans l'interval défini
		if ((ptr->noVersion>=start)&&(ptr->noVersion<=end)){
			sem_wait(&semAffichage);
			printf("%d \t %s\n",
				ptr->noVersion,
				ptr->ptrNoVersion);
			sem_post(&semAffichage);
			}
		if (ptr->noVersion>end){
			//L'ensemble des items potentiels sont maintenant passés
			//Déplacement immédiatement à la FIN de la liste des versions
			//Notez que le pointeur optr est toujours valide
			ptr=NULL;
			}
		else{
			ptr = ptr->suivant;
		}

	}

	//Affichage des pieds de colonnes
	sem_wait(&semAffichage);
	printf("======= ===============================================\n\n");
	sem_post(&semAffichage);
	}
	
	
//#######################################
//#
//# Affiche les items dont le numéro séquentiel est compris dans une plage de numero de ligne de code
//#
//void listItemsL(const int noVersion, const int start, const int end){
void listItemsL(struct paramLL* param){
	int noVersion, start, end;
	noVersion = param ->noVersion;
	start = param ->start;
	end = param ->end;
	free(param);
  
	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;
	
	// Verouillage de l'affichage
	sem_wait(&semAffichage);
	printf("noVersion noligne	texte                                          \n");
	printf("=======   ===========   ==================================\n");
	sem_post(&semAffichage);

	struct noeudL * ptr = ptrV->debutL;			//premier element de la liste de code d'une version


	while (ptr!=NULL){

		//L'item a un numéro séquentiel dans l'interval défini
		if ((ptr->ligne.noligne>=start)&&(ptr->ligne.noligne<=end)){
			sem_wait(&semAffichage);
			printf("%d \t %d \t %s\n",
				ptrV->noVersion,
			        ptr->ligne.noligne,
				ptr->ligne.ptrligne);
			sem_post(&semAffichage);
			}
		if (ptr->ligne.noligne>end){
			//L'ensemble des items potentiels sont maintenant passés
			//Déplacement immédiatement à la FIN de la liste
			//Notez que le pointeur optr est toujours valide
			ptr=NULL;
			}
		else{
			ptr = ptr->suivant;
		}

	}

	//Affichage des pieds de colonnes
	sem_wait(&semAffichage);
	printf("=========================================================\n\n");
	sem_post(&semAffichage);
	}

