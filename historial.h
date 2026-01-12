/* File: historial.h */
#ifndef HISTORIAL_H
#define HISTORIAL_H

#define MAX_DESC_HISTORIAL 8000 

typedef struct {
	int id;
	char cedulaPaciente[11];
	char codigoMedico[15];
	char fecha[11];
	char descripcion[MAX_DESC_HISTORIAL];
} Historial;

void gestionarHistorial(const char *codigoMedicoLogueado);

#endif
