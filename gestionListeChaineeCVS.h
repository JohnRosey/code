#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define FAUX 0
#define VRAI 1

struct infoligne{						
	int		noligne;				
	char	ptrligne[100];							
	};								 

struct noeudL{			
	struct infoligne	ligne;		
	struct noeudL		*suivant;	
	sem_t semL;
	};
	
struct noeudV{	
	int noVersion;
	char	ptrNoVersion[100];
	int oldVersion;
	int commited;
	sem_t semDebutL;
	sem_t semFinL;
	sem_t semV;
	struct noeudL		*debutL;
	struct noeudL		*finL;
	struct noeudV		*suivant;	
	};

struct paramAL{
	int noVersion;
    int noligne;
	char tligne[100];
	};

struct paramAV{
	int new;
    int nv;
	char tligne[100];
	};

struct paramCV{
	int new;
	int nv;
};


struct paramEV{
	int nv;
};


struct paramEL{
	int noVersion;
	int noline;
};

struct paramML{
	int noVersion;
	int noline;
	char tline[100];
};


struct paramLV{
	int start;
	int end;
};


struct paramLL{
	int noVersion;
	int start;
	int end;
};

struct paramSL{
	int noVersion;
};

struct paramX{
	char sourcefname[255];
};

sem_t semAffichage;	
	
void cls(void);
void error(const int exitcode, const char * message);

struct noeudL * findItemL(const int noV, const int no);
struct noeudL * findPrevL(const int noV, const int no);
struct noeudV * findItemV(const int no);
struct noeudV * findPrevV(const int no);

void addItemV(struct paramAV* param);
void addItemL(struct paramAL* param);		
void copyItemV(struct paramCV* param);
void removeItemV(struct paramEV* param);
void removeItemL(struct paramEL* param);
void modifyItemL(struct paramML* param);
void listItemsV(struct paramLV* param);
void listItemsL(struct paramLL* param);
void saveItemsV();
void saveItemsL(struct paramSL* param);
void executeFile(struct paramX* param);
void listFileC();


void loadVersions();
void loadVersion(const int noV);
void* readTrans(char* nomFichier);
