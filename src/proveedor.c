#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "Inventario.h"

#define TRUE  1
#define FALSE 0
#define LISTEN_CONNECTIONS 10
#define BUFFER_SIZE 10000

int sockfd;

/**
 * Imprimer un mensaje de error y aborta el programa.
 *
 * @param msg el mensaje a imprimir.
 * @return nada
 */
void error(const char *msg) {
    perror(msg);
    exit(1);
}

/**
 * Captura la senal CTRL+C envia al programa para cerrar el socket y liberar la
 * memoria restante antes de abortar el programa.
 *
 * @param signum la senal recibida.
 * @return nada
 */
void signal_callback_handler(int signum) {

  printf("La ejecucion del servidor ha sido interrumpida\n");
  close(sockfd);
  clearInventarios(inventarios);
  exit(signum);

}

/**
 * Carga los parametros entrada del programa servidor en las variables 
 * correspondientes.
 *
 * @param argc numero de argumentos recibidos 
 * @param argv arreglo de strings conteniendo los parametros de entrada del 
 * programa.
 * En la primera posición debe estar proveedor que es el nombre del programa.
 * @param archivoInventarios el string donde se guardara el nombre de archivo 
 * que contiene los inventarios a cargar.
 * @param puerto entero donde se guardara el puerto que utilizara el servidor.
 * @return true si las opciones son correctas, falso en caso contrario.
 */
int cargarParametros(int argc, char *argv[], char ** archivoInventarios, 
                     int * puerto) {

  int b [2] = {FALSE, FALSE};
  int k = 1;
  if(argc != 5)
    return FALSE;
  while(k < argc) {
    if(argv[k][0] == '-') {
      switch(argv[k][1]) {
        case 'f':
          if(b[0])
            return FALSE;
          b[0] = TRUE;  
          ++k;          
          *archivoInventarios = argv[k];
          break;
        case 'p':
          if(b[1])
            return FALSE;
          b[1] = TRUE;
          ++k;
          *puerto = atoi(argv[k]);
          break;
        default:
          return FALSE;
      } 
    } else
        return FALSE;
    ++k;
  }
  return TRUE;
}

/**
 * Ejecucion principal del servidor que se encarga de proveer dos tipos de 
 * servicios:
 *
 * El basico: Dada una peticion enviada por un cliente, la cual contiene
 * el nombre de varios productos, responde con todos los inventarios 
 * asociados a esos productos.
 *
 * EL avanzado: Dada una peticion enviado por un cliente, la cual contiene
 * varios pedidos (producto, cantidad), actualiza todos los inventarios
 * asociados a esos productos disminuyendo la cantidad solicitada para cada
 * uno de ellos.
 *
 * @param puerto el puerto a utilizar por el servidor.
 * @return nada.
 */
void servidor(int puerto) {

  int newsockfd, portno; socklen_t clilen, n;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR al abrir el socket");

  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = puerto;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR al enlazar");

  listen(sockfd,LISTEN_CONNECTIONS);
  clilen = sizeof(cli_addr);
  
  char operacion;
  char buffer[BUFFER_SIZE];
  char inventariosString [BUFFER_SIZE];	
  Inventario * inventario;
  char * pch;		
  char * str = buffer;
  char convert [33];
  int solicitudAceptada;

  while (TRUE) {

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) error("ERROR al aceptar la coneccion");

    bzero(buffer, BUFFER_SIZE);

    n = read(newsockfd, &operacion, 1);
    if (n < 0) error("ERROR al leer del socket");
    printf("La operacion a realizar es: %c\n", operacion);

    if(operacion == 'b') {

      n = read(newsockfd, buffer, BUFFER_SIZE);
      if (n < 0) error("ERROR al leer del socket");
      //printf("Here is the message: %d\n",strlen(buffer));
      printf("El pedido a satisfacer es: %s\n", buffer);

      bzero(inventariosString, BUFFER_SIZE);
      str = buffer;

      pch = strtok(str, "&");
      while (pch != NULL) {
        inventario = obtenerInventario(pch);
        if(inventario != NULL) {
          strcat(inventariosString, inventario->producto);
          strcat(inventariosString, "&");
          sprintf(convert, "%d", inventario->cantidad);
          strcat(inventariosString, convert);
          strcat(inventariosString, "&");
          sprintf(convert, "%f", inventario->precio);
          strcat(inventariosString, convert);
          strcat(inventariosString ,"&");
        }								 	
        pch = strtok (NULL, "&");
      }

      printf("El inventario a enviar es: %s\n", inventariosString);
      n = write(newsockfd, inventariosString, strlen(inventariosString));
      if (n < 0) error("ERROR al escribir en el socket");	     

    } else if(operacion == 'a') {

      n = read(newsockfd, buffer, BUFFER_SIZE);
      if (n < 0) error("ERROR al escribir en el sockett");
      printf("El inventario a actualizar es: %s\n", buffer);

      actualizarInventario(buffer);
      printf("El inventario luego de actualizar es: \n");
      imprimirInventarios((Inventario **)begin(inventarios), size(inventarios));

      solicitudAceptada = 1;

      n = write(newsockfd, &solicitudAceptada, sizeof(int));
      if (n < 0) error("ERROR al escribir en el socket");
    }
    printf("\n\n");
    close(newsockfd);
  }
  close(sockfd);
}

/**
 * Programa principal.
 *
 * @param argc numero de argumentos recibidos.
 * @param argv arreglo de strings conteniendo los parametros de entrada del 
 * programa.
 * En la primera posición debe estar proveedor que es el nombre del programa.
 * @return 0 si no hubieron errores durante la ejecucion del programa, 1 en 
 * caso contrario.
 * Dado que el servidor consta de un ciclo infinito para atender a infinitos 
 * clientes (uno por vez), nunca se devuelve ningun valor.
 */
int main(int argc, char *argv[]) {

  char * archivoInventarios; 
  int puerto;

  signal(SIGINT, signal_callback_handler);

  if(!cargarParametros(argc, argv, &archivoInventarios, &puerto)) {
    printf("Formato de entrada invalido\n");
    exit(1);
  }

  //printf("Archivo de inventarios: %s\n",archivoInventarios);
  //printf("Puerto: %d\n",puerto);
  //printf("\n");

  create(&inventarios);
  
  cargarInventarios(archivoInventarios);
  
  //imprimirInventarios((Inventario **)begin(inventarios), size(inventarios));

  servidor(puerto);

  return 0;

}

