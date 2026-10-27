#pragma once 
typedef struct{
	int nroMovimiento;
	int nroAlquiler;
	char tipoMovimiento[20];
	float monto;
	fecha fec;
	int dniCliente;
	char patenteVehiculo[7];
}movimiento;
void menuPagos(void);
void registrarPago(void);
void registrarMovimiento(int nroAlquiler, int dniCliente, char patenteVehiculo[], char tipo[], float monto, fecha fec);
