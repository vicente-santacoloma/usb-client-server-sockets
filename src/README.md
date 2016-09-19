Sistema Cliente-Servidor con Sockets                       
====================================

Integrantes
-----------

* 08-10243 Roque Contreras
* 08-11044 Vicente Santacoloma

Descripción
-----------

La ferretería **LA CADENA** desea implementar un sistema de ayuda para la emisión de pedidos a sus proveedores. Este sistema debe asistir en la obtención del mejor precio posible para los artículos buscados.

Estructura
----------

El software consta de los siguientes archivos:

* **Proveedor.h:** Header del archivo **Proveedor.c**
* **Proveedor.c:** Contiene la estructura **Proveedor**
* **Pedido.h:** Header del archivo **Pedido.c**
* **Pedido.c:** Contiene la estructura **Pedido**
* **Inventario.h:** Header del archivo **Inventario.c**
* **Inventario.c:** Contiene la estructura **Inventario**
* **Vector.h:** Header del archivo **Vector.c**
* **Vector.c:** Contiene la estructura **Vector** que es un contenedor, donde sus elementos están ordenados en estricta secuencia lineal. Está implementado como arreglos dinámicos. Pueden manipular apuntadores de cualquier tipo básico o estructura. 
* **Exception.h:** Header del archivo **Exception.c**
* **Exception.c:** Contiene las excepciones generadas por un formato inválido en los diversos archivos de entrada
* **ordenes.c:** Programa cliente que actúa como la ferretería **LA CADENA**.
* **proveedores.c:** Programa servidor que actúa como un proveedor de la ferretería **LA CADENA**

Para mayor información acerca de las funciones de cada archivo junto con su descripción, revisar los headers(.h).

Protocolo de Aplicación
-----------------------

* Se utilizó los Sockets como mecanismo de comunicación.
* La operación a realizar es pasada del cliente al servidor como un carácter, siendo **'a'** la operación avanzada y **'b'** la operación básica.
* Los pases de mensajes que contengan pedidos e inventarios se hacen a través de un buffer donde cada campo está separado por el carácter **'&'**.
* Una vez realizada la operación por el servidor, se envía un entero que confirma su exitosa ejecución.
* Una vez actualizado el inventario del servidor, éste es reportado por la salida estándar.

Modelo Cliente
--------------

El cliente interactúa con todos los servidor de manera iterativa, es decir, uno por vez. 
Se optó por esto en lugar de hacerlo mediante varios hilos debido a:

* La simplicidad del proyecto.
* Se tendrían que tener otros detalles acerca del hardware disponible sobre el que va a correr el cliente, ya que solo se tendrian beneficios si se tuviera un procesador de más de un núcleo, para lo cual se deberían crear hilos de kernel para poder ser procesados en cada núcleo. Además se tendría que evaluar el costo de crear cada uno, en comparación con interactuar con los servidores de manera iterativa. Además, cabe señalar que no se especifica la cantidad de servidores máximos o mínimos a ejecutar.

Modelo Servidor
---------------

* **Comunicación:**
	* **Orientada a conexiones:** se utiliza el mecanismo TCP.
* **Concurrencia:**
	* **Iterativo:** El servidor solo interactúa con un cliente a la vez. Esta selección se baso en lo especificado en el enunciado del proyecto, donde se indica que solo hay un cliente interactuando con los proveedores simultaneamente.

* **Estado:**
	* **Servidor Sin Estado:** No se requiere almacenar las interacciones anteriores con cada cliente. Cada petición se realiza de forma independiente de acuerdo a la operación indicada.

Instrucciones de Instalación
----------------------------

Para instalar el software primero debemos descomprimir el archivo en formato
**.tar.gz** mediante: 

```bash
tar -xvf Proyecto1.tar.gz
```

Para compilar el código en OS basado en Linux, deberemos posicionarnos en la carpeta **Proyecto1** y luego ejecutar el comando:

```bash
make
```

Luego se producirá dos ejecutables: **'ordenes'** y **'proveedores'**, correspondientes al programa cliente y al programa servidor respectivamente.

Instrucciones de Ejecución
--------------------------                      

El programa cliente se ejecuta mediante:

**ordenes -[a|b] -f [archivo de pedidos] -d [archivo de proveedores]**

donde:

* **-a** Indica nivel avanzado.
* **-b** Indica nivel básico.
* **-f** Indica que la siguiente entrada corresponde al archivo de pedidos.
* **-d** Indica que la siguiente entrada corresponde al archivo de proveedores.
* **[archivo de pedidos]** Ruta al archivo que contiene los pedidos.
* **[archivo de proveedores]** Ruta al archivo que contiene los proveedores.

Las opciones **-a** y **-b** no pueden usarse juntas.

El programa servidor se ejecuta mediante:

  **proveedor -f [archivo de inventarios] -p [puerto]**

donde:

* **-f** Indica que la siguiente entrada corresponde al archivo de inventarios.
* **-p** Indica que la siguiente entrada corresponde al puerto.
* **[archivo de inventarios]** Ruta al archivo que contiene los inventarios.
* **[puerto]** Puerto a utilizar durante la ejecución del servidor.

Formato de Parámetros de Entrada
--------------------------------

#### ARCHIVO DE PEDIDOS

Cada línea del archivo tendrá el formato: 

**[nombre producto] & [cantidad a solicitar]**

##### Limitaciones:
* **[nombre producto]** String no vacío de 30 caracteres como máximo.
* **[cantidad a solicitar]** Entero mayor a 0 de 10 dígitos como máximo.

#### ARCHIVO DE PROVEEDORES
Cada línea del archivo tendrá el formato:

**[nombre proveedor] & [direccion dns] & [puerto]**

##### Limitaciones:
* **[nombre proveedor]** String no vacío de 12 caracteres como máximo.
* **[direccion dns]** String no vacío de 20 caracteres como máximo.
* **[puerto]** Entero de 5 dígitos como máximo. No deberá estar en uso.

#### ARCHIVO DE INVENTARIOS

Cada linea del archivo tendrá el formato: 

**[nombre producto] & [cantidad disponible] & [precio unitario]**

##### Limitaciones:
* **[nombre producto]** String no vacío de 30 caracteres como máximo.
* **[cantidad disponible]** Entero mayor a 0 de 10 dígitos como máximo.
* **[precio unitario]** Float mayor a 0 de 10 dígitos como máximo.

Cualquier entrada que no se adapte a este formato tendrá un error por la salida estándar.

Estado Actual                             
-------------

100% Funcional.

Detalles Adicionales                            
--------------------

El código base para programar el cliente y el servidor fue extraído de la página [http://www.linuxhowtos.org/C_C++/socket.htm](http://www.linuxhowtos.org/C_C++/socket.htm)

