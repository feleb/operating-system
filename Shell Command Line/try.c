#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <string.h>
#include<errno.h>
//#include "file.h"
#define DELIM " \t\n"
#define BUFSIZE 600

FILE *history;

int home=0;
char line[600],first[100],*filename;
void handler(){
	FILE *LOG=fopen("log.txt","a");
	fputs("CHILD FINISHED\n",LOG);
	fclose(LOG);
	
}	
void set_enviro(){
	char *var,*val;
	var=strtok(line,"=");
	if(strcmp(var,"HOME")==0)home=1;	
	val=strtok(NULL,"=");
	setenv(var,val,1);
}

void excute (char **arg,int background){
	signal(SIGCHLD, handler); 
	 int pid=fork();
	if(strcmp(arg[0],"exit")==0)exit(0);
     if(pid==0){
      
	if(execv(arg[0],arg)!=-1)return;  
	
	int bufsize = 600, position = 0;
      char **paths = malloc(bufsize * sizeof(char*));
      char *token;
      token = strtok(getenv("PATH"),":");
      while (token != NULL) {
      paths[position++] = token;
      token = strtok(NULL,":");
      }
      paths[position] = NULL;
      for(int i=0;i<position;i++){
         char currpath[100] ;
	strcpy(currpath,paths[i]);
	strcat(currpath,"/");
         strcat(currpath,arg[0]);
	  if((i+1)==position &&execv(currpath,arg)==-1){
		fprintf(stderr,"COMMAND NOT FOUND\n"); exit(0);}	 
	 execv(currpath,arg);      
}
  }

  else {
    if(!background) wait(NULL);
  }
}


int interactive_OR_batchfile(){
	fgets(first ,100,stdin);
	filename=strtok(first," \n");
	filename=strtok(NULL," \n");
	if(fopen(filename,"r")!=NULL)
		return 1;
	
	return 0;
}
void read_parse(){

	int len=strlen(line);
	if(len>512){
		fprintf(stderr,"A very long command line\n");
      		return ;
	}

	if(strcmp(line,"\n")==0 || line[0]=='#') return ;

	int background =0; 
	if(line[len-2]=='&'){
		background=1;
	}

	for(int i=0;i<len;i++){
		if(line[i]=='='){
		set_enviro();
		return ;
	}
   }

   if(strstr(line,"echo $")!=NULL){

	char* getenviro=NULL;
	getenviro =strtok(line,"\n echo $");
   	printf(" %s\n", getenv(getenviro));
	return;
  }
 if(strstr(line,"cd")!=NULL){
	for(int i=0;i<len;i++){
		if(line[i]=='~'){
			if(home==1)fprintf(stderr,"error path or directory\n");
			else chdir(getenv("HOME"));
			return ;}		
	}	
	char *to;
	to=strtok(line," ");
	to=strtok(NULL," \n");
	int ret=chdir(to);//return 0 if the path is correct
	if(ret)fprintf(stderr,"error path or directory\n");
	return ;	
}

int bufsize = BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token,*expression;
int f=0;
  token = strtok(line, DELIM);
  while (token != NULL) {
	 if(strcmp(token,"&")!=0) tokens[position++] = token;

    token = strtok(NULL, DELIM);
  }
  tokens[position] = NULL;
excute(tokens,background);
}

void read_interactive(){
	fgets(line ,600,stdin);
	if(strstr(line,"history")!=NULL){		
		history=fopen("xx.txt","r");	
		char read[250];
		while(fgets(read, 250, (FILE*)history)!=NULL){
			printf("%s", read);	
		}
		
	}	
	else {
	history=fopen("xx.txt","a");
    	fprintf(history,"%s",line);
	fclose(history);
	read_parse();
	}
		
}

int read_batchfile(char *filename){
	FILE *fp;
	fp=fopen(filename,"r");
	while(fgets(line,600,fp)!=NULL){	
		read_parse();	
	}

	return 0;
}

int main(){
	
	int batch=0;
	batch=interactive_OR_batchfile();
	if(batch==0)perror("FILE is not exist\n");	
	else {	
		read_batchfile(filename);
		return 0;
	}	
	
	while(1){
	printf("shell>");
	read_interactive();	
	//read_parse();	
	
	}

    return 0;
}
