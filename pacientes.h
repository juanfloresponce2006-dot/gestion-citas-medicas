/* File: pacientes.h */
#ifndef PACIENTES_H
#define PACIENTES_H

typedef struct {
	char cedula[15];
	char nombre[50];
	int edad;
	char fechaNacimiento[11];
	char telefono[20];
	char correo[50];
	int activo;
} Paciente;

void registrarPaciente(void);
int listarPaciente(void);
int obtenerPaciente(const char *cedula, Paciente *out);
int existePaciente(char cedula[]);
void menuPacientes(int rolUsuario, char *idUsuario);
void bajaPropiaPaciente(const char *cedulaPaciente);

#endif
