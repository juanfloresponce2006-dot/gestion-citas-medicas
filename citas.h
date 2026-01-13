/* File: citas.h */
#ifndef CITAS_H
#define CITAS_H

typedef struct {
	int dia, mes, anio;
	char hora[6];        // HH:MM
} FechaHora;

typedef struct {
	int idCita;
	char cedulaPaciente[11];
	char codigoMedico[10];
	FechaHora fecha;
	int estado;          // 1 = activa, 0 = cancelada
	char observaciones[512];
} Cita;

void menuCitas(void);
void listarCitasDeMedico(const char *codigoMedico);
void registrarCita(void); 
void listarCitasDePaciente(const char *cedulaPaciente);
void listarCitasDeMedico(const char *codigoMedico);
void gestionarCitasMedico(const char *codigoMedico);

#endif
