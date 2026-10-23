
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "movimiento.h"


int fechaValida(int dia, int mes, int anio){
    struct tm fecha = {0};

    fecha.tm_mday = dia;
    fecha.tm_mon = mes - 1;
    fecha.tm_year = anio - 1900;

    struct tm copia = fecha;

    time_t controlFecha = mktime(&copia);

    if(controlFecha == -1 ||
       copia.tm_mday != dia ||
       copia.tm_mon != mes - 1 ||
       copia.tm_year != anio - 1900){
        return 0;
    }

    return 1;
}


void limpiarBuffer(void){
    int c;

    while((c = getchar()) != '\n' && c != EOF);
}


/* GENERAR NUMERO AUTOMATICO DE ALQUILER */

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


void menuAlquileres(void){
    int opcion;

    do{
        printf("\n=== MENU ALQUILERES ===\n");
        printf("1. Realizar alquiler:\n");
        printf("2. Listar alquileres:\n");
        printf("0. Volver al menu principal:\n");
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

    }while(opcion != 0);
}


void realizarAlquiler(void){
    alquiler nuevo;

    printf("\n=== REALIZAR ALQUILER ===\n");

    /* NUMERO AUTOMATICO */

    nuevo.nroAlquiler = generarNumeroAlquiler();

    printf("Numero de alquiler asignado: %d\n", nuevo.nroAlquiler);

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
                printf("[Error] El vehiculo no se encuentra disponible.\n");
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


    /* VALIDACION DE FECHAS */

    int fechasCorrectas = 0;

    do{
        printf("Ingrese fecha de inicio (DD/MM/AAAA): ");

        if(scanf("%d/%d/%d",
                 &nuevo.fecInicio.dia,
                 &nuevo.fecInicio.mes,
                 &nuevo.fecInicio.anio) != 3){

            printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
            limpiarBuffer();

        }else if(fechaValida(nuevo.fecInicio.dia,
                             nuevo.fecInicio.mes,
                             nuevo.fecInicio.anio) == 0){

            printf("[Error] La fecha de inicio no existe.\n");

        }else{

            printf("Ingrese fecha prevista de devolucion (DD/MM/AAAA): ");

            if(scanf("%d/%d/%d",
                     &nuevo.fecPrevitaDevolucion.dia,
                     &nuevo.fecPrevitaDevolucion.mes,
                     &nuevo.fecPrevitaDevolucion.anio) != 3){

                printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
                limpiarBuffer();

            }else if(fechaValida(nuevo.fecPrevitaDevolucion.dia,
                                 nuevo.fecPrevitaDevolucion.mes,
                                 nuevo.fecPrevitaDevolucion.anio) == 0){

                printf("[Error] La fecha de devolucion no existe.\n");

            }else{

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

                if(fechaInicio == -1 || fechaDevolucion == -1){

                    printf("[Error] No se pudieron procesar las fechas.\n");

                }else if(fechaDevolucion <= fechaInicio){

                    printf("[Error] La devolucion debe ser posterior al inicio.\n");

                }else{

                    fechasCorrectas = 1;
                }
            }
        }

    }while(fechasCorrectas == 0);


    /* CALCULAR CANTIDAD DE DIAS AUTOMATICAMENTE */

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

    nuevo.cantidadDias = (int)(difftime(fechaDevolucion, fechaInicio) / (60 * 60 * 24));

    printf("Cantidad de dias calculada: %d\n", nuevo.cantidadDias);


    /* CALCULAR IMPORTE */

    nuevo.importeTotal = precioPorDia * nuevo.cantidadDias;
    nuevo.importePagado = 0.0;
    nuevo.estado = 1;


    /* GUARDAR ALQUILER */

    FILE *fAlq = fopen("alquileres.dat", "ab");

    if(fAlq == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        fclose(fVeh);
        return;
    }

    fwrite(&nuevo, sizeof(alquiler), 1, fAlq);
    fclose(fAlq);


    /* ACTUALIZAR ESTADO DEL VEHICULO */

    v.estado = 2;

    fseek(fVeh, posVehiculo, SEEK_SET);
    fwrite(&v, sizeof(vehiculo), 1, fVeh);

    fflush(fVeh);
    fclose(fVeh);

    printf("[Exito] Alquiler registrado correctamente.\n");
    printf("Numero de alquiler: %d\n", nuevo.nroAlquiler);
    printf("Cantidad de dias: %d\n", nuevo.cantidadDias);
    printf("Importe total: $%.2f\n", nuevo.importeTotal);
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

        printf("\nNro Alquiler: %d | DNI Cliente: %d | Patente: %s\n",
               a.nroAlquiler, a.dniCliente, a.patenteVehiculo);

        printf("Inicio: %02d/%02d/%d | Prevista Devolucion: %02d/%02d/%d\n",
               a.fecInicio.dia,
               a.fecInicio.mes,
               a.fecInicio.anio,
               a.fecPrevitaDevolucion.dia,
               a.fecPrevitaDevolucion.mes,
               a.fecPrevitaDevolucion.anio);

        printf("Dias: %d | Importe Total: %.2f | Pagado: $%.2f | Estado: %d\n",
               a.cantidadDias,
               a.importeTotal,
               a.importePagado,
               a.estado);

        contador = contador + 1;
    }
    
    printf("\n\nCantidas de Alquileres: %d\n", contador);

    fclose(archivo);

    if(contador == 0){
        printf("No hay alquileres registrados en el sistema.\n");
    }
}

