/* File: citas.c */
//--- LIBRERIAS ---
#include "citas.h"
#include "pacientes.h"
#include "medicos.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//--- ARCHIVOS Y DEFINICIONES ---
static const char* nombreArchivoCitas = "data/citas.txt"; 
static const char* nombreArchivoMedicos = "data/medicos.txt"; 
#define DURACION_CITA 45 


// --- Funciones Auxiliares Privadas ---
static int parsearLineaCita(const char *linea, Cita *c) {
	return sscanf(linea, "%d|%14[^|]|%14[^|]|%d|%d|%d|%5[^|]|%d|%511[^\n]",
				  &c->idCita, c->cedulaPaciente, c->codigoMedico, &c->fecha.dia,
				  &c->fecha.mes, &c->fecha.anio, c->fecha.hora, &c->estado, c-> observaciones) == 9;
}

static int parsearLineaMedicoLocal(const char *linea, Medico *m) {
	return sscanf(linea, "%14[^|]|%49[^|]|%d|%10[^|]|%29[^|]|%14[^|]|%d",
				  m->codigo, m->nombre, &m->edad, m->fechaNacimiento,
				  m->especialidad, m->horario, &m->disponible) == 7;
}

static int verificarConflicto(const char *codMedico, FechaHora f) {
	FILE *archivo = fopen(nombreArchivoCitas, "r");
	if (!archivo) return 0; 
	Cita c;
	char linea[256];
	while (fgets(linea, sizeof(linea), archivo)) {
		if (parsearLineaCita(linea, &c)) {
			if (c.estado == 1 && strcmp(c.codigoMedico, codMedico) == 0 &&
				c.fecha.dia == f.dia && c.fecha.mes == f.mes &&
				c.fecha.anio == f.anio && strcmp(c.fecha.hora, f.hora) == 0) {
				fclose(archivo);
				return 1; 
			}
		}
	}
	fclose(archivo);
	return 0;
}

// --- ELEGIR ESPECIALIDAD ---
static int elegirEspecialidad(char *bufferDestino) {
	const char *listaEspecialidades[] = {
		"Medicina Familiar y General", "Pediatria", "Medicina Interna",
			"Ginecologia y Obstetricia", "Cardiologia", "Dermatologia",
			"Neurologia", "Psiquiatria", "Cirugia General"
	};
	int total = 9;
	char buffer[50];
	int opcion;
	
	printf("\n   --- SELECCIONE ESPECIALIDAD ---\n");
	for(int i=0; i<total; i++) {
		printf("   [%d] %s\n", i+1, listaEspecialidades[i]);
	}
	printf("   [0] Cancelar / Volver\n");
	
	while(1) {
		printf("   Ingrese el número de la opción: "); // Mensaje más claro
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
		
		// 1. Limpiar espacios iniciales y salto de línea
		buffer[strcspn(buffer, "\n")] = '\0';
		
		// Si el usuario solo presiona Enter, ignorar y preguntar de nuevo
		if (strlen(buffer) == 0) continue;
		
		// 2. Validar
		if(numVerify(buffer) == 1) {
			opcion = atoi(buffer);
			
			if (opcion == 0) return 0; // El usuario eligió cancelar
			
			if(opcion >= 1 && opcion <= total) {
				strcpy(bufferDestino, listaEspecialidades[opcion-1]);
				return 1; // Éxito
			}
		}
		
		// Si llegamos aquí, es que la entrada fue mala
		printf("   -> [!] Error: '%s' no es una opción valida. Ingrese un número del 1 al %d.\n", buffer, total);
	}
}

static int seleccionarMedicoDeLista(const char *especialidad, Medico *mDestino) {
	FILE *archivo = fopen(nombreArchivoMedicos, "r");
	if(!archivo) {
		printf(" -> Error: No hay base de datos de médicos.\n");
		return 0;
	}
	
	Medico encontrados[50]; 
	int count = 0;
	char linea[256];
	Medico temp;
	
	while(fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if(parsearLineaMedicoLocal(linea, &temp)) {
			if(temp.disponible == 1 && strcmp(temp.especialidad, especialidad) == 0) {
				encontrados[count] = temp;
				count++;
				if(count >= 50) break; 
			}
		}
	}
	fclose(archivo);
	
	if(count == 0) {
		printf("\n   [!] No hay médicos disponibles para %s.\n", especialidad);
		return 0;
	}
	
	char buffer[50];
	int opcion;
	
	printf("\n   --- MÉDICOS DISPONIBLES (%s) ---\n", especialidad);
	printf("   %-4s %-30s %-15s\n", "No.", "NOMBRE", "HORARIO");
	printf("   ----------------------------------------------------\n");
	for(int i=0; i<count; i++) {
		printf("   [%d]  %-30s %-15s\n", i+1, encontrados[i].nombre, encontrados[i].horario);
	}
	
	while(1) {
		printf("\n   Seleccione al médico (0 para cancelar): ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if(numVerify(buffer) == 1) {
			opcion = strtol(buffer, NULL, 10);
			if(opcion == 0) return 0; // Cancelar
			if(opcion >= 1 && opcion <= count) {
				*mDestino = encontrados[opcion-1]; 
				return 1;
			}
		}
		printf("   -> Error: Seleccione un número válido de la lista.\n");
	}
}

static int seleccionarTurno(Medico m, FechaHora f, char *bufferHora) {
	int hInicio, mInicio, hFin, mFin;
	char slots[20][6]; 
	int estados[20];    
	int count = 0;
	
	sscanf(m.horario, "%d:%d-%d:%d", &hInicio, &mInicio, &hFin, &mFin);
	int minActual = hInicio * 60 + mInicio;
	int minLimite = hFin * 60 + mFin;
	
	while (minActual + DURACION_CITA <= minLimite) {
		int h = minActual / 60;
		int min = minActual % 60;
		snprintf(slots[count], 6, "%02d:%02d", h, min);
		FechaHora temp = f; 
		strcpy(temp.hora, slots[count]);
		if (verificarConflicto(m.codigo, temp)) estados[count] = 1; 
		else estados[count] = 0; 
		count++;
		minActual += DURACION_CITA; 
		if (count >= 20) break; 
	}
	
	char buffer[50];
	int opcion;
	
	printf("\n   --- HORARIOS DISPONIBLES (%02d/%02d/%04d) ---\n", f.dia, f.mes, f.anio);
	printf("   %-4s %-15s %-10s\n", "No.", "HORARIO", "ESTADO");
	printf("   --------------------------------------\n");
	
	for (int i = 0; i < count; i++) {
		int hStart, mStart;
		sscanf(slots[i], "%d:%d", &hStart, &mStart);
		int minEnd = hStart * 60 + mStart + DURACION_CITA;
		char estadoStr[15];
		if(estados[i] == 1) strcpy(estadoStr, "[OCUPADO]");
		else strcpy(estadoStr, "DISPONIBLE");
		printf("   [%d]  %s - %02d:%02d    %s\n", i + 1, slots[i], (minEnd/60), (minEnd%60), estadoStr);
	}
	
	while(1) {
		printf("\n   Seleccione un turno (0 para cancelar): ");
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) return 0;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1) {
			opcion = strtol(buffer, NULL, 10);
			if (opcion == 0) return 0; // Cancelar
			if (opcion > 0 && opcion <= count) {
				if (estados[opcion - 1] == 1) {
					printf("   -> Error: Ese turno ya está ocupado. Elija otro.\n");
				} else {
					strcpy(bufferHora, slots[opcion - 1]);
					return 1;
				}
			} else {
				printf("   -> Error: Opción fuera de rango.\n");
			}
		} else {
			printf("   -> Error: Ingrese un número válido.\n");
		}
	}
}

static int obtenerSiguienteIDCita(void) {
	FILE *archivo = fopen(nombreArchivoCitas, "r");
	if (!archivo) return 1;
	Cita c;
	char linea[256];
	int ultimo = 0;
	while (fgets(linea, sizeof(linea), archivo)) {
		if (parsearLineaCita(linea, &c)) {
			if (c.idCita > ultimo) ultimo = c.idCita;
		}
	}
	fclose(archivo);
	return ultimo + 1;
}

// --- Funciones Públicas ---

void registrarCita(void) {
	Cita c;
	Paciente p;
	Medico m;
	char buffer[100];
	char especialidadElegida[50];
	c.estado = 1;
	
	// 1. PEDIR PACIENTE
	do {
		printf("Cédula del paciente [0 para Volver]: ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return; // Volver
		
		if (!obtenerPaciente(buffer, &p)) {
			printf(" -> Error: Paciente no encontrado.\n");
		} else if (!p.activo) {
			printf(" -> Error: Paciente inactivo.\n");
		} else {
			strcpy(c.cedulaPaciente, buffer);
			printf("    [Paciente: %s]\n", p.nombre);
			break;
		}
	} while (1);
	
	// 2. PEDIR FECHA
	do {
		printf("\nFecha de la cita (DD-MM-YYYY) [0 para Volver]: ");
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (strcmp(buffer, "0") == 0) return;
		
		if (!validarFechaCita(buffer)) {
			printf(" -> Error: Formato incorrecto (DD-MM-YYYY) o fecha pasada/muy futura.\n");
			continue;
		}
		
		int d, mes, a;
		// Escaneamos buscando guiones
		if (sscanf(buffer, "%d-%d-%d", &d, &mes, &a) == 3) {
			// Ya fue validado por validarFechaCita, asi que asignamos directo
			c.fecha.dia = d; c.fecha.mes = mes; c.fecha.anio = a;
			break;
		} else {
			printf(" -> Error: Formato incorrecto.\n");
		}
	} while (1);
	
	// 3. SELECCIONAR ESPECIALIDAD
	if (!elegirEspecialidad(especialidadElegida)) {
		printf("    -> Operacion cancelada.\n");
		return;
	}
	printf("    -> Filtro aplicado: %s\n", especialidadElegida);
	
	// 4. SELECCIONAR MÉDICO
	if (!seleccionarMedicoDeLista(especialidadElegida, &m)) {
		printf("    -> Operacion cancelada.\n");
		return; 
	}
	strcpy(c.codigoMedico, m.codigo);
	printf("    -> Medico asignado: %s\n", m.nombre);
	
	// 5. SELECCIÓN DE TURNO
	char horaSeleccionada[6];
	if (seleccionarTurno(m, c.fecha, horaSeleccionada)) {
		strcpy(c.fecha.hora, horaSeleccionada);
		printf("    -> Turno agendado: %s\n", c.fecha.hora);
	} else {
		printf("    -> Operacion cancelada.\n");
		return;
	}
	
	// 6. OBSERVACIONES (vacío por defecto)
	strcpy(c.observaciones, "SIN OBSERVACIONES");	
	
	// 7. GUARDAR
	c.idCita = obtenerSiguienteIDCita();
	FILE *archivo = fopen(nombreArchivoCitas, "a");
	if (!archivo) archivo = fopen(nombreArchivoCitas, "w");
	
	if (archivo) {
		fprintf(archivo, "%d|%s|%s|%d|%d|%d|%s|%d|%s\n",
				c.idCita, c.cedulaPaciente, c.codigoMedico,
				c.fecha.dia, c.fecha.mes, c.fecha.anio,
				c.fecha.hora, c.estado, c.observaciones);
		fclose(archivo);
		printf("\n\t[OK] Cita #%d registrada exitosamente.\n", c.idCita);
	} else {
		printf("\n\t[X] Error critico al guardar.\n");
	}
}

static void listarCitas(void) {
	FILE *archivo = fopen(nombreArchivoCitas, "r");
	if (!archivo) {
		printf("\n\t[!] No hay citas registradas.\n");
		return;
	}
	Cita c;
	char linea[256];
	int contador = 0;
	char *margen = "  "; 
	printf("\n");
	printf("%s", margen);
	printf("%-4s %-6s %-12s %-12s %-14s %-8s %-10s\n",
		   "#", "ID", "PACIENTE", "MÉDICO", "FECHA", "HORA", "ESTADO");
	printf("%s", margen);
	for(int i=0; i<80; i++) printf("-");
	printf("\n");
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if (parsearLineaCita(linea, &c)) {
			contador++;
			char fechaStr[15];
			snprintf(fechaStr, 15, "%02d/%02d/%04d", c.fecha.dia, c.fecha.mes, c.fecha.anio);
			printf("%s", margen);
			printf("%-4d %-6d %-12s %-12s %-14s %-8s %-10s\n",
				   contador, c.idCita, c.cedulaPaciente, c.codigoMedico,
				   fechaStr, c.fecha.hora, (c.estado == 1) ? "Activa" : "Cancel.");
		}
	}
	if (contador == 0) printf("%s| %-76s |\n", margen, "NO HAY CITAS");
	printf("%s", margen);
	for(int i=0; i<80; i++) printf("=");
	printf("\n");
	fclose(archivo);
}

static void cambiarEstadoCita(void) {
	char buffer[50];
	int idBuscado;
	
	printf("Ingrese ID de la cita (0 para Volver): ");
	if (!fgets(buffer, sizeof(buffer), stdin)) return;
	buffer[strcspn(buffer, "\n")] = '\0';
	
	if (numVerify(buffer) != 1) return;
	idBuscado = atoi(buffer);
	if (idBuscado == 0) return;
	
	FILE *in = fopen(nombreArchivoCitas, "r");
	FILE *out = fopen("data/citas.tmp", "w");
	if (!in || !out) {
		printf("Error DB.\n");
		if (in) fclose(in);
		if (out) fclose(out);
		return;
	}
	
	Cita c;
	char linea[1024];
	int encontrado = 0;
	
	while (fgets(linea, sizeof(linea), in)) {
		linea[strcspn(linea, "\n")] = '\0';
		
		if (parsearLineaCita(linea, &c)) {
			if (c.idCita == idBuscado) {
				c.estado = !c.estado;
				encontrado = 1;
			}
			
			fprintf(out, "%d|%s|%s|%d|%d|%d|%s|%d|%s\n",
					c.idCita, c.cedulaPaciente, c.codigoMedico,
					c.fecha.dia, c.fecha.mes, c.fecha.anio,
					c.fecha.hora, c.estado, c.observaciones);
		} else {
			// Línea corrupta o inválida: se copia tal cual
			fprintf(out, "%s\n", linea);
		}
	}
	
	fclose(in);
	fclose(out);
	
	if (encontrado) {
		remove(nombreArchivoCitas);
		rename("data/citas.tmp", nombreArchivoCitas);
		printf("\n\t[OK] Estado cambiado.\n");
	} else {
		remove("data/citas.tmp");
		printf("\n\t[!] Cita no encontrada.\n");
	}
}


static void editarObservacionesCita(const char *codigoMedico, const int idBuscado) {

	FILE *in = fopen(nombreArchivoCitas, "r");
	FILE *out = fopen("data/citas.tmp", "w");
	if (!in || !out) {
		printf("Error DB.\n");
		if (in) fclose(in);
		if (out) fclose(out);
		return;
	}
	
	Cita c;
	char linea[1024];
	int encontrado = 0;
	
	while (fgets(linea, sizeof(linea), in)) {
		linea[strcspn(linea, "\n")] = '\0';
		
		if (parsearLineaCita(linea, &c) && c.idCita == idBuscado) {
			if (strcmp(c.codigoMedico, codigoMedico) != 0) {
				printf("\n\t[!] No puede editar citas de otro médico.\n");
				fclose(in);
				fclose(out);
				remove("data/citas.tmp");
				return;
			}
			
			encontrado = 1;
			printf("Ingrese observaciones (máx 512 caracteres):\n> ");
			fgets(c.observaciones, sizeof(c.observaciones), stdin);
			c.observaciones[strcspn(c.observaciones, "\n")] = '\0';
			limpiarSeparadores(c.observaciones);
			
			fprintf(out, "%d|%s|%s|%d|%d|%d|%s|%d|%s\n",
					c.idCita, c.cedulaPaciente, c.codigoMedico,
					c.fecha.dia, c.fecha.mes, c.fecha.anio,
					c.fecha.hora, c.estado, c.observaciones);
		} else {
			fprintf(out, "%s\n", linea);
		}
	}
	
	fclose(in);
	fclose(out);
	
	if (encontrado) {
		remove(nombreArchivoCitas);
		rename("data/citas.tmp", nombreArchivoCitas);
		printf("\n\t[OK] Observaciones actualizadas.\n");
	} else {
		remove("data/citas.tmp");
		printf("\n\t[!] Cita no encontrada.\n");
	}
}


void menuCitas(void) {
	int opcion;
	char buffer[50];
	do {
		dibujarEncabezado("GESTIÓN DE CITAS");
		printf("\n\t[1] Crear nueva cita");
		printf("\n\t[2] Listar citas");
		printf("\n\t[3] Cancelar/Reactivar cita");
		printf("\n\t[0] Volver al menú principal");
		printf("\n\n\tIngrese opcion: ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0'; 
		if (numVerify(buffer) != 1) opcion = -1;
		else opcion = strtol(buffer, NULL, 10);
		
		switch(opcion) {
		case 1: 
			dibujarEncabezado("NUEVA CITA"); 
			registrarCita(); 
			pausa(); 
			break;
		case 2: dibujarEncabezado("LISTADO DE CITAS"); listarCitas(); pausa(); break;
		case 3: dibujarEncabezado("MODIFICAR CITA"); cambiarEstadoCita(); pausa(); break;
		case 0: break;
		default: printf("\n\tOpción no reconocida.\n"); pausa();
		}
	} while (opcion != 0);
}

void listarCitasDeMedico(const char *codigoMedico) {
	FILE *archivo = fopen("data/citas.txt", "r");
	if (!archivo) { printf("\n\t[!] No hay citas registradas.\n"); return; }
	
	Cita c;
	char linea[256];
	int contador = 0;
	char *margen = " "; 
	
	printf("\n%s MIS CITAS PROGRAMADAS (%s)\n", margen, codigoMedico);
	printf("%s%-4s %-11s %-12s %-8s %-10s\n", margen, "ID", "PACIENTE", "FECHA", "HORA", "ESTADO");
	for(int i=0; i<60; i++) printf("-");
	printf("\n");
	
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if (parsearLineaCita(linea, &c)) { 
			// FILTRO: Solo si el código del médico coincide
			if (strcmp(c.codigoMedico, codigoMedico) == 0) {
				contador++;
				char fechaStr[15];
				snprintf(fechaStr, 15, "%02d/%02d/%04d", c.fecha.dia, c.fecha.mes, c.fecha.anio);
				
				printf("%s%-4d %-11s %-12s %-8s %-10s\n",
					   margen, c.idCita, c.cedulaPaciente,
					   fechaStr, c.fecha.hora, (c.estado == 1) ? "Activa" : "Cancel.");
				
				printf("%s  Observaciones:\n", margen);
				printf("%s  %s\n", margen, c.observaciones);
				
			}
		}
	}
	if (contador == 0) printf("\n%s [i] No tiene citas asignadas.\n", margen);
	printf("\n");
	fclose(archivo);
}

void gestionarCitasMedico(const char *codigoMedico) {
	char buffer[50];
	int opcion;
	
	while (1) {
		listarCitasDeMedico(codigoMedico);
		
		printf("Ingrese ID de la cita para editar observaciones (0 para volver): ");
		if (!fgets(buffer, sizeof(buffer), stdin)) return;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) != 1) continue;
		opcion = atoi(buffer);
		
		if (opcion == 0) return;
		
		editarObservacionesCita(codigoMedico, opcion);
		pausa();
		limpiarPantalla();
		dibujarEncabezado("MIS CITAS");
	}
}


void listarCitasDePaciente(const char *cedulaPaciente) {
	FILE *archivo = fopen("data/citas.txt", "r");
	if (!archivo) { printf("\n\t[!] No hay citas registradas.\n"); return; }
	
	Cita c;
	char linea[256];
	int contador = 0;
	char *margen = " "; 
	
	printf("\n%s MIS CITAS MÉDICAS (Paciente: %s)\n", margen, cedulaPaciente);
	printf("%s%-4s %-10s %-12s %-8s %-10s\n", margen, "ID", "MÉDICO", "FECHA", "HORA", "ESTADO");
	for(int i=0; i<60; i++) printf("-");
	printf("\n");
	
	while (fgets(linea, sizeof(linea), archivo)) {
		linea[strcspn(linea, "\n")] = '\0';
		if (parsearLineaCita(linea, &c)) {
			// FILTRO: Solo si la cédula del paciente coincide
			if (strcmp(c.cedulaPaciente, cedulaPaciente) == 0) {
				contador++;
				char fechaStr[15];
				snprintf(fechaStr, 15, "%02d/%02d/%04d", c.fecha.dia, c.fecha.mes, c.fecha.anio);
				
				printf("%s%-4d %-10s %-12s %-8s %-10s\n",
					   margen, c.idCita, c.codigoMedico,
					   fechaStr, c.fecha.hora, (c.estado == 1) ? "Activa" : "Cancel.");
				
				printf("%s  Observaciones:\n", margen);
				printf("%s  %s\n", margen, c.observaciones);
				
			}
		}
	}
	if (contador == 0) printf("\n%s [i] No tiene citas registradas.\n", margen);
	printf("\n");
	fclose(archivo);
}
