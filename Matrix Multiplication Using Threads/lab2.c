#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>
int r1,c1,r2,c2;
int **mat1,**mat2,**matout1,**matout2;           //matrix input and output
char input1[50],input2[50],out1[50],out2[50];	//names of two input file and output file
struct timeval stop1,start1,stop2,start2;	//struct to get time exec


void *Method2(void *arg){ //method called by pthread_creat for Multiply_By_Rows
	int *r=(int *)arg;
	for(int i=0;i<c2;i++){
		matout2[r[0]][i]=0;
		for(int j=0;j<r2;j++){
			matout2[r[0]][i]+=(mat1[r[0]][j]*mat2[j][i]);
		}	
	}
	pthread_exit(NULL);
}

void Multiply_By_Rows(){  //second method to execute matrix mulyiplication
	pthread_t pt[r1];

	for(int i=0;i<r1;i++){
		int *r=malloc(1*sizeof(int*));		
		r[0]=i;	
		if(pthread_create(&pt[i],NULL,Method2,r)){
			printf("can't create a thread");
				exit(1);
		}
	}
	for(int k=0;k<r1;k++) 
		pthread_join(pt[k],NULL);		
}

void *method1 (void *arg){ //method called by pthread_creat for Multiply_By_Elements
	int *rc=(int *)arg;
	matout1[rc[0]][rc[1]]=0;
	for(int i=0;i<r2;i++){
		matout1[rc[0]][rc[1]]+=(mat1[rc[0]][i]*mat2[i][rc[1]]);	
	}
	pthread_exit(NULL);
}

void Multiply_By_Elements(){ // first method to execute matrix mulyiplication
	
	pthread_t p[r1*c2];

	int index=0;
	for(int i=0;i<r1;i++){
		for(int j=0;j<c2;j++){
			int *rc=malloc(2*sizeof(int*));
			rc[0]=i;
			rc[1]=j;
			if(pthread_create(&p[index++],NULL,method1,rc)){
				printf("can't create a thread");
				exit(1);
			}	
					  	
		}		
	}
	for(int i=0;i<index;i++)pthread_join(p[i],NULL);
}

void read_matrix(char input[], int***ptr,int id){  // store integers into matrics from input files
	FILE *f=fopen(input,"r");
	char line[100];	
	fgets(line,100,f);		
	int n,m;
	sscanf(line,"row=%d col=%d",&n,&m);
	if(id==1){r1=n;c1=m;}
	else {r2=n;c2=m;}	
	*ptr=malloc(n * sizeof(int *));
	int num=0;	
	for(int i=0;i<n;i++){
		(*ptr)[i]=malloc(m * sizeof(int*));
		for(int j=0;j<m;j++){
			fscanf(f,"%d",&num);
			(*ptr)[i][j]=num;		
		}
     }
}


void read_line(int args,char *arg[]){ //  decide if the user input its data or let it to default
 
	if(args==1){	
		strcpy(input1,"a.txt");// default files name
		strcpy(input2,"b.txt");
		strcpy(out1,"c1.txt");
		strcpy(out2,"c2.txt");
	}
	else {
		strcpy(input1,arg[1]);
		strcat(input1,".txt");
		strcpy(input2,arg[2]);
		strcat(input2,".txt");
		strcpy(out1,arg[3]);
		strcat(out1,"1.txt");
		strcpy(out2,arg[3]);
		strcat(out2,"2.txt");
	}
		read_matrix(input1,&mat1,1);
		read_matrix(input2,&mat2,2);
}

void write_output(char out[],int **matrix){ // write the output matrix into files
	FILE *file=fopen(out,"w");
	fprintf(file,"%s","row=");
	fprintf(file,"%d ",r1);

	fprintf(file,"%s","col=");
	fprintf(file,"%d\n",c2);
	for(int i=0;i<r1;i++){
		for(int j=0;j<c2;j++)fprintf(file,"%d  ",matrix[i][j]);	
		fprintf(file,"%s","\n");
	}

}

void print(){
	printf("The First Matrix\n");
	for(int i=0;i<r1;i++){
		for(int j=0;j<c1;j++){
			printf("%d  ",mat1[i][j]);
		}printf("\n");
	}

	
	printf("The Second Matrix\n");
	for(int i=0;i<r2;i++){
		for(int j=0;j<c2;j++){
			printf("%d  ",mat2[i][j]);
		}printf("\n");
	}
	printf("Method that multiply by element\n");
	printf("Number of Threads : %d\n",r1*c2);
	printf("Time excution in second : %lu\n",stop1.tv_sec - start1.tv_sec);
	printf("Time excution in Microsecond : %lu\n",stop1.tv_usec - start1.tv_usec);
	
	printf("The Matrix Output\n");	
	for(int i=0;i<r1;i++){
		for(int j=0;j<c2;j++){
			printf("%d  ",matout1[i][j]);
		}printf("\n");
	}
	printf("===============================\n");
	printf("Method that multiply by Rows\n");
	printf("Number of Threads : %d\n",r1);
	printf("Time excution in second : %lu\n",stop2.tv_sec - start2.tv_sec);
	printf("Time excution in Microsecond : %lu\n",stop2.tv_usec - start2.tv_usec);
	
	printf("The Matrix Output\n");	
	for(int i=0;i<r1;i++){
		for(int j=0;j<c2;j++){
			printf("%d  ",matout2[i][j]);
		}printf("\n");
	}
	
}

int main(int args ,char *arg[]){
	
	read_line(args,arg); // read the only input line
	if(c1!=r2){fprintf(stderr,"cols at matrix1 not equal rows at matrix 2"); return 0;}
	
	matout2=malloc(r1*sizeof(int *));
	matout1=malloc(r1*sizeof(int *));
	for(int j=0;j<c2;j++) {
		matout2[j]=malloc(c2*sizeof(int *));
		matout1[j]=malloc(c2*sizeof(int *));
	}
	gettimeofday(&start1,NULL);
	Multiply_By_Elements();
	gettimeofday(&stop1,NULL);
	
	gettimeofday(&start2,NULL);	
	Multiply_By_Rows();
	gettimeofday(&stop2,NULL);

	write_output(out1,matout1);
	write_output(out2,matout2);
	
	print();	
	return 0;
}
