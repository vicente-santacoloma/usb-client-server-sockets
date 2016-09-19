#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Inventario.h"
#include "Proveedor.h"
#include "Pedido.h"

#define TRUE  1
#define FALSE 0

#define BUFFER_SIZE 10000

char operacion;

/**
 * Imprimer un mensaje de error y aborta el programa.
 *
 * @param msg el mensaje a imprimir.
 * @return nada
 */
void error(const char *msg) {
  perror(msg);
  exit(0);
}

/**
 * Calcula la diferencia de precio de dos inventarios.

 * @param a un apuntador a inventario.
 * @param b un apuntador a inventario.
 * @return la diferencia de precios de dos inventarios.
 */
int compare (const void * a, const void * b) {

  float e1 = (*((Inventario **)a))->precio;
  float e2 = (*((Inventario **)b))->precio;
  if(e1 < e2)
    return -1;
  else if(e1 > e2)
    return 1;
  return 0;
}

/**
 * Carga los parametros entrada del programa cliente en las variables 
 * correspondientes.
 *
 * @param argc numero de argumentos recibidos 
 * @param argv arreglo de strings conteniendo los parametros de entrada del 
 * programa.
 * En la primera posición debe estar ordenes que es el nombre del programa.
 * @param archivoPedidos el string donde se guardara el nombre de archivo que 
 * contiene los pedidos a cargar.
 * @param archivoProveedores el string donde se guardara el nombre de archivo 
 * que contiene los proveedores a cargar.
 * @param nivel funcion que corresponde a la operacion a realizar.
 * @return true si las opciones son correctas, falso en caso contrario.
 */
int cargarParametros(int argc, char *argv[], char ** archivoPedidos, 
                                char **archivoProveedores) {

  int b [3] = {FALSE, FALSE, FALSE};
  int k = 1;
  if(argc != 6)
    return FALSE;
  while(k < argc) {
    if(argv[k][0] == '-') {
      switch(argv[k][1]) {
        case 'a':
          if(b[0])
            return FALSE;
          operacion = 'a';
					//printf("%c\n",operacion);
          b[0] = TRUE;
          break;
        case 'b':
          if(b[0])
            return FALSE;
          operacion = 'b';
					//printf("%c\n",operacion);
          b[0] = TRUE;          
          break;
        case 'd':
          if(b[1])
            return FALSE;
          b[1] = TRUE;
          ++k;          
          *archivoProveedores = argv[k];
          break;
        case 'f':
          if(b[2])
            return FALSE;
          b[2] = TRUE;
          ++k;
          *archivoPedidos = argv[k];
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
 * Imprime por la salida estandar el reporte correspondiente a la operacion 
 * basica o a la operacion avanzada segun sea el caso.
 *
 * @param reporte reporte a imprimir. 'b' corresponde al avanzado y 'a' al
 * avanzado.
 * @param inventariosProveedoresSolicitud vector que contiene los inventarios a
 * solicitar para cada proveedor.
 * @param inventariosProveedoresFaltantes vector que contiene los inventarios
 * faltantes que no pudieron ser satisfechos por los proveedores.
 * @return nada.
 */
void imprimirReporte(char reporte, Vector * inventariosProveedoresSolicitud, Vector * inventariosProveedoresFaltantes) {

  Vector * inventariosProveedorSolicitud;
  int sizeProveedores = size(proveedores);
  int sizePedidos = size(pedidos);
  int sizeInventariosProveedorSolicitud = 0;
  int sizeInventariosProveedoresFaltantes = size(inventariosProveedoresFaltantes);

  Pedido * pedido = NULL;
  Inventario * inventario = NULL;
  int Total = 0;
  int i = 0;
  int j = 0;
  int k = 0;

  printf("\n--------------------------------------------------------------------------------------------\n");
  if(reporte == 'a')
    printf("| PEDIDOS SOLICITADOS                                                                       |\n");
  else
    printf("| CONSULTA                                                                                  |\n");

  printf("---------------------------------------------------------------------------------------------\n");
  printf("| PRODUCTO                       | PROVEEDOR       | PRECIO     | CANTIDAD   | COSTO TOTAL  |\n");
  printf("---------------------------------------------------------------------------------------------\n");
  
  for(i = 0; i < sizePedidos; ++i) {
    pedido = (Pedido *)at(pedidos, i);
    for(j = 0; j < sizeProveedores; ++j) {
      inventariosProveedorSolicitud = (Vector *)at(inventariosProveedoresSolicitud, j);
      sizeInventariosProveedorSolicitud = size(inventariosProveedorSolicitud);
      for(k = 0; k < sizeInventariosProveedorSolicitud; ++k) {
        inventario = (Inventario *)at(inventariosProveedorSolicitud, k); 
        if((strcmp(pedido->producto, inventario->producto) == 0) && (inventario->cantidad > 0)) {
          printf("| %-30s | %-15s | %-10.2f | %-10d | %-12.2f |\n",
          inventario->producto, (inventario->proveedor)->nombre,
          inventario->precio, inventario->cantidad, 
          (inventario->precio)*(inventario->cantidad));
          Total += (inventario->precio)*(inventario->cantidad);
          break;
        }
      }
    }
  }
  printf("---------------------------------------------------------------------------------------------\n");
  printf("| TOTAL                                                                      | %-12d |\n",Total);
  printf("---------------------------------------------------------------------------------------------\n");
  printf("| PEDIDOS FALTANTES                           |\n");
  printf("-----------------------------------------------\n");
  printf("| PRODUCTO                       | CANTIDAD   |\n");
  printf("-----------------------------------------------\n");

  for(i = 0; i < sizeInventariosProveedoresFaltantes; ++i) {
    inventario = (Inventario *)at(inventariosProveedoresFaltantes, i);
    printf("| %-30s | %-10d |\n", inventario->producto, inventario->cantidad);
  }
  printf("-----------------------------------------------\n\n");

}

/**
 * Realiza el pedido deseado al proveedor correspondiente. Posteriormente manda a 
 * imprimir el reporte correspondiente a la operacion avanzada.
 *
 * @param inventariosProveedoresSolicitud vector que contiene los inventarios a
 * solicitar para cada proveedor.
 * @param inventariosProveedoresFaltantes vector que contiene los inventarios
 * faltantes que no pudieron ser satisfechos por los proveedores.
 * @return nada.
 */
void avanzado(Vector * inventariosProveedoresSolicitud, Vector * inventariosProveedoresFaltantes) {

  Vector * inventariosProveedorSolicitud;
  int sizeProveedores = size(proveedores);
  
  int sizeInventariosProveedorSolicitud;

  Inventario * inventario;
  Proveedor * proveedor;
  char solicitudString [BUFFER_SIZE];
  char convert [33];

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  int i, j;
  char operacion = 'a';
  int x;

  for(i = 0; i < sizeProveedores; ++i) {
    proveedor = (Proveedor *)at(proveedores, i);
    inventariosProveedorSolicitud = (Vector *)at(inventariosProveedoresSolicitud, i);
    
    if(!empty(inventariosProveedoresSolicitud)) {
      portno = proveedor->puerto;
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) error("ERROR al abrir el socket");
      server = gethostbyname(proveedor->dns);
      if (server == NULL) {
        fprintf(stderr,"ERROR, no se encontro el host\n");
        exit(0);
      }
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
                    server->h_length);
      serv_addr.sin_port = htons(portno);
      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR al conectar");

      sizeInventariosProveedorSolicitud = size(inventariosProveedorSolicitud);

      bzero(solicitudString, BUFFER_SIZE);

      for(j = 0; j < sizeInventariosProveedorSolicitud; ++j) {
        inventario = (Inventario *)at(inventariosProveedorSolicitud, j);
        strcat(solicitudString, inventario->producto);
        strcat(solicitudString, "&");
        sprintf(convert,"%d", inventario->cantidad);
			  strcat(solicitudString, convert);
        strcat(solicitudString, "&");
      }

      n = write(sockfd, &operacion, 1);
      if (n < 0) error("ERROR al escribir en el socket");

      n = write(sockfd, solicitudString, strlen(solicitudString));    
		  if (n < 0) error("ERROR al escribir en el socket");
      

      n = read(sockfd, &x, sizeof(int));
      if (n < 0) error("ERROR al leer del socket");

      if(x != 1)
        error("La operacion no pudo ser completada\n");

    	close(sockfd);
    }
    
  }

  imprimirReporte('a', inventariosProveedoresSolicitud, inventariosProveedoresFaltantes);

  printf("La solicitud Avanzada fue completada con exito\n");

  for(i = 0; i < sizeProveedores; ++i) {
    inventariosProveedorSolicitud = (Vector *)at(inventariosProveedoresSolicitud, i);
    free(inventariosProveedorSolicitud->array);
    free(inventariosProveedorSolicitud);
  }
  free(inventariosProveedoresSolicitud->array);
  free(inventariosProveedoresSolicitud);
  clearInventarios(inventariosProveedoresFaltantes);

}

/**
 * Selecciona el pedido a realizar de entre todos los inventarios enviados por
 * los proveedores tomando el que tenga el menor precio (En caso de que varios 
 * tengan el mismo precio se toma el primero por defecto). Si no se logra 
 * satisfacer la demanda se solicitara lo disponible y el restante quedara como
 * pedidos faltantes. Posteriormente manda a imprimir el reporte correspondiente 
 * a la operacion basica.
 *
 * @param inventariosProveedores vector que contiene todos los inventarios
 * enviados por los proveedores.
 * @return nada.
 */
void basico(Vector * inventariosProveedores) {

  //basico(inventariosProveedores);

  Vector * inventariosProveedor = NULL;
  Vector * inventariosPedidos = NULL;
  Vector * inventariosPedido = NULL;
  Vector * inventariosSolicitud = NULL;
  Vector * inventariosProveedoresSolicitud = NULL;
  Vector * inventariosProveedoresFaltantes = NULL;
  Vector * inventariosProveedorSolicitud = NULL; 

  int sizePedidos = size(pedidos);
  int sizeProveedores = size(proveedores);
  int sizeinventariosSolicitud;
  int sizeInventariosProveedor;
  int sizeInventariosPedido;

  Inventario * inventario = NULL;
  Inventario * inventarioFaltante = NULL;
  Pedido * pedido = NULL;
  Proveedor * proveedor = NULL;
  int i, j, k;

  create(&inventariosPedidos);
  create(&inventariosSolicitud);
  create(&inventariosProveedoresSolicitud);
  create(&inventariosProveedoresFaltantes);

//for(i = 0; i < sizeProveedores; ++i) {
//  inventariosPedido = (Vector *)at(inventariosProveedores, i);
//  imprimirInventarios((Inventario **)begin(inventariosPedido), size(inventariosPedido));
//  printf("\n");
//}
//printf("--------------------------------------------------------------------------------\n");

  for(i = 0; i < sizePedidos; ++i) {
    pedido = (Pedido *)at(pedidos, i);
    inventariosPedido = NULL;
    create(&inventariosPedido);
    for(j = 0; j < sizeProveedores; ++j) {
      inventariosProveedor = (Vector *)at(inventariosProveedores, j);
      sizeInventariosProveedor = size(inventariosProveedor);
      for(k = 0; k < sizeInventariosProveedor; ++k) {
        inventario = (Inventario *)at(inventariosProveedor, k);
        if(strcmp(inventario->producto, pedido->producto) == 0) {
          push_back(&inventariosPedido, inventario);
          break;          
        }
      }
    }
    push_back(&inventariosPedidos, inventariosPedido);
  }


//for(i = 0; i < sizePedidos; ++i) {  
//  inventariosPedido = (Vector *)at(inventariosPedidos, i);
//  imprimirInventarios((Inventario **)begin(inventariosPedido), size(inventariosPedido));
//  printf("\n");
//}
//printf("--------------------------------------------------------------------------------\n");

  Inventario ** inventariosPedidoArreglo;

  for(i = 0; i < sizePedidos; ++i) {
    inventariosPedido = (Vector *)at(inventariosPedidos, i);
    if(!empty(inventariosPedido)) {
      inventariosPedidoArreglo = (Inventario **)begin(inventariosPedido);
      qsort(inventariosPedidoArreglo, size(inventariosPedido), sizeof(Inventario *), compare);
    }
  }

//for(i = 0; i < sizePedidos; ++i) {
//  inventariosPedido = (Vector *)at(inventariosPedidos, i);
//  imprimirInventarios((Inventario **)begin(inventariosPedido), size(inventariosPedido));
//  printf("\n");
//}
//printf("--------------------------------------------------------------------------------\n");

  int cantidadSolicitada = 0;
  int cantidadDisponible = 0;
  
  for(i = 0; i < sizePedidos; ++i) {

    pedido = (Pedido *)at(pedidos, i);
    cantidadSolicitada = pedido->cantidad;
    inventariosPedido = (Vector *)at(inventariosPedidos, i);
    sizeInventariosPedido = size(inventariosPedido);

    for(j = 0; j < sizeInventariosPedido; ++j) {

      inventario = (Inventario *)at(inventariosPedido, j);
      cantidadDisponible = inventario->cantidad;
      push_back(&inventariosSolicitud, inventario);
      if(cantidadDisponible >= cantidadSolicitada) {
        inventario->cantidad = cantidadSolicitada;
        cantidadSolicitada = 0;
        break;
      } else {
        cantidadSolicitada -= cantidadDisponible;  
      }
    }
    if(cantidadSolicitada != 0) {
      inventarioFaltante = (Inventario *)malloc(sizeof(Inventario));
      proveedor = (Proveedor *)malloc(sizeof(Proveedor));
      proveedor->nombre = "";
      inventarioFaltante->producto = (char *)malloc(strlen(pedido->producto));
      strcpy(inventarioFaltante->producto, pedido->producto);
      inventarioFaltante->proveedor = proveedor;
      inventarioFaltante->cantidad = cantidadSolicitada;
      inventarioFaltante->precio = 0;
      push_back(&inventariosProveedoresFaltantes, inventarioFaltante);
    }      
  }

  sizeinventariosSolicitud = size(inventariosSolicitud);
//printf("SOLICITUD\n");
//imprimirInventarios((Inventario **)begin(inventariosSolicitud), sizeinventariosSolicitud);
//printf("\nFALTANTE\n");  
//imprimirInventarios((Inventario **)begin(inventariosFaltantes), size(inventariosFaltantes));

  Vector * v;

  for(i = 0; i < sizePedidos; ++i) {
    v = (Vector *)at(inventariosPedidos, i);
    free(v->array);
    free(v);
  }
  free(inventariosPedidos->array);
  free(inventariosPedidos);

  for(i = 0; i < sizeProveedores; ++i) {
    proveedor = (Proveedor *)at(proveedores, i);
    inventariosProveedorSolicitud = NULL;
    create(&inventariosProveedorSolicitud); 
    for(j = 0; j < sizeinventariosSolicitud; ++j) {
      inventario = (Inventario *)at(inventariosSolicitud, j);
      if(strcmp(proveedor->nombre, (inventario->proveedor)->nombre) == 0)
        push_back(&inventariosProveedorSolicitud, inventario);
    }
    push_back(&inventariosProveedoresSolicitud, inventariosProveedorSolicitud);
  }
  free(inventariosSolicitud->array);
  free(inventariosSolicitud);

//for(i = 0; i < sizeProveedores; ++i) {
//  printf("Proveedor: %d\n",i);
//  inventariosProveedorSolicitud = (Vector *)at(inventariosProveedoresSolicitud, i);
//  imprimirInventarios((Inventario **)begin(inventariosProveedorSolicitud), size(inventariosProveedorSolicitud));
//  printf("\n\n");
//}

  imprimirReporte('b', inventariosProveedoresSolicitud, inventariosProveedoresFaltantes);

  printf("La solicitud Basica fue completada con exito\n");

  if(operacion == 'a')
    avanzado(inventariosProveedoresSolicitud, inventariosProveedoresFaltantes);

}

/**
 * Ejecucion principal del cliente que se encarga de solicitar dos tipos de 
 * servicios:
 *
 * El basico: Realiza una peticion a todos los proveedores, la cual contiene
 * el nombre de varios productos, recibiendo de cada uno el inventario 
 * asociado a cada producto.
 *
 * EL avanzado: Selecciona el inventario deseado de los enviados por todos los
 * proveedores y posteriormente se le envia a cada uno.
 *
 * @param nivel funcion que corresponde a la operacion a realizar.
 * @return nada.
 */
void cliente() {

  int sizeProveedores = size(proveedores);

  // Inventario que cada proveedor tiene del pedido a solicitar
  Vector * inventariosProveedores;
  create(&inventariosProveedores);

  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  int i;
  Proveedor * proveedor;

  char buffer [BUFFER_SIZE];
  char operacionAux = 'b';

  for(i = 0; i < sizeProveedores; ++i) {
  
    proveedor = (Proveedor *)at(proveedores,i);

    portno = proveedor->puerto;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR al abrir el socket");

    server = gethostbyname(proveedor->dns);
    if (server == NULL) {
      fprintf(stderr,"ERROR, no se encontro el host\n");
      exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, 
                  server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
      error("ERROR al conectar");
    
    n = write(sockfd, &operacionAux, 1);    
		if (n < 0) error("ERROR al escribir en el socket");

  	n = write(sockfd, pedidosString, strlen(pedidosString));
  	if (n < 0) error("ERROR al escribir en el socket");

    bzero(buffer, BUFFER_SIZE);
    n = read(sockfd, buffer, BUFFER_SIZE);
    if (n < 0) error("ERROR al leer del socket");

    printf("El proveedor %s me mando el inventario: %s\n", proveedor->nombre, buffer);

    push_back(&inventariosProveedores, generarInventario(buffer, proveedor));

  	close(sockfd);
		
  }

  basico(inventariosProveedores);

  // Liberar espacio usado por vectores.
  Vector * inventariosProveedor;
  int j;
  int sizeInventariosProveedor;
  Inventario * inventario;
  
  for(i = 0; i < sizeProveedores; ++i) {
    inventariosProveedor = (Vector *)at(inventariosProveedores, i);
    sizeInventariosProveedor = size(inventariosProveedor);
    for(j = 0; j < sizeInventariosProveedor; ++j) {
      inventario = (Inventario *)at(inventariosProveedor, j);
      free(inventario->producto);
      free(inventario);
    }
    free(inventariosProveedor->array);
    free(inventariosProveedor);    
  
    //clearInventarios(inventariosProveedor);
  }
  free(inventariosProveedores->array);
  free(inventariosProveedores);
  
}


/**
 * Programa principal.
 *
 * @param argc numero de argumentos recibidos.
 * @param argv arreglo de strings conteniendo los parametros de entrada del 
 * programa.
 * En la primera posición debe estar ordenes que es el nombre del programa.
 * @return 0 si no hubieron errores durante la ejecucion del programa, 1 en 
 * caso contrario.
 */
int main(int argc, char *argv[]) {

  char * archivoPedidos; 
  char * archivoProveedores;

  if(!cargarParametros(argc, argv, &archivoPedidos, &archivoProveedores)) {
    printf("Formato de entrada invalido\n");
    exit(1);
  }

  //printf("Archivo de proveedores: %s\n",archivoProveedores);
  //printf("Archivo de pedidos: %s\n",archivoPedidos);
  //printf("\n");

  create(&proveedores);
  create(&pedidos);
 
  cargarPedidos(archivoPedidos);
  cargarProveedores(archivoProveedores);
  
  //imprimirProveedores((Proveedor **)begin(proveedores), size(proveedores));
  //printf("\n");
  //imprimirPedidos((Pedido **)begin(pedidos), size(pedidos));

  //printf("El pedido a enviar en string es: %s\n", pedidosString);

  cliente();
  
  clearProveedores(proveedores);
  clearPedidos(pedidos);
  
  return 0;

}
