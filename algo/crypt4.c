#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void set_text_file(char** texte,char* path,int line);

char* binaire(char tmp){
	int nombre = tmp;
	char * tab = calloc(8,sizeof(char));
	
	int quotient = 0;
	
	for(int i=0 ; i<8 ; i++){
		quotient = nombre/2;
		tab[7-i] = nombre%2 + 48 ;
		nombre = quotient;
	}
	return tab;
}

char debinaire(char* tab){
	int nombre = 0;
	for (int i=0 ; i<8 ; i++){
		nombre += (tab[7-i]-48)*pow(2,i);
	}
	char tmp = nombre;
	return tmp;
}

void crypto(char** text,char** textc,int line,char* key){
	
	for(int i=0 ; i<strlen(key) ; i++){
		if(key[i]<0) key[i]+=126;
	}
	
	char tmp;
	
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<1;i++){
		for(int j=0; j<strlen(text[i]) ;j++){
			char * tab;
			if(text[i][j]<=30 && text[i][j]!=9){
				tmp = text[i][j];
			}
			else{
				if((int)text[i][j]==9) text[i][j]=31;
				tmp = (((int)text[i][j] + (int)key[index] - 61) % 96) +31;
				if(tmp==31) tmp=9;
				index++;
				if(index==Imax) index=0;
			}
			tab = binaire(tmp);
			strcat(textc[i],tab);
			free(tab);
		}
	}
}

void decrypto(char** text,char** textd,int line,char* key){
	
	for(int i=0 ; i<strlen(key) ; i++){
		if(key[i]<0) key[i]+=126;
	}
	
	char * tab = calloc(8,sizeof(char));
	char tmp;
	
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<line;i++){
		for(int j=0, l=0; j<strlen(text[i]) ;j+=8, l++){
			for(int k=0 ; k<8 ; k++) tab[k]=text[i][j+k];
			tmp = debinaire(tab);
			
			if(tmp<=30 && tmp!=9){
				textd[i][l]=tmp;
			}
			else{
				if(tmp==9) tmp=31;
				textd[i][l]=((int)tmp - 31) + 61 - (int)key[index];
				if(textd[i][l]<31) textd[i][l]=(int)textd[i][l] + 96;
				if(textd[i][l]==31) textd[i][l]=9;
				index++;
				if(index==Imax) index=0;
			}
		}
	}
	free(tab);
}

char** allouer(int ligne,int colonne){
	char **tab=NULL;
	tab=(char**)calloc(ligne,sizeof(char*));
	for(int i=0;i<ligne;i++){
		*(tab+i)=(void*)calloc(colonne,sizeof(char));
	}
	return tab;
}

void get_text_file(char** texte,char* path){
	FILE *file1 = fopen(path,"r");
	if(file1==NULL) printf("Erreur d'ouverture!");
	int index=0;
	while(fgets(texte[index],500,file1)!=NULL){
		index++;
	}
	fclose(file1);
}

int getLineNumber(char* path){
	int line = 0;
	char wc[200];
	char nb[30]="/tmp/crypta";
	
	sprintf(wc,"wc -l < %s > %s",path,nb);
	system(wc);
	
	char tab[10];
	FILE *file1 = fopen(nb,"r");
	if(file1==NULL) return 0;
	fgets(tab,10,file1);
	fclose(file1);
	line = atoi(tab) + 1;
	
	//system("rm /tmp/crypta");
	return line;
}

void set_text_file(char** texte,char* path,int line){
	FILE *file = fopen(path,"w");
	if(file==NULL) printf("Erreur d'ouverture!");
	for(int i=0; i<line; i++){
		fprintf(file,"%s",texte[i]);
	}
	fclose(file);
}

int cryptage_file(char* path, char* key){
	int line = getLineNumber(path);
	
	char** text = allouer(line,250);
	char** textc = allouer(line,8*250);
	
	get_text_file(text,path);
	crypto(text,textc,line,key);
	set_text_file(textc,path,line);
	free(text);
	free(textc);
	return 0;
}

int decryptage_file(char* path, char* key){
	int line = getLineNumber(path);
	
	char** text = allouer(line,8*250);
	char** textd = allouer(line,250);
	
	get_text_file(text,path);
	decrypto(text,textd,line,key);
	set_text_file(textd,path,line);
	free(text);
	free(textd);
	return 0;
}

int decryptage_view(char* text,char* textd, char* key){
	decrypto(&text,&textd,1,key);
	return 0;
}

int cryptage_view(char* text,char* textd, char* key){
	crypto(&text,&textd,1,key);
	return 0;
}

