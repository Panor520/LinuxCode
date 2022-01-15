#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void IsFile(char * name);
void IsDir(char * name);

void IsDir(char * name)
{
	char path[258];

	DIR * dr;
	dr = opendir(name);
	if(dr == NULL){
		perror("open dir error\n");
		exit(1);
	}
	
	struct dirent * dt;
	while( (dt = readdir(dr)) != NULL ){
		
		if(strcmp(dt->d_name,".")==0 || strcmp(dt->d_name,"..")==0 ){
			continue;
		}
		
		sprintf(path,"%s/%s",name,dt->d_name);	
		if(strlen(path)>256){
			perror("path length can't greater than 256\n");
			exit(0);
		}

		IsFile(path);

	}
	
	
	closedir(dr);
}

void IsFile(char * name)
{
	struct stat  statbuf;
	int ret;
	ret = stat(name,&statbuf);
	if( ret == -1 ){
		perror("stat error \n");
		exit(0);
	}

	if( (statbuf.st_mode & S_IFMT) == S_IFDIR ){
		IsDir(name);
	}else{
		printf("%ld %s\n",statbuf.st_size,name);	
	}

	return;
}



int main(int argc,char * argv[])
{
	if( argc == 1 ){
		IsFile(".");
	}else{
		while(argc-- > 0)
			IsFile(*(argv++));
	}

	return 0;
}

