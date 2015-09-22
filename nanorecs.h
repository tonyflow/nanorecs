#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct metadata{
	
	int set; //revision number : first digit
	int version; //revision number : second digit
	int checkindate;
	int stable;
	char comments[100];
	char name[64]; 

};

typedef struct metadata METADATA;

struct ufile{

	METADATA meta;
	int ufiledesc;

};

typedef struct ufile UFILE;

typedef struct tree_node TREEN;

struct tree_node {

	TREEN** childs;
	UFILE* ufiles;
	int originalfiledesc;

};

//globals

METADATA* RmetaArray;
int total_number_of_versions;
int max;
