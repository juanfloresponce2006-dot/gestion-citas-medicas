/* File: autenticacion.h */
#ifndef AUTENTICACION_H
#define AUTENTICACION_H

typedef struct {
	char username[20];
	char password[20];
	int rol; 
	// 1 = Medico, 2 = Paciente, 3 = Admin
} Usuario;

//RECIBE UN PUNTERO PARA DEVOLVER EL USUARIO
int sistemaAutenticacion(char *usuarioLogueadoBuffer);

#endif
