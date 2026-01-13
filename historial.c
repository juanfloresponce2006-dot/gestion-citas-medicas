/* File: historial.c */
//--- LIBRERIAS ---
#include "historial.h"
#include "pacientes.h" 
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static const char *fileHist = "data/historial.txt";


// --- FUNCIÓN AUXILIAR PARA IMPRIMIR TEXTO ALINEADO ---
static void imprimirFilaHistorial(const char *fecha, const char *medico, const char *descripcion) {
	const int ANCHO_MAX_DESC = 80; 
	const char *INDENTACION = "   " "            " "               "; // Espacios para alinear
	
	size_t len = strlen(descripcion);
	size_t pos = 0;
	int primeraLinea = 1;
	
	while (pos < len) {
		if (primeraLinea) {
			printf("   %-12s %-15s ", fecha, medico);
			primeraLinea = 0;
		} else {
			printf("\n%s ", INDENTACION);
		}
		
		int charsAImprimir = ANCHO_MAX_DESC;
		
		if (pos + charsAImprimir > len) {
			charsAImprimir = len - pos;
		} else {
			int corteEncontrado = 0;
			// Buscamos espacio hacia atrás para no cortar palabras
			for (int i = charsAImprimir; i > 0; i--) {
				if (descripcion[pos + i] == ' ') {
					charsAImprimir = i;
					corteEncontrado = 1;
					break;
				}
			}
			if (!corteEncontrado) charsAImprimir = ANCHO_MAX_DESC;
		}
		
		printf("%.*s", charsAImprimir, descripcion + pos);
		pos += charsAImprimir;
		
		// Saltar espacios extra
		while (pos < len && descripcion[pos] == ' ') {
			pos++;
		}
	}
	printf("\n"); 
}

static void guardarEntradaHistorial(Historial h) {
	FILE *fp = fopen(fileHist, "a");
	if (!fp) fp = fopen(fileHist, "w");
	if (fp) {
		fprintf(fp, "%d|%s|%s|%s|%s\n", 
				h.id, h.cedulaPaciente, h.codigoMedico, h.fecha, h.descripcion);
		fclose(fp);
	}
}

static void verHistorialPaciente(const char *cedulaPac) {
	FILE *fp = fopen(fileHist, "r");
	if (!fp) {
		printf("\n   [i] Este paciente aun no tiene historial registrado.\n");
		return;
	}
	
	Historial h;
	// BÚFER GIGANTE: Debe ser mayor que MAX_DESC_HISTORIAL para contener toda la línea
	char linea[MAX_DESC_HISTORIAL + 100]; 
	int encontrado = 0;
	char *margen = "   ";
	
	printf("\n%s--- HISTORIA CLINICA DEL PACIENTE: %s ---\n", margen, cedulaPac);
	printf("%s%-12s %-15s %-50s\n", margen, "FECHA", "MEDICO", "DETALLE");
	for(int i=0; i<115; i++) printf("-");
	printf("\n");
	
	while (fgets(linea, sizeof(linea), fp)) {
		linea[strcspn(linea, "\n")] = '\0';
		
		// NOTA TÉCNICA: %7999[^\n] lee hasta 7999 chars hasta encontrar un salto de línea
		// Esto coincide con nuestro MAX_DESC_HISTORIAL aprox.
		if (sscanf(linea, "%d|%10[^|]|%14[^|]|%10[^|]|%7999[^\n]", 
				   &h.id, h.cedulaPaciente, h.codigoMedico, h.fecha, h.descripcion) == 5) {
			
			if (strcmp(h.cedulaPaciente, cedulaPac) == 0) {
				imprimirFilaHistorial(h.fecha, h.codigoMedico, h.descripcion);
				encontrado = 1;
				printf("   ....................................................................................\n");
			}
		}
	}
	if (!encontrado) printf("\n%s[i] No hay registros para este paciente.\n", margen);
	printf("\n");
	fclose(fp);
}

void gestionarHistorial(const char *codigoMedicoLogueado) {
	char buffer[200];
	char cedula[15];
	Paciente p; 
	
	dibujarEncabezado("HISTORIAL CLINICO (EXTENDIDO)");
	
	// 1. Pedir Cédula
	printf("\n   Ingrese Cedula del Paciente para gestionar su historial (0 para Volver): ");
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
	
	if (strcmp(buffer, "0") == 0) return;
	
	if (!obtenerPaciente(buffer, &p)) {
		printf("   [!] Error: Paciente no encontrado en la base de datos.\n");
		pausa();
		return;
	}
	strcpy(cedula, buffer);
	
	// 2. Ver Historial
	verHistorialPaciente(cedula);
	
	// 3. Agregar Nota
	printf("\n   Desea agregar una nueva nota/diagnostico? (si/no): ");
	fgets(buffer, sizeof(buffer), stdin);
	buffer[strcspn(buffer, "\n")] = '\0';
	
	if (strcmp(buffer, "si") == 0 || strcmp(buffer, "SI") == 0 || strcmp(buffer, "s") == 0) {
		Historial nueva;
		nueva.id = rand() % 9000 + 1000; 
		strcpy(nueva.cedulaPaciente, cedula);
		strcpy(nueva.codigoMedico, codigoMedicoLogueado);
		
		// Pedir Fecha
		do {
			printf("   Fecha de la consulta (DD-MM-YYYY): ");
			fgets(buffer, sizeof(buffer), stdin);
			buffer[strcspn(buffer, "\n")] = '\0';
			
			if (validarFechaCita(buffer)) {
				strcpy(nueva.fecha, buffer);
				break;
			} else {
				printf("   -> Fecha invalida (use formato DD-MM-YYYY).\n");
			}
		} while(1);
		
		// Pedir Descripción
		printf("\n   --- Escriba el diagnostico completo (Max 8000 caracteres) ---\n");
		printf("   [NOTA: Escriba todo en un parrafo continuo y presione ENTER al finalizar]\n");
		printf("   > ");
		
		// Usamos sizeof nueva.descripcion (8000)
		if (fgets(nueva.descripcion, sizeof(nueva.descripcion), stdin) != NULL) {
			nueva.descripcion[strcspn(nueva.descripcion, "\n")] = '\0';
			
			// Verificamos que no esté vacío
			if (strlen(nueva.descripcion) > 1) {
				guardarEntradaHistorial(nueva);
				printf("\n   [OK] Historial actualizado correctamente.\n");
			} else {
				printf("\n   [!] No se guardo porque el texto estaba vacio.\n");
			}
		}
	}
	pausa();
}
