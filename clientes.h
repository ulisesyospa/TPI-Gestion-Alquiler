typedef struct {
	
    int dni;
    char apellidoNombre[60];
    char telefono[20];
    char email[60];
    char direccion[100];
    char fechaAlta[11];
    int estado;
    
} cliente;

void menuClientes(void);
void registrarCliente(void);
int buscarCliente(int dni);
void consultarCliente(void);
void mostrarClientes(void);
void modificarCliente(void);
