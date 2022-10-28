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

char *CreateStringRandom(int n);
void getContato(Contato *contato);
void exibeContato(Contato *contato);
void *threadF(void *param);

int main(int argc, char const *argv[])
{

  Conexao conexao;
  strcpy(conexao.ip, argv[1]);
  strcpy(conexao.porta, argv[2]);
  int NTHREADS = atoi(argv[3]);
  pthread_t thread_id[NTHREADS];

  for (int i = 0; i < NTHREADS; i++)
  {
    printf("\nThread %d\n", i + 1);
    pthread_create(&thread_id[i], NULL, threadF, &conexao);
  }

  for (int j = 0; j < NTHREADS; j++)
  {
    printf("Finalizando Thread %d", j + 1);
    pthread_join(thread_id[j], NULL);
  }

  return 0;
}

void *threadF(void *param)
{
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
  int i, op, sz, cont = 0, qtde = 0;
  char pesquisaNome[100];
  char data[1024];

  op = rand() % 3;
  if (op == 0)
  {
    op = 1;
  }
  printf("Opção : %d\n", op);

  switch (op){
    case 1:
      getContato(&contato);
      send(sock, &op, sizeof(int), 0);
      send(sock, &contato, sizeof(contato), 0);
      break;
    case 2:
      printf("\nCONTATOS CADASTRADOS:\n");
      read(sock, &cont, sizeof(cont));
      Contato *vet = (Contato *)malloc(cont * sizeof(Contato));
      read(sock, vet, cont * sizeof(Contato));
      // printf("CONT= %d\n", cont);
      for (i = 0; i < cont; i++)
      {
        printf("\nContato %d", i + 1);
        exibeContato(&vet[i]);
        printf("\n");
      }
      printf("\n");

      break;
    default:
      printf("\nOpcao invalida!!!");
    }
  op = 4;
close(client_fd);
}

char *CreateStringRandom(int n)
{
  char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                       'h', 'i', 'j', 'k', 'l', 'm', 'n',
                       'o', 'p', 'q', 'r', 's', 't', 'u',
                       'v', 'w', 'x', 'y', 'z'};

  char *res;
  res = malloc(100 * sizeof(char));
  for (int i = 0; i < n; i++)
    res[i] = alphabet[rand() % 26];

  return res;
}

void getContato(Contato *contato)
{
  int idade = rand() % 100;
  strcpy(contato->nome, CreateStringRandom(10));
  strcpy(contato->endereco, CreateStringRandom(15));
  contato->idade = idade;
}

void exibeContato(Contato *contato)
{
  printf("\nNome: %s", contato->nome);
  printf("\nEndereco: %s", contato->endereco);
  printf("\n%d", contato->idade);
}
