#pragma once
typedef struct {
    char patente[10];
    char marca[30];
    char modelo[30];
    int anio;
    char tipo[30];
    int capacidad;
    float precioPorDia;
    int estado; // 1- Disponible, 2- Alquilado, 3- En mantenimiento, 0- Baja
} vehiculo;
void menuVehiculos(void);
void registrarVehiculo(void);
int buscarVehiculo(char patente[]);
int mostrarVehiculo(void);
int modificarVehiculo(void);
int esSoloTextoValido(const char *cadena);
int esCadenaVaciaOEspacios(const char *cadena);
int leerEnteroEnRango(int min, int max, const char *mensajeError);
