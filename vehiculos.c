#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "vehiculos.h"
// Valida que la cadena contenga unicamente letras y espacios entre palabras
int esSoloTextoValido(const char *cadena) {
    int i;
    if (strlen(cadena) == 0) return 0;
    for (i = 0; cadena[i] != '\0'; i++) {
        if (!isalpha((unsigned char)cadena[i]) && cadena[i] != ' ') return 0;
    }
    return 1;
}
// Comprueba si la cadena esta vacia o compuesta solo por espacios o tabulaciones
int esCadenaVaciaOEspacios(const char *cadena) {
    int i;
    if (strlen(cadena) == 0) return 1;
    for (i = 0; cadena[i] != '\0'; i++) {
        if (!isspace((unsigned char)cadena[i])) return 0;
    }
    return 1;
}
// Lee enteros dentro de un rango especifico mediante buffer seguro para evitar errores de tipeo
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
// Verifica el formato oficial de patente argentina de 7 caracteres (AA000AA)
int esFormatoPatenteValido(char *patente) {
    if (strlen(patente) != 7) return 0;
    if (!isalpha((unsigned char)patente[0]) || !isalpha((unsigned char)patente[1])) return 0;
    if (!isdigit((unsigned char)patente[2]) || !isdigit((unsigned char)patente[3]) || !isdigit((unsigned char)patente[4])) return 0;
    if (!isalpha((unsigned char)patente[5]) || !isalpha((unsigned char)patente[6])) return 0;
    return 1;
}
// Menu interactivo para las operaciones del modulo de vehiculos
void menuVehiculos(void) {
    int opcion;
    do {
        printf("\n========================================\n");
        printf("             MENU VEHICULOS             \n");
        printf("========================================\n");
        printf("1. Registrar vehiculo\n");
        printf("2. Buscar vehiculo\n");
        printf("3. Mostrar vehiculos\n");
        printf("4. Modificar vehiculo\n");
        printf("0. Volver al menu principal\n");
        printf("----------------------------------------\n");
        printf("Seleccione una opcion: ");
        opcion = leerEnteroEnRango(0, 4, "[Error] Opcion invalida. Ingrese un numero entre 0 y 4: ");
        switch (opcion) {
            case 1:
                registrarVehiculo();
                break;
            case 2: {
                // Busqueda individual de vehiculo por consola mediante ingreso de patente
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
                printf("[Error] Opcion invalida.\n");
                break;
        }
    } while (opcion != 0);
}
// Carga de un nuevo vehiculo en la flota con capacidad estrictamente vinculada al tipo ingresado
void registrarVehiculo(void) {
    vehiculo v;
    FILE *archivo;
    int i;
    printf("\n========================================\n");
    printf("           REGISTRAR VEHICULO           \n");
    printf("========================================\n");
    // Lectura y normalizacion de patente
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
    // Ingreso y validacion de Marca
    do {
        printf("Marca: ");
        if (fgets(v.marca, sizeof(v.marca), stdin) != NULL) v.marca[strcspn(v.marca, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.marca)) printf("[Error] La marca no puede estar vacia ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.marca)) printf("[Error] La marca solo debe contener letras y espacios entre palabras.\n");
    } while (esCadenaVaciaOEspacios(v.marca) || !esSoloTextoValido(v.marca));
    // Ingreso y validacion de Modelo
    do {
        printf("Modelo: ");
        if (fgets(v.modelo, sizeof(v.modelo), stdin) != NULL) v.modelo[strcspn(v.modelo, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.modelo)) printf("[Error] El modelo no puede estar vacio ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.modelo)) printf("[Error] El modelo solo debe contener letras y espacios entre palabras.\n");
    } while (esCadenaVaciaOEspacios(v.modelo) || !esSoloTextoValido(v.modelo));
    // Ingreso de Año dentro del rango permitido
    printf("Anio (1886 a 2026): ");
    v.anio = leerEnteroEnRango(1886, 2026, "[Error] Anio invalido. Ingrese un numero entero entre 1886 y 2026: ");
    // Ingreso del Tipo de vehiculo
    do {
        printf("Tipo de vehiculo (Auto, SUV, Camioneta): ");
        if (fgets(v.tipo, sizeof(v.tipo), stdin) != NULL) v.tipo[strcspn(v.tipo, "\n")] = '\0';
        if (esCadenaVaciaOEspacios(v.tipo)) printf("[Error] El tipo no puede estar vacio ni contener solo espacios.\n");
        else if (!esSoloTextoValido(v.tipo)) printf("[Error] El tipo solo debe contener letras y espacios.\n");
    } while (esCadenaVaciaOEspacios(v.tipo) || !esSoloTextoValido(v.tipo));
    // Asignacion automatica de capacidad vinculada al tipo seleccionado
    if (strcasecmp(v.tipo, "Auto") == 0 || strcasecmp(v.tipo, "Camioneta") == 0) {
        v.capacidad = 5;
        printf("[Aviso] Capacidad asignada automaticamente (%s): %d pasajeros.\n", v.tipo, v.capacidad);
    } else if (strcasecmp(v.tipo, "SUV") == 0) {
        v.capacidad = 7;
        printf("[Aviso] Capacidad asignada automaticamente (%s): %d pasajeros.\n", v.tipo, v.capacidad);
    }
    // Lectura segura del precio por dia
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
    // Estado inicial automatico (1 = Disponible)
    v.estado = 1;
    // Escritura de la estructura en vehiculos.dat
    archivo = fopen("vehiculos.dat", "ab");
    if (archivo == NULL) {
        printf("\n[Error] No se pudo abrir el archivo 'vehiculos.dat'.\n");
        return;
    }
    if (fwrite(&v, sizeof(vehiculo), 1, archivo) == 1) printf("\n[Exito] Vehiculo registrado correctamente.\n");
    else printf("\n[Error] No se pudo guardar el vehiculo en el archivo.\n");
    fclose(archivo);
}
// Recorre vehiculos.dat buscando la patente. Retorna 1 si existe o 0 en caso contrario
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
// Imprime en pantalla el listado de la flota formateado en columnas
int mostrarVehiculo(void) {
    FILE *archivo = fopen("vehiculos.dat", "rb");
    if (archivo == NULL) {
        printf("\n[Error] No se pudo abrir el archivo de vehiculos (o aun no hay registros).\n");
        return 0;
    }
    vehiculo v;
    int contador = 0;
    printf("\n========================================\n");
    printf("      LISTADO GENERAL DE VEHICULOS      \n");
    printf("========================================\n");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("%-10s   %-15s   %-15s   %-6s   %-12s   %-10s   %-12s   %-12s\n", "Patente", "Marca", "Modelo", "Anio", "Tipo", "Capacidad", "Precio/Dia", "Estado");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        char strEstado[15];
        if (v.estado == 1) strcpy(strEstado, "Disponible");
        else if (v.estado == 2) strcpy(strEstado, "Alquilado");
        else if (v.estado == 0) strcpy(strEstado, "Baja");
        else strcpy(strEstado, "Desconocido");
        printf("%-10s   %-15.15s   %-15.15s   %-6d   %-12.12s   %-10d   $%-11.2f   %-12s\n", 
               v.patente, v.marca, v.modelo, v.anio, v.tipo, v.capacidad, v.precioPorDia, strEstado);
        contador++;
    }
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    if (contador == 0) printf("[Aviso] No hay vehiculos registrados en el sistema.\n");
    else printf("Total de vehiculos mostrados: %d\n", contador);
    fclose(archivo);
    return 1;
}
// Permite modificar los datos de un vehiculo actualizando capacidad de forma automatica al cambiar el tipo
int modificarVehiculo(void) {
    char patenteBusq[15];
    int i;
    printf("\n========================================\n");
    printf("           MODIFICAR VEHICULO           \n");
    printf("========================================\n");
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
    long posicionActual = 0;
    while (fread(&v, sizeof(vehiculo), 1, archivo) == 1) {
        if (strcmp(v.patente, patenteBusq) == 0) {
            encontrado = 1;
            posicionActual = ftell(archivo) - sizeof(vehiculo);
            break;
        }
    }
    if (!encontrado) {
        fclose(archivo);
        printf("\n[Error] No existe un vehiculo registrado con la patente %s.\n", patenteBusq);
        return 0;
    }
    int opcionMod;
    do {
        printf("\n--- DATOS ACTUALES DEL VEHICULO (%s) ---\n", v.patente);
        printf("1. Marca: %s\n", v.marca);
        printf("2. Modelo: %s\n", v.modelo);
        printf("3. Anio: %d\n", v.anio);
        printf("4. Tipo: %s (Capacidad asociada: %d pasajeros)\n", v.tipo, v.capacidad);
        printf("5. Precio por dia: $%.2f\n", v.precioPorDia);
        char strEstado[15];
        if (v.estado == 1) strcpy(strEstado, "Disponible");
        else if (v.estado == 2) strcpy(strEstado, "Alquilado");
        else if (v.estado == 0) strcpy(strEstado, "Baja");
        else strcpy(strEstado, "Desconocido");
        printf("6. Estado: %s (%d)\n", strEstado, v.estado);
        printf("0. Guardar cambios y volver\n");
        printf("----------------------------------------\n");
        printf("Seleccione una opcion a modificar: ");
        opcionMod = leerEnteroEnRango(0, 6, "[Error] Opcion invalida. Ingrese un numero entre 0 y 6: ");
        switch (opcionMod) {
            case 1:
                do {
                    printf("Ingrese la nueva marca: ");
                    if (fgets(v.marca, sizeof(v.marca), stdin) != NULL) v.marca[strcspn(v.marca, "\n")] = '\0';
                    if (esCadenaVaciaOEspacios(v.marca)) printf("[Error] La marca no puede estar vacia ni contener solo espacios.\n");
                    else if (!esSoloTextoValido(v.marca)) printf("[Error] La marca solo debe contener letras y espacios entre palabras.\n");
                } while (esCadenaVaciaOEspacios(v.marca) || !esSoloTextoValido(v.marca));
                break;
            case 2:
                do {
                    printf("Ingrese el nuevo modelo: ");
                    if (fgets(v.modelo, sizeof(v.modelo), stdin) != NULL) v.modelo[strcspn(v.modelo, "\n")] = '\0';
                    if (esCadenaVaciaOEspacios(v.modelo)) printf("[Error] El modelo no puede estar vacio ni contener solo espacios.\n");
                    else if (!esSoloTextoValido(v.modelo)) printf("[Error] El modelo solo debe contener letras y espacios entre palabras.\n");
                } while (esCadenaVaciaOEspacios(v.modelo) || !esSoloTextoValido(v.modelo));
                break;
            case 3:
                printf("Ingrese el nuevo anio (1886 a 2026): ");
                v.anio = leerEnteroEnRango(1886, 2026, "[Error] Anio invalido. Ingrese un numero entero entre 1886 y 2026: ");
                break;
            case 4: {
                int capacidadAnterior = v.capacidad;
                do {
                    printf("Ingrese el nuevo tipo de vehiculo (Auto, SUV, Camioneta): ");
                    if (fgets(v.tipo, sizeof(v.tipo), stdin) != NULL) v.tipo[strcspn(v.tipo, "\n")] = '\0';
                    if (esCadenaVaciaOEspacios(v.tipo)) printf("[Error] El tipo no puede estar vacio ni contener solo espacios.\n");
                    else if (!esSoloTextoValido(v.tipo)) printf("[Error] El tipo solo debe contener letras y espacios.\n");
                } while (esCadenaVaciaOEspacios(v.tipo) || !esSoloTextoValido(v.tipo));
                // Recalcula la capacidad segun el nuevo tipo ingresado
                if (strcasecmp(v.tipo, "Auto") == 0 || strcasecmp(v.tipo, "Camioneta") == 0) {
                    v.capacidad = 5;
                } else if (strcasecmp(v.tipo, "SUV") == 0) {
                    v.capacidad = 7;
                } else {
                    printf("Ingrese la capacidad de pasajeros para el tipo %s (1 a 10): ", v.tipo);
                    v.capacidad = leerEnteroEnRango(1, 10, "[Error] Capacidad invalida. Ingrese un numero entero entre 1 y 10: ");
                }
                // Informa el cambio o permanencia de capacidad
                if (v.capacidad != capacidadAnterior) {
                    printf("[Aviso] La capacidad cambio automaticamente de %d a %d pasajeros por el tipo (%s).\n", capacidadAnterior, v.capacidad, v.tipo);
                } else {
                    printf("[Aviso] La capacidad se mantiene en %d pasajeros para el tipo (%s).\n", v.capacidad, v.tipo);
                }
                break;
            }
            case 5: {
                char bufferPrecio[100];
                float precioTemp;
                while (1) {
                    printf("Ingrese el nuevo precio por dia: ");
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
                break;
            }
            case 6:
                printf("Ingrese el nuevo estado (1: Disponible | 2: Alquilado | 0: Baja): ");
                v.estado = leerEnteroEnRango(0, 2, "[Error] Estado invalido. Ingrese 1 (Disponible), 2 (Alquilado) o 0 (Baja): ");
                break;
            case 0:
                // Reubica el puntero en la posicion original y actualiza la estructura modificada
                fseek(archivo, posicionActual, SEEK_SET);
                fwrite(&v, sizeof(vehiculo), 1, archivo);
                fflush(archivo);
                printf("\n[Exito] Vehiculo con patente %s modificado correctamente.\n", v.patente);
                break;
        }
    } while (opcionMod != 0);
    fclose(archivo);
    return 1;
}
// Notificacion al salir del submenu de vehiculos
void volverAlMenuPrincipal(void) {
    printf("\nRegresando al menu principal...\n");
}
