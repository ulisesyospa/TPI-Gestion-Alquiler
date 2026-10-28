#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "clientes.h"
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
    if (soloCeros) 
	return 0;
    return 1;
}
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
int fechaValidaCliente(int dia, int mes, int anio){
    struct tm fecha = {0};
    fecha.tm_mday = dia;
    fecha.tm_mon = mes - 1;
    fecha.tm_year = anio - 1900;
    struct tm copia = fecha;
    time_t controlFecha = mktime(&copia);
    if(controlFecha == -1 ||
       copia.tm_mday != dia ||
       copia.tm_mon != mes - 1 ||
       copia.tm_year != anio - 1900){
        return 0;
    }
    time_t tActual = time(NULL);
    if (controlFecha > tActual) {
        return 0;
    }
    return 1;
}
void menuClientes(void) { 
    int opcion; 
	do {
	     printf("\n=== MENU CLIENTES ===\n"); 
	     printf("1. Registrar cliente\n"); 
	     printf("2. Buscar cliente\n"); 
	     printf("3. Mostrar clientes\n"); 
	     printf("4. Modificar cliente\n"); 
	     printf("0. Volver al menu principal\n"); 
	     printf("Seleccione una opcion: "); 
	     scanf("%d", &opcion); 
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
	           return;
	           break;	 
	           default: printf("Opcion incorrecta\n");
	        } 
       }while(opcion != 0);
}
void registrarCliente(void){	
	int resultado;
	int caracter;	
	cliente c;
	FILE *archivo;
	printf("\n--- REGISTRAR CLIENTE ---\n");	
	printf("DNI: ");
	while (scanf("%d", &c.dni) != 1 || c.dni < 10000000 || c.dni > 99999999) {
        printf("DNI invalido. Debe tener exactamente 8 caracteres (numeros). Ingrese nuevamente: ");
        fflush(stdin);
    }
    fflush(stdin);   
	if (buscarCliente(c.dni) == 1) {
        printf("\nYa existe un cliente con ese DNI.\n");
        return;
    }
	printf("Apellido y Nombre: ");
	fgets(c.apellidoNombre, 60, stdin);
	c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	while(strcmp(c.apellidoNombre, "") == 0 || !esNombreValido(c.apellidoNombre)){
		printf("Nombre invalido (no puede estar vacio ni contener solo numeros). Ingrese nuevamente: ");
	    fgets(c.apellidoNombre, 60, stdin);
	    c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	}
	printf("Telefono (10 digitos sin ceros consecutivos): ");
	fgets(c.telefono, 20, stdin);
	c.telefono[strcspn(c.telefono, "\n")]='\0';
	while(!esTelefonoValido(c.telefono)){
		printf("Telefono invalido. Debe tener estrictamente 10 numeros y no ser todo ceros. Ingrese nuevamente: ");
	    fgets(c.telefono, 20, stdin);
	    c.telefono[strcspn(c.telefono, "\n")]='\0';
	}
	printf("Direccion (ej: San Martin 123): ");
	fgets(c.direccion, 100, stdin);
	c.direccion[strcspn(c.direccion, "\n")]='\0';
	while(!esDireccionValida(c.direccion)){
		printf("Direccion invalida (debe contener obligatoriamente letras y numeros, ej: Rivadavia 450). Ingrese nuevamente: ");
	    fgets(c.direccion, 100, stdin);
	    c.direccion[strcspn(c.direccion, "\n")]='\0';
	}
	printf("E-mail: ");
	fgets(c.email, 60, stdin);
	c.email[strcspn(c.email, "\n")]='\0';
	while(!esEmailValido(c.email)){
		printf("E-mail invalido (formato correcto ej: usuario@dominio.com, sin espacios). Ingrese nuevamente: ");
	    fgets(c.email, 60, stdin);
	    c.email[strcspn(c.email, "\n")]='\0';
	}	
	int diaAlta, mesAlta, anioAlta;
	do {
		printf("Fecha de alta (DD/MM/AAAA): ");
		if(scanf("%d/%d/%d", &diaAlta, &mesAlta, &anioAlta) != 3){
			printf("Formato invalido. Use DD/MM/AAAA.\n");
			fflush(stdin);
			diaAlta = 0;
		} else if(!fechaValidaCliente(diaAlta, mesAlta, anioAlta)){
			printf("La fecha no existe, es invalida o es posterior a la fecha actual.\n");
			diaAlta = 0;
		}
	} while(diaAlta == 0);
	fflush(stdin);
	sprintf(c.fechaAlta, "%02d/%02d/%d", diaAlta, mesAlta, anioAlta);
	c.estado=1;
	archivo= fopen("clientes.dat", "ab");
	if(archivo==NULL){
		printf("\nError: el archivo no pudo ser abierto.");
		return;
	}
	if(fwrite(&c, sizeof(cliente), 1, archivo) ==1 ){
		printf("\nCliente registrado correctamente.");	
	}else 
	    printf("\nError: no se pudo guardar el cliente.\n");
	fclose(archivo);
}
int buscarCliente(int dni) {	
    cliente c;
    FILE *archivo;
    archivo= fopen("clientes.dat", "rb");
    if(archivo == NULL) {  	
        return 0;       
    }
    while(fread(&c, sizeof(cliente), 1, archivo) == 1) {    	
        if (c.dni==dni) {        	
            fclose(archivo);
            return 1;            
        }
    }
    fclose(archivo);
    return 0;
}
void consultarCliente(void){
	cliente c;
	FILE *archivo;
	int dni;
	int encontrado=0;	
	printf("\n--- BUSCAR CLIENTE ---\n");
	printf("Ingrese del DNI: ");
	while (scanf("%d", &dni) != 1 || dni <= 0) {
        printf("DNI invalido. Ingrese un numero mayor que cero: ");
        fflush(stdin);
    }
    fflush(stdin);
    archivo= fopen("clientes.dat", "rb");
    if(archivo==NULL){
    	printf("No se pudo abrir el archivo de clientes.\n");
    	return;
	}	
	while(encontrado == 0 && fread(&c, sizeof(cliente), 1, archivo) == 1){
        if(c.dni == dni){
            encontrado = 1;
		}
    }	
	if(encontrado==1){	
		printf("\n---DATOS DEL CLIENTE---\n");
		printf("DNI: %d\n", c.dni);
		printf("Apellido y Nombre: %s\n", c.apellidoNombre);
		printf("Telefono: %s\n", c.telefono);
		printf("E-mail: %s\n", c.email);
		printf("Direccion: %s\n", c.direccion);
		printf("Fecha de alta: %s\n", c.fechaAlta);		
		if(c.estado==1){
			printf("Estado: Activo\n");
		}else{
			printf("Estado: Inactivo\n");
		}
	}else 
	    printf("\nNo se encontro un cliente con ese DNI.\n");		
	fclose(archivo);
}
void mostrarClientes(void){
	cliente c;
	FILE *archivo;
	int contador=0;	
	printf("\n--- LISTADO DE CLIENTES ---\n");	
	archivo= fopen("clientes.dat", "rb");
	if(archivo==NULL){
		printf("No se pudo abrir el archivo de clientes.\n");
		return;
	}	
	while(fread(&c, sizeof(cliente), 1, archivo) == 1){
		printf("DNI: %d\n", c.dni);
		printf("Apellido y Nombre: %s\n", c.apellidoNombre);
		printf("Telefono: %s\n", c.telefono);
		printf("E-mail: %s\n", c.email);
		printf("Direccion: %s\n", c.direccion);
		printf("Fecha de alta: %s\n", c.fechaAlta);		
		if(c.estado==1){
			printf("Estado: Activo\n");
		}else{
			printf("Estado: Inactivo\n");
		}		
		printf("-----------------------\n");
		contador++;
	}	
	if(contador==0){
		printf("No hay clientes registrados.\n");
	}else 
	    printf("Total de clientes: %d\n", contador);	
	fclose(archivo);    
}
void modificarCliente(void){	
	cliente c;
	FILE *archivo;
	int dni;
	int encontrado=0;
	int opcion;
	long posicion;
	int resultado;
	int caracter;	
	printf("\n--- MODIFICAR CLIENTE ---\n");
	printf("Ingrese el DNI del cliente: ");	
	while(scanf("%d", &dni) !=1 || dni <=0 ){
		printf("DNI invalido. Ingrese un numero mayor que cero: ");
		fflush(stdin);
	}
	fflush(stdin);	
	archivo= fopen("clientes.dat", "r+b");
	if(archivo==NULL){
		printf("No se pudo abrir el archivo de cliente.\n");
		return;
	}	
	while(encontrado==0 && fread(&c, sizeof(cliente), 1, archivo) == 1){
		if(c.dni==dni){
			encontrado= 1;
			posicion= ftell(archivo) - sizeof(cliente);
		}
	}	
	if(encontrado==0){
		printf("\nNo se encontro un cliente con ese DNI.\n");
		fclose(archivo);
		return;
	}	
	printf("\n--- DATOS DEL CLIENTE ---\n");
	printf("DNI: %d\n", c.dni);
	printf("Nombre y Apellido: %s\n", c.apellidoNombre);
	printf("Telefono: %s\n", c.telefono);
	printf("E-mail: %s\n", c.email);
	printf("Direccion: %s\n", c.direccion);
	printf("Fecha de alta: %s\n", c.fechaAlta);
	if(c.estado==1){
		printf("Estado: Activo");
	}else 
	    printf("Estado: Inactivo");	
	do{
		printf("\n--- MODIFICAR CLIENTE ---\n");
		printf("1. Modificar nombre y apellidos\n");
		printf("2. Modificar Telefono\n");
		printf("3. Modificar Direccion\n");
		printf("4. Modificar E-mail\n");
		printf("5. Modificar Estado\n");
		printf("0. Guardar y volver\n");
		printf("Seleccione una opcion: ");
		scanf("%d", &opcion);
		switch(opcion){			
			case 1:
				getchar();
				printf("Ingrese el nuevo Nombre y Apellido: ");
				fgets(c.apellidoNombre, 60, stdin);
	            c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	            while(strcmp(c.apellidoNombre, "") == 0 || !esNombreValido(c.apellidoNombre)){
		            printf("Nombre invalido (no puede estar vacio ni contener solo numeros). Ingrese nuevamente: ");
	                fgets(c.apellidoNombre, 60, stdin);
	                c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	            }
			break;			
			case 2:
				getchar(); 
				printf("Ingrese el nuevo Telefono: ");
				fgets(c.telefono, 20, stdin);
	            c.telefono[strcspn(c.telefono, "\n")]='\0';
	            while(!esTelefonoValido(c.telefono)){
		            printf("Telefono invalido. Debe tener estrictamente 10 numeros y no ser todo ceros. Ingrese nuevamente: ");
	                fgets(c.telefono, 20, stdin);
	                c.telefono[strcspn(c.telefono, "\n")]='\0';
	            }
			break;			
			case 3:
				getchar();
				printf("Ingrese la nueva Direccion: ");
	            fgets(c.direccion, 100, stdin);
	            c.direccion[strcspn(c.direccion, "\n")]='\0';
	            while(!esDireccionValida(c.direccion)){
		            printf("Direccion invalida (debe contener obligatoriamente letras y numeros). Ingrese nuevamente: ");
	                fgets(c.direccion, 100, stdin);
	                c.direccion[strcspn(c.direccion, "\n")]='\0';
	            }
			break;			
			case 4:
				getchar();
				printf("Ingrese el nuevo E-mail: ");
	            fgets(c.email, 60, stdin);
	            c.email[strcspn(c.email, "\n")]='\0';
	            while(!esEmailValido(c.email)){
		            printf("E-mail invalido (formato correcto ej: usuario@dominio.com, sin espacios). Ingrese nuevamente: ");
	                fgets(c.email, 60, stdin);
	                c.email[strcspn(c.email, "\n")]='\0';
	            }
			break;			
			case 5:
				printf("Estado (1=Activo, 0=Inactivo): ");
	            resultado=scanf("%d", &c.estado);
	            caracter=getchar();
	            while (resultado!=1 || (c.estado!=0 && c.estado!=1) || (caracter!='\n' && caracter!=EOF)) {
                    fflush(stdin);
		            printf("Estado invalido. Ingrese 1(Activo) o 0(Inactivo): ");
                    resultado=scanf("%d", &c.estado);
	                caracter=getchar();
                }
                fflush(stdin);
			break;			
			case 0:
				if(fseek(archivo, posicion, SEEK_SET) == 0){
					if(fwrite(&c, sizeof(cliente), 1, archivo) == 1){
						printf("Cliente modificado correctamente.\n");
					}else {
						printf("Error al guardar los cambios.\n");
					}
				}else {
					printf("Error al posicionarse en el archivo.\n");
				}
			break;			
			default:
				printf("Opcion invalida.\n");
		}			
	}while(opcion != 0);
	fclose(archivo);
}
