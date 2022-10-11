// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

typedef struct cont
{
  char nome[100];
  char endereco[200];
  int idade;
} Contato;

typedef struct cone
{
    char ip[20];
    char porta[5];
} Conexao;


#define NTHREADS 2

int exibeMenu();
int getFileSize();
void * threadF(void * param);
void getContato(Contato *contato);
void exibeContato(Contato *contato);
void exibeContatos(FILE *fp);
void insertContato(Contato contato, FILE *fp);

int main(int argc, char const *argv[]){

  Conexao conexao;
  strcpy(conexao.ip, argv[1]);
  strcpy(conexao.porta, argv[2]);
  pthread_t thread_id[NTHREADS];

  for (int i = 0; i < NTHREADS; i++){
        printf("\nThread %d\n",i+1);
        pthread_create(&thread_id[i], NULL, threadF, &conexao);
    }

  for (int j = 0; j < NTHREADS; j++){
      printf("Finalizando Thread %d",j+1);
      pthread_join(thread_id[j], NULL);
    }

  return 0;
}

void * threadF(void * param){
  int sock = 0, client_fd;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};
  Conexao conexao = *(Conexao *)param;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(conexao.porta));

  // Convert IPv4 and IPv6 addresses from text to binary
  // form
  if (inet_pton(AF_INET, conexao.ip, &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
  }

  if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr,
                           sizeof(serv_addr))) < 0)
  {
    printf("\nConnection Failed \n");
  }

  Contato contato;
  int i,op, sz, cont=0, qtde=0;
  char pesquisaNome[100];
  char data[1024];

  do
  {
    op = exibeMenu();
    fflush(stdin);
    if (op != 4)
    {
      send(sock, &op, sizeof(op), 0);

      switch (op)
      {
        case 1:
          getContato(&contato);
          send(sock, &contato, sizeof(contato), 0);
          break;
        case 2:
          printf("\nCONTATOS CADASTRADOS:");
          read(sock, &cont, sizeof(cont));
          Contato *vet = (Contato *) malloc(cont*sizeof(Contato));
          read(sock, vet, cont* sizeof(Contato));
          //printf("CONT= %d\n", cont);
          for(i=0; i<cont; i++){
            printf("\nContato %d", i+1);
            exibeContato(&vet[i]);
            printf("\n");
          }
          printf("\n");

          break;
        case 3:
          printf("Digite o nome a ser pesquisado: ");
          scanf(" %100[0-9a-zA-Z ]", pesquisaNome);
          printf("Nome: %s\n", pesquisaNome);
          send(sock, pesquisaNome, sizeof(pesquisaNome), 0);
          read(sock, &qtde, sizeof(qtde));
          printf("Quantidade de contatos: %d\n\n", qtde);

          for(i=0; i<qtde; i++){
            read(sock, &contato, sizeof(Contato));
            printf("%s\n", contato.nome);
             printf("%s\n", contato.endereco);
             printf("%d\n", contato.idade);
             printf("--------------------\n");
          }

          break;
        default:
          printf("\nOpcao invalida!!!");
      }
    }

  } while (op != 4);

  // valread = read(sock, buffer, sizeof(buffer));
  // printf("%s\n", buffer);

  // closing the connected socket
  // send(sock, teste, strlen(teste), 0);
  close(client_fd);
}

int exibeMenu()
{
  int op;
  printf("\nESCOLHA UMA OPCAO:");
  printf("\n1-) Cadastrar novo contato");
  printf("\n2-) Exibir todos os contatos");
  printf("\n3-) Buscar contato por nome");
  printf("\n4-) Sair\n");
  scanf("%d", &op);
  return op;
}

int getFileSize(FILE *fp)
{
  int sz;
  rewind(fp);
  fseek(fp, 0L, SEEK_END);
  sz = ftell(fp);
  rewind(fp);
  return sz;
}

void getContato(Contato *contato)
{
  int sock = 0;
  printf("\nDigite o nome: ");
  scanf(" %100[0-9a-zA-Z ]", contato->nome);
  printf("\nDigite o endereco: ");
  scanf(" %100[0-9a-zA-Z ]", contato->endereco);
  printf("\nDigite a idade: ");
  scanf("%d", &contato->idade);
}

void exibeContato(Contato *contato)
{
  printf("\nNome: %s", contato->nome);
  printf("\nEndereco: %s", contato->endereco);
  printf("\n%d", contato->idade);
}

void insertContato(Contato c, FILE *fp)
{
  Contato cAux;
  int achei = 0, nR = 0;
  rewind(fp);

  // se arquivo vazio
  if ((nR = fread(&cAux, sizeof(Contato), 1, fp)) == 0)
  {
    fwrite(&c, sizeof(Contato), 1, fp);
    return;
  }
  rewind(fp);

  // procurando a posicao certa
  do
  {
    nR = fread(&cAux, sizeof(Contato), 1, fp);
    if (nR == 1)
    {
      if (strcmp(cAux.nome, c.nome) > 0)
      {
        achei = 1;
      }
    }
  } while (achei == 0 && nR == 1);

  // se o que estou guardando eh o ultimo nome
  if (achei == 0)
  {
    fwrite(&c, sizeof(Contato), 1, fp);
  }
  else
  { // senao, devo fazer o shift
    do
    {
      fseek(fp, -sizeof(Contato), SEEK_CUR);
      fwrite(&c, sizeof(Contato), 1, fp);
      c = cAux;
      nR = fread(&cAux, sizeof(Contato), 1, fp);
    } while (nR == 1);

    fwrite(&c, sizeof(Contato), 1, fp);
  }

  return;
}

void exibeContatos(FILE *fp)
{
  Contato aux;
  rewind(fp);
  while (fread(&aux, sizeof(Contato), 1, fp))
  {
    exibeContato(&aux);
  }
}