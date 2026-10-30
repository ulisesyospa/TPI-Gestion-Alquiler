#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "alquileres.h"
#include "vehiculos.h"
#include "devoluciones.h"
#include "movimiento.h"
// Menu principal interactivo para la gestion de devoluciones de la flota
void menuDevoluciones(void) {
    int opcion;
    char buffer[100];
    char basura;
    do {
        printf("\n========================================\n");
        printf("           MENU DEVOLUCIONES            \n");
        printf("========================================\n");
        printf("1. Registrar devolucion\n");
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
        switch(opcion) {
            case 1:
                registrarDevolucion();
                break;
            case 0:
                printf("\nRegresando al menu principal...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    } while(opcion != 0);
}
// Utiliza funciones de tiempo para calcular la diferencia exacta en dias entre dos fechas
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
    if (dias >= 0) {
        return dias;
    } else {
        return -1;
    }
}
// Procesa el cierre de contrato, libera el vehiculo, recalcula el total por dias reales y cobra el monto
void registrarDevolucion(void) {
    int nroAlq;
    fecha fechaEfectiva;   
    char bufferFecha[100];
    char bufferMonto[100];
    char basura;
    printf("\n========================================\n");
    printf("          REGISTRAR DEVOLUCION          \n");
    printf("========================================\n");
    // Apertura y filtrado de alquileres activos
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if (fAlq == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }   
    alquiler a;
    int contadorActivos = 0;
    printf("\n--- ALQUILERES ACTIVOS EN CURSO ---\n");
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
    printf("----------------------------------------\n");
    // Lectura e ingreso seguro del numero de alquiler
    while (1) {
        printf("Ingrese numero de alquiler activo: ");
        if (fgets(bufferFecha, sizeof(bufferFecha), stdin) != NULL) {
            bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
            if (bufferFecha[0] == '\0') {
                printf("[Aviso] No ingreso ningun numero.\n");
                continue;
            }
            if (sscanf(bufferFecha, " %d %c", &nroAlq, &basura) == 1 && nroAlq > 0) {
                break;
            }
        }
        printf("[Error] Numero invalido. Ingrese un entero mayor a 0.\n");
    }
    rewind(fAlq);
    int encontrado = 0;
    long posAlquiler = 0;   
    // Busqueda del contrato seleccionado y ubicacion en el archivo
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
    // Ingreso y validacion de la fecha de devolucion efectiva
    do {
        printf("Ingrese fecha efectiva de devolucion (DD/MM/AAAA): ");
        if (fgets(bufferFecha, sizeof(bufferFecha), stdin) == NULL) continue;
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        if (sscanf(bufferFecha, "%d/%d/%d %c", &fechaEfectiva.dia, &fechaEfectiva.mes, &fechaEfectiva.anio, &basura) != 3) {
            printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
            continue;
        }
        if (!fechaValida(fechaEfectiva.dia, fechaEfectiva.mes, fechaEfectiva.anio)) {
            printf("[Error] La fecha ingresada no existe en el calendario. Intente nuevamente.\n");
            continue;
        }
        int diasDesdeInicio = calcularDiasTranscurridosDevolucion(a.fecInicio, fechaEfectiva);
        if (diasDesdeInicio < 0) {
            printf("[Error] La fecha de devolucion no puede ser anterior al inicio del alquiler (%02d/%02d/%d).\n",
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
    // Busqueda del vehiculo correspondiente para obtener su tarifa diaria
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
    // Recalculo del importe total segun los dias reales transcurridos
    a.cantidadDias = diasReales;
    a.importeTotal = precioPorDia * diasReales;
    float saldoPendienteReal = a.importeTotal - a.importePagado;
    if (saldoPendienteReal < 0) {
        saldoPendienteReal = 0;
    }
    printf("\n--- LIQUIDACION FINAL DEL ALQUILER ---\n");
    printf("Dias reales utilizados: %d\n", diasReales);
    printf("Importe Total a Pagar: $%.2f | Ya Abonado Previamente: $%.2f\n", a.importeTotal, a.importePagado);
    printf("Saldo Pendiente de Cobro: $%.2f\n", saldoPendienteReal);
    float montoAbonadoAhora = 0.0;
    // Cobro del monto de la liquidacion controlando no exceder el saldo pendiente real
    do {
        printf("Ingrese el monto que abona el cliente en este momento: ");
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
        if (montoAbonadoAhora > saldoPendienteReal) {
            printf("[Error] El monto ingresado ($%.2f) supera el saldo pendiente ($%.2f).\n", montoAbonadoAhora, saldoPendienteReal);
            montoAbonadoAhora = -1;
            continue;
        }
        if (montoAbonadoAhora < saldoPendienteReal) {
            printf("[Aviso] El monto ingresado no cubre la totalidad de la deuda. Quedara saldo pendiente.\n");
        }
    } while (montoAbonadoAhora < 0);
    // Actualizacion de montos y definicion del estado del contrato
    a.importePagado += montoAbonadoAhora;
    float saldoRestante = a.importeTotal - a.importePagado;
    if (saldoRestante <= 0) {
        a.estado = 2; // Finalizado totalmente
        saldoRestante = 0;
    } else {
        a.estado = 1; // Mantiene activo por deuda pendiente
    }
    fseek(fAlq, posAlquiler, SEEK_SET);
    fwrite(&a, sizeof(alquiler), 1, fAlq);
    fflush(fAlq);
    fclose(fAlq);  
    // Liberacion inmediata del vehiculo asignando estado Disponible (1)
    v.estado = 1; 
    fseek(fVeh, posVehiculo, SEEK_SET);
    fwrite(&v, sizeof(vehiculo), 1, fVeh);
    fflush(fVeh);
    fclose(fVeh);   
    // Anexo de transaccion en movimientos.dat y archivo .txt si hubo cobro
    if (montoAbonadoAhora > 0) {
        registrarMovimiento(a.nroAlquiler, a.dniCliente, a.patenteVehiculo, "Pago Devolucion", montoAbonadoAhora, fechaEfectiva);
    }
    printf("\n[Exito] Devolucion registrada correctamente. El vehiculo ya se encuentra disponible.\n");
    printf("Importe Total: $%.2f | Total Pagado: $%.2f | Saldo Pendiente: $%.2f\n", 
           a.importeTotal, a.importePagado, saldoRestante);
}
