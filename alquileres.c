#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "alquileres.h"
#include "vehiculos.h"
void menuAlquileres(void){
	int opcion;
	do{
		printf("\n=== MENU ALQUILERES ===\n");
		printf("1. Realizar alquiler: \n");
		printf("2. Listar alquileres: \n");
		printf("0. Volver al menu principal: \n");
		printf("Seleccione una opcion: ");
		scanf("%d", &opcion);
		switch(opcion){
			case 1:
				realizarAlquiler();
				break;
			case 2:
				listarAlquileres();
				break;
			case 0:
				printf("Regresando al menu principal...\n");
				break;
			default:
				printf("Opcion invalida.\n");
		}
	}while(opcion!=0);
}
void realizarAlquiler(void){
	alquiler nuevo;
	printf("\n=== REALIZAR ALQUILER ===\n");
	printf("Ingrese numero de alquiler: ");
	scanf("%d", &nuevo.nroAlquiler);
	printf("Ingrese DNI del cliente: ");
	scanf("%d", &nuevo.dniCliente);
	printf("Ingrese patente del vehiculo: ");
	scanf("%s", nuevo.patenteVehiculo);
	FILE *fVeh = fopen("vehiculos.dat", "rb+");
	if(fVeh == NULL){
		printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
		return;
	}
	vehiculo v;
	int vehiculoEncontrado = 0;
	float precioPorDia = 0.0;
	long posVehiculo = 0;
	while(fread(&v, sizeof(vehiculo), 1, fVeh) == 1){
		if(strcmp(v.patente, nuevo.patenteVehiculo) == 0){
			vehiculoEncontrado = 1;
			if(v.estado == 1){
				precioPorDia = v.precioPorDia;
				posVehiculo = ftell(fVeh) - sizeof(vehiculo);
			}else{
				printf("[Error] El vehiculo no existe o no se encuentra disponible.\n");
				fclose(fVeh);
				return;
			}
			break;
		}
	}
	if(!vehiculoEncontrado){
		printf("[Error] No existe un vehiculo con esa patente.\n");
		fclose(fVeh);
		return;
	}
	printf("Ingrese fecha de inicio (DD/MM/AAAA): ");
	scanf("%d/%d/%d", &nuevo.fecInicio.dia, &nuevo.fecInicio.mes, &nuevo.fecInicio.anio);
	printf("Ingrese fecha prevista de devolucion (DD/MM/AAAA): ");
	scanf("%d/%d/%d", &nuevo.fecPrevitaDevolucion.dia, &nuevo.fecPrevitaDevolucion.mes, &nuevo.fecPrevitaDevolucion.anio);
	printf("Ingrese cantidad de dias: ");
	scanf("%d", &nuevo.cantidadDias);
	if(nuevo.cantidadDias <= 0){
		printf("[Error] La cantidad de dias debe ser mayor a cero (0).\n");
		fclose(fVeh);
		return;
	}
	nuevo.importeTotal = precioPorDia * nuevo.cantidadDias;
	nuevo.importePagado = 0.0;
	nuevo.estado = 1;
	FILE *fAlq = fopen("alquileres.dat", "ab");
	if(fAlq == NULL){
		printf("[Error] No se pudo abrir el archivo de alquileres.\n");
		fclose(fVeh);
		return;
	}
	fwrite(&nuevo, sizeof(alquiler), 1, fAlq);
	fclose(fAlq);
	v.estado = 2;
	fseek(fVeh, posVehiculo, SEEK_SET);
	fwrite(&v, sizeof(vehiculo), 1, fVeh);
	fflush(fVeh);
	fclose(fVeh);
	printf("[Exito] Alquiler registrado correctamente. Importe total: %.2f\n", nuevo.importeTotal);
}
void listarAlquileres(void){
	FILE *archivo = fopen("alquileres.dat", "rb");
	if(archivo == NULL){
		printf("[Error] No se pudo abrir el archivo de alquileres o aun no hay registros.\n");
		return;
	}
	alquiler a;
	int contador = 0;
	printf("\n=== LISTADO DE ALQUILERES ===\n");
	while(fread(&a, sizeof(alquiler), 1, archivo) == 1){
		printf("Nro Alquiler: %d | DNI Cliente: %s | Patente: %s\n", a.nroAlquiler, a.dniCliente, a.patenteVehiculo);
		printf("Inicio: %02d/%02d/%d | Prevista Devolucion: %02d/%02d/%d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio, a.fecPrevitaDevolucion.dia, a.fecEfectivaDevolucion.mes, a.fecEfectivaDevolucion.anio);
		printf("Dias: %d | Importe Total: %.2f | Pagado: $%.2f | Estado: %d\n", a.cantidadDias, a.importeTotal, a.importePagado, a.estado);
		contador = contador + 1;
	}
	fclose(archivo);
	if(contador == 0){
		printf("No hay alquileres registrados en el sistema.\n");
	}
}
