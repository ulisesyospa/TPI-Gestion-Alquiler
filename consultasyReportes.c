#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "consultasReportes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "clientes.h"
#include "movimiento.h"
// Menu interactivo para las 10 consultas e informes del sistema
void menuConsultasyReportes(void){
    int opcion;
    char buffer[100];
    char basura;
    do{
        printf("\n========================================\n");
        printf("         CONSULTAS Y REPORTES           \n");
        printf("========================================\n");
        printf("1. Vehiculos disponibles\n");
        printf("2. Vehiculos alquilados\n");
        printf("3. Alquileres por rango de fechas\n");
        printf("4. Alquileres por cliente\n");
        printf("5. Alquileres por vehiculo\n");
        printf("6. Alquileres por estado\n");
        printf("7. Alquileres por importe\n");
        printf("8. Pagos realizados\n");
        printf("9. Clientes con deuda\n");
        printf("10. Estadisticas de la empresa\n");
        printf("0. Volver al menu principal\n");
        printf("----------------------------------------\n");
        printf("Seleccione una opcion: ");
        if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            opcion = -1;
            continue;
        }
        if(buffer[0] == '\n' || buffer[0] == '\r'){
            printf("[Aviso] No ingreso ninguna opcion.\n");
            opcion = -1;
            continue;
        }
        if(sscanf(buffer, " %d %c", &opcion, &basura) != 1){
            printf("[Error] Entrada invalida. Ingrese solo un numero.\n");
            opcion = -1;
            continue;
        }
        switch(opcion){
            case 1:
                vehiculosDisponibles();
                break;
            case 2:
                vehiculosAlquilados();
                break;
            case 3:
                alquileresPorRangoFechas();
                break;
            case 4:
                alquileresPorCliente();
                break;
            case 5:
                alquileresPorVehiculo();
                break;
            case 6:
                alquileresPorEstado();
                break;
            case 7:
                alquileresPorImporte();
                break;
            case 8:
                pagosRealizados(); 
                break;
            case 9:
                clientesConDeuda();                
                break;
            case 10:
                estadisticasEmpresa();
                break;
            case 0:
                printf("\nRegresando al menu principal...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    }while(opcion != 0);
}
// Filtra e imprime unicamente los vehiculos con estado Disponible (1)
void vehiculosDisponibles(void){	 
    vehiculo v;
    int contador = 0;
    FILE *archivo = fopen("vehiculos.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }	
    printf("\n========================================\n");
    printf("         VEHICULOS DISPONIBLES          \n");
    printf("========================================\n");
    while(fread(&v, sizeof(vehiculo), 1, archivo) == 1){
        if(v.estado == 1){
            printf("Patente: %s\n", v.patente);
            printf("Marca: %s\n", v.marca);
            printf("Modelo: %s\n", v.modelo);
            printf("Anio: %d\n", v.anio);
            printf("Tipo: %s\n", v.tipo);
            printf("Capacidad: %d pasajeros\n", v.capacidad);
            printf("Precio por dia: $%.2f\n", v.precioPorDia);
            printf("----------------------------------------\n");			
            contador = contador + 1;
        }
    }	
    fclose(archivo);	
    if(contador == 0){
        printf("[Aviso] No hay vehiculos disponibles en este momento.\n");
    } else {
        printf("Total de vehiculos disponibles: %d\n", contador);
    }
}
// Cruza archivos para mostrar vehiculos alquilados junto a los datos del cliente y fechas
void vehiculosAlquilados(void){	
    FILE *archivoV, *archivoA, *archivoC;
    vehiculo v;
    alquiler a;
    cliente c;
    int encontrado = 0;
    int contador = 0;	
    archivoV = fopen("vehiculos.dat", "rb");
    if(archivoV == NULL){
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }	
    archivoA = fopen("alquileres.dat", "rb");
    if (archivoA == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        fclose(archivoV);
        return;
    }
    archivoC = fopen("clientes.dat", "rb");
    if (archivoC == NULL){
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        fclose(archivoV);
        fclose(archivoA);
        return;
    }
    printf("\n========================================\n");
    printf("          VEHICULOS ALQUILADOS          \n");
    printf("========================================\n");
    while (fread(&v, sizeof(vehiculo), 1, archivoV) == 1){
        if (v.estado == 2){
            rewind(archivoA);
            encontrado = 0;
            while (fread(&a, sizeof(alquiler), 1, archivoA) == 1 && encontrado == 0){
                if (strcmp(v.patente, a.patenteVehiculo) == 0 && a.estado == 1){
                    rewind(archivoC);
                    while (fread(&c, sizeof(cliente), 1, archivoC) == 1){
                        if (c.dni == a.dniCliente){
                            printf("Patente: %s\n", v.patente);
                            printf("Marca y Modelo: %s %s\n", v.marca, v.modelo);
                            printf("Cliente: %s (DNI: %d)\n", c.apellidoNombre, c.dni);
                            printf("Fecha Inicio: %02d/%02d/%04d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
                            printf("Devolucion Prevista: %02d/%02d/%04d\n", a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
                            printf("----------------------------------------\n");
                            contador++;
                            encontrado = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (contador == 0){
        printf("[Aviso] No hay vehiculos alquilados actualmente.\n");
    } else {
        printf("Total de vehiculos alquilados: %d\n", contador);
    }
    fclose(archivoV);
    fclose(archivoA);
    fclose(archivoC);
}
// Valida la existencia real de una fecha ingresada para el modulo de consultas
int fechaValidaCR(int dia, int mes, int anio){
    struct tm t;
    time_t resultado;
    if (anio < 1900 || mes < 1 || mes > 12 || dia < 1 || dia > 31){
        return 0;
    }
    memset(&t, 0, sizeof(t));
    t.tm_mday = dia;
    t.tm_mon = mes - 1;
    t.tm_year = anio - 1900;
    resultado = mktime(&t);
    if (resultado == -1){
        return 0;
    }
    if (t.tm_mday != dia || t.tm_mon != mes - 1 || t.tm_year != anio - 1900){
        return 0;
    }
    return 1;
}
// Lee y valida una fecha ingresada por consola en formato DD/MM/AAAA
int leerFechaCR(fecha *f){
    char texto[100];
    int i;
    printf("Ingrese la fecha (DD/MM/AAAA): ");
    if(fgets(texto, sizeof(texto), stdin) == NULL){
        return 0;
    }
    if(strchr(texto, '\n') == NULL){
        limpiarEntradaCR();
        return 0;
    }
    texto[strcspn(texto, "\n")] = '\0';
    if(strlen(texto) != 10){
        return 0;
    }
    if(texto[2] != '/' || texto[5] != '/'){
        return 0;
    }
    for(i = 0; i < 10; i++){
        if(i != 2 && i != 5){
            if(texto[i] < '0' || texto[i] > '9'){
                return 0;
            }
        }
    }
    if(sscanf(texto, "%2d/%2d/%4d", &f->dia, &f->mes, &f->anio) != 3){
        return 0;
    }
    return fechaValidaCR(f->dia, f->mes, f->anio);
}
// Vacía el buffer de entrada en caso de lecturas incompletas
void limpiarEntradaCR(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF){
    }
}
// Muestra los contratos de alquiler iniciados dentro de un rango de fechas
void alquileresPorRangoFechas(void){
    FILE *archivo;
    alquiler a;
    fecha inicial, final;
    int encontrado = 0;
    printf("\n========================================\n");
    printf("     ALQUILERES POR RANGO DE FECHAS     \n");
    printf("========================================\n");
    while (1){
        printf("\n--- Fecha Inicial ---\n");
        if (leerFechaCR(&inicial) == 0){
            printf("[Error] Fecha incorrecta. Intente nuevamente.\n");
        } else {
            break;
        }
    }
    while (1){
        printf("\n--- Fecha Final ---\n");
        if (leerFechaCR(&final) == 0){
            printf("[Error] Fecha incorrecta. Intente nuevamente.\n");
        } else if (final.anio < inicial.anio || (final.anio == inicial.anio && final.mes < inicial.mes) || (final.anio == inicial.anio && final.mes == inicial.mes && final.dia < inicial.dia)){
            printf("[Error] La fecha final no puede ser anterior a la fecha inicial.\n");
        } else {
            break;
        }
    }
    archivo = fopen("alquileres.dat", "rb");
    if (archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    printf("\n--- ALQUILERES ENTRE %02d/%02d/%04d Y %02d/%02d/%04d ---\n", inicial.dia, inicial.mes, inicial.anio, final.dia, final.mes, final.anio);
    while (fread(&a, sizeof(alquiler), 1, archivo) == 1){
        int desdeInicial = a.fecInicio.anio > inicial.anio || (a.fecInicio.anio == inicial.anio && a.fecInicio.mes > inicial.mes) || (a.fecInicio.anio == inicial.anio && a.fecInicio.mes == inicial.mes && a.fecInicio.dia >= inicial.dia);
        int hastaFinal = a.fecInicio.anio < final.anio || (a.fecInicio.anio == final.anio && a.fecInicio.mes < final.mes) || (a.fecInicio.anio == final.anio && a.fecInicio.mes == final.mes && a.fecInicio.dia <= final.dia);
        if (desdeInicial && hastaFinal){
            printf("Numero de Alquiler: %d\n", a.nroAlquiler);
            printf("DNI Cliente: %d\n", a.dniCliente);
            printf("Patente Vehiculo: %s\n", a.patenteVehiculo);
            printf("Fecha Inicio: %02d/%02d/%04d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
            printf("Fecha Prevista Devolucion: %02d/%02d/%04d\n", a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
            printf("Cantidad de Dias: %d\n", a.cantidadDias);
            printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
            printf("----------------------------------------\n");
            encontrado = 1;
        }
    }
    if (encontrado == 0){
        printf("[Aviso] No se encontraron alquileres registrados en ese rango de fechas.\n");
    }
    fclose(archivo);
}
// Solicita un DNI y muestra todo el historial de contratos asociados al cliente
void alquileresPorCliente(void) {
    FILE *archivoC, *archivoA;
    cliente c;
    alquiler a;
    char texto[100];
    int dni, i;
    int valido, existe, encontrado;
    printf("\n========================================\n");
    printf("        ALQUILERES POR CLIENTE          \n");
    printf("========================================\n");
    archivoC = fopen("clientes.dat", "rb");
    if (archivoC == NULL) {
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        return;
    }
    printf("\n--- DNI DE CLIENTES REGISTRADOS ---\n");
    existe = 0;
    while (fread(&c, sizeof(cliente), 1, archivoC) == 1) {
        printf("DNI: %d | Nombre: %s\n", c.dni, c.apellidoNombre);
        existe = 1;
    }
    fclose(archivoC);
    if (existe == 0) {
        printf("[Aviso] No hay clientes registrados en el sistema.\n");
        return;
    }
    while (1) {
        printf("\nIngrese DNI del cliente: ");
        if (fgets(texto, sizeof(texto), stdin) == NULL) {
            return;
        }
        texto[strcspn(texto, "\n")] = '\0';
        valido = 1;
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] < '0' || texto[i] > '9') {
                valido = 0;
            }
        }
        if (i != 8) {
            valido = 0;
        }
        if (valido == 0) {
            printf("[Error] DNI incorrecto. Ingrese exactamente 8 digitos numericos.\n");
        } else {
            dni = atoi(texto);
            break;
        }
    }
    archivoC = fopen("clientes.dat", "rb");
    if (archivoC == NULL) {
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        return;
    }
    existe = 0;
    while (fread(&c, sizeof(cliente), 1, archivoC) == 1 && existe == 0) {
        if (c.dni == dni) {
            existe = 1;
        }
    }
    fclose(archivoC);
    if (existe == 0) {
        printf("[Error] No existe un cliente registrado con ese DNI.\n");
        return;
    }
    archivoA = fopen("alquileres.dat", "rb");
    if (archivoA == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    encontrado = 0;
    printf("\n--- ALQUILERES DEL CLIENTE DNI %d ---\n", dni);
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1) {
        if (a.dniCliente == dni) {
            printf("Numero de Alquiler: %d\n", a.nroAlquiler);
            printf("Patente Vehiculo: %s\n", a.patenteVehiculo);
            printf("Fecha Inicio: %02d/%02d/%04d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
            printf("Fecha Prevista Devolucion: %02d/%02d/%04d\n", a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
            printf("Cantidad de Dias: %d\n", a.cantidadDias);
            printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
            printf("----------------------------------------\n");
            encontrado = 1;
        }
    }
    fclose(archivoA);
    if (encontrado == 0) {
        printf("[Aviso] El cliente existe pero no tiene alquileres registrados.\n");
    }
}
// Pide una patente y muestra el historial completo de contratos de ese vehiculo
void alquileresPorVehiculo(void) {
    FILE *archivoV, *archivoA;
    vehiculo v;
    alquiler a;
    char texto[100];
    char patenteAlquiler[10];
    char patenteArchivo[10];
    int i, valido, existe, encontrado;
    printf("\n========================================\n");
    printf("        ALQUILERES POR VEHICULO         \n");
    printf("========================================\n");
    archivoV = fopen("vehiculos.dat", "rb");
    if (archivoV == NULL) {
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    printf("\n--- VEHICULOS EN LA FLOTA ---\n");
    existe = 0;
    while (fread(&v, sizeof(vehiculo), 1, archivoV) == 1) {
        printf("Patente: %s | Marca/Modelo: %s %s\n", v.patente, v.marca, v.modelo);
        existe = 1;
    }
    fclose(archivoV);
    if (existe == 0) {
        printf("[Aviso] No hay vehiculos registrados en el sistema.\n");
        return;
    }
    do {
        valido = 1;
        printf("\nIngrese patente del vehiculo (Ej: AB123CD): ");
        if (fgets(texto, sizeof(texto), stdin) == NULL) {
            return;
        }
        texto[strcspn(texto, "\n")] = '\0';
        for (i = 0; texto[i] != '\0'; i++) {
            if (texto[i] >= 'a' && texto[i] <= 'z') {
                texto[i] = texto[i] - 32;
            }
        }
        if (strlen(texto) != 7) {
            valido = 0;
        } else {
            for (i = 0; i < 7; i++) {
                if (i == 0 || i == 1 || i == 5 || i == 6) {
                    if (texto[i] < 'A' || texto[i] > 'Z') {
                        valido = 0;
                    }
                } else {
                    if (texto[i] < '0' || texto[i] > '9') {
                        valido = 0;
                    }
                }
            }
        }
        if (valido == 0) {
            printf("[Error] Patente invalida. Debe tener formato AB123CD.\n");
        }
    } while (valido == 0);
    strcpy(patenteAlquiler, texto);
    archivoV = fopen("vehiculos.dat", "rb");
    if (archivoV == NULL) {
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    existe = 0;
    while (fread(&v, sizeof(vehiculo), 1, archivoV) == 1 && existe == 0) {
        strcpy(patenteArchivo, v.patente);
        for (i = 0; patenteArchivo[i] != '\0'; i++) {
            if (patenteArchivo[i] >= 'a' && patenteArchivo[i] <= 'z') {
                patenteArchivo[i] = patenteArchivo[i] - 32;
            }
        }
        if (strcmp(patenteArchivo, patenteAlquiler) == 0) {
            existe = 1;
        }
    }
    fclose(archivoV);
    if (existe == 0) {
        printf("[Error] No existe un vehiculo registrado con esa patente.\n");
        return;
    }
    archivoA = fopen("alquileres.dat", "rb");
    if (archivoA == NULL) {
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    printf("\n--- ALQUILERES DEL VEHICULO %s ---\n", patenteAlquiler);
    encontrado = 0;
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1) {
        strcpy(patenteArchivo, a.patenteVehiculo);
        for (i = 0; patenteArchivo[i] != '\0'; i++) {
            if (patenteArchivo[i] >= 'a' && patenteArchivo[i] <= 'z') {
                patenteArchivo[i] = patenteArchivo[i] - 32;
            }
        }
        if (strcmp(patenteArchivo, patenteAlquiler) == 0) {
            printf("Numero de Alquiler: %d\n", a.nroAlquiler);
            printf("DNI Cliente: %d\n", a.dniCliente);
            printf("Fecha Inicio: %02d/%02d/%04d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
            printf("Fecha Prevista Devolucion: %02d/%02d/%04d\n", a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
            printf("Cantidad de Dias: %d\n", a.cantidadDias);
            printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
            printf("----------------------------------------\n");
            encontrado = 1;
        }
    }
    fclose(archivoA);
    if (encontrado == 0) {
        printf("[Aviso] Este vehiculo no registra alquileres guardados.\n");
    }
}
// Muestra contratos filtrados segun su estado (Activo=1, Finalizado=2, Cancelado=0)
void alquileresPorEstado(void){
    FILE *archivo;
    alquiler a;
    char texto[100];
    char nombreEstado[20];
    int estado;
    int contador;
    do{
        contador = 0;
        printf("\n========================================\n");
        printf("         ALQUILERES POR ESTADO          \n");
        printf("========================================\n");
        printf("1. Activos\n");
        printf("2. Finalizados\n");
        printf("0. Cancelados\n");
        printf("9. Volver al menu de consultas y reportes\n");
        printf("----------------------------------------\n");
        while(1){
            printf("Seleccione una opcion: ");
            if(fgets(texto, sizeof(texto), stdin) == NULL){
                return;
            }
            if(strchr(texto, '\n') == NULL){
                limpiarEntradaCR();
                printf("[Error] Opcion invalida. Ingrese 1, 2, 0 o 9.\n");
                continue;
            }
            texto[strcspn(texto, "\n")] = '\0';
            if(strlen(texto) == 1 && (texto[0] == '0' || texto[0] == '1' || texto[0] == '2' || texto[0] == '9')){
                estado = texto[0] - '0';
                break;
            }
            printf("[Error] Opcion invalida. Ingrese 1, 2, 0 o 9.\n");
        }
        if(estado != 9){
            if(estado == 1){
                strcpy(nombreEstado, "Activo");
            } else if(estado == 2){
                strcpy(nombreEstado, "Finalizado");
            } else {
                strcpy(nombreEstado, "Cancelado");
            }
            archivo = fopen("alquileres.dat", "rb");
            if(archivo == NULL){
                printf("[Error] No se pudo abrir el archivo de alquileres.\n");
                return;
            }
            printf("\n--- ALQUILERES CON ESTADO: %s ---\n", nombreEstado);
            while(fread(&a, sizeof(alquiler), 1, archivo) == 1){
                if(a.estado == estado){
                    printf("Numero de Alquiler: %d\n", a.nroAlquiler);
                    printf("DNI Cliente: %d\n", a.dniCliente);
                    printf("Patente Vehiculo: %s\n", a.patenteVehiculo);
                    printf("Fecha Inicio: %02d/%02d/%04d\n", a.fecInicio.dia, a.fecInicio.mes, a.fecInicio.anio);
                    printf("Fecha Prevista Devolucion: %02d/%02d/%04d\n", a.fecPrevitaDevolucion.dia, a.fecPrevitaDevolucion.mes, a.fecPrevitaDevolucion.anio);
                    if(a.estado == 2){
                        printf("Fecha Efectiva Devolucion: %02d/%02d/%04d\n", a.fecEfectivaDevolucion.dia, a.fecEfectivaDevolucion.mes, a.fecEfectivaDevolucion.anio);
                    }
                    printf("Cantidad de Dias: %d\n", a.cantidadDias);
                    printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
                    printf("----------------------------------------\n");
                    contador++;
                }
            }
            if(contador == 0){
                printf("[Aviso] No hay alquileres con estado %s.\n", nombreEstado);
            } else {
                printf("Total de alquileres con estado %s: %d\n", nombreEstado, contador);
            }
            fclose(archivo);
        }
    }while (estado != 9);
}
// Filtra contratos cuyos importes sean superiores o inferiores a un monto ingresado
void alquileresPorImporte(void){
    FILE *archivo;
    alquiler a;
    char texto[100];
    float importe;
    int opcion;
    int contador = 0;
    int valido;
    int i;
    printf("\n========================================\n");
    printf("        ALQUILERES POR IMPORTE          \n");
    printf("========================================\n");
    while(1){
        printf("Ingrese el importe limite: ");
        if(fgets(texto, sizeof(texto), stdin) == NULL){
            return;
        }
        if(strchr(texto, '\n') == NULL){
            limpiarEntradaCR();
            printf("[Error] Importe invalido.\n");
            continue;
        }
        texto[strcspn(texto, "\n")] = '\0';
        valido = 1;
        for(i = 0; texto[i] != '\0'; i++){
            if((texto[i] < '0' || texto[i] > '9') && texto[i] != '.'){
                valido = 0;
            }
        }
        if(i == 0 || valido == 0){
            printf("[Error] Ingrese un importe numerico valido.\n");
            continue;
        }
        importe = (float)atof(texto);
        if(importe < 0){
            printf("[Error] El importe no puede ser negativo.\n");
            continue;
        }
        break;
    }
    while(1){
        printf("\n1. Alquileres con importe SUPERIOR a $%.2f\n", importe);
        printf("2. Alquileres con importe INFERIOR a $%.2f\n", importe);
        printf("Seleccione una opcion: ");
        if(fgets(texto, sizeof(texto), stdin) == NULL){
            return;
        }
        if(strchr(texto, '\n') == NULL){
            limpiarEntradaCR();
            printf("[Error] Opcion invalida. Ingrese 1 o 2.\n");
            continue;
        }
        texto[strcspn(texto, "\n")] = '\0';
        if(strlen(texto) == 1 && (texto[0] == '1' || texto[0] == '2')){
            opcion = texto[0] - '0';
            break;
        }
        printf("[Error] Opcion invalida. Ingrese 1 o 2.\n");
    }
    archivo = fopen("alquileres.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    printf("\n--- RESULTADOS DE LA BUSQUEDA ---\n");
    while(fread(&a, sizeof(alquiler), 1, archivo) == 1){
        if((opcion == 1 && a.importeTotal > importe) || (opcion == 2 && a.importeTotal < importe)){
            printf("Nro Alquiler: %d | DNI: %d | Patente: %s\n", a.nroAlquiler, a.dniCliente, a.patenteVehiculo);
            printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
            printf("----------------------------------------\n");
            contador++;
        }
    }
    if(contador == 0){
        printf("[Aviso] No se encontraron alquileres que cumplan el criterio.\n");
    } else {
        printf("Total de alquileres encontrados: %d\n", contador);
    }
    fclose(archivo);
}
// Muestra los pagos asentados en movimientos.dat dentro de un rango de fechas
void pagosRealizados(void){
    FILE *archivo;
    movimiento m;
    fecha inicial, final;
    int numInicial, numFinal, numPago;
    int contador = 0;
    printf("\n========================================\n");
    printf("            PAGOS REALIZADOS            \n");
    printf("========================================\n");
    while(1){
        printf("\n--- Fecha Inicial ---\n");
        if(leerFechaCR(&inicial) == 0){
            printf("[Error] Fecha incorrecta. Intente nuevamente.\n");
        } else {
            break;
        }
    }
    numInicial = inicial.anio * 10000 + inicial.mes * 100 + inicial.dia;
    while(1){
        printf("\n--- Fecha Final ---\n");
        if(leerFechaCR(&final) == 0){
            printf("[Error] Fecha incorrecta. Intente nuevamente.\n");
        } else {
            numFinal = final.anio * 10000 + final.mes * 100 + final.dia;
            if(numFinal < numInicial){
                printf("[Error] La fecha final no puede ser anterior a la inicial.\n");
            } else {
                break;
            }
        }
    }
    archivo = fopen("movimientos.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de movimientos o aun no hay pagos registrados.\n");
        return;
    }
    printf("\n--- PAGOS REGISTRADOS ENTRE %02d/%02d/%04d Y %02d/%02d/%04d ---\n", inicial.dia, inicial.mes, inicial.anio, final.dia, final.mes, final.anio);
    while(fread(&m, sizeof(movimiento), 1, archivo) == 1){
        numPago = m.fec.anio * 10000 + m.fec.mes * 100 + m.fec.dia;
        if(strcmp(m.tipoMovimiento, "Pago") == 0 && numPago >= numInicial && numPago <= numFinal){
            printf("Nro Movimiento: %d | Nro Alquiler: %d\n", m.nroMovimiento, m.nroAlquiler);
            printf("DNI Cliente: %d | Patente: %s\n", m.dniCliente, m.patenteVehiculo);
            printf("Fecha Pago: %02d/%02d/%04d | Monto: $%.2f\n", m.fec.dia, m.fec.mes, m.fec.anio, m.monto);
            printf("----------------------------------------\n");
            contador++;
        }
    }
    if(contador == 0){
        printf("[Aviso] No se encontraron pagos en ese periodo.\n");
    } else {
        printf("Total de pagos encontrados: %d\n", contador);
    }
    fclose(archivo);
}
// Lista todos los contratos que registren saldo pendiente de cobro junto al nombre del cliente
void clientesConDeuda(void) {
    FILE *archivoA, *archivoC;
    alquiler a;
    cliente c;
    char nombre[60];
    float saldo;
    int contador = 0;
    archivoA = fopen("alquileres.dat", "rb");
    if (archivoA == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    archivoC = fopen("clientes.dat", "rb");
    if (archivoC == NULL){
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        fclose(archivoA);
        return;
    }
    printf("\n========================================\n");
    printf("           CLIENTES CON DEUDA           \n");
    printf("========================================\n");
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1){
        saldo = a.importeTotal - a.importePagado;
        if (a.estado != 0 && saldo > 0){
            strcpy(nombre, "Cliente no encontrado");
            rewind(archivoC);
            while (fread(&c, sizeof(cliente), 1, archivoC) == 1){
                if (c.dni == a.dniCliente){
                    strcpy(nombre, c.apellidoNombre);
                    break;
                }
            }
            printf("DNI: %d | Cliente: %s\n", a.dniCliente, nombre);
            printf("Nro Alquiler: %d\n", a.nroAlquiler);
            printf("Importe Total: $%.2f | Importe Pagado: $%.2f\n", a.importeTotal, a.importePagado);
            printf("Saldo Pendiente de Cobro: $%.2f\n", saldo);
            printf("----------------------------------------\n");
            contador++;
        }
    }
    if (contador == 0){
        printf("[Aviso] No hay clientes registrados con deuda pendiente.\n");
    } else {
        printf("Cantidad de alquileres con deuda: %d\n", contador);
    }
    fclose(archivoA);
    fclose(archivoC);
}
// Consolida las metricas ejecutivas de vehiculos, alquileres, montos cobrados y pendientes
void estadisticasEmpresa(void){
    FILE *archivoV;
    FILE *archivoA;
    vehiculo v;
    alquiler a;
    int totalVehiculos = 0;
    int disponibles = 0;
    int alquilados = 0;
    int totalAlquileres = 0;
    int alquileresActivos = 0;
    float totalCobrado = 0.0;
    float totalPendiente = 0.0;
    archivoV = fopen("vehiculos.dat", "rb");
    if(archivoV == NULL){
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    // Conteo general de vehiculos y disponibilidad en flota
    while(fread(&v, sizeof(vehiculo), 1, archivoV) == 1){
        totalVehiculos++;
        if(v.estado == 1){
            disponibles++;
        } else if(v.estado == 2){
            alquilados++;
        }
    }
    fclose(archivoV);
    archivoA = fopen("alquileres.dat", "rb");
    if(archivoA == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }
    // Conteo de alquileres, total cobrado y saldos pendientes de contratos validos
    while(fread(&a, sizeof(alquiler), 1, archivoA) == 1){
        totalAlquileres++;
        if(a.estado == 1){
            alquileresActivos++;
        }
        totalCobrado += a.importePagado;
        if(a.estado != 0){
            totalPendiente += (a.importeTotal - a.importePagado);
        }
    }
    fclose(archivoA);
    // Impresion de resultados consolidados
    printf("\n========================================\n");
    printf("       ESTADISTICAS DE LA EMPRESA       \n");
    printf("========================================\n");
    printf("Cantidad total de vehiculos: %d\n", totalVehiculos);
    printf("Vehiculos disponibles: %d\n", disponibles);
    printf("Vehiculos alquilados: %d\n", alquilados);
    printf("----------------------------------------\n");
    printf("Cantidad total de alquileres: %d\n", totalAlquileres);
    printf("Cantidad de alquileres activos: %d\n", alquileresActivos);
    printf("----------------------------------------\n");
    printf("Importe total cobrado: $%.2f\n", totalCobrado);
    printf("Importe total pendiente de cobro: $%.2f\n", totalPendiente);
}
