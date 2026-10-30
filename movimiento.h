#pragma once
#include "alquileres.h" 
typedef struct {
    int nroMovimiento;
    int nroAlquiler;
    char tipoMovimiento[20];
    float monto;
    fecha fec;
    int dniCliente;
    char patenteVehiculo[10];
} movimiento;
void menuPagos(void);
void registrarPago(void);
int registrarMovimiento(int nroAlquiler, int dniCliente, char patenteVehiculo[], char tipo[], float monto, fecha fec);
int obtenerSiguienteNroMovimiento(void);
