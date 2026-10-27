#pragma once 
typedef struct{
	char patente[7];
	char marca[30];
	char modelo[30];
	int anio;
	char tipo[30];
	int capacidad;
	float precioPorDia;
	int estado; //1- disponible 2- no disponible
}vehiculo;
void menuVehiculos(void);
void registrarVehiculo(void);
int buscarVehiculo(char patente[]);
void consultarVehiculo(void);
void mostrarVehiculos(void);
void modificarVehiculos(void);
