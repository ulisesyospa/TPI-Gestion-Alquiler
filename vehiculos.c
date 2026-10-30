#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vehiculos.h"

// Valida que una cadena contenga unicamente letras y espacios
int esSoloTextoValido(const char *cadena) {
    if (cadena == NULL || strlen(cadena) == 0) return 0;
    int tieneLetra = 0;
    int i;
    for (i = 0; cadena[i] != '\0'; i++) {
        unsigned char c = (unsigned char)cadena[i];
        if (isalpha(c)) {
            tieneLetra = 1;
        } else if (!isspace(c)) {
            return 0;
        }
    }
    return tieneLetra;
}

// Verifica si una cadena esta vacia o compuesta unicamente por espacios
int esCadenaVaciaOEspacios(const char *cadena) {
    if (cadena == NULL) return 1;
    int i;
    for (i = 0; cadena[i] != '\0'; i++) {
        if (!isspace((unsigned char)cadena[i])) {
            return 0;
        }
    }
    return 1;
}

// Solicita y valida la lectura de un entero comprendido entre un rango minimo y maximo
int leerEnteroEnRango(int min, int max, const char *mensajeError) {
    char buffer[100];
    char basura;
    int valor;
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (buffer[0] == '\n' || buffer[0] == '\r') {
                printf("[Aviso] No ingreso ningun dato. Intente nuevamente: ");
                continue;
            }
            if (sscanf(buffer, " %d %c", &valor, &basura) == 1) {
                if (valor >= min && valor <= max) {
                    return valor;
                }
            }
        }
        printf("%s", mensajeError);
    }
}

// Convierte la patente ingresada a mayusculas y valida el formato estandar AA000AA
int validarFormatoPatente(char *patente) {
    int len = strlen(patente);
    if (len != 7) return 0;
    int i;
    for (i = 0; i < len; i++) {
        if (patente[i] >= 'a' && patente[i] <= 'z') {
            patente[i] = patente[i] - 32;
        }
    }
    if (isalpha((unsigned char)patente[0]) && isalpha((unsigned char)patente[1]) &&
        isdigit((unsigned char)patente[2]) && isdigit((unsigned char)patente[3]) && isdigit((unsigned char)patente[4]) &&
        isalpha((unsigned char)patente[5]) && isalpha((unsigned char)patente[6])) {
        return 1;
    }
    return 0;
}

// Menu interactivo para las operaciones sobre la flota de vehiculos
void menuVehiculos(void) {
    int opcion;
    char buffer[100];
    char basura;
    do {
        printf("\n========================================\n");
        printf("             MENU VEHICULOS             \n");
        printf("========================================\n");
        printf("1. Registrar vehiculo\n");
        printf("2. Mostrar vehiculos\n");
        printf("3. Modificar vehiculo\n");
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
                registrarVehiculo();
                break;
            case 2:
                mostrarVehiculo();
                break;
            case 3:
                modificarVehiculo();
                break;
            case 0:
                printf("\nRegresando al menu principal...\n");
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    } while (opcion != 0);
}

// Busca una patente en vehiculos.dat. Retorna 1 si existe o 0 si no
int buscarVehiculo(char patente[]) {
    vehiculo v;
    FILE *archivo = fopen("vehiculos.dat", "rb");
    if (archivo == NULL) {
        return 0;
    }
    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        if (strcasecmp(v.patente, patente) == 0) {
            fclose(archivo);
            return 1;
        }
    }
    fclose(archivo);
    return 0;
}

// Alta de un nuevo vehiculo fijando capacidad por tipo y asignando estado inicial Disponible (1)
void registrarVehiculo(void) {
    vehiculo v;
    FILE *archivo;
    char buffer[100];
    int tipoOpcion;

    printf("\n========================================\n");
    printf("           REGISTRAR VEHICULO           \n");
    printf("========================================\n");

    // Lectura y validacion de Patente
    while (1) {
        printf("Ingrese la patente (Formato AA000AA): ");
        if (fgets(v.patente, sizeof(v.patente), stdin) == NULL) continue;
        v.patente[strcspn(v.patente, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.patente)) {
            printf("[Aviso] La patente no puede estar vacia.\n");
            continue;
        }
        if (!validarFormatoPatente(v.patente)) {
            printf("[Error] Formato invalido. Debe responder al patron AA000AA (2 letras, 3 numeros, 2 letras).\n");
            continue;
        }
        if (buscarVehiculo(v.patente) == 1) {
            printf("[Error] Ya existe un vehiculo registrado con esa patente.\n");
            return;
        }
        break;
    }

    // Lectura y validacion de Marca
    while (1) {
        printf("Ingrese la marca: ");
        if (fgets(v.marca, sizeof(v.marca), stdin) == NULL) continue;
        v.marca[strcspn(v.marca, "\n")] = '\0';
        if (!esSoloTextoValido(v.marca)) {
            printf("[Error] Marca invalida. Ingrese unicamente texto.\n");
            continue;
        }
        break;
    }

    // Lectura y validacion de Modelo
    while (1) {
        printf("Ingrese el modelo: ");
        if (fgets(v.modelo, sizeof(v.modelo), stdin) == NULL) continue;
        v.modelo[strcspn(v.modelo, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.modelo)) {
            printf("[Error] El modelo no puede estar vacio.\n");
            continue;
        }
        break;
    }

    // Lectura y validacion del Anio (Rango 1990 - 2026)
    printf("Ingrese el anio de fabricacion (1990 - 2026): ");
    v.anio = leerEnteroEnRango(1990, 2026, "[Error] Anio fuera de rango. Ingrese entre 1990 y 2026: ");

    // Seleccion de Tipo de vehiculo y capacidad automatica
    printf("\n--- SELECCION DE TIPO DE VEHICULO ---\n");
    printf("1. Auto (Capacidad: 5 pasajeros)\n");
    printf("2. Camioneta (Capacidad: 5 pasajeros)\n");
    printf("3. SUV (Capacidad: 7 pasajeros)\n");
    printf("4. Otros (Ingreso manual de capacidad)\n");
    printf("Seleccione tipo: ");
    tipoOpcion = leerEnteroEnRango(1, 4, "[Error] Opcion invalida. Ingrese entre 1 y 4: ");

    if (tipoOpcion == 1) {
        strcpy(v.tipo, "Auto");
        v.capacidad = 5;
    } else if (tipoOpcion == 2) {
        strcpy(v.tipo, "Camioneta");
        v.capacidad = 5;
    } else if (tipoOpcion == 3) {
        strcpy(v.tipo, "SUV");
        v.capacidad = 7;
    } else {
        strcpy(v.tipo, "Otros");
        printf("Ingrese capacidad de pasajeros (1 a 20): ");
        v.capacidad = leerEnteroEnRango(1, 20, "[Error] Capacidad invalida. Ingrese un valor entre 1 y 20: ");
    }

    // Lectura del precio de alquiler por dia
    char basura;
    while (1) {
        printf("Ingrese el precio de alquiler por dia ($): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (sscanf(buffer, " %f %c", &v.precioPorDia, &basura) == 1 && v.precioPorDia > 0) {
            break;
        }
        printf("[Error] Precio invalido. Ingrese un numero positivo mayor a 0.\n");
    }

    v.estado = 1; // Asignacion por defecto a estado Disponible

    archivo = fopen("vehiculos.dat", "ab");
    if (archivo == NULL) {
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return;
    }
    if (fwrite(&v, sizeof(vehiculo), 1, archivo) == 1) {
        printf("\n[Exito] Vehiculo registrado correctamente en la flota.\n");
    } else {
        printf("\n[Error] No se pudo guardar el vehiculo en el archivo.\n");
    }
    fclose(archivo);
}

// Imprime el listado completo de vehiculos almacenados en vehiculos.dat
int mostrarVehiculo(void) {
    vehiculo v;
    FILE *archivo = fopen("vehiculos.dat", "rb");
    int contador = 0;

    printf("\n========================================\n");
    printf("          FLOTA DE VEHICULOS            \n");
    printf("========================================\n");

    if (archivo == NULL) {
        printf("[Aviso] No se pudo abrir el archivo o aun no hay vehiculos cargados.\n");
        return 0;
    }

    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        printf("Patente: %s\n", v.patente);
        printf("Marca: %s | Modelo: %s | Anio: %d\n", v.marca, v.modelo, v.anio);
        printf("Tipo: %s | Capacidad: %d pasajeros\n", v.tipo, v.capacidad);
        printf("Precio por dia: $%.2f\n", v.precioPorDia);

        if (v.estado == 1) {
            printf("Estado: Disponible\n");
        } else if (v.estado == 2) {
            printf("Estado: Alquilado\n");
        } else if (v.estado == 3) {
            printf("Estado: En Mantenimiento\n");
        } else {
            printf("Estado: Baja\n");
        }
        printf("----------------------------------------\n");
        contador++;
    }

    fclose(archivo);
    if (contador == 0) {
        printf("[Aviso] No hay vehiculos registrados.\n");
    } else {
        printf("Total de vehiculos en sistema: %d\n", contador);
    }
    return contador;
}

// Permite modificar datos individuales de un vehiculo incluyendo los 4 estados exigidos
int modificarVehiculo(void) {
    vehiculo v;
    FILE *archivo;
    char patenteBusqueda[20];
    int encontrado = 0;
    long posicion;
    int opcion;
    char buffer[100];
    char basura;

    printf("\n========================================\n");
    printf("           MODIFICAR VEHICULO           \n");
    printf("========================================\n");
    printf("Ingrese la patente del vehiculo a modificar: ");

    if (fgets(patenteBusqueda, sizeof(patenteBusqueda), stdin) == NULL) return 0;
    patenteBusqueda[strcspn(patenteBusqueda, "\n")] = '\0';

    archivo = fopen("vehiculos.dat", "r+b");
    if (archivo == NULL) {
        printf("[Error] No se pudo abrir el archivo de vehiculos.\n");
        return 0;
    }

    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        if (strcasecmp(v.patente, patenteBusqueda) == 0) {
            encontrado = 1;
            posicion = ftell(archivo) - sizeof(vehiculo);
            break;
        }
    }

    if (!encontrado) {
        printf("[Error] No se encontro un vehiculo con la patente %s.\n", patenteBusqueda);
        fclose(archivo);
        return 0;
    }

    do {
        printf("\n--- DATOS ACTUALES ---\n");
        printf("Patente: %s | Marca/Modelo: %s %s\n", v.patente, v.marca, v.modelo);
        printf("Anio: %d | Tipo: %s | Capacidad: %d | Precio/Dia: $%.2f\n", v.anio, v.tipo, v.capacidad, v.precioPorDia);
        
        char estStr[25];
        if (v.estado == 1) {
            strcpy(estStr, "Disponible");
        } else if (v.estado == 2) {
            strcpy(estStr, "Alquilado");
        } else if (v.estado == 3) {
            strcpy(estStr, "En Mantenimiento");
        } else {
            strcpy(estStr, "Baja");
        }
        printf("Estado Actual: %s\n", estStr);

        printf("\n--- OPCIONES DE MODIFICACION ---\n");
        printf("1. Modificar Marca\n");
        printf("2. Modificar Modelo\n");
        printf("3. Modificar Anio\n");
        printf("4. Modificar Tipo y Capacidad\n");
        printf("5. Modificar Precio por dia\n");
        printf("6. Modificar Estado (Disponible, Alquilado, Mantenimiento, Baja)\n");
        printf("0. Guardar cambios y volver\n");
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
                while (1) {
                    printf("Ingrese la nueva marca: ");
                    if (fgets(v.marca, sizeof(v.marca), stdin) == NULL) continue;
                    v.marca[strcspn(v.marca, "\n")] = '\0';
                    if (!esSoloTextoValido(v.marca)) {
                        printf("[Error] Marca invalida.\n");
                        continue;
                    }
                    break;
                }
                break;
            case 2:
                while (1) {
                    printf("Ingrese el nuevo modelo: ");
                    if (fgets(v.modelo, sizeof(v.modelo), stdin) == NULL) continue;
                    v.modelo[strcspn(v.modelo, "\n")] = '\0';
                    if (esCadenaVaciaOEspacios(v.modelo)) {
                        printf("[Error] El modelo no puede estar vacio.\n");
                        continue;
                    }
                    break;
                }
                break;
            case 3:
                printf("Ingrese el nuevo anio de fabricacion (1990 - 2026): ");
                v.anio = leerEnteroEnRango(1990, 2026, "[Error] Anio fuera de rango: ");
                break;
            case 4: {
                int tOp;
                printf("\n--- NUEVO TIPO DE VEHICULO ---\n");
                printf("1. Auto (Capacidad: 5)\n");
                printf("2. Camioneta (Capacidad: 5)\n");
                printf("3. SUV (Capacidad: 7)\n");
                printf("4. Otros (Manual)\n");
                printf("Seleccione tipo: ");
                tOp = leerEnteroEnRango(1, 4, "[Error] Seleccione entre 1 y 4: ");
                if (tOp == 1) {
                    strcpy(v.tipo, "Auto");
                    v.capacidad = 5;
                } else if (tOp == 2) {
                    strcpy(v.tipo, "Camioneta");
                    v.capacidad = 5;
                } else if (tOp == 3) {
                    strcpy(v.tipo, "SUV");
                    v.capacidad = 7;
                } else {
                    strcpy(v.tipo, "Otros");
                    printf("Ingrese capacidad de pasajeros (1 a 20): ");
                    v.capacidad = leerEnteroEnRango(1, 20, "[Error] Capacidad invalida: ");
                }
                break;
            }
            case 5:
                while (1) {
                    printf("Ingrese el nuevo precio por dia ($): ");
                    if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
                    buffer[strcspn(buffer, "\n")] = '\0';
                    if (sscanf(buffer, " %f %c", &v.precioPorDia, &basura) == 1 && v.precioPorDia > 0) {
                        break;
                    }
                    printf("[Error] Precio invalido. Ingrese un numero positivo.\n");
                }
                break;
            case 6:
                printf("\n--- SELECCIONE NUEVO ESTADO ---\n");
                printf("1. Disponible\n");
                printf("2. Alquilado\n");
                printf("3. En Mantenimiento\n");
                printf("0. Baja\n");
                printf("Seleccione estado: ");
                v.estado = leerEnteroEnRango(0, 3, "[Error] Opcion invalida. Ingrese entre 0 y 3: ");
                break;
            case 0:
                if (fseek(archivo, posicion, SEEK_SET) == 0) {
                    if (fwrite(&v, sizeof(vehiculo), 1, archivo) == 1) {
                        printf("\n[Exito] Cambios guardados correctamente.\n");
                    } else {
                        printf("\n[Error] No se pudieron escribir los cambios en el archivo.\n");
                    }
                } else {
                    printf("\n[Error] No se pudo posicionar el puntero del archivo.\n");
                }
                break;
            default:
                printf("[Error] Opcion invalida.\n");
        }
    } while (opcion != 0);

    fclose(archivo);
    return 1;
}
