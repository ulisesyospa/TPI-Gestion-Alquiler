#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "movimiento.h"
// Solicita un entero mayor a cero utilizando lectura por buffer para prevenir colapsos por entrada invalida
int verificacionEntero(const char *mensaje) {
    char buffer[100];
    char basura;
    int numero;
    while (1) {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                printf("[Aviso] No ingreso ningun dato. Por favor, ingrese un DNI.\n\n");
                continue;
            }
            if (sscanf(buffer, " %d %c", &numero, &basura) == 1) {
                if (numero > 0) {
                    return numero;
                } else {
                    printf("[Error] El numero debe ser un entero positivo mayor a 0.\n\n");
                }
            } else {
                printf("[Error] Entrada invalida. Debe ingresar estrictamente un numero (sin letras ni espacios).\n\n");
            }
        }
    }
}
// Valida que la patente ingresada tenga 7 caracteres alfanumericos y convierte las letras a mayusculas
void patenteValida(char *destino, int tamano, const char *mensaje) {
    char buffer[100];
    int i;
    int longitud;
    while (1) {
        printf("%s", mensaje);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strlen(buffer) == 0) {
                printf("[Aviso] No ingreso ningun dato. Por favor, ingrese la patente.\n\n");
                continue;
            }
            longitud = (int)strlen(buffer);
            if (longitud != 7) {
                printf("[Error] La patente debe tener 7 caracteres.\n\n");
                continue;
            }
            int esValido = 1;
            for (i = 0; i < longitud; i++) {
                char c = buffer[i];
                if (c >= 'a' && c <= 'z') {
                    buffer[i] = c - 32;
                } else if (!(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9')) {
                    esValido = 0;
                    break;
                }
            }
            if (!esValido) {
                printf("[Error] La patente solo debe contener letras y numeros (sin espacios ni simbolos).\n\n");
                continue;
            }
            strncpy(destino, buffer, tamano - 1);
            destino[tamano - 1] = '\0';
            break;
        }
    }
}
// Comprueba si una fecha representada por dia, mes y anio existe en el calendario
int fechaValida(int dia, int mes, int anio){
    struct tm fecha = {0};
    fecha.tm_mday = dia;
    fecha.tm_mon = mes - 1;
    fecha.tm_year = anio - 1900;
    struct tm copia = fecha;
    time_t controlFecha = mktime(&copia);
    if(controlFecha == -1 || copia.tm_mday != dia || copia.tm_mon != mes - 1 || copia.tm_year != anio - 1900){
        return 0;
    }
    return 1;
}
// Vacía caracteres pendientes del buffer de entrada en lecturas directas
void limpiarBuffer(void){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}
// Obtiene el identificador autoincremental para el nuevo contrato leyendo el archivo binario
int generarNumeroAlquiler(void){
    FILE *archivo = fopen("alquileres.dat", "rb");
    if(archivo == NULL){
        return 1;
    }
    alquiler a;
    int mayor = 0;
    while(fread(&a, sizeof(alquiler), 1, archivo) == 1){
        if(a.nroAlquiler > mayor){
            mayor = a.nroAlquiler;
        }
    }
    fclose(archivo);
    return mayor + 1;
}
// Menu interactivo para las operaciones del modulo de alquileres
void menuAlquileres(void){
    int opcion;
    char buffer[100];
    char basura;
    do{
        printf("\n========================================\n");
        printf("            MENU ALQUILERES             \n");
        printf("========================================\n");
        printf("1. Realizar alquiler\n");
        printf("2. Listar alquileres\n");
        printf("0. Volver al menu principal\n");
        printf("----------------------------------------\n");
        printf("Seleccione una opcion: ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                opcion = -1;
                continue;
            }
            if (sscanf(buffer, " %d %c", &opcion, &basura) != 1) {
                printf("[Error] Entrada invalida. Ingrese solo numeros.\n");
                opcion = -1;
                continue;
            }
        }
        switch(opcion){
            case 1:
                realizarAlquiler();
                break;
            case 2:
                listarAlquileres();
                break;
            case 0:
                printf("\nRegresando al menu principal...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    }while(opcion != 0);
}
// Procesa un nuevo alquiler asociando cliente, vehiculo, periodo de dias y calculo de importe
void realizarAlquiler(void){
    alquiler nuevo;
    char bufferFecha[100];
    char basuraFecha;
    char bufferDni[100];
    char basuraDni;
    int dniIngresado;
    printf("\n========================================\n");
    printf("           REALIZAR ALQUILER            \n");
    printf("========================================\n");
    nuevo.nroAlquiler = generarNumeroAlquiler();
    printf("Numero de alquiler asignado: %d\n", nuevo.nroAlquiler);
    while(1) {
        printf("Ingrese DNI del cliente (exactamente 8 digitos, o 0 para volver): ");
        if(fgets(bufferDni, sizeof(bufferDni), stdin) == NULL) continue;
        bufferDni[strcspn(bufferDni, "\n")] = '\0';
        if(bufferDni[0] == '\0'){
            printf("[Aviso] No ingreso ningun dato.\n");
            continue;
        }
        if(sscanf(bufferDni, " %d %c", &dniIngresado, &basuraDni) != 1){
            printf("[Error] Entrada invalida. Ingrese solo numeros.\n");
            continue;
        }
        if(dniIngresado == 0){
            return;
        }
        if(dniIngresado < 10000000 || dniIngresado > 99999999){
            printf("[Error] DNI invalido. Debe tener exactamente 8 digitos numericos.\n");
            continue;
        }
        if(buscarCliente(dniIngresado) == 0){
            printf("[Error] El DNI ingresado no esta registrado en el sistema de clientes. Intente nuevamente.\n");
            continue;
        }
        nuevo.dniCliente = dniIngresado;
        break;
    }
    FILE *fVeh = fopen("vehiculos.dat", "rb+");
    if(fVeh == NULL){
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    vehiculo v;
    int contadorDisponibles = 0;
    printf("\n--- VEHICULOS DISPONIBLES EN FLOTA ---\n");
    while(fread(&v, sizeof(vehiculo), 1, fVeh) == 1){
        if(v.estado == 1){
            printf("Patente: %s | Marca/Modelo: %s %s | Precio por dia: $%.2f\n", v.patente, v.marca, v.modelo, v.precioPorDia);
            contadorDisponibles++;
        }
    }
    if(contadorDisponibles == 0){
        printf("[Aviso] No hay vehiculos disponibles en este momento para alquilar.\n");
        fclose(fVeh);
        return;
    }
    printf("----------------------------------------\n");
    
    // Bucle para permitir reintentar la patente si hay errores o no existe
    int vehiculoEncontrado = 0;
    float precioPorDia = 0.0;
    long posVehiculo = 0;
    while(!vehiculoEncontrado){
        patenteValida(nuevo.patenteVehiculo, sizeof(nuevo.patenteVehiculo), "Ingrese la patente del vehiculo a alquilar: ");
        rewind(fVeh);
        while(fread(&v, sizeof(vehiculo), 1, fVeh) == 1){
            if(strcasecmp(v.patente, nuevo.patenteVehiculo) == 0){
                if(v.estado == 1){
                    vehiculoEncontrado = 1;
                    precioPorDia = v.precioPorDia;
                    posVehiculo = ftell(fVeh) - sizeof(vehiculo);
                }else{
                    printf("[Error] El vehiculo seleccionado no se encuentra disponible.\n");
                }
                break;
            }
        }
        if(!vehiculoEncontrado){
            printf("[Error] No existe un vehiculo registrado con esa patente o esta ocupado. Intente nuevamente.\n");
        }
    }
    
    int fechasCorrectas = 0;
    time_t tActual = time(NULL);
    struct tm *infoActual = localtime(&tActual);
    int anioActual = infoActual->tm_year + 1900;
    do{
        printf("Ingrese fecha de inicio (DD/MM/AAAA): ");
        if(fgets(bufferFecha, sizeof(bufferFecha), stdin) == NULL){
            continue;
        }
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        if(bufferFecha[0] == '\0'){
            printf("[Aviso] No ingreso ningun dato. Por favor, ingrese la fecha de inicio.\n\n");
            continue;
        }
        int leidosInicio = sscanf(bufferFecha, "%d/%d/%d %c", &nuevo.fecInicio.dia, &nuevo.fecInicio.mes, &nuevo.fecInicio.anio, &basuraFecha);
        if(leidosInicio != 3){
            printf("[Error] Formato invalido o caracteres extra. Use DD/MM/AAAA.\n\n");
        }else if(fechaValida(nuevo.fecInicio.dia, nuevo.fecInicio.mes, nuevo.fecInicio.anio) == 0){
            printf("[Error] La fecha de inicio no existe en el calendario.\n\n");
        }else{
            struct tm inicio = {0};
            inicio.tm_mday = nuevo.fecInicio.dia;
            inicio.tm_mon = nuevo.fecInicio.mes - 1;
            inicio.tm_year = nuevo.fecInicio.anio - 1900;
            time_t fechaInicio = mktime(&inicio);
            if(fechaInicio < (tActual - 86400)){
                printf("[Error] La fecha de inicio no puede ser anterior a la actual.\n\n");
            }else if(nuevo.fecInicio.anio > anioActual + 1){
                printf("[Error] No se puede registrar un alquiler con tanta anticipacion.\n\n");
            }else{
                int devolucionCorrecta = 0;
                while(!devolucionCorrecta){
                    printf("Ingrese fecha prevista de devolucion (DD/MM/AAAA): ");
                    if(fgets(bufferFecha, sizeof(bufferFecha), stdin) == NULL){
                        continue;
                    }
                    bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
                    if(bufferFecha[0] == '\0'){
                        printf("[Aviso] No ingreso ningun dato. Por favor, ingrese la fecha de devolucion.\n\n");
                        continue;
                    }
                    int leidosDev = sscanf(bufferFecha, "%d/%d/%d %c", &nuevo.fecPrevitaDevolucion.dia, &nuevo.fecPrevitaDevolucion.mes, &nuevo.fecPrevitaDevolucion.anio, &basuraFecha);
                    if(leidosDev != 3){
                        printf("[Error] Formato invalido o caracteres extra. Use DD/MM/AAAA.\n\n");
                    }else if(fechaValida(nuevo.fecPrevitaDevolucion.dia, nuevo.fecPrevitaDevolucion.mes, nuevo.fecPrevitaDevolucion.anio) == 0){
                        printf("[Error] La fecha de devolucion no existe en el calendario.\n\n");
                    }else{
                        struct tm devolucion = {0};
                        devolucion.tm_mday = nuevo.fecPrevitaDevolucion.dia;
                        devolucion.tm_mon = nuevo.fecPrevitaDevolucion.mes - 1;
                        devolucion.tm_year = nuevo.fecPrevitaDevolucion.anio - 1900;
                        time_t fechaDevolucion = mktime(&devolucion);
                        if(fechaDevolucion <= fechaInicio){
                            printf("[Error] La devolucion debe ser posterior al inicio.\n\n");
                        }else{
                            fechasCorrectas = 1;
                            devolucionCorrecta = 1;
                        }
                    }
                }
            }
        }
    }while(fechasCorrectas == 0);
    struct tm inicio = {0};
    inicio.tm_mday = nuevo.fecInicio.dia;
    inicio.tm_mon = nuevo.fecInicio.mes - 1;
    inicio.tm_year = nuevo.fecInicio.anio - 1900;
    struct tm devolucion = {0};
    devolucion.tm_mday = nuevo.fecPrevitaDevolucion.dia;
    devolucion.tm_mon = nuevo.fecPrevitaDevolucion.mes - 1;
    devolucion.tm_year = nuevo.fecPrevitaDevolucion.anio - 1900;
    time_t fechaInicio = mktime(&inicio);
    time_t fechaDevolucion = mktime(&devolucion);
    nuevo.cantidadDias = (int)(difftime(fechaDevolucion, fechaInicio) / 86400.0);
    nuevo.importeTotal = precioPorDia * nuevo.cantidadDias;
    if(nuevo.importeTotal <= 0){
        printf("[Error] El importe total calculado no es valido (debe ser mayor a 0).\n");
        fclose(fVeh);
        return;
    }
    nuevo.importePagado = 0;
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
    printf("\n[Exito] Alquiler registrado correctamente.\n");
    printf("Numero de alquiler: %d\n", nuevo.nroAlquiler);
    printf("Cantidad de dias: %d\n", nuevo.cantidadDias);
    printf("Importe total: $%.2f\n", nuevo.importeTotal);
}
// Muestra el listado general de contratos guardados en alquileres.dat
void listarAlquileres(void){
    FILE *archivo = fopen("alquileres.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres o aun no hay registros.\n");
        return;
    }
    alquiler a;
    int contador = 0;
    printf("\n========================================\n");
    printf("         LISTADO DE ALQUILERES          \n");
    printf("========================================\n");
    while(fread(&a, sizeof(alquiler), 1, archivo) == 1){
        printf("Nro Alquiler: %d | DNI Cliente: %d | Patente: %s\n", a.nroAlquiler, a.dniCliente, a.patenteVehiculo);
        printf("Inicio: %02d/%02d/%d | Prevista Devolucion: %02d/%02d/%d\n",
               a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio,
               a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
        char estadoStr[20];
        if(a.estado == 1){
            strcpy(estadoStr, "Activo");
        }else if(a.estado == 2){
            strcpy(estadoStr, "Finalizado");
        }else{
            strcpy(estadoStr, "Cancelado");
        }
        printf("Dias: %d | Importe Total: $%.2f | Pagado: $%.2f | Estado: %s\n",
               a.cantidadDias, a.importeTotal, a.importePagado, estadoStr);
        printf("----------------------------------------\n");
        contador = contador + 1;
    }
    fclose(archivo);
    if(contador == 0){
        printf("[Aviso] No hay alquileres registrados en el sistema.\n");
    }else{
        printf("Cantidad total de alquileres: %d\n", contador);
    }
}
