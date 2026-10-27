#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "clientes.h"
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
	while(strcmp(c.apellidoNombre, "")==0){
		printf("El apellido y nombre no pueden ser vacios. Ingrese nuevamente: ");
	    fgets(c.apellidoNombre, 60, stdin);
	    c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	}
	printf("Telefono: ");
	fgets(c.telefono, 20, stdin);
	c.telefono[strcspn(c.telefono, "\n")]='\0';
	while(strcmp(c.telefono, "")==0){
		printf("El telefono no puede ser vacio. Ingrese nuevamente: ");
	    fgets(c.telefono, 20, stdin);
	    c.telefono[strcspn(c.telefono, "\n")]='\0';
	}
	printf("Direccion: ");
	fgets(c.direccion, 100, stdin);
	c.direccion[strcspn(c.direccion, "\n")]='\0';
	while(strcmp(c.direccion, "")==0){
		printf("La direccion no puede ser vacio. Ingrese nuevamente: ");
	    fgets(c.direccion, 100, stdin);
	    c.direccion[strcspn(c.direccion, "\n")]='\0';
	}
	printf("E-mail: ");
	fgets(c.email, 60, stdin);
	c.email[strcspn(c.email, "\n")]='\0';
	while(strcmp(c.email, "")==0 || strchr(c.email, '@') == NULL){
		printf("E-mail invalido. Ingrese nuevamente: ");
	    fgets(c.email, 60, stdin);
	    c.email[strcspn(c.email, "\n")]='\0';
	}	
	printf("Fecha de alta (DD/MM/AAAA): ");
	fgets(c.fechaAlta, 11, stdin);
	c.fechaAlta[strcspn(c.fechaAlta, "\n")]='\0';
	while(strlen(c.fechaAlta)!=10 || c.fechaAlta[2]!='/' || c.fechaAlta[5]!='/'){
		printf("Formato invalido. Ingrese DD/MM/AAAA: ");
		fgets(c.fechaAlta, 11, stdin);
		c.fechaAlta[strcspn(c.fechaAlta, "\n")]='\0';	
	}
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
		printf("2. Modificar Telefino\n");
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
	            while(strcmp(c.apellidoNombre, "")==0){
		            printf("El apellido y nombre no pueden ser vacios. Ingrese nuevamente: ");
	                fgets(c.apellidoNombre, 60, stdin);
	                c.apellidoNombre[strcspn(c.apellidoNombre, "\n")]='\0';
	            }
			break;			
			case 2:
				getchar(); 
				printf("Ingrese el nuevo Telefono: ");
				fgets(c.telefono, 20, stdin);
	            c.telefono[strcspn(c.telefono, "\n")]='\0';
	            while(strcmp(c.telefono, "")==0){
		           printf("El telefono no puede ser vacio. Ingrese nuevamente: ");
	               fgets(c.telefono, 20, stdin);
	               c.telefono[strcspn(c.telefono, "\n")]='\0';
	            }
			break;			
			case 3:
				getchar();
				printf("Ingrese la nueva Direccion: ");
	            fgets(c.direccion, 100, stdin);
	            c.direccion[strcspn(c.direccion, "\n")]='\0';
	            while(strcmp(c.direccion, "")==0){
		           printf("La direccion no puede ser vacio. Ingrese nuevamente: ");
	               fgets(c.direccion, 100, stdin);
	               c.direccion[strcspn(c.direccion, "\n")]='\0';
	            }
			break;			
			case 4:
				getchar();
				printf("Ingrese el nuevo E-mail: ");
	            fgets(c.email, 60, stdin);
	            c.email[strcspn(c.email, "\n")]='\0';
	            while(strcmp(c.email, "")==0 || strchr(c.email, '@') == NULL){
		           printf("E-mail invalido. Ingrese nuevamente: ");
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
