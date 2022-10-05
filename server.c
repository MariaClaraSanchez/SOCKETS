// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct cont
{
  char nome[100];
  char endereco[200];
  int idade;
} Contato;

int getFileSize();
void insertContato(Contato contato, FILE *fp);
void exibeContato(Contato *contato, int sock);
void exibeContatos(FILE *fp, int sz, int sock);
void verificarQtdePesquisa(char pesquisaNome[100], int sock, FILE *fp, int sz);
void exibeContatoPesquisado(Contato *contato, int sock, char pesquisaNome[100]);
int contaIguais(Contato *contato, char pesquisaNome[100]);
void chamaExibeContatoPesquisado(char pesquisaNome[100], int sock, FILE *fp, int sz);

int main(int argc, char const *argv[])
{

  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  Contato contato;

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET,
                 SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(argv[1]));

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  // read(new_socket, idade, sizeof(idade));
  // send(new_socket, nome, sizeof(nome), 0);
  // read(new_socket, endereco, sizeof(endereco));
  // valread = read(new_socket, buffer, sizeof(buffer));
  /*valread2 = read(new_socket, buffer.idsade, sizeof(buffer.idade));
  valread3 = read(new_socket, buffer.endereco, sizeof(buffer.endereco));*/

  // Criaão arquivo
  FILE *fp;
  fp = fopen("dados.dat", "rb+");

  if (fp == NULL)
  {
    fp = fopen("dados.dat", "wb+");
  }

  int sz, op;
  char pesquisaNome[100];

  do
  {
    char data[1024];
    read(new_socket, &op, sizeof(op));


    switch (op)
    {
    case 1:
      // cadastrar contato
  
      read(new_socket, &contato, sizeof(contato));
      insertContato(contato, fp);
      fflush(fp);
      break;
    case 2:
      // exibir contatos
      sz = getFileSize(fp);
      exibeContatos(fp, sz, new_socket);
      fflush(stdout);
      break;
    case 3:
      read(new_socket, pesquisaNome, sizeof(pesquisaNome));
      //printf("Pesquisando contato pelo nome.....");
      //printf("\n%s\n", pesquisaNome);
      sz = getFileSize(fp);
      verificarQtdePesquisa(pesquisaNome, new_socket, fp, sz);
      fflush(stdout);
      chamaExibeContatoPesquisado(pesquisaNome, new_socket, fp, sz);
      // montar a pesquisa do nome
    case 4:
      printf("Saindo... até mais! :)");
    default:
      printf("Opção Invalida!\n");
    }
  } while (op != 4);

  /*printf("%s\n", buffer.idade);
  printf("%s\n", buffer.endereco);*/

  // send(new_socket, hello, strlen(hello), 0);
  // printf("Hello message sent\n");

  // closing the connected socket
  close(new_socket);
  // closing the listening socket
  shutdown(server_fd, SHUT_RDWR);
  return 0;
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

void insertContato(Contato c, FILE *fp)
{
  printf("Nome: %s\n", c.nome);
  printf("Idade: %d anos\n", c.idade);
  printf("Endereço:%s\n", c.endereco);
  printf("CONTATO INSERIDO COM SUCESSO!!!\n");
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
      fflush(fp);
      c = cAux;
      nR = fread(&cAux, sizeof(Contato), 1, fp);
    } while (nR == 1);

    fwrite(&c, sizeof(Contato), 1, fp);
  }

  return;
}

void exibeContato(Contato *contato, int sock)
{
  char data[1024];
  printf("\n*********************************");
  printf("\nNome: %s", contato->nome);
  printf("\nEndereco: %s", contato->endereco);
  printf("\nIdade: %d", contato->idade);
}

void exibeContatos(FILE *fp, int sz, int sock)
{
  Contato aux;
  int i;
  rewind(fp);
  int cont=sz/sizeof(Contato);
  Contato *vet = (Contato *) malloc(cont*sizeof(Contato));

  for(i=0; i<cont; i++){
      if(fread(&vet[i], sizeof(Contato), 1, fp))
        exibeContato(&vet[i], sock);
  }

  printf("Cont: %d", cont);

  send(sock, &cont, sizeof(cont), 0);
  send(sock, vet, cont*sizeof(Contato), 0);
  
  free(vet);
}

void verificarQtdePesquisa(char pesquisaNome[100], int sock, FILE *fp, int sz){
  int i, qtde=0;
  rewind(fp);
  int cont=sz/sizeof(Contato);
  Contato *vet = (Contato *) malloc(cont*sizeof(Contato));
  for(i=0; i<cont; i++){
      if(fread(&vet[i], sizeof(Contato), 1, fp)){
        qtde += contaIguais(&vet[i], pesquisaNome);
      }
  }
  
  send(sock, &qtde, sizeof(qtde), 0);
}

int contaIguais(Contato *contato, char pesquisaNome[100]){
  int qtde = 0;
  if(strcmp(contato->nome, pesquisaNome) == 0){
    qtde++;
  }
  
  return qtde;
}

void chamaExibeContatoPesquisado(char pesquisaNome[100], int sock, FILE *fp, int sz){
  rewind(fp);
  int i;
  int cont=sz/sizeof(Contato);
  Contato *vet = (Contato *) malloc(cont*sizeof(Contato));

  for(i=0; i<cont; i++){
      if(fread(&vet[i], sizeof(Contato), 1, fp)){
        exibeContatoPesquisado(&vet[i], sock, pesquisaNome);
      }
  }
}

void exibeContatoPesquisado(Contato *contato, int sock, char pesquisaNome[100]){
  if(strcmp(contato->nome, pesquisaNome) == 0){
    send(sock, contato, sizeof(Contato), 0);
    fflush(stdout);
    //printf("SEND\n");
  }
}