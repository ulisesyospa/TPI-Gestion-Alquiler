#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "alquileres.h"
#include "movimiento.h"
static int leerEnteroPositivo(const char *mensaje) {
    char buffer[100];
    char basura;
    int numero;
    while (1) {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                printf("[Aviso] No ingreso ningun dato. Por favor, ingrese un valor valido.\n\n");
                continue;
            }
            if (sscanf(buffer, " %d %c", &numero, &basura) == 1) {
                if (numero > 0) {
                    return numero;
                } else {
                    printf("[Error] El numero debe ser un entero positivo mayor a 0.\n\n");
                }
            } else {
                printf("[Error] Entrada invalida. Debe ingresar estrictamente un numero.\n\n");
            }
        }
    }
}
void menuPagos(void){
	int opcion;
	char buffer[100];
	char basura;
	do{
		printf("\n========================================\n");
		printf("        MENU PAGOS Y MOVIMIENTOS        \n");
		printf("========================================\n");
		printf("1. Registrar pago o ajuste\n");
		printf("0. Volver al menu principal\n");
		printf("----------------------------------------\n");
		
		while(1){
			printf("Seleccione una opcion: ");
			if(fgets(buffer, sizeof(buffer), stdin) == NULL){
				continue;
			}
			if(buffer[0] == '\n' || buffer[0] == '\r'){
				printf("[Aviso] No ingreso ninguna opcion.\n\n");
				continue;
			}
			if(sscanf(buffer, " %d %c", &opcion, &basura) != 1){
				printf("[Error] Opcion invalida. Ingrese solo un numero.\n\n");
				continue;
			}
			if(opcion != 0 && opcion != 1){
				printf("[Error] Opcion invalida. Ingrese 0 o 1.\n\n");
				continue;
			}
			break;
		}
		
		switch(opcion){
			case 1:
				registrarPago();
				break;
			case 0:
				printf("\nRegresando al menu principal...\n");
				break;
		}
	}while(opcion != 0);
}
int obtenerSiguienteNroMovimiento(void){
	FILE *archivo = fopen("movimientos.dat", "rb");
	if(archivo == NULL) return 1;
	movimiento m;
	int ultimo = 0;
	while(fread(&m, sizeof(movimiento), 1, archivo) == 1){
		if(m.nroMovimiento > ultimo){
			ultimo = m.nroMovimiento;
		}
	}
	fclose(archivo);
	return ultimo + 1;
}
int registrarMovimiento(int nroAlquiler, int dniCliente, char patenteVehiculo[], char tipo[], float monto, fecha fec){
	if(!fechaValida(fec.dia, fec.mes, fec.anio)){
        printf("[Error] La fecha del movimiento no es valida.\n");
        return 0;
    }
	movimiento nuevo;
	nuevo.nroMovimiento = obtenerSiguienteNroMovimiento();
	nuevo.nroAlquiler = nroAlquiler;
	strcpy(nuevo.tipoMovimiento, tipo);
	nuevo.monto = monto;
	nuevo.fec = fec;
	nuevo.dniCliente = dniCliente;
	strcpy(nuevo.patenteVehiculo, patenteVehiculo);
	FILE *fBin = fopen("movimientos.dat", "ab");
	if(fBin == NULL){
		printf("[Error] No se pudo abrir el archivo de movimientos.\n");
		return 0;
	}
	if(fwrite(&nuevo, sizeof(movimiento), 1, fBin) != 1){
		printf("[Error] No se pudo escribir el movimiento.\n");
		fclose(fBin);
		return 0;
	}
	fclose(fBin);
	char nombreArchivoTxt[40];
	snprintf(nombreArchivoTxt, sizeof(nombreArchivoTxt), "movimientos_%d.txt", dniCliente);
	FILE *fTxt = fopen(nombreArchivoTxt, "a");
	if(fTxt == NULL){
		printf("[Error] No se pudo abrir el archivo %s.\n", nombreArchivoTxt);
		return 0;
	}
	fprintf(fTxt, "Movimiento Nro: %d | Alquiler: %d | Tipo: %s | Monto: %.2f | Fecha: %02d/%02d/%d | Patente: %s\n",
	        nuevo.nroMovimiento, nuevo.nroAlquiler, nuevo.tipoMovimiento, nuevo.monto,
	        nuevo.fec.dia, nuevo.fec.mes, nuevo.fec.anio, nuevo.patenteVehiculo);
	fclose(fTxt);
	return 1;
}
void registrarPago(void){
    char buf[100];
    char basura;
    int nroAlq;
    float montoPago;
    fecha fecPago;
    printf("\n========================================\n");
    printf("         REGISTRAR PAGO O AJUSTE        \n");
    printf("========================================\n");
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if(fAlq == NULL){
        printf("\n[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    alquiler a;
    int contadorActivos = 0;
    printf("\n--- ALQUILERES CON SALDO PENDIENTE ---\n");
    while(fread(&a, sizeof(alquiler), 1, fAlq) == 1){
        if(a.estado == 1 || a.estado == 2){
            float saldo = a.importeTotal - a.importePagado;
            if(saldo > 0.005f){
                printf("Nro: %d | DNI: %d | Patente: %s | Total: $%.2f | Saldo: $%.2f\n", 
                       a.nroAlquiler, a.dniCliente, a.patenteVehiculo, a.importeTotal, saldo);
                contadorActivos++;
            }
        }
    }
    if(contadorActivos == 0){
        printf("[Aviso] No hay alquileres con saldo pendiente registrados en el sistema.\n");
        fclose(fAlq);
        return;
    }
    printf("----------------------------------------\n");
    int encontrado = 0;
    long posAlquiler = 0;
    int alquilerValidoParaPagar = 0;
    while(!alquilerValidoParaPagar){
        printf("Ingrese numero de alquiler (o 0 para volver): ");
        if(fgets(buf, sizeof(buf), stdin) == NULL) continue;
        buf[strcspn(buf, "\n")] = '\0';
        if(buf[0] == '\0'){
            printf("[Aviso] No ingreso ningun dato.\n");
            continue;
        }
        int i;
        int esNumero = 1;
        for(i = 0; buf[i] != '\0'; i++){
            if(!isdigit((unsigned char)buf[i])){
                esNumero = 0;
                break;
            }
        }
        if(!esNumero || sscanf(buf, "%d", &nroAlq) != 1){
            printf("[Error] Entrada invalida. Ingrese solo numeros.\n");
            continue;
        }
        if(nroAlq == 0){
            fclose(fAlq);
            return;
        }
        rewind(fAlq);
        encontrado = 0;
        while(fread(&a, sizeof(alquiler), 1, fAlq) == 1){
            if(a.nroAlquiler == nroAlq){
                encontrado = 1;
                float saldoActual = a.importeTotal - a.importePagado;
                if(a.estado == 0){
                    printf("[Error] No se pueden registrar pagos en un alquiler cancelado.\n\n");
                }else if(saldoActual <= 0.005f){
                    printf("[Error] Este alquiler ya se encuentra totalmente saldado.\n\n");
                }else{
                    alquilerValidoParaPagar = 1;
                    posAlquiler = ftell(fAlq) - sizeof(alquiler);
                }
                break;
            }
        }
        if(alquilerValidoParaPagar){
            break;
        }
        if(!encontrado){
            printf("[Error] El numero de alquiler ingresado no existe. Intente nuevamente.\n\n");
        }
    }
    float saldoPendiente = a.importeTotal - a.importePagado;
    printf("Importe Total: $%.2f | Ya Pagado: $%.2f | Saldo Restante: $%.2f\n", a.importeTotal, a.importePagado, saldoPendiente);
    do {
        printf("Ingrese monto a abonar: ");
        if(fgets(buf, sizeof(buf), stdin) == NULL){
            continue;
        }
        if(buf[0] == '\n' || buf[0] == '\r'){
            printf("[Aviso] No ingreso ningun dato.\n\n");
            continue;
        }
        if(sscanf(buf, " %f %c", &montoPago, &basura) != 1){
            printf("[Error] Monto invalido. Ingrese solo un numero.\n\n");
            montoPago = 0;
            continue;
        }
        if(montoPago <= 0){
            printf("[Error] El monto debe ser mayor a 0.\n\n");
        } else if(montoPago > saldoPendiente + 0.005f){
            printf("[Error] El monto ingresado ($%.2f) supera el saldo pendiente ($%.2f).\n\n", montoPago, saldoPendiente);
        }
    } while(montoPago <= 0 || montoPago > saldoPendiente + 0.005f);
    int fechaValidaPago = 0;
    time_t tActual = time(NULL);
    do {
        printf("Ingrese fecha del pago (DD/MM/AAAA): ");
        if(fgets(buf, sizeof(buf), stdin) == NULL){
            continue;
        }
        buf[strcspn(buf, "\n")] = '\0';
        if(buf[0] == '\0'){
            printf("[Aviso] No ingreso ninguna fecha.\n\n");
            continue;
        }
        char basuraFecha;
        if(sscanf(buf, "%d/%d/%d %c", &fecPago.dia, &fecPago.mes, &fecPago.anio, &basuraFecha) != 3){
            printf("[Error] Formato invalido. Use estrictamente DD/MM/AAAA.\n\n");
            continue;
        }
        if(!fechaValida(fecPago.dia, fecPago.mes, fecPago.anio)){
            printf("[Error] La fecha ingresada no existe en el calendario.\n\n");
            continue;
        }
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
        if(tPago < tInicio){
            printf("[Error] La fecha del pago no puede ser anterior al inicio del alquiler (%02d/%02d/%d).\n\n", 
            a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
        } else if(tPago > (tActual + 86400)){
            printf("[Error] La fecha del pago no puede ser posterior al dia de hoy.\n\n");
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
