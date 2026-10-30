#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "clientes.h"
// Valida que el nombre solo contenga letras, espacios y caracteres especiales en español
int esNombreValido(char *cadena) {
    if (strlen(cadena) == 0) return 0;
    int tieneLetra = 0;
    int i;
    for (i = 0; cadena[i] != '\0'; i++) {
        unsigned char c = (unsigned char)cadena[i];
        if (isalpha(c) || c == 'á' || c == 'é' || c == 'í' || c == 'ó' || c == 'ú' || 
            c == 'Á' || c == 'É' || c == 'Í' || c == 'Ó' || c == 'Ú' || 
            c == 'ñ' || c == 'Ñ' || c == 'ü' || c == 'Ü') {
            tieneLetra = 1;
        } else if (!isspace(c)) {
            return 0;
        }
    }
    return tieneLetra;
}
// Verifica que el telefono tenga exactamente 10 digitos y no sea una secuencia de ceros
int esTelefonoValido(char *telefono) {
    if (strlen(telefono) != 10) return 0;
    int i;
    int soloCeros = 1;
    for (i = 0; i < 10; i++) {
        if (!isdigit(telefono[i])) {
            return 0;
        }
        if (telefono[i] != '0') {
            soloCeros = 0;
        }
    }
    if (soloCeros) {
        return 0;
    }
    return 1;
}
// Controla que la direccion tenga al menos 3 caracteres y combine letras y numeros
int esDireccionValida(char *direccion) {
    if (strlen(direccion) < 3) return 0;
    int i;
    int tieneLetra = 0;
    int tieneNumero = 0;
    for (i = 0; direccion[i] != '\0'; i++) {
        unsigned char c = (unsigned char)direccion[i];
        if (isalpha(c) || c == 'á' || c == 'é' || c == 'í' || c == 'ó' || c == 'ú' || 
            c == 'Á' || c == 'É' || c == 'Í' || c == 'Ó' || c == 'Ú' || 
            c == 'ñ' || c == 'Ñ' || c == 'ü' || c == 'Ü') {
            tieneLetra = 1;
        } else if (isdigit(c)) {
            tieneNumero = 1;
        } else if (!isspace(c) && c != '.' && c != '#' && c != '-' && c != ',') {
            return 0;
        }
    }
    return (tieneLetra && tieneNumero);
}
// Valida el formato de e-mail comprobando presencia de arroba y punto sin espacios
int esEmailValido(char *email) {
    int len = strlen(email);
    if (len < 6) return 0;
    if (strchr(email, ' ') != NULL) return 0;
    char *arroba = strchr(email, '@');
    if (arroba == NULL || arroba == email || *(arroba + 1) == '\0') return 0;
    if (strchr(arroba + 1, '@') != NULL) return 0;
    char *punto = strrchr(arroba + 1, '.');
    if (punto == NULL || punto == arroba + 1 || *(punto + 1) == '\0') return 0;
    return 1;
}
// Normaliza la fecha ingresada y valida que coincida estrictamente con el dia actual
int fechaValidaCliente(int dia, int mes, int anio){
    struct tm fecha = {0};
    fecha.tm_mday = dia;
    fecha.tm_mon = mes - 1;
    fecha.tm_year = anio - 1900;
    struct tm copia = fecha;
    time_t controlFecha = mktime(&copia);
    if(controlFecha == -1 || copia.tm_mday != dia || copia.tm_mon != mes - 1 || copia.tm_year != anio - 1900){
        return 0;
    }
    time_t tActual = time(NULL);
    struct tm *infoActual = localtime(&tActual);
    infoActual->tm_hour = 0;
    infoActual->tm_min = 0;
    infoActual->tm_sec = 0;
    time_t tActualSinHora = mktime(infoActual);
    struct tm *infoIngresada = localtime(&controlFecha);
    infoIngresada->tm_hour = 0;
    infoIngresada->tm_min = 0;
    infoIngresada->tm_sec = 0;
    time_t tIngresadaSinHora = mktime(infoIngresada);
    if (tIngresadaSinHora != tActualSinHora) {
        return 0;
    }
    return 1;
}
// Menu interactivo para la administracion y consulta de clientes
void menuClientes(void) { 
    int opcion; 
    char buffer[100];
    char basura;
    do {
        printf("\n========================================\n");
        printf("             MENU CLIENTES              \n");
        printf("========================================\n");
        printf("1. Registrar cliente\n"); 
        printf("2. Buscar cliente\n"); 
        printf("3. Mostrar clientes\n"); 
        printf("4. Modificar cliente\n"); 
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
                registrarCliente(); 
                break; 
            case 2:
                consultarCliente(); 
                break; 
            case 3:
                mostrarClientes(); 
                break; 
            case 4: 
                modificarCliente(); 
                break; 
            case 0:
                printf("\nRegresando al menu principal...\n");
                return;
            default:
                printf("[Error] Opcion invalida.\n");
        } 
    } while(opcion != 0);
}
// Alta de un nuevo cliente con verificacion completa de DNI, datos personales y guardado binario
void registrarCliente(void){	
    cliente c;
    FILE *archivo;
    char buffer[100];
    char basura;
    printf("\n========================================\n");
    printf("           REGISTRAR CLIENTE            \n");
    printf("========================================\n");
    printf("DNI: ");
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') {
            printf("[Aviso] No ingreso ningun dato. Ingrese DNI: ");
            continue;
        }
        if (sscanf(buffer, " %d %c", &c.dni, &basura) == 1 && c.dni >= 10000000 && c.dni <= 99999999) {
            break;
        }
        printf("[Error] DNI invalido. Debe tener exactamente 8 caracteres numericos. Ingrese nuevamente: ");
    }   
    if (buscarCliente(c.dni) == 1) {
        printf("\n[Error] Ya existe un cliente registrado con ese DNI.\n");
        return;
    }
    printf("Apellido y Nombre: ");
    fgets(c.apellidoNombre, 60, stdin);
    c.apellidoNombre[strcspn(c.apellidoNombre, "\n")] = '\0';
    while(strcmp(c.apellidoNombre, "") == 0 || !esNombreValido(c.apellidoNombre)){
        printf("[Error] Nombre invalido (no puede estar vacio ni contener solo numeros). Ingrese nuevamente: ");
        fgets(c.apellidoNombre, 60, stdin);
        c.apellidoNombre[strcspn(c.apellidoNombre, "\n")] = '\0';
    }
    printf("Telefono (10 digitos sin ceros consecutivos): ");
    fgets(c.telefono, 20, stdin);
    c.telefono[strcspn(c.telefono, "\n")] = '\0';
    while(!esTelefonoValido(c.telefono)){
        printf("[Error] Telefono invalido. Debe tener estrictamente 10 numeros y no ser todo ceros. Ingrese nuevamente: ");
        fgets(c.telefono, 20, stdin);
        c.telefono[strcspn(c.telefono, "\n")] = '\0';
    }
    printf("Direccion (ej: San Martin 123): ");
    fgets(c.direccion, 100, stdin);
    c.direccion[strcspn(c.direccion, "\n")] = '\0';
    while(!esDireccionValida(c.direccion)){
        printf("[Error] Direccion invalida (debe contener obligatoriamente letras y numeros). Ingrese nuevamente: ");
        fgets(c.direccion, 100, stdin);
        c.direccion[strcspn(c.direccion, "\n")] = '\0';
    }
    printf("E-mail: ");
    fgets(c.email, 60, stdin);
    c.email[strcspn(c.email, "\n")] = '\0';
    while(!esEmailValido(c.email)){
        printf("[Error] E-mail invalido (formato correcto ej: usuario@dominio.com, sin espacios). Ingrese nuevamente: ");
        fgets(c.email, 60, stdin);
        c.email[strcspn(c.email, "\n")] = '\0';
    }	
    int diaAlta, mesAlta, anioAlta;
    char bufferFecha[100];
    do {
        printf("Fecha de alta (DD/MM/AAAA - Debe ser la fecha de hoy): ");
        if (fgets(bufferFecha, sizeof(bufferFecha), stdin) == NULL) {
            diaAlta = 0;
            continue;
        }
        bufferFecha[strcspn(bufferFecha, "\n")] = '\0';
        if (bufferFecha[0] == '\0') {
            printf("[Aviso] No ingreso ningun dato.\n");
            diaAlta = 0;
            continue;
        }
        if(sscanf(bufferFecha, "%d/%d/%d", &diaAlta, &mesAlta, &anioAlta) != 3){
            printf("[Error] Formato invalido. Use DD/MM/AAAA.\n");
            diaAlta = 0;
        } else if(!fechaValidaCliente(diaAlta, mesAlta, anioAlta)){
            printf("[Error] La fecha de alta debe ser estrictamente la fecha actual del sistema.\n");
            diaAlta = 0;
        }
    } while(diaAlta == 0);
    sprintf(c.fechaAlta, "%02d/%02d/%d", diaAlta, mesAlta, anioAlta);
    c.estado = 1;
    archivo = fopen("clientes.dat", "ab");
    if(archivo == NULL){
        printf("\n[Error] No se pudo abrir el archivo de clientes.\n");
        return;
    }
    if(fwrite(&c, sizeof(cliente), 1, archivo) == 1){
        printf("\n[Exito] Cliente registrado correctamente.\n");	
    } else {
        printf("\n[Error] No se pudo guardar el cliente en el archivo.\n");
    }
    fclose(archivo);
}
// Recorre clientes.dat buscando coincidencia por DNI. Retorna 1 si lo encuentra o 0 si no
int buscarCliente(int dni) {	
    cliente c;
    FILE *archivo = fopen("clientes.dat", "rb");
    if(archivo == NULL) {  	
        return 0;       
    }
    while(fread(&c, sizeof(cliente), 1, archivo) == 1) {    	
        if (c.dni == dni) {        	
            fclose(archivo);
            return 1;            
        }
    }
    fclose(archivo);
    return 0;
}
// Imprime la ficha del cliente correspondiente al DNI solicitado
void consultarCliente(void){
    cliente c;
    FILE *archivo;
    int dni;
    int encontrado = 0;
    char buffer[100];
    char basura;    
    printf("\n========================================\n");
    printf("             BUSCAR CLIENTE             \n");
    printf("========================================\n");
    while (1) {
        printf("Ingrese el DNI (exactamente 8 digitos, o ingrese 0 para volver): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') {
            printf("[Aviso] No ingreso ningun dato.\n");
            continue;
        }
        if (sscanf(buffer, " %d %c", &dni, &basura) != 1) {
            printf("[Error] Entrada invalida. Ingrese solo numeros.\n");
            continue;
        }
        if (dni == 0) {
            return;
        }
        if (dni < 10000000 || dni > 99999999) {
            printf("[Error] DNI invalido. Debe tener exactamente 8 digitos numericos.\n");
            continue;
        }
        archivo = fopen("clientes.dat", "rb");
        if(archivo == NULL){
            printf("[Error] No se pudo abrir el archivo de clientes o aun no hay registros.\n");
            return;
        }    
        encontrado = 0;
        while(fread(&c, sizeof(cliente), 1, archivo) == 1){
            if(c.dni == dni){
                encontrado = 1;
                break;
            }
        }    
        fclose(archivo);
        if(encontrado == 1){    
            printf("\n--- DATOS DEL CLIENTE ---\n");
            printf("DNI: %d\n", c.dni);
            printf("Apellido y Nombre: %s\n", c.apellidoNombre);
            printf("Telefono: %s\n", c.telefono);
            printf("E-mail: %s\n", c.email);
            printf("Direccion: %s\n", c.direccion);
            printf("Fecha de alta: %s\n", c.fechaAlta);        
            if(c.estado == 1){
                printf("Estado: Activo\n");
            } else {
                printf("Estado: Inactivo\n");
            }
            break;
        } else {
            printf("[Error] No se encontro un cliente con el DNI %d. Intente nuevamente.\n\n", dni);
        }
    }
}
// Muestra el padron completo de clientes almacenados en el archivo binario
void mostrarClientes(void){
    cliente c;
    FILE *archivo;
    int contador = 0;	
    printf("\n========================================\n");
    printf("          LISTADO DE CLIENTES           \n");
    printf("========================================\n");
    archivo = fopen("clientes.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        return;
    }	
    while(fread(&c, sizeof(cliente), 1, archivo) == 1){
        printf("DNI: %d\n", c.dni);
        printf("Apellido y Nombre: %s\n", c.apellidoNombre);
        printf("Telefono: %s\n", c.telefono);
        printf("E-mail: %s\n", c.email);
        printf("Direccion: %s\n", c.direccion);
        printf("Fecha de alta: %s\n", c.fechaAlta);		
        if(c.estado == 1){
            printf("Estado: Activo\n");
        } else {
            printf("Estado: Inactivo\n");
        }		
        printf("----------------------------------------\n");
        contador++;
    }	
    if(contador == 0){
        printf("[Aviso] No hay clientes registrados en el sistema.\n");
    } else {
        printf("Total de clientes mostrados: %d\n", contador);
    }
    fclose(archivo);    
}
// Permite actualizar campos individuales del cliente sobreescribiendo con fseek
void modificarCliente(void){    
    cliente c;
    FILE *archivo;
    int dni;
    int encontrado = 0;
    int opcion;
    long posicion;
    char buffer[100];
    char basura;    
    printf("\n========================================\n");
    printf("            MODIFICAR CLIENTE           \n");
    printf("========================================\n");
    archivo = fopen("clientes.dat", "rb");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de clientes o aun no hay registros.\n");
        return;
    }    
    int contadorClientes = 0;
    printf("\n--- CLIENTES REGISTRADOS (DNI) ---\n");
    while(fread(&c, sizeof(cliente), 1, archivo) == 1){
        printf("DNI: %d | Nombre: %s\n", c.dni, c.apellidoNombre);
        contadorClientes++;
    }
    fclose(archivo);
    if(contadorClientes == 0){
        printf("[Aviso] No hay clientes registrados en el sistema para modificar.\n");
        return;
    }
    printf("----------------------------------------\n");
    while(1) {
        printf("Ingrese el DNI del cliente (exactamente 8 digitos, o ingrese 0 para volver): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') {
            printf("[Aviso] No ingreso ningun dato.\n");
            continue;
        }
        if (sscanf(buffer, " %d %c", &dni, &basura) != 1) {
            printf("[Error] Entrada invalida. Ingrese solo numeros.\n");
            continue;
        }
        if (dni == 0) {
            return;
        }
        if (dni < 10000000 || dni > 99999999) {
            printf("[Error] DNI invalido. Debe tener exactamente 8 digitos numericos.\n");
            continue;
        }
        break;
    }    
    archivo = fopen("clientes.dat", "r+b");
    if(archivo == NULL){
        printf("[Error] No se pudo abrir el archivo de clientes.\n");
        return;
    }    
    while(encontrado == 0 && fread(&c, sizeof(cliente), 1, archivo) == 1){
        if(c.dni == dni){
            encontrado = 1;
            posicion = ftell(archivo) - sizeof(cliente);
        }
    }    
    if(encontrado == 0){
        printf("\n[Error] No se encontro un cliente con el DNI %d.\n", dni);
        fclose(archivo);
        return;
    }    
    printf("\n--- DATOS ACTUALES DEL CLIENTE ---\n");
    printf("DNI: %d\n", c.dni);
    printf("Nombre y Apellido: %s\n", c.apellidoNombre);
    printf("Telefono: %s\n", c.telefono);
    printf("E-mail: %s\n", c.email);
    printf("Direccion: %s\n", c.direccion);
    printf("Fecha de alta: %s\n", c.fechaAlta);
    if(c.estado == 1){
        printf("Estado: Activo\n");
    } else {
        printf("Estado: Inactivo\n");
    }
    do{
        printf("\n--- OPCIONES DE MODIFICACION ---\n");
        printf("1. Modificar nombre y apellido\n");
        printf("2. Modificar telefono\n");
        printf("3. Modificar direccion\n");
        printf("4. Modificar e-mail\n");
        printf("5. Modificar estado\n");
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
        switch(opcion){            
            case 1:
                printf("Ingrese el nuevo Nombre y Apellido: ");
                fgets(c.apellidoNombre, 60, stdin);
                c.apellidoNombre[strcspn(c.apellidoNombre, "\n")] = '\0';
                while(strcmp(c.apellidoNombre, "") == 0 || !esNombreValido(c.apellidoNombre)){
                    printf("[Error] Nombre invalido. Ingrese nuevamente: ");
                    fgets(c.apellidoNombre, 60, stdin);
                    c.apellidoNombre[strcspn(c.apellidoNombre, "\n")] = '\0';
                }
                break;            
            case 2:
                printf("Ingrese el nuevo Telefono: ");
                fgets(c.telefono, 20, stdin);
                c.telefono[strcspn(c.telefono, "\n")] = '\0';
                while(!esTelefonoValido(c.telefono)){
                    printf("[Error] Telefono invalido (debe tener 10 digitos). Ingrese nuevamente: ");
                    fgets(c.telefono, 20, stdin);
                    c.telefono[strcspn(c.telefono, "\n")] = '\0';
                }
                break;            
            case 3:
                printf("Ingrese la nueva Direccion: ");
                fgets(c.direccion, 100, stdin);
                c.direccion[strcspn(c.direccion, "\n")] = '\0';
                while(!esDireccionValida(c.direccion)){
                    printf("[Error] Direccion invalida (debe combinar letras y numeros). Ingrese nuevamente: ");
                    fgets(c.direccion, 100, stdin);
                    c.direccion[strcspn(c.direccion, "\n")] = '\0';
                }
                break;            
            case 4:
                printf("Ingrese el nuevo E-mail: ");
                fgets(c.email, 60, stdin);
                c.email[strcspn(c.email, "\n")] = '\0';
                while(!esEmailValido(c.email)){
                    printf("[Error] E-mail invalido. Ingrese nuevamente: ");
                    fgets(c.email, 60, stdin);
                    c.email[strcspn(c.email, "\n")] = '\0';
                }
                break;            
            case 5:
                while(1) {
                    printf("Estado (1 = Activo | 0 = Inactivo): ");
                    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                        buffer[strcspn(buffer, "\n")] = '\0';
                        if (sscanf(buffer, " %d %c", &c.estado, &basura) == 1 && (c.estado == 0 || c.estado == 1)) {
                            break;
                        }
                    }
                    printf("[Error] Estado invalido. Ingrese 1 (Activo) o 0 (Inactivo).\n");
                }
                break;            
            case 0:
                if(fseek(archivo, posicion, SEEK_SET) == 0){
                    if(fwrite(&c, sizeof(cliente), 1, archivo) == 1){
                        printf("\n[Exito] Cliente modificado correctamente.\n");
                    } else {
                        printf("\n[Error] No se pudieron guardar los cambios en el archivo.\n");
                    }
                } else {
                    printf("\n[Error] No se pudo reubicar el puntero en el archivo.\n");
                }
                break;            
            default:
                printf("[Error] Opcion invalida.\n");
        }            
    } while(opcion != 0);
    fclose(archivo);
}
