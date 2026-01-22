/* File: pacientes.c */
//--- LIBRERIAS ---
#include "pacientes.h"
#include "utils.h"
#include "historial.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- ARCHIVOS ---
static const char *nombreArchivo = "data/pacientes.txt";


// Formato: cedula|nombre|edad|fechaNac|telefono|correo|activo
static int parsearLineaPaciente(const char *linea, Paciente *p) {
	return sscanf(linea, "%12[^|]|%49[^|]|%d|%10[^|]|%19[^|]|%49[^|]|%d",
				  p->cedula, p->nombre, &p->edad, p->fechaNacimiento, 
				  p->telefono, p->correo, &p->activo) == 7;
}


// --- Funciones Públicas ---
int obtenerPaciente(const char *cedula, Paciente *out) {
	FILE *archivo = fopen(nombreArchivo, "r");
	if (!archivo) return 0;
	
	Paciente p;
	char linea[256];
	
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		
		if (parsearLineaPaciente(linea, &p)) {
			if (strcmp(p.cedula, cedula) == 0) {
				*out = p;
				fclose(archivo);
				return 1;
			}
		}
	}
	fclose(archivo);
	return 0;
}

int existePaciente(char cedula[]) {
	Paciente temp;
	return obtenerPaciente(cedula, &temp);
}

void registrarPaciente(void) {
	Paciente p;
	char buffer[100];
	
	printf("[!] ADVERTENCIA: Crear un paciente por este medio evitará que el mismo pueda usar el sistema\n");
	pausa();
	limpiarPantalla();
	
	p.activo = 1; // Por defecto activo
	
	dibujarEncabezado("REGISTRO DE PACIENTE");
	
	// 1. CEDULA
	do {
		printf("Cedula (10 digitos) [0 para Volver]: ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return; 
		
		if (!esSoloNumeros(buffer) || strlen(buffer) != 10) {
			printf(" -> Error: Cedula invalida (debe tener 10 numeros).\n");
		} else if (existePaciente(buffer)) {
			printf(" -> Error: Paciente ya registrado.\n");
		} else {
			strcpy(p.cedula, buffer);
			break;
		}
	} while(1);
	
	// 2. NOMBRE
	do {
		printf("Nombre completo [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return; 
		
		if (esTextoConEspacios(buffer)) { 
			strncpy(p.nombre, buffer, 49);
			p.nombre[49] = '\0';
			break;
		}
		printf(" -> Error: El nombre solo puede contener letras y espacios.\n");
	} while(1);
	
	// 3. EDAD
	do {
		printf("Edad [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (numVerify(buffer) == 1) { 
			int edadTemp = strtol(buffer, NULL, 10);
			if (edadTemp > 0 && edadTemp < 120) {
				p.edad = edadTemp;
				break; 
			}
		}
		printf(" -> Error: Ingrese una edad valida (1-120).\n");
	} while (1);
	
	// 4. FECHA DE NACIMIENTO
	do {
		printf("Fecha de Nacimiento (DD-MM-YYYY) [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (validarFechaNacimiento(buffer)) {
			strcpy(p.fechaNacimiento, buffer);
			break;
		}
		printf(" -> Error: Formato invalido (use DD-MM-YYYY) o fecha imposible.\n");
	} while(1);
	
	// 5. TELEFONO
	do {
		printf("Telefono (10 digitos) [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (esSoloNumeros(buffer) && strlen(buffer) == 10) {
			strcpy(p.telefono, buffer);
			break;
		}
		printf(" -> Error: Telefono invalido.\n");
	} while(1);
	
	// 6. CORREO
	do {
		printf("Correo electronico [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (strlen(buffer) > 5 && strchr(buffer, '@') && strchr(buffer, '.')) {
			strncpy(p.correo, buffer, 49);
			p.correo[49] = '\0';
			break;
		}
		printf(" -> Error: Correo invalido.\n");
	} while(1);
	
	// GUARDAR
	FILE *archivo = fopen(nombreArchivo, "a");
	if (!archivo) archivo = fopen(nombreArchivo, "w");
	
	if (archivo) {
		// Guardamos los 7 campos
		fprintf(archivo, "%s|%s|%d|%s|%s|%s|%d\n",
				p.cedula, p.nombre, p.edad, p.fechaNacimiento, 
				p.telefono, p.correo, p.activo);
		fclose(archivo);
		printf("\n\t[OK] Paciente registrado exitosamente.\n");
	} else {
		printf("\n\t[X] Error al guardar en archivo.\n");
	}
}

int listarPaciente(void) {
	FILE *archivo = fopen(nombreArchivo, "r");
	if (!archivo) {
		printf("\n\t[!] Base de datos vacia o no encontrada.\n");
		return 0;
	}
	
	Paciente p;
	char linea[256];
	int contador = 0;
	char *margen = " "; 
	
	// FORMATO DE TABLA
	printf("\n%s%-4s %-11s %-25s %-4s %-10s %-11s %-25s %-7s\n", 
		   margen, "#", "CEDULA", "NOMBRE", "EDAD", "NACIM.", "TELEFONO", "CORREO", "ESTADO");
	
	for(int i=0; i<118; i++) printf("-");
	printf("\n");
	
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		
		if (parsearLineaPaciente(linea, &p)) {
			contador++;
			printf("%s%-4d %-11s %-25s %-4d %-10s %-11s %-25s %-7s\n",
				   margen, contador, p.cedula, p.nombre, p.edad, p.fechaNacimiento,
				   p.telefono, p.correo, (p.activo ? "Activo" : "Baja"));
		}
	}
	
	if (contador == 0) {
		printf("%s| %-100s |\n", margen, "NO HAY PACIENTES REGISTRADOS");
	}
	
	printf("\n");
	fclose(archivo);
	return 1;
}

void cambiarEstadoPaciente(void) {
	char buffer[100];
	char cedulaBuscada[15];
	
	// 1. Pedir Cédula
	do {
		printf("Ingrese Cedula del paciente (0 para Volver): ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (esSoloNumeros(buffer)) {
			strcpy(cedulaBuscada, buffer);
			break;
		}
		printf(" -> Error: Cedula invalida.\n");
	} while(1);
	
	// 2. Buscar y Modificar
	FILE *archivo = fopen(nombreArchivo, "r+");
	if (!archivo) {
		printf("\n\t[!] No se encontro la base de datos.\n");
		return;
	}
	
	Paciente p;
	char linea[256];
	long pos;
	int encontrado = 0;
	
	while(1) {
		pos = ftell(archivo);
		if(!fgets(linea, sizeof(linea), archivo)) break;
		
		char lineaLimpia[256];
		strcpy(lineaLimpia, linea);
		lineaLimpia[strcspn(lineaLimpia, "\n")] = '\0';
		
		if (parsearLineaPaciente(lineaLimpia, &p)) {
			if (strcmp(p.cedula, cedulaBuscada) == 0) {
				encontrado = 1;
				p.activo = !p.activo; // Cambiar estado
				
				fseek(archivo, pos, SEEK_SET);
				
				// IMPORTANTE: Sobrescribir con TODOS los campos nuevos
				fprintf(archivo, "%s|%s|%d|%s|%s|%s|%d\n",
						p.cedula, p.nombre, p.edad, p.fechaNacimiento, 
						p.telefono, p.correo, p.activo);
				
				printf("\n\t[OK] Estado del paciente '%s' cambiado a: %s\n", 
					   p.nombre, p.activo ? "ACTIVO" : "INACTIVO (Baja)");
				break;
			}
		}
	}
	
	if (!encontrado) {
		printf("\n\t[!] Paciente no encontrado.\n");
	}
	
	fclose(archivo);
}

void menuPacientes(int rolUsuario, char *idUsuario) {
	char buffer[10];
	int opcion;
	
	do {
		dibujarEncabezado("GESTION DE PACIENTES");
		printf("\n\t[1] Registrar nuevo paciente");
		printf("\n\t[2] Listar pacientes");
		
		// El ADMIN (Rol 3) puede dar de baja
		if (rolUsuario == 3) {
			printf("\n\t[3] Dar de Baja / Reactivar paciente");
		}
		
		// EL MEDICO (Rol 1) puede ver Historial
		if (rolUsuario == 1) {
			printf("\n\t[4] Crear/Editar Historial Clinico");
		}
		
		printf("\n\t[0] Volver al menu principal");
		printf("\n\n\tIngrese opcion: ");
		
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1) {
			opcion = strtol(buffer, NULL, 10);
			
			switch (opcion) {
			case 1: 
				dibujarEncabezado("REGISTRO DE PACIENTE");
				registrarPaciente(); 
				pausa();
				break;
			case 2: 
				dibujarEncabezado("LISTA DE PACIENTES");
				listarPaciente(); 
				pausa();
				break;
			case 3:
				// Solo Admin entra aquí
				if (rolUsuario == 3) {
					dibujarEncabezado("MODIFICAR ESTADO PACIENTE");
					printf("Funcion reservada para Admin (use logica anterior o hagala publica)\n"); 
				} else {
					printf("\n\tOpcion no valida para su rol.\n");
				}
				pausa();
				break;
			case 4:
				// Solo Médico entra aquí
				if (rolUsuario == 1) {
					gestionarHistorial(idUsuario); // Llamamos al módulo historial
				} else {
					printf("\n\tOpcion no valida para su rol.\n");
					pausa();
				}
				break;
			case 0: break;
			default: printf("\n\tOpcion no reconocida.\n"); pausa();
			}
		} else {
			printf("\n\tOpcion invalida.\n");
			pausa();
			opcion = -1;
		}
	} while (opcion != 0);
}

void bajaPropiaPaciente(const char *cedulaPaciente) {
	char confirmacion[10];
	printf("\n   [ATENCION] Esta a punto de darse de baja.");
	printf("\n   Su cuenta quedara inactiva.");
	printf("\n   Desea continuar? (si/no): ");
	
	fgets(confirmacion, sizeof(confirmacion), stdin);
	confirmacion[strcspn(confirmacion, "\n")] = '\0';
	
	if (strcmp(confirmacion, "si") != 0 && strcmp(confirmacion, "SI") != 0) {
		printf("   -> Operacion cancelada.\n");
		return;
	}
	
	FILE *archivo = fopen("data/pacientes.txt", "r+");
	if (!archivo) {
		printf("\n   [!] Error al abrir la base de datos de pacientes.\n");
		return;
	}
	
	Paciente p;
	char linea[256];
	long pos;
	int encontrado = 0;
	
	while (1) {
		pos = ftell(archivo);
		if (!fgets(linea, sizeof(linea), archivo)) break;
		
		char lineaCopia[256];
		strcpy(lineaCopia, linea);
		
		// Usamos el parser interno de pacientes.c
		if (parsearLineaPaciente(lineaCopia, &p)) {
			if (strcmp(p.cedula, cedulaPaciente) == 0) {
				encontrado = 1;
				p.activo = 0; // BAJA
				
				fseek(archivo, pos, SEEK_SET);
				// IMPORTANTE
				fprintf(archivo, "%s|%s|%d|%s|%s|%s|%d\n",
						p.cedula, p.nombre, p.edad, p.fechaNacimiento, 
						p.telefono, p.correo, p.activo);
				
				printf("\n   [OK] Su cuenta ha sido desactivada.\n");
				break;
			}
		}
	}
	
	if (!encontrado) {
		printf("\n   [!] Error: No se encontro su registro en el archivo.\n");
	}
	
	fclose(archivo);
}
