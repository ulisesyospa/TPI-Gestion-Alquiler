#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "movimiento.h"
#include "alquileres.h"
// Calcula e incrementa de forma autoincremental el numero de movimiento en movimientos.dat
int obtenerSiguienteNroMovimiento(void) {
    FILE *archivo = fopen("movimientos.dat", "rb");
    if (archivo == NULL) {
        return 1;
    }
    movimiento m;
    int maxNro = 0;
    while (fread(&m, sizeof(movimiento), 1, archivo) == 1) {
        if (m.nroMovimiento > maxNro) {
            maxNro = m.nroMovimiento;
        }
    }
    fclose(archivo);
    return maxNro + 1;
}
// Menu interactivo para la administracion de pagos y ajustes de saldo
void menuPagos(void) {
    int opcion;
    char buffer[100];
    char basura;
    do {
        printf("\n========================================\n");
        printf("               MENU PAGOS               \n");
        printf("========================================\n");
        printf("1. Registrar pago o ajuste\n");
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
        switch (opcion) {
            case 1:
                registrarPago();
                break;
            case 0:
                printf("\nRegresando al menu principal...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    } while (opcion != 0);
}
// Registra la transaccion en movimientos.dat y genera el respaldo en texto plano
int registrarMovimiento(int nroAlquiler, int dniCliente, char patenteVehiculo[], char tipo[], float monto, fecha fec) {
    movimiento m;
    m.nroMovimiento = obtenerSiguienteNroMovimiento();
    m.nroAlquiler = nroAlquiler;
    m.dniCliente = dniCliente;
    strcpy(m.patenteVehiculo, patenteVehiculo);
    strcpy(m.tipoMovimiento, tipo);
    m.monto = monto;
    m.fec = fec;
    FILE *fBin = fopen("movimientos.dat", "ab");
    if (fBin == NULL) {
        printf("[Error] No se pudo abrir movimientos.dat\n");
        return 0;
    }
    fwrite(&m, sizeof(movimiento), 1, fBin);
    fclose(fBin);
    // Registro de comprobante en archivo de texto plano ordenado por DNI
    char nombreArchivoTxt[100];
    sprintf(nombreArchivoTxt, "movimientos_%d.txt", dniCliente);
    FILE *fTxt = fopen(nombreArchivoTxt, "a");
    if (fTxt != NULL) {
        fprintf(fTxt, "Nro Movimiento: %d | Nro Alquiler: %d | Tipo: %s | Monto: $%.2f | Fecha: %02d/%02d/%d | Patente: %s\n",
                m.nroMovimiento, m.nroAlquiler, m.tipoMovimiento, m.monto, m.fec.dia, m.fec.mes, m.fec.anio, m.patenteVehiculo);
        fclose(fTxt);
    }
    return 1;
}
// Interfaz para el cobro parcial o total de saldos pendientes de contratos activos
void registrarPago(void) {
    int nroAlq;
    char buffer[100];
    char basura;
    printf("\n========================================\n");
    printf("        REGISTRAR PAGO O AJUSTE         \n");
    printf("========================================\n");
    FILE *fAlq = fopen("alquileres.dat", "rb+");
    if (fAlq == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    alquiler a;
    int contadorDeudores = 0;
    printf("\n--- ALQUILERES CON SALDO PENDIENTE ---\n");
    while (fread(&a, sizeof(alquiler), 1, fAlq) == 1) {
        float saldo = a.importeTotal - a.importePagado;
        if (a.estado == 1 && saldo > 0) {
            printf("Nro Alquiler: %d | DNI: %d | Patente: %s | Total: $%.2f | Pagado: $%.2f | Saldo: $%.2f\n",
                   a.nroAlquiler, a.dniCliente, a.patenteVehiculo, a.importeTotal, a.importePagado, saldo);
            contadorDeudores++;
        }
    }
    if (contadorDeudores == 0) {
        printf("[Aviso] No hay alquileres activos con saldo pendiente de cobro.\n");
        fclose(fAlq);
        return;
    }
    printf("----------------------------------------\n");
    printf("Ingrese el numero de alquiler a cobrar: ");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        if (sscanf(buffer, " %d %c", &nroAlq, &basura) != 1) {
            printf("[Error] Numero de alquiler invalido.\n");
            fclose(fAlq);
            return;
        }
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
    float saldoPendiente = a.importeTotal - a.importePagado;
    printf("Saldo pendiente actual: $%.2f\n", saldoPendiente);
    // Seleccion del tipo de movimiento (Pago o Ajuste)
    int tipoOp;
    char tipoStr[20];
    printf("\n--- SELECCIONE TIPO DE TRANSACCION ---\n");
    printf("1. Pago de cliente\n");
    printf("2. Ajuste de saldo\n");
    printf("Seleccione opcion: ");
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, " %d %c", &tipoOp, &basura) == 1 && (tipoOp == 1 || tipoOp == 2)) {
                break;
            }
        }
        printf("[Error] Opcion invalida. Ingrese 1 para Pago o 2 para Ajuste: ");
    }
    if (tipoOp == 1) {
        strcpy(tipoStr, "Pago");
    } else {
        strcpy(tipoStr, "Ajuste");
    }
    float montoAbonado = 0.0;
    do {
        printf("Ingrese el monto a registrar ($): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (sscanf(buffer, " %f %c", &montoAbonado, &basura) != 1 || montoAbonado <= 0) {
            printf("[Error] Monto invalido. Ingrese un valor numerico positivo mayor a 0.\n");
            montoAbonado = -1;
            continue;
        }
        if (montoAbonado > saldoPendiente) {
            printf("[Error] El monto ($%.2f) excede el saldo pendiente ($%.2f).\n", montoAbonado, saldoPendiente);
            montoAbonado = -1;
            continue;
        }
    } while (montoAbonado <= 0);
    // Ingreso y validacion de la fecha del cobro
    fecha fPago;
    int fechaValidaFlag = 0;
    do {
        printf("Ingrese fecha del registro (DD/MM/AAAA): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (sscanf(buffer, "%d/%d/%d %c", &fPago.dia, &fPago.mes, &fPago.anio, &basura) == 3) {
            if (fechaValida(fPago.dia, fPago.mes, fPago.anio)) {
                fechaValidaFlag = 1;
            } else {
                printf("[Error] La fecha ingresada no existe en el calendario.\n");
            }
        } else {
            printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
        }
    } while (!fechaValidaFlag);
    a.importePagado += montoAbonado;
    float nuevoSaldo = a.importeTotal - a.importePagado;
    if (nuevoSaldo <= 0) {
        a.estado = 2;
    }
    fseek(fAlq, posAlquiler, SEEK_SET);
    fwrite(&a, sizeof(alquiler), 1, fAlq);
    fflush(fAlq);
    fclose(fAlq);
    registrarMovimiento(a.nroAlquiler, a.dniCliente, a.patenteVehiculo, tipoStr, montoAbonado, fPago);
    printf("\n[Exito] %s registrado correctamente.\n", tipoStr);
    if (nuevoSaldo < 0) {
        nuevoSaldo = 0.0;
    }
    printf("Monto Registrado: $%.2f | Total Pagado: $%.2f | Saldo Restante: $%.2f\n",
           montoAbonado, a.importePagado, nuevoSaldo);
}
