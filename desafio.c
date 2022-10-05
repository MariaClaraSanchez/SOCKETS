#include <stdio.h>
#include <string.h>

typedef struct cont{
	char nome[100];
	char endereco[200];
	int idade;
}Contato; 

int exibeMenu();
int getFileSize();
void getContato(Contato *contato);
void exibeContato(Contato *contato);
void exibeContatos(FILE *fp);
void insertContato(Contato contato, FILE *fp);

int main(){
    FILE *fp;
    Contato contato;
    int op, sz;
    fp = fopen("dados.dat", "rb+");

    if(fp == NULL){
        fp = fopen("dados.dat", "wb+");
        if(fp == NULL){
            printf("\nErro na criacao do arquivo!");
            return 1;
        }
    }

    do{
        op = exibeMenu();
        if(op != 3){
            switch(op){
                case 1: getContato(&contato);
                        sz = getFileSize(fp);
                        if(sz == 0){
                            fwrite(&contato, sizeof(Contato), 1, fp);
                        }else{
                            insertContato(contato, fp);
                        }
                        
                        break;
                case 2: exibeContatos(fp);
                        break;
                case 3: 
                        break;
                default: printf("\nOpcao invalida!!!");
            }
        }

    }while(op!=3);

    fclose(fp);
    return 0;
}

int exibeMenu(){
    int op;
    printf("\n1-) Cadastrar novo contato");
    printf("\n2-) Exibir todos os contatos");
    printf("\n3-) Sair\n");
    scanf("%d", &op);
    return op;
}

int getFileSize(FILE *fp){
    int sz;
    rewind(fp);
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    return sz;
}

void getContato(Contato *contato){
    printf("\nDigite o nome: ");
    scanf(" %100[0-9a-zA-Z ]", contato->nome);
    printf("\nDigite o endereco: ");
    scanf(" %100[0-9a-zA-Z ]", contato->endereco);
    printf("\nDigite a idade: ");
    scanf("%d", &contato->idade);
}

void exibeContato(Contato *contato){
    printf("\nNome: %s", contato->nome);
    printf("\nEndereco: %s", contato->endereco);
    printf("\n%d", contato->idade);
}

void insertContato(Contato c, FILE *fp){
    Contato cAux;
    int achei=0, nR=0;
    rewind(fp);

    //se arquivo vazio
    if((nR = fread(&cAux,sizeof(Contato),1,fp)) == 0){
        fwrite(&c,sizeof(Contato),1,fp);
        return;
        
    }
    rewind(fp);
    
    //procurando a posicao certa
    do{
        nR = fread(&cAux,sizeof(Contato),1,fp);
        if(nR == 1){
            if(strcmp(cAux.nome,c.nome) > 0){
                achei = 1;
            }
        }        
    }while(achei == 0 && nR == 1);

    //se o que estou guardando eh o ultimo nome
    if(achei == 0){
        fwrite(&c,sizeof(Contato),1,fp);
    }
    else{ //senao, devo fazer o shift
        do{
            fseek(fp,-sizeof(Contato),SEEK_CUR);
            fwrite(&c,sizeof(Contato),1,fp);
            c = cAux;
            nR = fread(&cAux,sizeof(Contato),1,fp);
        }while(nR == 1);
        
        fwrite(&c,sizeof(Contato),1,fp);
    }

    return;
}

void exibeContatos(FILE *fp){
    Contato aux;
    rewind(fp);
    while(fread(&aux, sizeof(Contato), 1, fp)){
        exibeContato(&aux);
    }
}