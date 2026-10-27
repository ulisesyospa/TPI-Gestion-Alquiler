#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "devoluciones.h"
// Menú de Devoluciones
void menuDevoluciones(void) {
    int opcion;
    do {
        printf("\n=== MENU DEVOLUCIONES ===\n");
        printf("1. Registrar devolucion\n");
        printf("0. Volver al menu principal\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);
        
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
// Función auxiliar para calcular los días transcurridos entre dos fechas usando <time.h>
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
    return dias > 0 ? dias : 0;
}
// 3. REGISTRAR DEVOLUCIÓN
void registrarDevolucion(void) {
    int nroAlq;
    fecha fechaEfectiva;   
    printf("\n=== REGISTRAR DEVOLUCION ===\n");
    printf("Ingrese numero de alquiler activo: ");
    scanf("%d", &nroAlq);    
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if (fAlq == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }    
    alquiler a;
    int encontrado = 0;
    long posAlquiler = 0;    
    while (fread(&a, sizeof(alquiler), 1, fAlq) == 1) {
        if (a.nroAlquiler == nroAlq && a.estado == 1) { // 1: Activo
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
    // Bucle para validar que la fecha ingresada sea real y lógica usando fechaValida
    do {
        printf("Ingrese fecha efectiva de devolucion (DD/MM/AAAA): ");
        scanf("%d/%d/%d", &fechaEfectiva.dia, &fechaEfectiva.mes, &fechaEfectiva.anio);       
        if (!fechaValida(fechaEfectiva.dia, fechaEfectiva.mes, fechaEfectiva.anio)) {
            printf("[Error] La fecha ingresada no existe o es invalida. Intente nuevamente.\n");
        } else if (calcularDiasTranscurridosDevolucion(a.fecInicio, fechaEfectiva) < 0) {
            printf("[Error] La fecha de devolucion no puede ser anterior a la fecha de inicio del alquiler.\n");
            fechaEfectiva.dia = 0; // Forzar repetición del bucle
        }
    } while (!fechaValida(fechaEfectiva.dia, fechaEfectiva.mes, fechaEfectiva.anio));   
    // Guardar fecha efectiva
    a.fecEfectivaDevolucion = fechaEfectiva;    
    // Abrir vehículos para buscar el precio por día y liberar el vehículo
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
        // CORREGIDO: strcasecmp para buscar la patente sin importar mayúsculas/minúsculas
        if (strcasecmp(v.patente, a.patenteVehiculo) == 0) {
            precioPorDia = v.precioPorDia;
            posVehiculo = ftell(fVeh) - sizeof(vehiculo);
            break;
        }
    }    
    // Calcular días reales transcurridos desde el inicio del alquiler hasta la devolución
    int diasReales = calcularDiasTranscurridosDevolucion(a.fecInicio, fechaEfectiva);    
    // Caso 1: Devolución fuera de término (Retraso -> Aumenta importe)
    if (diasReales > a.cantidadDias) {
        int diasAdicionales = diasReales - a.cantidadDias;
        float recargo = diasAdicionales * precioPorDia;
        a.importeTotal += recargo;
        printf("[Aviso] Devolucion fuera de termino. Dias de atraso: %d. Recargo aplicado: $%.2f\n", diasAdicionales, recargo);
    }
    // Caso 2: Devolución anticipada (Devuelve antes -> Disminuye importe)
    else if (diasReales < a.cantidadDias && diasReales > 0) {
        int diasNoUtilizados = a.cantidadDias - diasReales;
        float descuento = diasNoUtilizados * precioPorDia;
        a.importeTotal -= descuento;
        if (a.importeTotal < 0) a.importeTotal = 0; // Seguridad para que no dé negativo
        printf("[Aviso] Devolucion anticipada. Dias no utilizados: %d. Descuento aplicado: -$%.2f\n", diasNoUtilizados, descuento);
    }   
    // Actualizar estado del alquiler a Finalizado (estado = 2)
    a.estado = 2;   
    fseek(fAlq, posAlquiler, SEEK_SET);
    fwrite(&a, sizeof(alquiler), 1, fAlq);
    fflush(fAlq);
    fclose(fAlq);  
    // Actualizar el estado del vehículo a Disponible (estado = 1)
    v.estado = 1; 
    fseek(fVeh, posVehiculo, SEEK_SET);
    fwrite(&v, sizeof(vehiculo), 1, fVeh);
    fflush(fVeh);
    fclose(fVeh);    
    // Cálculo del saldo pendiente restando lo que ya abonó
    float saldoPendiente = a.importeTotal - a.importePagado;
    printf("[Exito] Devolucion registrada correctamente.\n");
    printf("Importe Total Actualizado: $%.2f | Ya Pagado: $%.2f | Saldo Restante a Abonar: $%.2f\n", 
           a.importeTotal, a.importePagado, saldoPendiente);
}
