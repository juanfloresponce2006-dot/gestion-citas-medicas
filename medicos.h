/* File: medicos.h */
#ifndef MEDICOS_H
#define MEDICOS_H

typedef struct {
	char codigo[15]; 
	char nombre[50];
	int edad;
	char fechaNacimiento[11];
	char especialidad[30];
	char horario[15];
	int disponible;
} Medico;

void registrarMedico(void);
void listarMedicos(void);
int obtenerMedico(const char *codigo, Medico *out);
int existeMedico(char codigo[]);
void menuMedicos(void);
void bajaPropiaMedico(const char *codigoMedico);

#endif
