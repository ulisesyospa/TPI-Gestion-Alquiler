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
	int estado;
}alquiler;
void menuAlquileres(void);
void realizarAlquiler(void);
void listarAlquileres(void);
