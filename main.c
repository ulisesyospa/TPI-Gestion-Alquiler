#include <stdio.h>
#include <stdlib.h>
#include "clientes.h"
#include "vehiculos.h"
#include "consultasReportes.h"
int main(){
    int opcion;
    do{
        printf("\n=====================================\n");
        printf("    SISTEMA DE ALQUILER DE VEHICULOS\n");
        printf("=====================================\n");
        printf("1.Clientes\n");
        printf("2.Vehiculos\n");
        printf("3.Alquileres\n");
        printf("4.Devoluciones\n");
        printf("5.Pagos\n");
        printf("6.Consultas y Reportes\n");
        printf("0.Salir\n");
        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);
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
                //menuDevoluciones();
                printf("Modulo en desarrollo.\n");
            break;
            case 5:
                menuPagos();
                printf("Modulo en desarrollo.\n");
            break;
            case 6:
                menuConsultasyReportes();
            break;
            case 0:
                printf("Saliendo...\n");
            break;
            default:
                printf("Opcion incorrecta.\n");
        }
    }while(opcion != 0);
    return 0;
    int op;
    do {
        printf("\n--- CONSULTAS Y REPORTES ---\n");
        printf("1. Vehículos Disponibles\n");
        printf("2. Vehículos Alquilados\n");
        printf("3. Alquileres por Cliente\n");
        printf("4. Clientes con Deuda\n");
        printf("5. Estadísticas de la Empresa\n");
        printf("0. Volver al Menú Principal\n");
        printf("Seleccione una opción: ");
        scanf("%d", &op);
        switch(op) {
            case 1: mostrarVehiculosDisponibles(); break;
            case 2: mostrarVehiculosAlquilados(); break;
            case 3: mostrarAlquileresPorCliente(); break;
            case 4: mostrarClientesConDeuda(); break;
            case 5: mostrarEstadisticasEmpresa(); break;
            case 0: break;
            default: printf("[Error] Opción inválida.\n");
        }
    } while(op != 0);
}
