/* File: medicos.c */
//--- LIBRERIAS ---
#include "medicos.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static const char *nombreArchivo = "data/medicos.txt";


// Parser actualizado (7 campos)
static int parsearLinea(const char *linea, Medico *m) {
	return sscanf(linea, "%14[^|]|%49[^|]|%d|%10[^|]|%29[^|]|%14[^|]|%d",
				  m->codigo, m->nombre, &m->edad, m->fechaNacimiento,
				  m->especialidad, m->horario, &m->disponible) == 7;
}

int obtenerMedico(const char *codigo, Medico *out) {
	FILE *archivo = fopen(nombreArchivo, "r");
	if (!archivo) return 0;
	Medico m;
	char linea[256];
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if (parsearLinea(linea, &m)) {
			if (strcmp(m.codigo, codigo) == 0) {
				*out = m;
				fclose(archivo);
				return 1;
			}
		}
	}
	fclose(archivo);
	return 0;
}

int existeMedico(char codigo[]) {
	Medico temp;
	return obtenerMedico(codigo, &temp);
}

void registrarMedico(void) {
	Medico m;
	char buffer[100];
	
	printf("[!] ADVERTENCIA: Crear un médico por este medio evitará que el mismo pueda usar el sistema\n");
	pausa();
	limpiarPantalla();
	
	// Lista de especialidades definida
	const char *listaEspecialidades[] = {
		"Medicina Familiar y General", "Pediatria", "Medicina Interna",
			"Ginecologia y Obstetricia", "Cardiologia", "Dermatologia",
			"Neurologia", "Psiquiatria", "Cirugia General"
	};
	int totalEspecialidades = 9;
	
	m.disponible = 1; //Activo por defecto
	
	dibujarEncabezado("REGISTRO DE MEDICO (ADMIN)");
	
	// 1. CODIGO
	do {
		printf("Codigo/Cedula (10 digitos) [0 para Volver]: ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "0") == 0) return;
		
		if (!esSoloNumeros(buffer)) printf(" -> Error: Solo numeros.\n");
		else if (existeMedico(buffer)) printf(" -> Error: Codigo ya registrado.\n");
		else { strcpy(m.codigo, buffer); break; }
	} while (1);
	
	// 2. NOMBRE
	do {
		printf("Nombre Completo [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "0") == 0) return;
		if (esTextoConEspacios(buffer)) {
			strncpy(m.nombre, buffer, 49); m.nombre[49] = '\0'; break;
		}
		printf(" -> Error: Nombre invalido.\n");
	} while (1);
	
	// 3. EDAD
	do {
		printf("Edad [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "0") == 0) return;
		if (numVerify(buffer) == 1) { 
			int edadTemp = strtol(buffer, NULL, 10);
			if (edadTemp >= 24 && edadTemp <= 90) { m.edad = edadTemp; break; }
		}
		printf(" -> Error: Edad invalida.\n");
	} while (1);
	
	// 4. FECHA NACIMIENTO (CAMBIO AQUI: NUEVA FUNCION)
	do {
		printf("Fecha Nacimiento (DD-MM-YYYY) [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		// Usamos la nueva funcion especifica para nacimientos
		if (validarFechaNacimiento(buffer)) {
			strcpy(m.fechaNacimiento, buffer);
			break;
		}
		printf(" -> Error: Formato invalido (DD-MM-YYYY) o fecha imposible.\n");
	} while(1);
	
	// 5. ESPECIALIDAD
	int opcionEsp = 0;
	do {
		printf("\n   --- Seleccione Especialidad ---\n");
		for(int i = 0; i < totalEspecialidades; i++) printf("   [%d] %s\n", i + 1, listaEspecialidades[i]);
		printf("   [0] Cancelar\n   Opcion: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		if (numVerify(buffer) == 1) {
			opcionEsp = strtol(buffer, NULL, 10);
			if (opcionEsp == 0) return;
			if (opcionEsp >= 1 && opcionEsp <= totalEspecialidades) {
				strcpy(m.especialidad, listaEspecialidades[opcionEsp - 1]);
				break;
			}
		}
		printf("   -> Error: Opcion invalida.\n");
	} while (1);
	
	// 6. HORARIO
	do {
		printf("\nHorario (HH:MM-HH:MM) [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		if (strcmp(buffer, "0") == 0) return;
		if (esHorarioValido(buffer)) { strcpy(m.horario, buffer); break; }
		printf(" -> Error: Formato incorrecto.\n");
	} while (1);
	
	// GUARDAR
	FILE *archivo = fopen(nombreArchivo, "a");
	if (!archivo) archivo = fopen(nombreArchivo, "w");
	if (archivo) {
		fprintf(archivo, "%s|%s|%d|%s|%s|%s|%d\n",
				m.codigo, m.nombre, m.edad, m.fechaNacimiento, 
				m.especialidad, m.horario, m.disponible);
		fclose(archivo);
		printf("\n\t[OK] Medico registrado correctamente.\n");
	}
}

void listarMedicos(void) {
	FILE *archivo = fopen(nombreArchivo, "r");
	if (!archivo) { printf("\n\t[!] No hay medicos.\n"); return; }
	Medico m;
	char linea[256];
	int contador = 0;
	char *margen = " ";
	
	printf("\n%s%-4s %-10s %-20s %-4s %-10s %-28s %-12s %-6s\n",
		   margen, "#", "CODIGO", "NOMBRE", "EDAD", "NACIM.", "ESPECIALIDAD", "HORARIO", "ESTADO");
	for(int i=0; i<118; i++) printf("-");
	printf("\n");
	
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if (parsearLinea(linea, &m)) {
			contador++;
			char nombreCorto[21]; strncpy(nombreCorto, m.nombre, 20); nombreCorto[20]='\0';
			printf("%s%-4d %-10s %-20s %-4d %-10s %-28s %-12s %-6s\n",
				   margen, contador, m.codigo, nombreCorto, m.edad, m.fechaNacimiento,
				   m.especialidad, m.horario, (m.disponible ? "Disp." : "No"));
		}
	}
	printf("\n");
	fclose(archivo);
}

void menuMedicos(void) {
	char buffer[20];
	int opcion;
	do {
		dibujarEncabezado("GESTION DE MEDICOS");
		printf("\n\t[1] Registrar nuevo medico");
		printf("\n\t[2] Listar medicos");
		printf("\n\t[0] Volver al menu principal");
		printf("\n\n\tIngrese opcion: ");
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		if (numVerify(buffer) == 1) {
			opcion = strtol(buffer, NULL, 10);
			switch (opcion) {
			case 1: dibujarEncabezado("REGISTRO DE MEDICO"); registrarMedico(); pausa(); break;
			case 2: dibujarEncabezado("LISTA DE MEDICOS"); listarMedicos(); pausa(); break;
			case 0: break;
			default: printf("\n\t[!] Opcion no reconocida.\n"); pausa();
			}
		}
	} while (opcion != 0);
}


void bajaPropiaMedico(const char *codigoMedico) {
	char confirmacion[10];
	printf("\n   [ATENCION] Esta a punto de darse de baja (No Disponible).");
	printf("\n   Ya no aparecera en las listas para nuevas citas.");
	printf("\n   Desea continuar? (si/no): ");
	
	fgets(confirmacion, sizeof(confirmacion), stdin);
	confirmacion[strcspn(confirmacion, "\n")] = '\0';
	
	if (strcmp(confirmacion, "si") != 0 && strcmp(confirmacion, "SI") != 0) {
		printf("   -> Operacion cancelada.\n");
		return;
	}
	
	FILE *archivo = fopen("data/medicos.txt", "r+");
	if (!archivo) {
		printf("\n   [!] Error al abrir la base de datos de medicos.\n");
		return;
	}
	
	Medico m;
	char linea[256];
	long pos;
	int encontrado = 0; 
	
	while (1) {
		pos = ftell(archivo);
		if (!fgets(linea, sizeof(linea), archivo)) break;
		
		char lineaCopia[256];
		strcpy(lineaCopia, linea);
		
		if (parsearLinea(lineaCopia, &m)) {
			if (strcmp(m.codigo, codigoMedico) == 0) {
				encontrado = 1; 
				m.disponible = 0; // BAJA
				
				fseek(archivo, pos, SEEK_SET);
				fprintf(archivo, "%s|%s|%d|%s|%s|%s|%d\n",
						m.codigo, m.nombre, m.edad, m.fechaNacimiento, 
						m.especialidad, m.horario, m.disponible);
				
				printf("\n   [OK] Su estado ahora es: NO DISPONIBLE.\n");
				break;
			}
		}
	}
	
	if (!encontrado) {
		printf("\n   [!] Error: No se encontro su registro de medico en el archivo.\n");
	}
	
	fclose(archivo);
}
