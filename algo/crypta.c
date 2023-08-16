#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>

int tab[5]={182,164,161,167,191};
int esp[5]={162,163,165,176,181};

void setText(wchar_t** texte,char* path,int line);

void crypt(wchar_t** text,wchar_t** textc,int line,char* path){
	srand(time(NULL));
	char key[20]="mafy";
	printf("Entrez votre Clé : ");
	//scanf("%s",key);
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<line;i++){
		for(int j=0; j<wcslen(text[i]) ;j++){
			if(text[i][j]<=126 && text[i][j]>=32){
				textc[i][j]=(((int)text[i][j] + (int)key[index] - 63) % 95) +32;
				index++;
				if(index==Imax) index=0;
			}
			else if(text[i][j]==9){
				int random=rand() %5;
				textc[i][j]=tab[random];
			}
			/*else if(text[i][j]==32){
				int random1=rand() %5;
				textc[i][j]=esp[random1];
			}*/
			else{
				textc[i][j]=text[i][j];
			}
		}
	}
	setText(textc,path,line);
}

void decrypt(wchar_t** text,wchar_t** textd,int line,char* path){
	char key[20]="mafy";
	printf("Entrez votre Clé : ");
	//scanf("%s",key);
	int Imax=strlen(key);
	int index=0;
	for (int i=0;i<line;i++){
		for(int j=0; j<wcslen(text[i]) ;j++){
			if(text[i][j]<=126 && text[i][j]>=32){
				textd[i][j]=((int)text[i][j] - 32) + 63 - (int)key[index];
				if(textd[i][j]<32) textd[i][j]=(int)textd[i][j] + 95;
				index++;
				if(index==Imax) index=0;
			}
			else if(text[i][j]==182 || text[i][j]==164 || text[i][j]==161 || text[i][j]==167 || text[i][j]==191){
				textd[i][j]=9;
			}
			/*else if(text[i][j]==162 || text[i][j]==163 || text[i][j]==165 || text[i][j]==176 || text[i][j]==181){
				textd[i][j]=32;
			}*/
			else{
				textd[i][j]=text[i][j];
			}
		}
	}

	setText(textd,path,line);
}

wchar_t** allouer(int ligne,int colonne){
	wchar_t **tab=NULL;
	tab=(wchar_t**)malloc(sizeof(wchar_t*)*ligne);
	for(int i=0;i<ligne;i++){
		*(tab+i)=(void*)calloc(colonne,sizeof(wchar_t));
	}
	return tab;
}

void getText(wchar_t** texte,char* path,int *line){
	FILE *file1 = fopen(path,"r");
	if(file1==NULL) printf("Erreur d'ouverture!");
	
	while(fgetws(texte[*line],100,file1)!=NULL){
		(*line)++;
	}
	fclose(file1);
}

void setText(wchar_t** texte,char* path,int line){
	FILE *file = fopen(path,"w");
	if(file==NULL) printf("Erreur d'ouverture!");
	for(int i=0;i<line;i++){
		setlocale(LC_ALL, "");
		fwprintf(file,L"%ls",texte[i]);
	}
	fclose(file);
}


int main(){
	setlocale(LC_ALL, "");
	char path[100]="/home/arrow/fraction.c";
	int line=0;
	wchar_t** text=allouer(202,255);
	getText(text,path,&line);
	wchar_t** textc=allouer(line,255);
	wchar_t** textd=allouer(line,255);
	
	int option;
	printf("CRYPTER (1)   DECRYPTER (2)\n");
	scanf("%d",&option);
	
	if(option==1){
		crypt(text,textc,line,path);
	}
	
	else if(option==2){
		decrypt(text,textd,line,path); 
	}
	
	
	
	free(text);
	free(textc);
	free(textd);
	
	return 0;
}
