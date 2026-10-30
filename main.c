#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientes.h"
#include "vehiculos.h"
#include "alquileres.h"
#include "devoluciones.h"
#include "movimiento.h"
#include "consultasReportes.h"
int main(){
    int opcion;
    char buffer[100];
    char basura;
    do{
        printf("\========================================\n");
        printf("    SISTEMA DE ALQUILER DE VEHICULOS    \n");
        printf("========================================\n");
        printf("1. Clientes\n");
        printf("2. Vehiculos\n");
        printf("3. Alquileres\n");
        printf("4. Devoluciones\n");
        printf("5. Pagos\n");
        printf("6. Consultas y Reportes\n");
        printf("0. Salir\n");
        printf("----------------------------------------\n");
        printf("Seleccione una opcion: ");    
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                printf("[Aviso] Debe ingresar una opcion valida.\n");
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
                menuClientes();
                break;
            case 2:
                menuVehiculos();
                break;
            case 3:
                menuAlquileres();
                break;
            case 4:
                menuDevoluciones();
                break;
            case 5:
                menuPagos();
                break;
            case 6:
                menuConsultasyReportes();
                break;
            case 0:
                printf("\nSaliendo del sistema...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");    
        }
    }while(opcion != 0);
    return 0;
}
