#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "vehiculos.h"
int esFormatoPatenteValido(char *patente) {
    if (strlen(patente) != 7) {
        return 0;
    }
    if (!isalpha(patente[0]) || !isalpha(patente[1])) {
        return 0;
    }
    if (!isdigit(patente[2]) || !isdigit(patente[3]) || !isdigit(patente[4])) {
        return 0;
    }
    if (!isalpha(patente[5]) || !isalpha(patente[6])) {
        return 0;
    }
    return 1;
}
void menuVehiculos(void){
	int opcion;
	do{
		printf("\n=== MENU VEHICULOS ===\n");
		printf("1. Registrar vehiculo\n");
		printf("2. Buscar vehiculo\n");
		printf("3. Mostrar vehiculos\n");
		printf("4. Modificar vehiculo\n");
		printf("0. Volver al menu principal\n");
		printf("Seleccione una opciocion: ");
		scanf("%d", &opcion);
	switch(opcion){			
			case 1:
				registrarVehiculo();
			break;			
			case 2: {
				char patenteBusq[7];
				printf("Ingrese la patente a buscar: ");
				scanf("%s", patenteBusq);
				if(buscarVehiculo(patenteBusq)){
					printf("[Exito] El vehiculo con patente %s se encuentra registrado. \n", patenteBusq);
				}else{
					printf("[Error] No existe vehiculo con esa patente.\n");
				}
				break;
			}			
			case 3:
				mostrarVehiculo();				
			break;			
			case 4:
				modificarVehiculo();				
			break;			
			case 0:
				volverAlMenuPrincipal();
				return;
			break;			
			default:
				printf("Opcion invalida.\n");
		}		
	}while(opcion != 0);
}
void registrarVehiculo(void) {
    vehiculo v;
    FILE *archivo;
    int resultado;
    int caracter;
    int i;
    printf("\n--- REGISTRAR VEHICULO ---\n"); 
    getchar();
    while (1) {
        printf("Patente (Formato AA000AA): ");
        fgets(v.patente, sizeof(v.patente), stdin);
        v.patente[strcspn(v.patente, "\n")] = '\0';
        for(i=0;v.patente[i];i++) {
            v.patente[i] = toupper(v.patente[i]);
        }
        if (!esFormatoPatenteValido(v.patente)) {
            printf("[Error] Formato invalido. Debe ingresar exactamente 2 letras, 3 numeros y 2 letras (Ej: AB123CD).\n");
            continue;
        }
        break;
    }
    if(buscarVehiculo(v.patente) == 1) {
        printf("\nYa existe un vehiculo con esa patente.\n");
        return;
    }
    printf("Marca: ");
    fgets(v.marca, sizeof(v.marca), stdin);
    v.marca[strcspn(v.marca, "\n")] = '\0';
    while (strcmp(v.marca, "") == 0){
        printf("La marca no puede estar vacia. Ingrese nuevamente: ");
        fgets(v.marca, sizeof(v.marca), stdin);
        v.marca[strcspn(v.marca, "\n")] = '\0';
    }
    printf("Modelo: ");
    fgets(v.modelo, sizeof(v.modelo), stdin);
    v.modelo[strcspn(v.modelo, "\n")] = '\0';
    while(strcmp(v.modelo, "") == 0){
        printf("El modelo no puede estar vacio. Ingrese nuevamente: ");
        fgets(v.modelo, sizeof(v.modelo), stdin);
        v.modelo[strcspn(v.modelo, "\n")] = '\0';
    }
    printf("Anio: ");
    resultado = scanf("%d", &v.anio);
    caracter = getchar();
    while(resultado != 1 || v.anio < 1886 || v.anio > 2026 || (caracter != '\n' && caracter != EOF)){
        while(caracter != '\n' && caracter != EOF){
            caracter = getchar();
        }
        printf("Anio invalido. Ingrese un anio entre 1886 y 2026: ");
        resultado = scanf("%d", &v.anio);
        caracter = getchar();
    }
    printf("Tipo de vehiculo: ");
    fgets(v.tipo, sizeof(v.tipo), stdin);
    v.tipo[strcspn(v.tipo, "\n")] = '\0';
    while(strcmp(v.tipo, "") == 0){
        printf("El tipo no puede estar vacio. Ingrese nuevamente: ");
        fgets(v.tipo, sizeof(v.tipo), stdin);
        v.tipo[strcspn(v.tipo, "\n")] = '\0';
    }   
    printf("Capacidad de pasajeros: ");
    resultado = scanf("%d", &v.capacidad);
    caracter = getchar();
    while(resultado != 1 || v.capacidad <= 0 || (caracter != '\n' && caracter != EOF)){
        while(caracter != '\n' && caracter != EOF){
            caracter = getchar();
        }
        printf("Capacidad invalida. Ingrese un numero mayor que cero: ");
        resultado = scanf("%d", &v.capacidad);
        caracter = getchar();
    }  
    printf("Precio por dia: ");
    resultado = scanf("%f", &v.precioPorDia);
    caracter = getchar();
    while(resultado != 1 || v.precioPorDia <= 0 || (caracter != '\n' && caracter != EOF)){
        while(caracter != '\n' && caracter != EOF){
            caracter = getchar();
        }
        printf("Precio invalido. Ingrese un valor mayor que cero: ");
        resultado = scanf("%f", &v.precioPorDia);
        caracter = getchar();
    }    
    v.estado = 1;     
    archivo = fopen("vehiculos.dat", "ab");
    if(archivo == NULL) {
        printf("No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    if(fwrite(&v, sizeof(vehiculo), 1, archivo) == 1){
        printf("\nVehiculo registrado correctamente.\n");
    }else{
        printf("\nError al guardar el vehiculo.\n");
    }
    fclose(archivo);
}
int buscarVehiculo(char patente[]){
	vehiculo v;
	FILE *archivo;
	int encontrado = 0;	
	archivo = fopen("vehiculos.dat", "rb");
	if(archivo == NULL){
		return 0;
	}	
	while(encontrado == 0 && fread(&v, sizeof(vehiculo), 1, archivo) == 1){
		if(strcmp(v.patente, patente) == 0){
			encontrado = 1;
		}
	}		
	fclose(archivo);
	return encontrado;
}
int mostrarVehiculo(void){
	FILE *archivo = fopen("vehiculos.dat", "rb");
	if(archivo == NULL){
		printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
		return 0;
	}
	vehiculo v;
	int contador = 0;
	printf("\n === LISTADO GENERAL DE VEHICULOS ===\n");
	while(fread(&v, sizeof(vehiculo), 1, archivo) == 1){
		printf("Patente: %s | Marca: %s | Modelo: %s | Ano: %d | Tipo: %s | Capacidad: %d | Precio/Dia: %.2f | Estado: %d\n", v.patente, v.marca, v.modelo, v.anio, v.tipo, v.capacidad, v.precioPorDia, v.estado);
		contador = contador + 1;
	}
	fclose(archivo);
	if(contador == 0){
		printf("No hay vehiculos registrados en el sistema.\n");
	}
    return 1;
}
int modificarVehiculo(void){
	char patente[7];
	printf("\n=== MODIFICAR VEHICULO === \n");
	printf("Ingrese la patente del vehiculo a modificar: ");
	scanf("%s", patente);
	FILE *archivo = fopen("vehiculos.dat", "rb+");
	if(archivo == NULL){
		printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
		return 0;
	}
	vehiculo v;
	int encontrado = 0;
	while(fread(&v, sizeof(vehiculo), 1, archivo) == 1){
		if(strcmp(v.patente, patente) == 0){
			encontrado = 1;
			long posicionActual = ftell(archivo) - sizeof(vehiculo);
			printf("Vehiculo encontrado. Ingrese los nuevos datos:\n");
			printf("Nueva marca (Actual: %s): ", v.marca);
			scanf("%s", v.marca);
			printf("Nuevo modelo (Actual: %s): ", v.modelo);
			scanf("%s", v.modelo);
			printf("Nuevo año (Actual: %d): ", v.anio);
			scanf("%d", &v.anio);
			printf("Nueva capacidad (Actual: %d): ", v.capacidad);
			scanf("%d", &v.capacidad);
			printf("Nuevo Precio por dia (Actual: %.2f): ", v.precioPorDia);
			scanf("%f", &v.precioPorDia);
			printf("Nuevo estado 1. Disponible | 2. Alquilado | 3. Bajo Mantenimiento - (Actual: %d): ", v.estado);
			scanf("%d", &v.estado);
			fseek(archivo, posicionActual, SEEK_SET);
			fwrite(&v, sizeof(vehiculo), 1, archivo);
			fflush(archivo);
			printf("[Exito] Vehiculo modificado correctamente.\n");
			break;
		}
	}
	fclose(archivo);
	if(!encontrado){
		printf("[Error] No existe un vehiculo registrado con esa patente.\n");
	}
    return 1;
}
void volverAlMenuPrincipal(void){
	printf("\nRegresando al menu principal...\n");
}
