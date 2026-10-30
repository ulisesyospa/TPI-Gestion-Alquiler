#pragma once
#include "alquileres.h"
void menuConsultasyReportes(void);
void vehiculosDisponibles(void);
void vehiculosAlquilados(void);
void alquileresPorRangoFechas(void);
void alquileresPorCliente(void);
void alquileresPorVehiculo(void);
void alquileresPorEstado(void);
void alquileresPorImporte(void);
void pagosRealizados(void);
void clientesConDeuda(void);
void estadisticasEmpresa(void);
int fechaValidaCR(int dia, int mes, int anio);
int leerFechaCR(fecha *f);
void limpiarEntradaCR(void);
