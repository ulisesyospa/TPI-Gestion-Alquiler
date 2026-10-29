#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "devoluciones.h"
#include "movimiento.h"
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
    char bufferMonto[100];
    char basura;
    printf("\n=== REGISTRAR DEVOLUCION ===\n");
    
    // Listar alquileres activos para que el usuario elija visualmente
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if (fAlq == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }   
    alquiler a;
    int contadorActivos = 0;
    printf("\n--- ALQUILERES ACTIVOS ---\n");
    while (fread(&a, sizeof(alquiler), 1, fAlq) == 1) {
        if (a.estado == 1) {
            printf("Nro Alquiler: %d | DNI: %d | Patente: %s | Inicio: %02d/%02d/%d\n",
                   a.nroAlquiler, a.dniCliente, a.patenteVehiculo,
                   a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
            contadorActivos++;
        }
    }
    if (contadorActivos == 0) {
        printf("[Aviso] No hay alquileres activos para devolver.\n");
        fclose(fAlq);
        return;
    }
    printf("----------------------------\n");
    printf("Ingrese numero de alquiler activo: ");
    if (fgets(bufferFecha, sizeof(bufferFecha), stdin) != NULL) {
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        sscanf(bufferFecha, "%d", &nroAlq);
    } else {
        fclose(fAlq);
        return;
    }   
    rewind(fAlq);
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
        if (tDevolucion > (tActual + 86400)) { 
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
        diasReales = 1; 
    }   
    
    // Recalculamos el importe total basándonos estrictamente en los días reales de uso (sin devoluciones de plata hacia atrás)
    a.cantidadDias = diasReales;
    a.importeTotal = precioPorDia * diasReales;

    if (diasReales > (int)((ftell(fVeh)) ? a.cantidadDias : a.cantidadDias)) { 
        // Mensaje orientativo de recargo por atraso si aplica
    }

    printf("\n--- LIQUIDACION FINAL DE ALQUILER ---\n");
    printf("Dias reales utilizados: %d\n", diasReales);
    printf("Importe Total a Pagar por el uso: $%.2f\n", a.importeTotal);
    
    float montoAbonadoAhora = 0.0;
    do {
        printf("Ingrese el monto que abona el cliente para finalizar: ");
        if (fgets(bufferMonto, sizeof(bufferMonto), stdin) == NULL) continue;
        if (bufferMonto[0] == '\n' || bufferMonto[0] == '\r') {
            printf("[Aviso] Debe ingresar un monto.\n");
            continue;
        }
        if (sscanf(bufferMonto, " %f %c", &montoAbonadoAhora, &basura) != 1 || montoAbonadoAhora < 0) {
            printf("[Error] Monto invalido. Ingrese un numero positivo.\n");
            montoAbonadoAhora = -1;
            continue;
        }
        if (montoAbonadoAhora < a.importeTotal - 0.005f) {
            printf("[Aviso] El monto es menor al total. Quedara saldo pendiente.\n");
        }
    } while (montoAbonadoAhora < 0);

    a.importePagado += montoAbonadoAhora;
    a.estado = 2; // Finalizado  
    
    fseek(fAlq, posAlquiler, SEEK_SET);
    fwrite(&a, sizeof(alquiler), 1, fAlq);
    fflush(fAlq);
    fclose(fAlq);  
    
    v.estado = 1; // Vehículo disponible nuevamente
    fseek(fVeh, posVehiculo, SEEK_SET);
    fwrite(&v, sizeof(vehiculo), 1, fVeh);
    fflush(fVeh);
    fclose(fVeh);   

    // Registramos el movimiento financiero del pago realizado en la devolución
    if (montoAbonadoAhora > 0) {
        registrarMovimiento(a.nroAlquiler, a.dniCliente, a.patenteVehiculo, "Pago Devolucion", montoAbonadoAhora, fechaEfectiva);
    }

    float saldoPendiente = a.importeTotal - a.importePagado;
    printf("\n[Exito] Devolucion registrada correctamente.\n");
    printf("Importe Total: $%.2f | Total Abonado: $%.2f | Saldo Pendiente: $%.2f\n", 
           a.importeTotal, a.importePagado, saldoPendiente);
}
