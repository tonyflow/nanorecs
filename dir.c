#include <dirent.h>
#include <errno.h>
#include <stdio.h>

main(int argc,char* argv[]){

	struct dirent *direntp;
	DIR* dirp;

	dirp=opendir(argv[1]);

	while((direntp=readdir(dirp))!=NULL)
		printf("%s\n",direntp->d_name);

	while((closedir(dirp)==-1) && errno==EINTR);

}
