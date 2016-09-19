CC=gcc
CFLAGS=-c -Wall

all: ordenes proveedor

ordenes: ordenes.o Inventario.o Proveedor.o Pedido.o Vector.o Exception.o
	$(CC) ordenes.o Inventario.o Proveedor.o Pedido.o Vector.o Exception.o -o ordenes

proveedor: proveedor.o Inventario.o Vector.o Exception.o
	$(CC) proveedor.o Inventario.o Vector.o Exception.o -o proveedor

ordenes.o: ordenes.c
	$(CC) $(CFLAGS) ordenes.c

proveedor.o: proveedor.c
	$(CC) $(CFLAGS) proveedor.c

Proveedor.o: Proveedor.h Proveedor.c
	$(CC) $(CFLAGS) Proveedor.c

Pedido.o: Pedido.h Pedido.c
	$(CC) $(CFLAGS) Pedido.c

Inventario.o: Inventario.h Inventario.c
	$(CC) $(CFLAGS) Inventario.c

Vector.o: Vector.h Vector.c
	$(CC) $(CFLAGS) Vector.c

Exception.o: Exception.h Exception.c
	$(CC) $(CFLAGS) Exception.c

clean:
	rm -f *.o ordenes proveedor
