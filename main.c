/* File: main.c */
//--- LIBRERIAS ---
#include "pacientes.h"
#include "medicos.h"
#include "citas.h"
#include "historial.h"
#include "utils.h"
#include "autenticacion.h" 
#include "fecha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


//--- PROTOTIPOS ---
void menuAdmin(void);
void menuMedicoLogueado(char *codigoMedico);
void menuPacienteLogueado(char *cedulaPaciente);


int main()
{
	getFechaActual();
	
	SetConsoleOutputCP(1252); 
	SetConsoleCP(1252);
	system("mode con cols=120 lines=50");
	system("color 1F");
	
	char usuarioLogueado[20] = {0};
	int rol = 0;
	
	while (1)
	{
		rol = sistemaAutenticacion(usuarioLogueado);
		
		if (rol == 0)
		{
			printf("\n\tSaliendo del sistema...\n");
			break; 
		}
		
		if (rol == 3)
		{
			menuAdmin();
		} 
		else if (rol == 1)
		{
			menuMedicoLogueado(usuarioLogueado);
		}
		else if (rol == 2)
		{
			menuPacienteLogueado(usuarioLogueado);
		}
	}
	
	return 0;
}


// --- MENU PARA ADMINISTRADORES ---
void menuAdmin(void)
{
	char buffer[50];
	int opcion;
	char *m = "                                    "; 
	
	do
	{
		dibujarEncabezado("MENU ADMINISTRADOR");
		printf("\n%s[1] Gestión de Pacientes\n", m);
		printf("%s[2] Gestión de Médicos\n", m);
		printf("%s[3] Gestión de Citas\n", m);
		printf("%s[0] Cerrar Sesion\n", m);
		
		printf("\n%sIngrese su opcion: ", m);
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1) opcion = atoi(buffer);
		else opcion = -1;
		switch(opcion)
		{
		case 1: 
			// Admin (Rol 3), ID "ADMIN"
			menuPacientes(3, "ADMIN"); 
			break;
		case 2: menuMedicos(); break;
		case 3: menuCitas(); break;
		case 0: printf("\n%sCerrando sesión...\n", m); break;
		default: printf("\n%s[!] Opción invalida.\n", m); pausa();
		}
	} while (opcion != 0);
}


// --- MENU PARA MEDICOS ---
void menuMedicoLogueado(char *codigoMedico)
{
	char buffer[50];
	int opcion;
	char *m = "                                    "; 
	
	do
	{
		char titulo[100];
		sprintf(titulo, "PANEL MÉDICO [%s]", codigoMedico);
		dibujarEncabezado(titulo);
		
		printf("\n%s[1] Gestión de Pacientes (Historial)\n", m);
		printf("%s[2] Mis Citas Agendadas\n", m);
		//printf("%s[3] Darse de Baja\n", m);
		printf("%s[0] Cerrar Sesión\n", m);
		
		printf("\n%sIngrese su opción: ", m);
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1) opcion = atoi(buffer);
		else opcion = -1;
		
		switch(opcion)
		{
		case 1: 
			// Médico (Rol 1), Pasamos su ID
			menuPacientes(1, codigoMedico); 
			break;
		case 2: 
			dibujarEncabezado("MIS CITAS");
			listarCitasDeMedico(codigoMedico);
			pausa();
			break;
		//case 3: 
			//bajaPropiaMedico(codigoMedico);
			//pausa();
			//break;
		case 0: break;
		default: printf("\n%s[!] Opción invalida.\n", m); pausa();
		}
	} while (opcion != 0);
}


// --- MENU PARA PACIENTES ---
void menuPacienteLogueado(char *cedulaPaciente) 
{
	char buffer[50];
	int opcion;
	char *m = "                                    "; 
	
	do {
		char titulo[100];
		sprintf(titulo, "PANEL PACIENTE [%s]", cedulaPaciente);
		dibujarEncabezado(titulo);
		
		printf("\n%s[1] Ver mis Citas\n", m);
		printf("%s[2] Agendar Nueva Cita\n", m);
		printf("%s[3] Darse de Baja\n", m);
		printf("%s[0] Cerrar Sesión\n", m);
		
		printf("\n%sIngrese su opción: ", m);
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1) opcion = atoi(buffer);
		else opcion = -1;
		
		switch(opcion)
		{
		case 1: 
			dibujarEncabezado("MIS CITAS");
			listarCitasDePaciente(cedulaPaciente);
			pausa();
			break;
		case 2:
			dibujarEncabezado("NUEVA CITA");
			registrarCita(); 
			pausa();
			break;
		case 3: 
			bajaPropiaPaciente(cedulaPaciente);
			pausa();
			break;
		case 0: break;
		default: printf("\n%s[!] Opción invalida.\n", m); pausa();
		}
	} while (opcion != 0);
}
