#pragma once 
typedef struct{
	int mes, dia, anio;
}fecha;
typedef struct{
	int nroAlquiler;
	int dniCliente;
	char patenteVehiculo[10];
	fecha fecInicio;
	fecha fecPrevitaDevolucion;
	fecha fecEfectivaDevolucion;
	int cantidadDias;
	float importeTotal;
	float importePagado;
	int estado; //1- activo 2- finalizado 0- cancelado
}alquiler;
void menuAlquileres(void);
void realizarAlquiler(void);
void listarAlquileres(void);
int verificacionEntero(const char *mensaje);
void patenteValida(char *destino, int tamano, const char *mensaje);
int buscarCliente(int dni);
