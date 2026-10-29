#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "vehiculos.h"

// Valida que la cadena contenga unicamente letras y espacios
int esSoloTextoValido(const char *cadena) {
    int i;
    if (strlen(cadena) == 0) return 0;
    for (i = 0; cadena[i] != '\0'; i++) {
        if (!isalpha((unsigned char)cadena[i]) && cadena[i] != ' ') return 0;
    }
    return 1;
}

// Verifica si la cadena esta vacia o compuesta solo por espacios/tabulaciones
int esCadenaVaciaOEspacios(const char *cadena) {
    int i;
    if (strlen(cadena) == 0) return 1;
    for (i = 0; cadena[i] != '\0'; i++) {
        if (!isspace((unsigned char)cadena[i])) return 0;
    }
    return 1;
}

// Lee enteros de forma segura impidiendo saltos de linea, espacios solos o decimales
int leerEnteroEnRango(int min, int max, const char *mensajeError) {
    char buffer[100];
    int valor;
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = '\0';
            if (esCadenaVaciaOEspacios(buffer)) {
                printf("%s", mensajeError);
                continue;
            }
            char basura;
            if (sscanf(buffer, "%d %c", &valor, &basura) == 1) {
                if (valor >= min && valor <= max) return valor;
            }
        }
        printf("%s", mensajeError);
    }
}

// Controla que el formato sea exactamente 2 letras, 3 numeros y 2 letras (AA000AA)
int esFormatoPatenteValido(char *patente) {
    if (strlen(patente) != 7) return 0;
    if (!isalpha((unsigned char)patente[0]) || !isalpha((unsigned char)patente[1])) return 0;
    if (!isdigit((unsigned char)patente[2]) || !isdigit((unsigned char)patente[3]) || !isdigit((unsigned char)patente[4])) return 0;
    if (!isalpha((unsigned char)patente[5]) || !isalpha((unsigned char)patente[6])) return 0;
    return 1;
}

// Submenu principal de gestion de vehiculos
void menuVehiculos(void) {
    int opcion;
    do {
        printf("\n================================= MENU VEHICULOS =================================\n");
        printf("1. Registrar vehiculo\n2. Buscar vehiculo\n3. Mostrar vehiculos\n4. Modificar vehiculo\n0. Volver al menu principal\n");
        printf("Seleccione una opcion: ");
        opcion = leerEnteroEnRango(0, 4, "[Error] Opcion invalida. Ingrese un numero entre 0 y 4: ");
        switch (opcion) {
            case 1:
                registrarVehiculo();
                break;
            case 2: { // Busqueda interactiva de patente por consola
                char patenteBusq[15];
                int i;
                while (1) {
                    printf("Ingrese la patente a buscar (Formato AA000AA): ");
                    if (fgets(patenteBusq, sizeof(patenteBusq), stdin) != NULL) {
                        patenteBusq[strcspn(patenteBusq, "\n")] = '\0';
                    }
                    if (esCadenaVaciaOEspacios(patenteBusq)) {
                        printf("[Error] La patente no puede estar vacia ni contener solo espacios.\n");
                        continue;
                    }
                    for (i = 0; patenteBusq[i] != '\0'; i++) patenteBusq[i] = toupper((unsigned char)patenteBusq[i]);
                    if (!esFormatoPatenteValido(patenteBusq)) {
                        printf("[Error] Formato invalido. Debe ingresar exactamente 2 letras, 3 numeros y 2 letras (Ej: AB123CD).\n");
                        continue;
                    }
                    break;
                }
                if (buscarVehiculo(patenteBusq)) {
                    printf("[Exito] El vehiculo con patente %s se encuentra registrado.\n", patenteBusq);
                } else {
                    printf("[Error] No existe un vehiculo registrado con la patente %s.\n", patenteBusq);
                }
                break;
            }
            case 3:
                mostrarVehiculo();
                break;
            case 4:
                modificarVehiculo();
                break;
            case 0:
                volverAlMenuPrincipal();
                return;
            default:
                printf("Opcion invalida.\n");
                break;
        }
    } while (opcion != 0);
}

// Carga de un nuevo vehiculo guardandolo en vehiculos.dat con estado inicial Disponible (1)
void registrarVehiculo(void) {
    vehiculo v;
    FILE *archivo;
    int i;
    printf("\n--------------------------------- REGISTRAR VEHICULO ---------------------------------\n");
    // Ingreso y validacion de Patente
    while (1) {
        printf("Patente (Formato AA000AA): ");
        if (fgets(v.patente, sizeof(v.patente), stdin) != NULL) v.patente[strcspn(v.patente, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.patente)) continue;
        for (i = 0; v.patente[i]; i++) v.patente[i] = toupper((unsigned char)v.patente[i]);
        if (!esFormatoPatenteValido(v.patente)) {
            printf("[Error] Formato invalido. Debe ingresar exactamente 2 letras, 3 numeros y 2 letras (Ej: AB123CD).\n");
            continue;
        }
        break;
    }
    if (buscarVehiculo(v.patente) == 1) {
        printf("\n[Error] Ya existe un vehiculo registrado con esa patente.\n");
        return;
    }
    // Ingreso de Marca
    do {
        printf("Marca: ");
        if (fgets(v.marca, sizeof(v.marca), stdin) != NULL) v.marca[strcspn(v.marca, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.marca)) printf("[Error] La marca no puede estar vacia ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.marca)) printf("[Error] La marca solo debe contener letras y espacios entre palabras.\n");
    } while (esCadenaVaciaOEspacios(v.marca) || !esSoloTextoValido(v.marca));
    // Ingreso de Modelo
    do {
        printf("Modelo: ");
        if (fgets(v.modelo, sizeof(v.modelo), stdin) != NULL) v.modelo[strcspn(v.modelo, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.modelo)) printf("[Error] El modelo no puede estar vacio ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.modelo)) printf("[Error] El modelo solo debe contener letras y espacios entre palabras.\n");
    } while (esCadenaVaciaOEspacios(v.modelo) || !esSoloTextoValido(v.modelo));
    // Ingreso de Anio
    printf("Anio (1886 a 2026): ");
    v.anio = leerEnteroEnRango(1886, 2026, "[Error] Anio invalido. Debe ingresar un numero entero entre 1886 y 2026: ");
    // Ingreso de Tipo
    do {
        printf("Tipo de vehiculo (ej: Auto, Camioneta, SUV): ");
        if (fgets(v.tipo, sizeof(v.tipo), stdin) != NULL) v.tipo[strcspn(v.tipo, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.tipo)) printf("[Error] El tipo no puede estar vacio ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.tipo)) printf("[Error] El tipo solo debe contener letras y espacios.\n");
    } while (esCadenaVaciaOEspacios(v.tipo) || !esSoloTextoValido(v.tipo));
    // Ingreso de Capacidad de pasajeros
    printf("Capacidad de pasajeros (1 a 10): ");
    v.capacidad = leerEnteroEnRango(1, 10, "[Error] Capacidad invalida. Debe ingresar un numero entero entre 1 y 10: ");
    // Ingreso de Precio por dia
    char bufferPrecio[100];
    float precioTemp;
    while (1) {
        printf("Precio por dia: ");
        if (fgets(bufferPrecio, sizeof(bufferPrecio), stdin) != NULL) {
            bufferPrecio[strcspn(bufferPrecio, "\n")] = '\0';
            if (!esCadenaVaciaOEspacios(bufferPrecio)) {
                char basura;
                if (sscanf(bufferPrecio, "%f %c", &precioTemp, &basura) == 1 && precioTemp > 0) {
                    v.precioPorDia = precioTemp;
                    break;
                }
            }
        }
        printf("[Error] Precio invalido. Ingrese un valor numerico mayor que cero: ");
    }
    // Estado inicial automatico: 1 = Disponible
    v.estado = 1;
    // Guardado binario en archivo
    archivo = fopen("vehiculos.dat", "ab");
    if (archivo == NULL) {
        printf("\n[Error] No se pudo abrir el archivo 'vehiculos.dat'.\n");
        return;
    }
    if (fwrite(&v, sizeof(vehiculo), 1, archivo) == 1) printf("\n[Exito] Vehiculo registrado correctamente.\n");
    else printf("\n[Error] No se pudo guardar el vehiculo en el archivo.\n");
    fclose(archivo);
}

// Busca un vehiculo en el archivo binario por su patente. Retorna 1 si existe o 0 si no.
int buscarVehiculo(char patente[]) {
    int i;
    if (patente == NULL || esCadenaVaciaOEspacios(patente)) return 0;
    char patenteMayus[15];
    strncpy(patenteMayus, patente, sizeof(patenteMayus) - 1);
    patenteMayus[sizeof(patenteMayus) - 1] = '\0';
    for (i = 0; patenteMayus[i] != '\0'; i++) patenteMayus[i] = toupper((unsigned char)patenteMayus[i]);
    vehiculo v;
    FILE *archivo = fopen("vehiculos.dat", "rb");
    if (archivo == NULL) return 0;
    int encontrado = 0;
    while (encontrado == 0 && fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        if (strcmp(v.patente, patenteMayus) == 0) encontrado = 1;
    }
    fclose(archivo);
    return encontrado;
}

// Muestra la lista completa de vehiculos almacenados formateados en tabla
int mostrarVehiculo(void) {
    FILE *archivo = fopen("vehiculos.dat", "rb");
    if (archivo == NULL) {
        printf("\n[Error] No se pudo abrir el archivo de vehiculos (o aun no hay registros).\n");
        return 0;
    }
    vehiculo v;
    int contador = 0;
    printf("\n=================================  LISTADO GENERAL DE VEHICULOS ================================= \n");
    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("%-10s %-12s\t%-12s\t%-6s\t%-10s\t%-10s\t%-12s\t%-12s\t\n", "Patente", "Marca", "Modelo", "Anio", "Tipo", "Capacidad", "Precio/Dia", "Estado");
    printf("-----------------------------------------------------------------------------------------------------\n");
    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        char strEstado[15];
        if (v.estado == 1) strcpy(strEstado, "Disponible");
        else if (v.estado == 2) strcpy(strEstado, "Alquilado");
        else if (v.estado == 0) strcpy(strEstado, "Baja");
        else strcpy(strEstado, "Desconocido");
        printf("%-10s %-12s %-12s %-6d %-10s %-10d $%-11.2f %-12s\n", v.patente, v.marca, v.modelo, v.anio, v.tipo, v.capacidad, v.precioPorDia, strEstado);
        contador++;
    }
    printf("-----------------------------------------------------------------------------------------------------\n");
    if (contador == 0) printf("No hay vehiculos registrados en el sistema.\n");
    else printf("Total de vehiculos mostrados: %d\n", contador);
    fclose(archivo);
    return 1;
}

// Permite buscar un vehiculo por patente y modificar todos sus atributos (incluido estado: 1, 2 o 0)
int modificarVehiculo(void) {
    char patenteBusq[15];
    int i;
    printf("\n================================= MODIFICAR VEHICULO =================================\n");
    while (1) {
        printf("Ingrese la patente del vehiculo a modificar (Formato AA000AA): ");
        if (fgets(patenteBusq, sizeof(patenteBusq), stdin) != NULL) patenteBusq[strcspn(patenteBusq, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(patenteBusq)) {
            printf("[Error] La patente no puede estar vacia ni contener solo espacios.\n");
            continue;
        }
        for (i = 0; patenteBusq[i] != '\0'; i++) patenteBusq[i] = toupper((unsigned char)patenteBusq[i]);
        if (!esFormatoPatenteValido(patenteBusq)) {
            printf("[Error] Formato invalido. Debe ingresar exactamente 2 letras, 3 numeros y 2 letras (Ej: AB123CD).\n");
            continue;
        }
        break;
    }
    FILE *archivo = fopen("vehiculos.dat", "rb+");
    if (archivo == NULL) {
        printf("\n[Error] No se pudo abrir el archivo de vehiculos (o aun no existen registros).\n");
        return 0;
    }
    vehiculo v;
    int encontrado = 0;
    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        if (strcmp(v.patente, patenteBusq) == 0) {
            encontrado = 1;
            long posicionActual = ftell(archivo) - sizeof(vehiculo);
            printf("\nVehiculo encontrado (%s). Ingrese los nuevos datos:\n", v.patente);
            // Nueva Marca
            do {
                printf("Nueva marca (Actual: %s): ", v.marca);
                if (fgets(v.marca, sizeof(v.marca), stdin) != NULL) v.marca[strcspn(v.marca, "\n")] = '\0';
                if (esCadenaVaciaOEspacios(v.marca)) printf("[Error] La marca no puede estar vacia ni contener solo espacios.\n");
                else if (!esSoloTextoValido(v.marca)) printf("[Error] La marca solo debe contener letras y espacios entre palabras.\n");
            } while (esCadenaVaciaOEspacios(v.marca) || !esSoloTextoValido(v.marca));
            // Nuevo Modelo
            do {
                printf("Nuevo modelo (Actual: %s): ", v.modelo);
                if (fgets(v.modelo, sizeof(v.modelo), stdin) != NULL) v.modelo[strcspn(v.modelo, "\n")] = '\0';
                if (esCadenaVaciaOEspacios(v.modelo)) printf("[Error] El modelo no puede estar vacio ni contener solo espacios.\n");
                else if (!esSoloTextoValido(v.modelo)) printf("[Error] El modelo solo debe contener letras y espacios entre palabras.\n");
            } while (esCadenaVaciaOEspacios(v.modelo) || !esSoloTextoValido(v.modelo));
            // Nuevo Anio
            printf("Nuevo anio (Actual: %d): ", v.anio);
            v.anio = leerEnteroEnRango(1886, 2026, "[Error] Anio invalido. Ingrese un numero entero entre 1886 y 2026: ");
            // Nuevo Tipo
            do {
                printf("Nuevo tipo de vehiculo (Actual: %s): ", v.tipo);
                if (fgets(v.tipo, sizeof(v.tipo), stdin) != NULL) v.tipo[strcspn(v.tipo, "\n")] = '\0';
                if (esCadenaVaciaOEspacios(v.tipo)) printf("[Error] El tipo no puede estar vacio ni contener solo espacios.\n");
                else if (!esSoloTextoValido(v.tipo)) printf("[Error] El tipo solo debe contener letras y espacios.\n");
            } while (esCadenaVaciaOEspacios(v.tipo) || !esSoloTextoValido(v.tipo));
            // Nueva Capacidad
            printf("Nueva capacidad (Actual: %d): ", v.capacidad);
            v.capacidad = leerEnteroEnRango(1, 10, "[Error] Capacidad invalida. Ingrese un numero entero entre 1 y 10: ");
            // Nuevo Precio por dia
            char bufferPrecio[100];
            float precioTemp;
            while (1) {
                printf("Nuevo precio por dia (Actual: %.2f): ", v.precioPorDia);
                if (fgets(bufferPrecio, sizeof(bufferPrecio), stdin) != NULL) {
                    bufferPrecio[strcspn(bufferPrecio, "\n")] = '\0';
                    if (!esCadenaVaciaOEspacios(bufferPrecio)) {
                        char basura;
                        if (sscanf(bufferPrecio, "%f %c", &precioTemp, &basura) == 1 && precioTemp > 0) {
                            v.precioPorDia = precioTemp;
                            break;
                        }
                    }
                }
                printf("[Error] Precio invalido. Ingrese un valor numerico mayor a cero.\n");
            }
            // Nuevo Estado (Limitado estrictamente a 1: Disponible | 2: Alquilado | 0: Baja)
            printf("Nuevo estado (1: Disponible | 2: Alquilado | 0: Baja) - Actual (%d): ", v.estado);
            v.estado = leerEnteroEnRango(0, 2, "[Error] Estado invalido. Ingrese 1 (Disponible), 2 (Alquilado) o 0 (Baja): ");
            // Sobreescribe el registro en la posicion correspondiente del archivo
            fseek(archivo, posicionActual, SEEK_SET);
            fwrite(&v, sizeof(vehiculo), 1, archivo);
            fflush(archivo);
            printf("\n[Exito] Vehiculo con patente %s modificado correctamente.\n", v.patente);
            break;
        }
    }
    fclose(archivo);
    if (!encontrado) printf("\n[Error] No existe un vehiculo registrado con la patente %s.\n", patenteBusq);
    return 1;
}

// Mensaje de retorno al flujo principal
void volverAlMenuPrincipal(void) {
    printf("\nRegresando al menu principal...\n");
}
