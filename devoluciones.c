#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "devoluciones.h"
void menuDevoluciones(void) {
    int opcion;
    char buffer[100];
    char basura;
    do {
        printf("\n=== MENU DEVOLUCIONES ===\n");
        printf("1. Registrar devolucion\n");
        printf("0. Volver al menu principal\n");
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
        switch(opcion) {
            case 1:
                registrarDevolucion();
                break;
            case 0:
                printf("Regresando al menu principal...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while(opcion != 0);
}
int calcularDiasTranscurridosDevolucion(fecha fInicio, fecha fFin) {
    struct tm inicio = {0};
    struct tm fin = {0};   
    inicio.tm_mday = fInicio.dia;
    inicio.tm_mon = fInicio.mes - 1;
    inicio.tm_year = fInicio.anio - 1900;    
    fin.tm_mday = fFin.dia;
    fin.tm_mon = fFin.mes - 1;
    fin.tm_year = fFin.anio - 1900;  
    time_t tInicio = mktime(&inicio);
    time_t tFin = mktime(&fin);   
    double segundos = difftime(tFin, tInicio);
    int dias = (int)(segundos / 86400.0);    
    return dias >= 0 ? dias : -1;
}
void registrarDevolucion(void) {
    int nroAlq;
    fecha fechaEfectiva;   
    char bufferFecha[100];
    printf("\n=== REGISTRAR DEVOLUCION ===\n");
    printf("Ingrese numero de alquiler activo: ");
    if (fgets(bufferFecha, sizeof(bufferFecha), stdin) != NULL) {
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        sscanf(bufferFecha, "%d", &nroAlq);
    } else {
        return;
    }   
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if (fAlq == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }   
    alquiler a;
    int encontrado = 0;
    long posAlquiler = 0;   
    while (fread(&a, sizeof(alquiler), 1, fAlq) == 1) {
        if (a.nroAlquiler == nroAlq && a.estado == 1) {
            encontrado = 1;
            posAlquiler = ftell(fAlq) - sizeof(alquiler);
            break;
        }
    }   
    if (!encontrado) {
        printf("[Error] No se encontro un alquiler activo con ese numero.\n");
        fclose(fAlq);
        return;
    }   
    int fechaValidaDevolucion = 0;
    time_t tActual = time(NULL);
    do {
        printf("Ingrese fecha efectiva de devolucion (DD/MM/AAAA): ");
        if (fgets(bufferFecha, sizeof(bufferFecha), stdin) == NULL) {
            continue;
        }
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        if (sscanf(bufferFecha, "%d/%d/%d", &fechaEfectiva.dia, &fechaEfectiva.mes, &fechaEfectiva.anio) != 3) {
            printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
            continue;
        }
        if (!fechaValida(fechaEfectiva.dia, fechaEfectiva.mes, fechaEfectiva.anio)) {
            printf("[Error] La fecha ingresada no existe o es invalida. Intente nuevamente.\n");
            continue;
        }
        int diasDesdeInicio = calcularDiasTranscurridosDevolucion(a.fecInicio, fechaEfectiva);
        if (diasDesdeInicio < 0) {
            printf("[Error] La fecha de devolucion no puede ser anterior a la fecha de inicio del alquiler (%02d/%02d/%d).\n",
                   a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
            continue;
        }
        struct tm fComprobacion = {0};
        fComprobacion.tm_mday = fechaEfectiva.dia;
        fComprobacion.tm_mon = fechaEfectiva.mes - 1;
        fComprobacion.tm_year = fechaEfectiva.anio - 1900;
        time_t tDevolucion = mktime(&fComprobacion);
        if (tDevolucion > (tActual + 86400)) { // Margen de 1 día por huso horario
            printf("[Error] La fecha de devolucion no puede ser posterior a la fecha actual.\n");
            continue;
        }
        fechaValidaDevolucion = 1;
    } while (!fechaValidaDevolucion);   
    a.fecEfectivaDevolucion = fechaEfectiva;   
    FILE *fVeh = fopen("vehiculos.dat", "rb+");
    if (fVeh == NULL) {
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        fclose(fAlq);
        return;
    }   
    vehiculo v;
    float precioPorDia = 0.0;
    long posVehiculo = 0;  
    while (fread(&v, sizeof(vehiculo), 1, fVeh) == 1) {
        if (strcasecmp(v.patente, a.patenteVehiculo) == 0) {
            precioPorDia = v.precioPorDia;
            posVehiculo = ftell(fVeh) - sizeof(vehiculo);
            break;
        }
    }   
    int diasReales = calcularDiasTranscurridosDevolucion(a.fecInicio, fechaEfectiva);
    if (diasReales == 0) {
        diasReales = 1; // Mínimo 1 día de cobro si se devuelve el mismo día
    }   
    if (diasReales > a.cantidadDias) {
        int diasAdicionales = diasReales - a.cantidadDias;
        float recargo = diasAdicionales * precioPorDia;
        a.importeTotal += recargo;
        printf("[Aviso] Devolucion fuera de termino. Dias de atraso: %d. Recargo aplicado: $%.2f\n", diasAdicionales, recargo);
    } else if (diasReales < a.cantidadDias) {
        int diasNoUtilizados = a.cantidadDias - diasReales;
        float descuento = diasNoUtilizados * precioPorDia;
        a.importeTotal -= descuento;
        if (a.importeTotal < 0) a.importeTotal = 0;
        printf("[Aviso] Devolucion anticipada. Dias no utilizados: %d. Descuento aplicado: -$%.2f\n", diasNoUtilizados, descuento);
    }   
    a.estado = 2;   
    fseek(fAlq, posAlquiler, SEEK_SET);
    fwrite(&a, sizeof(alquiler), 1, fAlq);
    fflush(fAlq);
    fclose(fAlq);  
    v.estado = 1; 
    fseek(fVeh, posVehiculo, SEEK_SET);
    fwrite(&v, sizeof(vehiculo), 1, fVeh);
    fflush(fVeh);
    fclose(fVeh);   
    float saldoPendiente = a.importeTotal - a.importePagado;
    printf("[Exito] Devolucion registrada correctamente.\n");
    printf("Importe Total Actualizado: $%.2f | Ya Pagado: $%.2f | Saldo Restante a Abonar: $%.2f\n", 
           a.importeTotal, a.importePagado, saldoPendiente);
}
