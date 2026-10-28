#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "alquileres.h"
#include "movimiento.h"
void menuPagos(void){
	int opcion;
	do{
		printf("\n=== MENU PAGOS Y MOVIMIENTOS ===\n");
		printf("1. Registrar pago\n");
		printf("0. Volver al menu principal\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &opcion);
		switch(opcion){
			case 1:
				registrarPago();
				break;
			case 0:
				printf("Regresando al menu principal...\n");
				break;
			default:
				printf("Opcion invalida.\n");
		}
	}while(opcion != 0);
}
int obtenerSiguienteNroMovimiento(void){
	FILE *archivo = fopen("movimientos.dat", "rb");
	if(archivo == NULL) return 1;
	movimiento m;
	int ultimo = 0;
	while(fread(&m, sizeof(movimiento), 1, archivo) == 1){
		ultimo = m.nroMovimiento;
	}
	fclose(archivo);
	return ultimo + 1;
}
void registrarMovimiento(int nroAlquiler, int dniCliente, char patenteVehiculo[], char tipo[], float monto, fecha fec){
	movimiento nuevo;
	nuevo.nroMovimiento = obtenerSiguienteNroMovimiento();
	nuevo.nroAlquiler = nroAlquiler;
	strcpy(nuevo.tipoMovimiento, tipo);
	nuevo.monto = monto;
	nuevo.fec = fec;
	nuevo.dniCliente = dniCliente;
	strcpy(nuevo.patenteVehiculo, patenteVehiculo);
	FILE *fBin = fopen("movimientos.dat", "ab");
	if(fBin != NULL){
		fwrite(&nuevo, sizeof(movimiento), 1, fBin);
		fclose(fBin);
	}
	char nombreArchivoTxt[40];
	sprintf(nombreArchivoTxt, "movimientos_%d.txt", dniCliente);
	FILE *fTxt = fopen(nombreArchivoTxt, "a");
	if(fTxt != NULL){
		fprintf(fTxt, "\Movimiento Nro: %d | Alquiler: %d | Tipo: %s | Monto: %.2f | Fecha: %02d/%02d/%d | Patente: %s\n", nuevo.nroMovimiento, nuevo.nroAlquiler, nuevo.tipoMovimiento, nuevo.monto,
                nuevo.fec.dia, nuevo.fec.mes, nuevo.fec.anio, nuevo.patenteVehiculo);
                fclose(fTxt);
	}
}
void registrarPago(void){
	int nroAlq;
	float montoPago;
	fecha fecPago;
	printf("\n=== REGISTRAR PAGO ===\n");
	printf("Ingrese numero de alquiler: ");
	scanf("%d", &nroAlq);
	FILE *fAlq = fopen("alquileres.dat", "rb+");
	if(fAlq == NULL){
		printf("\n[Error] No se pudo abrir el archivo de alquileres.\n");
		return;
	}
	alquiler a;
	int encontrado = 0;
	long posAlquiler = 0;
	while(fread(&a, sizeof(alquiler), 1, fAlq) == 1){
		if(a.nroAlquiler == nroAlq){
			encontrado = 1;
			posAlquiler = ftell(fAlq) - sizeof(alquiler);
			break;
		}
	}
	if(!encontrado){
		printf("\n[Error] Alquiler inexistente.\n");
		fclose(fAlq);
		return;
	}
	float saldoPendiente = a.importeTotal - a.importePagado;
	printf("Importe Total: $%.2f | Ya Pagado: $%.2f | Saldo Restante: $%.2f\n", a.importeTotal, a.importePagado, saldoPendiente);
	if(saldoPendiente <= 0){
		printf("\n[Aviso] Este alquiler ya se encuentra totalmente saldado.\n");
		fclose(fAlq);
		return;
	}
	
	// Bucle para validar que el monto a abonar sea correcto y no supere el saldo pendiente
	do {
		printf("Ingrese monto a abonar: ");
		scanf("%f", &montoPago);
		if(montoPago <= 0){
			printf("\n[Error] El monto debe ser mayor a 0. Intente nuevamente.\n");
		} else if(montoPago > saldoPendiente){
			printf("\n[Error] El monto ingresado ($%.2f) supera el saldo pendiente ($%.2f). Debe abonar un monto valido.\n", montoPago, saldoPendiente);
		}
	} while(montoPago <= 0 || montoPago > saldoPendiente);

	// Validación para evitar pagos con fechas anteriores al inicio del alquiler
	int fechaValidaPago = 0;
	do {
		printf("Ingrese fecha del pago (DD/MM/AAAA): ");
		scanf("%d/%d/%d", &fecPago.dia, &fecPago.mes, &fecPago.anio);
		struct tm fInicio = {0};
		fInicio.tm_mday = a.fecInicio.dia;
		fInicio.tm_mon = a.fecInicio.mes - 1;
		fInicio.tm_year = a.fecInicio.anio - 1900;
		struct tm fPago = {0};
		fPago.tm_mday = fecPago.dia;
		fPago.tm_mon = fecPago.mes - 1;
		fPago.tm_year = fecPago.anio - 1900;
		time_t tInicio = mktime(&fInicio);
		time_t tPago = mktime(&fPago);
		if(tPago == -1){
			printf("\n[Error] La fecha ingresada no existe o es invalida. Intente nuevamente.\n");
		} else if(tPago < tInicio){
			printf("\n[Error] La fecha del pago no puede ser anterior a la fecha de inicio del alquiler (%02d/%02d/%d).\n", 
			       a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
		} else {
			fechaValidaPago = 1;
		}
	} while(!fechaValidaPago);
	a.importePagado = a.importePagado + montoPago;
	fseek(fAlq, posAlquiler, SEEK_SET);
	fwrite(&a, sizeof(alquiler), 1, fAlq);
	fflush(fAlq);
	fclose(fAlq);
	registrarMovimiento(a.nroAlquiler, a.dniCliente, a.patenteVehiculo, "Pago", montoPago, fecPago);
	printf("\n[Exito] Pago de $%.2f registrado correctamente.\n", montoPago);
}
