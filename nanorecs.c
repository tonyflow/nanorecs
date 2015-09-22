
#include "nanorecs.h"
#include <stdio.h>
#include <sys/stat.h> //for stat()
#include <sys/types.h> //for stat and DIR*
#include <sys/stat.h>
#include <dirent.h> //for DIR*
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>



main(int argc,char* argv[]){

	int date;
	int set; //revision number's first digit
	int version; //revision number's second digit
	char name[64];
	char param[16];
	
	if(argc<2){
		printf("Too few arguments\n");
		exit(1);
	}
	
	total_number_of_versions=0;
	max=100;
	
	RmetaArray=malloc(max*sizeof(METADATA));
	
	//PARSING
	if(strcmp(argv[1],"checkin")==0){
	
		if(strcmp(argv[2],"-r")==0){
			set=atoi(strtok(argv[3],"."));
			version=atoi(strtok(NULL,"."));
			strcpy(name,argv[4]);
			strcpy(param,argv[2]);
			
			checkin("RepositoryDir",name,param,set,version,0,0);

		}
		
		else if(strcmp(argv[2],"-d")==0){
			date=atoi(argv[3]);
			strcpy(name,argv[4]);
			strcpy(param,argv[2]);
			
			checkin("RepositoryDir",name,param,0,0,date,0);
		}
			
		else if(strcmp(argv[2],"-stable")==0){
			strcpy(name,argv[3]);
			strcpy(param,argv[2]);
			
			checkin("RepositoryDir",name,param,0,0,0,1);
			
		}
		
		else
			strcpy(name,argv[2]);
		
	
		
	
	}
	
	else if(strcmp(argv[1],"checkout")==0){
	
		if(strcmp(argv[2],"-r")==0){
			set=atoi(strtok(argv[3],"."));
			version=atoi(strtok(NULL,"."));
			strcpy(param,argv[2]);
		}
			
		else if(strcmp(argv[2],"-d")==0){
			atoi(argv[3]);
			strcpy(name,argv[4]);
			strcpy(param,argv[2]);
		}
			
		else if(strcmp(argv[2],"-stable")==0){}
	
	}
	
	else if(strcmp(argv[1],"tag")==0){}
	
	else if(strcmp(argv[1],"changes")==0){
	
		if(strcmp(argv[2],"-r")==0){
			set=atoi(strtok(argv[3],"."));
			version=atoi(strtok(NULL,"."));
		}

			
		else if(strcmp(argv[2],"-d")==0)
			atoi(argv[3]);
			
	}

	else if(strcmp(argv[1],"make")==0)
		//make repository directory for keeping the *.u files
		mkdir("RepositoryDir",O_RDWR);
	
	else if(strcmp(argv[1],"remove")==0)
		rmdir("RepositoryDir");
		
	//END OF PARSING
	

}


int checkin(char* dir,char* name,char* param,int set,int version,int date,int stable){

	//name is name of file or directory
	struct dirent *direntp;
	struct stat statbuf;
	DIR* dirp;
	METADATA* temp;
	char rdirectory[64];
	char rbuffer[64];
	int myfd;
	int i;
	int maxnos=0; //max number of set
	int maxnov=0; //max number of version
	
	

	//check if file already exists in repository
	if(strcmp(param,"-r")==0){
		for(i=0;i<total_number_of_versions;i++)
			if(strcmp(RmetaArray[i].name,name)==0 && RmetaArray[i].set==set && RmetaArray[i].version==version){
				printf("Version of %s with revision number %d.%d already exists in the nanorecs' repository\n\n",name,set,version);
				return -1;
			}
	}
	
	else if(strcmp(param,"-d")==0){
		for(i=0;i<total_number_of_versions;i++)
			if(strcmp(RmetaArray[i].name,name)==0 && RmetaArray[i].checkindate==date){
				printf("Version of %s with entry date %d already exists in the nanorecs' repository\n\n",name,date);
				return -1;
			}
	}
	
	else if(strcmp(param,"-stable")==0){
		for(i=0;i<total_number_of_versions;i++)
			if(strcmp(RmetaArray[i].name,name)==0 && RmetaArray[i].stable==1){
				printf("Stable version of %s already exists in the nanorecs' repository\n\n",name);
				return -1;
			}
	}
	
	//end of checking
	
	
	


	chdir(dir);
	//mkdir(name,O_RDWR);

	if(stat(name,&statbuf)==-1){
		perror("Failed to gain access");
		return -1;
	}
	else{
		//given name represents a folder
		if(statbuf.st_mode & S_IFMT == S_IFDIR){
			dirp=opendir(name);

			while((direntp=readdir(dirp))!=NULL){
				//ftiaxno to neo onoma katalogou gia thn anadromikh klisi ths checkin
				//gia na broume olous tous ypofakelous tou parent directory
				sprintf(rdirectory,"%s/%s/",dir,direntp->d_name);
				//dhmiourgo sto repository directory enan fakelo me to onoma tou fakelou
				//opou mesa tha mpoune eite alloi ypofakeloi eite arxeia an h ierarxia 
				//stamataei se auton
				
				mkdir(direntp->d_name,O_RDWR);
				checkin(rdirectory,direntp->d_name,NULL,set,version,date,stable);
				
			}
				
			while((closedir(dirp)==-1) && errno==EINTR);

		
		}
		
		//name given represents a file 
		else if(statbuf.st_mode & S_IFMT == S_IFREG){
			//one more version
			
			for(i=0;i<total_number_of_versions;i++)
				if(strcmp(RmetaArray[i].name,name)==0 && RmetaArray[i].set>maxnos){
					maxnos=set;
					if(RmetaArray[i].version>version)
						maxnov=version;
				}
			
			
			total_number_of_versions++;
			
			//create subfolder with the name of the file
			mkdir(name,O_RDWR);
			
			//enter subfolder
			chdir(name);
			bzero(rbuffer,sizeof(rbuffer));
			sprintf(rbuffer,"set%d",maxnos);
			
			//create SET subfolder
			mkdir(rbuffer,O_RDWR);
			
			//enter SET subfolder
			chdir(rbuffer);
			bzero(rbuffer,sizeof(rbuffer));
			sprintf(rbuffer,"%d.c",maxnov);
			
			//create delta file
			diff(name,name,rbuffer);			
			
			if(total_number_of_versions>=max){
				temp=realloc(RmetaArray,(max+100)*sizeof(METADATA));
				max+=100;
				if(temp!=NULL)
					RmetaArray=temp;
				else{
					printf("Cannot allocate any more memmory...\n");
					return 1;
				}
			}
			
			//new entry for new version
			for(i=0;i<total_number_of_versions;i++)
				if(RmetaArray[i].name==NULL){ //free slot
					strcpy(RmetaArray[i].name,name);
					RmetaArray[i].set=maxnos;
					RmetaArray[i].version=maxnov;
					RmetaArray[i].checkindate=date;
					RmetaArray[i].stable=stable;
					printf("Enter some comments about the new versin (100 char max) : ");
					scanf("%s",RmetaArray[i].comments);
					printf("\n\n");

				}
	
		}//close else if(statbuf.st_mode & S_IFMT == S_IFREG)
	
	}//close BIG else

}


int checkout(){




}


int tag(){







}


int changes(){







}


int diff(char* nfile,char* ofile,char* dfile){

	//nfile=new file
	//ofile=old file
	//dfile=delta file
	
	int fd[2];
	pid_t cpid;
	int ffd, n;
	char buf[10];

	if( pipe(fd) == -1) {
		perror("pipe");
		return 0;
	}

	if( (cpid = fork()) == -1 ) {
		perror("fork");
		return 0;
	}

	if( cpid == 0 ) {
		close(fd[0]);
		close(0);
		dup2(fd[1], 1); /*Child's stdout is set to write end of pipe*/
		execlp("diff", "diff", nfile, ofile, NULL);

   }
   
   else{
		close(fd[1]);
		if( (ffd = open(dfile, O_WRONLY | O_CREAT, 0666) ) == -1 ) {
			perror("open");
			return 0;
		}
   
		/*Parent reads from read end of pipe*/
		while( (n=read(fd[0], buf, sizeof(buf))) > 0 ) {

			if( write(ffd, buf, n) < 0 ) { 
				perror("write");
				return 0;
			}

		}
		close(fd[0]);

	}
}

int patch(void){

	int fd[2];
	pid_t cpid;
	int ffd, n;
	char buf[32];

	if( pipe(fd) == -1) {
		perror("pipe");
		return 0;
	}

	if( (cpid = fork()) == -1 ) {
		perror("fork");
		return 0;
	}

	if( cpid == 0 ) {

		close(fd[1]);
		close(0);
		dup2(fd[0], 0); /*Child's stdin is set to read end of pipe*/
		execlp("patch", "patch", "1.c", NULL);

	}
	
	else {

		close(fd[0]);
		if( (ffd = open("delta",O_RDONLY)) == -1 ) {
			perror("open");
			return 0;
		}
   
		/*Parent writes to write end of pipe*/
		while( (n=read(ffd, buf, sizeof(buf))) > 0 ) {

			if( write(fd[1], buf, n) < 0 ) { 
				perror("write");
				return 0;
			}

		}
		close(fd[1]);

	}

}