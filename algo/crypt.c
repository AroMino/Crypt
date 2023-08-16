#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void setText(char** texte,char* path,int line);

void crypt(char** text,char** textc,int line,char* key){
	
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<line;i++){
		for(int j=0; j<strlen(text[i]) ;j++){
			if(text[i][j]<=31){
				textc[i][j]=text[i][j];
			}
			else{
				textc[i][j]=(((int)text[i][j] + (int)key[index] - 63) % 95) +32;
				index++;
				if(index==Imax) index=0;
			}
		}
	}
}

void decrypt(char** text,char** textd,int line,char* key){
	
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<line;i++){
		for(int j=0; j<strlen(text[i]) ;j++){
			if(text[i][j]<=31){
				textd[i][j]=text[i][j];
			}
			else{
				textd[i][j]=((int)text[i][j] - 32) + 63 - (int)key[index];
				if(textd[i][j]<32) textd[i][j]=(int)textd[i][j] + 95;
				index++;
				if(index==Imax) index=0;
			}
		}
	}
}

char** allouer(int ligne,int colonne){
	char **tab=NULL;
	tab=(char**)malloc(sizeof(char*)*ligne);
	for(int i=0;i<ligne;i++){
		*(tab+i)=(void*)calloc(colonne,sizeof(char));
	}
	return tab;
}

void getText(char** texte,char* path,int *line){
	FILE *file1 = fopen(path,"r");
	if(file1==NULL) printf("Erreur d'ouverture!");
	
	while(fgets(texte[*line],sizeof(char)*100,file1)!=NULL){
		(*line)++;
	}
	fclose(file1);
}

void setText(char** texte,char* path,int line){
	FILE *file = fopen(path,"w");
	if(file==NULL) printf("Erreur d'ouverture!");
	for(int i=0;i<line;i++){
		fprintf(file,"%s",texte[i]);
	}
	fclose(file);
}

int main(int argc, char** argv){
	
	if(argc==3){
		if(strcmp("-c", argv[1])==0){
			char** text=allouer(1000,500);
			char* key=(char*)malloc(sizeof(char)*100);
			printf("KEY : ");
			scanf("%s",key);
			
			int line=0;
			getText(text,argv[2],&line);
			char** textc=allouer(line,500);
			crypt(text,textc,line,key);
			setText(textc,argv[2],line);
			
			free(text);
			free(textc);
			free(key);
		}
		else if(strcmp("-d", argv[1])==0){
			char** text=allouer(1000,500);
			char* key=(char*)malloc(sizeof(char)*100);
			printf("KEY : ");
			scanf("%s",key);
			
			int line=0;
			getText(text,argv[2],&line);
			char** textd=allouer(line,500);
			decrypt(text,textd,line,key);
			setText(textd,argv[2],line);
			free(text);
			free(textd);
			free(key);
		}
		else{
			printf("TSY METY");
		}
	}
	else {
		printf("TSY METY");
	}
	
	return 0;
}
