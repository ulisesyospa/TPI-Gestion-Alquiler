#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "consultasReportes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "clientes.h"
#include "movimiento.h"


void menuConsultasyReportes(void){
    int opcion;

    do{
        printf("\n=== CONSULTAS Y REPORTES ===\n");
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
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

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
                printf("Volviendo al menu principal...\n");
                break;

            default:
                printf("Opcion invalida.\n");
        }

    }while(opcion != 0);
}



void vehiculosDisponibles(void){
	 
	vehiculo v;
	int contador=0;
	FILE *archivo;
	
	archivo= fopen("vehiculos.dat", "rb");
	if(archivo==NULL){
		printf("[ERROR] No se pudo abrir el archivo.\n");
		return;
	}
	
	
	printf("\n--- VEHICULOS DISPONIBLES ---\n");
	
	while(fread(&v, sizeof(vehiculo), 1, archivo)==1){
		if(v.estado==1){
			printf("Patente: %s\n", v.patente);
			printf("Marca: %s\n", v.marca);
			printf("Modelo: %s\n", v.modelo);
			printf("Anio: %d\n", v.anio);
			printf("Tipo: %s\n", v.tipo);
			printf("Capacidad: %d\n", v.capacidad);
			printf("Precio por dia: $%.2f\n", v.precioPorDia);
			printf("--------------------------");
			
			contador=contador+1;
		}
	}
	
	fclose(archivo);
	
	if(contador==0){
		printf("No hay vehiculos disponibles.\n");
	}
}



void vehiculosAlquilados(){
	
	FILE *archivoV, *archivoA, *archivoC;
	vehiculo v;
	alquiler a;
	cliente c;
	int encontrado=0, contador=0;
	
	archivoV= fopen("vehiculos.dat", "rb");
	if(archivoV==NULL){
		printf("[ERROR] No se pudo abrir el archivo de vehiculos.\n");
		return;
	}
	
	
	 archivoA = fopen("alquileres.dat", "rb");

    if (archivoA == NULL)
    {
        printf("No se pudo abrir el archivo de alquileres.\n");
        fclose(archivoV);
        return;
    }

    archivoC = fopen("clientes.dat", "rb");

    if (archivoC == NULL)
    {
        printf("No se pudo abrir el archivo de clientes.\n");
        fclose(archivoV);
        fclose(archivoA);
        return;
    }

    printf("\n--- VEHICULOS ALQUILADOS ---\n");

    while (fread(&v, sizeof(vehiculo), 1, archivoV) == 1)
    {
        if (v.estado == 2)
        {
            rewind(archivoA);
            encontrado = 0;

            while (fread(&a, sizeof(alquiler), 1, archivoA) == 1 && encontrado == 0)
            {
                if (strcmp(v.patente, a.patenteVehiculo) == 0 && a.estado == 1)
                {
                    rewind(archivoC);

                    while (fread(&c, sizeof(cliente), 1, archivoC) == 1)
                    {
                        if (c.dni == a.dniCliente)
                        {
                            printf("\nPatente: %s", v.patente);
                            printf("\nMarca: %s", v.marca);
                            printf("\nModelo: %s", v.modelo);
                            printf("\nCliente: %s", c.apellidoNombre);

                            printf("\nFecha de inicio: %02d/%02d/%04d",
                                   a.fecInicio.dia,
                                   a.fecInicio.mes,
                                   a.fecInicio.anio);

                            printf("\nFecha prevista de devolucion: %02d/%02d/%04d\n",
                                   a.fecPrevitaDevolucion.dia,
                                   a.fecPrevitaDevolucion.mes,
                                   a.fecPrevitaDevolucion.anio);

                            contador++;
                            encontrado = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (contador == 0)
    {
        printf("No hay vehiculos alquilados actualmente.\n");
    }

    fclose(archivoV);
    fclose(archivoA);
    fclose(archivoC);
}




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

int leerFechaCR(fecha *f)
{
    char texto[100];
    int i;

    printf("Ingrese la fecha (DD/MM/AAAA): ");

    if (scanf(" %99[^\n]", texto) != 1){
        return 0;
    }

    limpiarEntradaCR();

    if (strlen(texto) != 10 || texto[2] != '/' || texto[5] != '/'){
        return 0;
    }

    for (i = 0; i < 10; i++){
        if (i != 2 && i != 5){
            if (texto[i] < '0' || texto[i] > '9'){
                return 0;
            }
        }
    }

    if (sscanf(texto, "%2d/%2d/%4d", &f->dia, &f->mes, &f->anio) != 3){
        return 0;
    }

    return fechaValidaCR(f->dia, f->mes, f->anio);
}


void limpiarEntradaCR(void){
    int c;

    while ((c = getchar()) != '\n' && c != EOF){
    }
}




void alquileresPorRangoFechas(void){
    FILE *archivo;
    alquiler a;
    fecha inicial, final;
    int encontrado = 0;

    printf("\n--- ALQUILERES POR RANGO DE FECHAS ---\n");
 
    
    /* Ingresar fecha inicial */
    while (1){
        printf("\nFecha inicial:\n");

        if (leerFechaCR(&inicial) == 0){
            printf("Fecha incorrecta. Intente nuevamente.\n");
        }
        else{
            break;
        }
    }

    /* Ingresar fecha final */
    while (1){
        printf("\nFecha final:\n");

        if (leerFechaCR(&final) == 0){
            printf("Fecha incorrecta. Intente nuevamente.\n");
        }
        else if (final.anio < inicial.anio || (final.anio == inicial.anio && final.mes < inicial.mes) || (final.anio == inicial.anio && final.mes == inicial.mes && final.dia < inicial.dia)){
            printf("La fecha final no puede ser anterior a la inicial.\n");
        }
        else{
            break;
        }
    }

    /* Abrir el archivo de alquileres */
    archivo = fopen("alquileres.dat", "rb");

    if (archivo == NULL){
        printf("No se pudo abrir el archivo de alquileres.\n");
        return;
    }

    printf("\nAlquileres entre %02d/%02d/%04d y %02d/%02d/%04d:\n", inicial.dia, inicial.mes, inicial.anio, final.dia, final.mes, final.anio);

    /* Recorrer los alquileres */
    while (fread(&a, sizeof(alquiler), 1, archivo) == 1){
        /* Verificar si la fecha de inicio esta dentro del rango */
        int desdeInicial = a.fecInicio.anio > inicial.anio || (a.fecInicio.anio == inicial.anio && a.fecInicio.mes > inicial.mes) || (a.fecInicio.anio == inicial.anio && a.fecInicio.mes == inicial.mes && a.fecInicio.dia >= inicial.dia);

        int hastaFinal = a.fecInicio.anio < final.anio || (a.fecInicio.anio == final.anio && a.fecInicio.mes < final.mes) || (a.fecInicio.anio == final.anio && a.fecInicio.mes == final.mes && a.fecInicio.dia <= final.dia);

        if (desdeInicial && hastaFinal){
            printf("\nNumero de alquiler: %d\n", a.nroAlquiler);
            printf("DNI del cliente: %d\n", a.dniCliente);
            printf("Patente del vehiculo: %s\n", a.patenteVehiculo);
            printf("Fecha de inicio: %02d/%02d/%04d\n",a.fecInicio.dia, a.fecInicio.mes,a.fecInicio.anio);
            printf("Fecha prevista de devolucion: %02d/%02d/%04d\n",a.fecPrevitaDevolucion.dia,a.fecPrevitaDevolucion.mes,a.fecPrevitaDevolucion.anio);
            printf("Cantidad de dias: %d\n", a.cantidadDias);
            printf("Importe total: $%.2f\n", a.importeTotal);
            printf("Importe pagado: $%.2f\n", a.importePagado);

            encontrado = 1;
        }
    }

    if (encontrado == 0){
        printf("\nNo se encontraron alquileres en ese rango de fechas.\n");
    }

    fclose(archivo);
}





void alquileresPorCliente(void){
    FILE *archivoC, *archivoA;
    cliente c;
    alquiler a;
    char texto[100];
    int dni, i;
    int valido, existe, encontrado;

    printf("\n--- ALQUILERES POR CLIENTE ---\n");

    limpiarEntradaCR();
    /* Ingresar y validar el DNI */
    while (1){
        printf("Ingrese el DNI del cliente: ");

        fgets(texto, sizeof(texto), stdin);
        texto[strcspn(texto, "\n")] = '\0';

        valido = 1;

        /* Verificar que todos los caracteres sean numeros */
        for (i = 0; texto[i] != '\0'; i++){
            if (texto[i] < '0' || texto[i] > '9'){
                valido = 0;
            }
        }

        /* Verificar que tenga entre 7 y 8 digitos */
        if (i < 7 || i > 8){
            valido = 0;
        }

        if (valido == 0){
            printf("DNI incorrecto. Ingrese entre 7 y 8 digitos numericos.\n");
        }
        else{
            dni = atoi(texto);
            break;
        }
    }

    /* Comprobar que el cliente exista */
    archivoC = fopen("clientes.dat", "rb");

    if (archivoC == NULL){
        printf("No se pudo abrir el archivo de clientes.\n");
        return;
    }

    existe = 0;

    while (fread(&c, sizeof(cliente), 1, archivoC) == 1){
        if (c.dni == dni){
            existe = 1;
            break;
        }
    }

    fclose(archivoC);

    if (existe == 0){
        printf("No existe un cliente con ese DNI.\n");
        return;
    }

    /* Abrir el archivo de alquileres */
    archivoA = fopen("alquileres.dat", "rb");

    if (archivoA == NULL){
        printf("No se pudo abrir el archivo de alquileres.\n");
        return;
    }

    encontrado = 0;

    printf("\nAlquileres del cliente con DNI %d:\n", dni);

    /* Buscar todos los alquileres del cliente */
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1){
        if (a.dniCliente == dni){
            printf("\nNumero de alquiler: %d\n", a.nroAlquiler);
            printf("Patente del vehiculo: %s\n", a.patenteVehiculo);

            printf("Fecha de inicio: %02d/%02d/%04d\n",
                   a.fecInicio.dia,
                   a.fecInicio.mes,
                   a.fecInicio.anio);

            printf("Fecha prevista de devolucion: %02d/%02d/%04d\n",
                   a.fecPrevitaDevolucion.dia,
                   a.fecPrevitaDevolucion.mes,
                   a.fecPrevitaDevolucion.anio);

            printf("Cantidad de dias: %d\n", a.cantidadDias);
            printf("Importe total: $%.2f\n", a.importeTotal);
            printf("Importe pagado: $%.2f\n", a.importePagado);

            encontrado = 1;
        }
    }

    if (encontrado == 0){
        printf("\nEl cliente existe, pero no tiene alquileres registrados.\n");
    }

    fclose(archivoA);
}





void alquileresPorVehiculo(void){
    FILE *archivoV, *archivoA;
    vehiculo v;
    alquiler a;
    char texto[100];
    char patenteAlquiler[10];
    int i, valido, existe, encontrado;

    printf("\n--- ALQUILERES POR VEHICULO ---\n");

    limpiarEntradaCR();
    /* Ingresar y validar la patente */
    while (1){
        printf("Ingrese la patente del vehiculo: ");

        fgets(texto, sizeof(texto), stdin);
        texto[strcspn(texto, "\n")] = '\0';

        valido = 1;

        /* Verificar que tenga 7 caracteres */
        if (strlen(texto) != 7){
            valido = 0;
        }

        /* Verificar que solo tenga letras y numeros */
        for (i = 0; texto[i] != '\0'; i++){
            if (!((texto[i] >= 'A' && texto[i] <= 'Z') ||
                  (texto[i] >= 'a' && texto[i] <= 'z') ||
                  (texto[i] >= '0' && texto[i] <= '9'))){
                valido = 0;
            }

            /* Convertir las letras minusculas a mayusculas */
            if (texto[i] >= 'a' && texto[i] <= 'z'){
                texto[i] = texto[i] - 32;
            }
        }

        if (valido == 0){
            printf("Patente incorrecta. Ingrese 7 caracteres alfanumericos.\n");
        }
        else{
            break;
        }
    }

    /* Comprobar que el vehiculo exista */
    archivoV = fopen("vehiculos.dat", "rb");

    if (archivoV == NULL){
        printf("No se pudo abrir el archivo de vehiculos.\n");
        return;
    }

    existe = 0;

    while (fread(&v, sizeof(vehiculo), 1, archivoV) == 1){
        if (strcmp(v.patente, texto) == 0){
            existe = 1;
            break;
        }
    }

    fclose(archivoV);

    if (existe == 0){
        printf("No existe un vehiculo con esa patente.\n");
        return;
    }

    /* Abrir el archivo de alquileres */
    archivoA = fopen("alquileres.dat", "rb");

    if (archivoA == NULL){
        printf("No se pudo abrir el archivo de alquileres.\n");
        return;
    }

    encontrado = 0;

    printf("\nHistorial de alquileres de la patente %s:\n", texto);

    /* Buscar todos los alquileres asociados a la patente */
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1){
        strcpy(patenteAlquiler, a.patenteVehiculo);

        /* Convertir la patente guardada a mayusculas */
        for (i = 0; patenteAlquiler[i] != '\0'; i++){
            if (patenteAlquiler[i] >= 'a' &&
                patenteAlquiler[i] <= 'z'){
                patenteAlquiler[i] = patenteAlquiler[i] - 32;
            }
        }

        if (strcmp(patenteAlquiler, texto) == 0){
            printf("\nNumero de alquiler: %d\n", a.nroAlquiler);
            printf("DNI del cliente: %d\n", a.dniCliente);

            printf("Fecha de inicio: %02d/%02d/%04d\n",
                   a.fecInicio.dia,
                   a.fecInicio.mes,
                   a.fecInicio.anio);

            printf("Fecha prevista de devolucion: %02d/%02d/%04d\n",
                   a.fecPrevitaDevolucion.dia,
                   a.fecPrevitaDevolucion.mes,
                   a.fecPrevitaDevolucion.anio);

            printf("Cantidad de dias: %d\n", a.cantidadDias);
            printf("Importe total: $%.2f\n", a.importeTotal);
            printf("Importe pagado: $%.2f\n", a.importePagado);

            printf("Estado del alquiler: %d\n", a.estado);

            encontrado = 1;
        }
    }

    if (encontrado == 0){
        printf("\nEl vehiculo existe, pero no tiene alquileres registrados.\n");
    }

    fclose(archivoA);
}

void alquileresPorEstado(void){
    FILE *archivo;
    alquiler a;
    char texto[100];
    char nombreEstado[20];
    int estado;
    int contador;
 
    do{
        contador = 0;
 
        printf("\n--- ALQUILERES POR ESTADO ---\n");
        printf("1. Activos\n");
        printf("2. Finalizados\n");
        printf("0. Cancelados\n");
        printf("9. Volver al menu de consultas y reportes\n");
 
        while (1){
            printf("Seleccione una opcion: ");
 
            if (scanf(" %99[^\n]", texto) != 1){
                return;
            }
 
            limpiarEntradaCR();
 
            // control para que solo se admita 1,2,0 o 9
			 
            if (strlen(texto) == 1 && (texto[0] == '0' || texto[0] == '1' || texto[0] == '2' || texto[0] == '9')){
                estado = texto[0] - '0';
                break;
            }
 
            printf("Opcion incorrecta. Ingrese 1 (Activos), 2 (Finalizados), 0 (Cancelados) o 9 (Volver).\n");
        }
 
        //si detecta 9 no entra al while
        if (estado != 9){
 
            if (estado == 1){
                strcpy(nombreEstado, "Activo");
            }
            else if (estado == 2){
                strcpy(nombreEstado, "Finalizado");
            }
            else{
                strcpy(nombreEstado, "Cancelado");
            }
 
            archivo = fopen("alquileres.dat", "rb");
 
            if (archivo == NULL){
                printf("No se pudo abrir el archivo de alquileres.\n");
                return;
            }
 
            printf("\nAlquileres con estado %s:\n", nombreEstado);
 
            // Recorrer los alquileres y muestrar los que tienen ese estado 
            while (fread(&a, sizeof(alquiler), 1, archivo) == 1){
                if (a.estado == estado){
                    printf("\nNumero de alquiler: %d\n", a.nroAlquiler);
                    printf("DNI del cliente: %d\n", a.dniCliente);
                    printf("Patente del vehiculo: %s\n", a.patenteVehiculo);
 
                    printf("Fecha de inicio: %02d/%02d/%04d\n",
                           a.fecInicio.dia,
                           a.fecInicio.mes,
                           a.fecInicio.anio);
 
                    printf("Fecha prevista de devolucion: %02d/%02d/%04d\n",
                           a.fecPrevitaDevolucion.dia,
                           a.fecPrevitaDevolucion.mes,
                           a.fecPrevitaDevolucion.anio);
 
                    // entra solo cuando el vehiculo ya fue devuelto
                    if (a.estado == 2){
                        printf("Fecha efectiva de devolucion: %02d/%02d/%04d\n",
                               a.fecEfectivaDevolucion.dia,
                               a.fecEfectivaDevolucion.mes,
                               a.fecEfectivaDevolucion.anio);
                    }
 
                    printf("Cantidad de dias: %d\n", a.cantidadDias);
                    printf("Importe total: $%.2f\n", a.importeTotal);
                    printf("Importe pagado: $%.2f\n", a.importePagado);
                    printf("Estado del alquiler: %s\n", nombreEstado);
 
                    contador++;
                }
            }
 
            if (contador == 0){
                printf("\nNo hay alquileres con estado %s.\n", nombreEstado);
            }
            else{
                printf("\nTotal de alquileres con estado %s: %d\n", nombreEstado, contador);
            }
 
            fclose(archivo);
        }
 
    }while (estado != 9);

}





void alquileresPorImporte(void){
    FILE *archivo;
    alquiler a;
    float importe;
    int opcion;
    int contador = 0;

    archivo = fopen("alquileres.dat", "rb");

    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de alquileres.\n");
        return;
    }

    printf("\n=== ALQUILERES POR IMPORTE ===\n");
    printf("Ingrese un importe: ");
    scanf("%f", &importe);

    printf("\n1. Alquileres superiores al importe\n");
    printf("2. Alquileres inferiores al importe\n");
    printf("Seleccione una opcion: ");
    scanf("%d", &opcion);

    if(opcion != 1 && opcion != 2){
        printf("[Error] Opcion invalida.\n");
        fclose(archivo);
        return;
    }

    while(fread(&a, sizeof(alquiler), 1, archivo) == 1){

        if((opcion == 1 && a.importeTotal > importe) ||
           (opcion == 2 && a.importeTotal < importe)){

            printf("\nNro Alquiler: %d\n", a.nroAlquiler);
            printf("DNI Cliente: %d\n", a.dniCliente);
            printf("Patente: %s\n", a.patenteVehiculo);
            printf("Importe Total: $%.2f\n", a.importeTotal);

            contador++;
        }
    }

    if(contador == 0){
        printf("\nNo se encontraron alquileres con ese criterio.\n");
    }

    fclose(archivo);
}






void pagosRealizados(void) {
	
	FILE *archivo;
    movimiento m;
    fecha inicial, final;
    int numInicial, numFinal, numPago;
    int contador = 0;
 
    printf("\n--- PAGOS REALIZADOS ---\n");
 
    // se ingresa la primera fecha
    while (1){
        printf("\nFecha inicial:\n");
 
        if (leerFechaCR(&inicial) == 0){
            printf("Fecha incorrecta. Intente nuevamente.\n");
        }
        else{
            break;
        }
    }
 
    // se convierte a un numero AAAAMMDD para poder compararlas 
    numInicial = inicial.anio * 10000 + inicial.mes * 100 + inicial.dia;
 
    // se ingresa la fecha final 
    while (1){
        printf("\nFecha final:\n");
 
        if (leerFechaCR(&final) == 0){
            printf("Fecha incorrecta. Intente nuevamente.\n");
        }
        else{
        	
        	// se convierte a un numero AAAAMMDD para poder compararlas 
            numFinal = final.anio * 10000 + final.mes * 100 + final.dia;
 
            if (numFinal < numInicial){
                printf("La fecha final no puede ser anterior a la inicial.\n");
            }
            else{
                break;
            }
        }
    }
 
    archivo = fopen("movimientos.dat", "rb");
 
    if (archivo == NULL){
        printf("No se pudo abrir el archivo de movimientos o aun no hay pagos registrados.\n");
        return;
    }
 
    printf("\nPagos realizados entre %02d/%02d/%04d y %02d/%02d/%04d:\n",
           inicial.dia, inicial.mes, inicial.anio,
           final.dia, final.mes, final.anio);
 
    // Recorre los movimientos y mmuestra solo los pagos dentro del periodo 
    
    while (fread(&m, sizeof(movimiento), 1, archivo) == 1){
        numPago = m.fec.anio * 10000 + m.fec.mes * 100 + m.fec.dia;
 
        if (strcmp(m.tipoMovimiento, "Pago") == 0 && numPago >= numInicial && numPago <= numFinal){
            printf("\nNumero de movimiento: %d\n", m.nroMovimiento);
            printf("Numero de alquiler: %d\n", m.nroAlquiler);
            printf("DNI del cliente: %d\n", m.dniCliente);
            printf("Patente del vehiculo: %s\n", m.patenteVehiculo);
 
            printf("Fecha del pago: %02d/%02d/%04d\n",
                   m.fec.dia,
                   m.fec.mes,
                   m.fec.anio);
 
            printf("Monto: $%.2f\n", m.monto);
 
            contador++;
        }
    }
 
    if (contador == 0){
        printf("\nNo se encontraron pagos en ese periodo.\n");
    }
    else{
        printf("\nCantidad de pagos encontrados: %d\n", contador);
    }
 
    fclose(archivo);
}

void clientesConDeuda(void) {
	FILE *archivoA, *archivoC;
    alquiler a;
    cliente c;
    char nombre[60];
    float saldo;
    int contador = 0;
 
    archivoA = fopen("alquileres.dat", "rb");
 
    if (archivoA == NULL){
        printf("No se pudo abrir el archivo de alquileres.\n");
        return;
    }
 
    archivoC = fopen("clientes.dat", "rb");
 
    if (archivoC == NULL){
        printf("No se pudo abrir el archivo de clientes.\n");
        fclose(archivoA);
        return;
    }
 
    printf("\n--- CLIENTES CON DEUDA ---\n");
 
    // recorremos los alquileres 
    while (fread(&a, sizeof(alquiler), 1, archivoA) == 1){
        saldo = a.importeTotal - a.importePagado;
 
        // Hay deuda si el importe total supera al pagado.
        //Los alquileres cancelados (estado 0) no se cobran, asi que no cuentan. 
        if (a.estado != 0 && saldo > 0.005){
 
            // Busca nombre del cliente 
            strcpy(nombre, "Cliente no encontrado");
            rewind(archivoC);
 
            while (fread(&c, sizeof(cliente), 1, archivoC) == 1){
                if (c.dni == a.dniCliente){
                    strcpy(nombre, c.apellidoNombre);
                    break;
                }
            }
 
            printf("\nDNI: %d\n", a.dniCliente);
            printf("Nombre: %s\n", nombre);
            printf("Alquiler: %d\n", a.nroAlquiler);
            printf("Importe total: $%.2f\n", a.importeTotal);
            printf("Importe pagado: $%.2f\n", a.importePagado);
            printf("Saldo pendiente: $%.2f\n", saldo);
 
            contador++;
        }
    }
 
    if (contador == 0){
        printf("\nNo hay clientes con deuda.\n");
    }
    else{
        printf("\nCantidad de alquileres con deuda: %d\n", contador);
    }
 
    fclose(archivoA);
    fclose(archivoC);
}





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

    float totalCobrado = 0;
    float totalPendiente = 0;

    archivoV = fopen("vehiculos.dat", "rb");

    if(archivoV == NULL){
        printf("[Error] No se pudo abrir vehiculos.dat\n");
        return;
    }

    while(fread(&v, sizeof(vehiculo), 1, archivoV) == 1){

        totalVehiculos++;

        if(v.estado == 1){
            disponibles++;
        }else if(v.estado == 2){
            alquilados++;
        }
    }

    fclose(archivoV);

    archivoA = fopen("alquileres.dat", "rb");

    if(archivoA == NULL){
        printf("[Error] No se pudo abrir alquileres.dat\n");
        return;
    }

    while(fread(&a, sizeof(alquiler), 1, archivoA) == 1){

        totalAlquileres++;

        if(a.estado == 1){
            alquileresActivos++;
        }

        totalCobrado += a.importePagado;

        totalPendiente += a.importeTotal - a.importePagado;
    }

    fclose(archivoA);

    printf("\n===== ESTADISTICAS DE LA EMPRESA =====\n");

    printf("Cantidad total de vehiculos: %d\n", totalVehiculos);
    printf("Vehiculos disponibles: %d\n", disponibles);
    printf("Vehiculos alquilados: %d\n", alquilados);

    printf("Cantidad total de alquileres: %d\n", totalAlquileres);
    printf("Cantidad de alquileres activos: %d\n", alquileresActivos);

    printf("Importe total cobrado: $%.2f\n", totalCobrado);
    printf("Importe total pendiente de cobro: $%.2f\n", totalPendiente);
}

