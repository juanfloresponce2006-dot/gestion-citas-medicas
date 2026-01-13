/* File: autenticacion.c */
//--- LIBRERIAS ---
#include "autenticacion.h"
#include "utils.h"
#include "fecha.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

  
//--- ARCHIVOS ---
static const char *fileAuth = "data/usuarios.txt";
static const char *filePac = "data/pacientes.txt";
static const char *fileMed = "data/medicos.txt";


//--- DEFINIIONES ---
#define CLAVE_MAESTRA_MEDICO "claveMedico000"
#define ADMIN_USER "ADMIN000"
#define ADMIN_PASS "admin123000"


// --- Funciones Privadas de Ayuda ---
static void pedirPasswordSeguro(char *buffer, int maxLen) {
	int i = 0;
	char ch;
	while (1) {
		ch = _getch();
		if (ch == 13) { buffer[i] = '\0'; printf("\n"); break; }
		else if (ch == 8) { if (i > 0) { i--; printf("\b \b"); } }
		else { if (i < maxLen - 1) { buffer[i] = ch; i++; printf("*"); } }
	}
}

static int existeUsuario(const char *user) {
	FILE *archivo = fopen(fileAuth, "r");
	if (!archivo) return 0;
	char linea[100], uLeido[20], pLeido[20];
	int rLeido;
	while (fgets(linea, sizeof(linea), archivo)) {
		if (sscanf(linea, "%19[^|]|%19[^|]|%d", uLeido, pLeido, &rLeido) == 3) {
			if (strcmp(uLeido, user) == 0) { fclose(archivo); return 1; }
		}
	}
	fclose(archivo);
	return 0;
}

// --- REGISTRO DE USUARIOS ---
static void registrarUsuario(void) {
	Usuario u;
	char buffer[100];
	int opcionRol = 0;
	
	dibujarEncabezado("REGISTRO DE NUEVO USUARIO");
	
	// 1. ROL
	do {
		printf("\n   Seleccione su tipo de cuenta:");
		printf("\n   [1] Médico");
		printf("\n   [2] Paciente");
		printf("\n   [0] Cancelar");
		printf("\n   Opcion: ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) return;
		buffer[strcspn(buffer, "\n")] = '\0';
		if (numVerify(buffer) == 1) {
			opcionRol = strtol(buffer, NULL, 10);
			if (opcionRol == 0) return;
			if (opcionRol == 1 || opcionRol == 2) { u.rol = opcionRol; break; }
		}
	} while(1);
	
	// 2. SEGURIDAD MEDICO
	if (u.rol == 1) {
		printf("\n   [SEGURIDAD] Ingrese Clave Maestra: ");
		pedirPasswordSeguro(buffer, 50);
		if (strcmp(buffer, CLAVE_MAESTRA_MEDICO) != 0) {
			printf("\n   [X] Clave incorrecta.\n"); pausa(); return;
		}
	}
	
	// 3. USUARIO (CEDULA)
	do {
		printf("\n   Ingrese su Cedula (sera su usuario): ");
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) return;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (!esSoloNumeros(buffer)) {
			printf("   -> Error: Solo numeros.\n");
		} else if (strlen(buffer) != 10) { 
			printf("   -> Error: La cédula debe tener exactamente 10 digitos.\n");
		} else if (existeUsuario(buffer)) {
			printf("   -> Error: Usuario ya registrado.\n");
		} else { 
			strcpy(u.username, buffer); 
			break; 
		}
	} while (1);
	
	// 4. PASSWORD
	do {
		printf("   Cree una contraseña: ");
		pedirPasswordSeguro(buffer, 20);
		if (strlen(buffer) < 3) printf("   -> Error: Muy corta (min 3).\n");
		else { strcpy(u.password, buffer); break; }
	} while (1);
	
	// --- CAPTURA DE DATOS ESPECÍFICOS ---
	if (u.rol == 2) { // PACIENTE
		char nombre[50], fechaNac[11], telefono[20], correo[50];
		int edad;
		printf("\n   --- DATOS DEL PACIENTE ---\n");
		
		do { printf("   Nombre Completo: "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if(esTextoConEspacios(buffer)) { strcpy(nombre, buffer); break; }
		else printf("   -> Error: Solo letras y espacios.\n");
		} while(1);
		
		/*do { printf("   Edad: "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if(validacionEdad(buffer)) { edad = atoi(buffer); break; }
		else printf("   -> Error: Ingrese una edad valida (1-120).\n");
		} while(1);*/
		
		// --- VALIDAR NACIEMIENTO ---
		do
		{ 
			printf("   Fecha Nacimiento (DD-MM-YYYY): "); 
			fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
			
			if(validarFechaNacimiento(buffer))
			{
				strcpy(fechaNac, buffer); break; 
			}
			else
			{
				printf("   -> Error: Formato invalido (DD-MM-YYYY) o fecha imposible.\n");
			}
		} while(1);
	
		edad = calcularEdad(fechaNac);
		
		do { printf("   Telefono (10 digitos): "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if (!esSoloNumeros(buffer)) printf("   -> Error: Solo numeros.\n");
		else if (strlen(buffer) != 10) printf("   -> Error: Debe tener 10 digitos.\n");
		else { strcpy(telefono, buffer); break; }
		} while(1);
		
		do { printf("   Correo (ej: usuario@gmail.com): "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if (strlen(buffer) > 5 && strchr(buffer, '@') && strchr(buffer, '.')) { strcpy(correo, buffer); break; }
		else printf("   -> Error: Correo invalido.\n");
		} while(1);
		
		FILE *fp = fopen(filePac, "a");
		if(!fp) fp = fopen(filePac, "w");
		if(fp) { fprintf(fp, "%s|%s|%d|%s|%s|%s|1\n", u.username, nombre, edad, fechaNac, telefono, correo); fclose(fp); }
		
	} else { // MEDICO
		char nombre[50], fechaNac[11], especialidad[30], horario[15];
		int edad;
		const char *esps[] = {"Medicina Familiar y General","Pediatría","Medicina Interna","Ginecología y Obstetricia","Cardiología","Dermatología","Neurología","Psiquiatría","Cirugía General"};
		printf("\n   --- DATOS DEL MÉDICO ---\n");
		
		do { printf("   Nombre Completo: "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if(esTextoConEspacios(buffer)) { strcpy(nombre, buffer); break; }
		else printf("   -> Error: Solo letras y espacios.\n");
		} while(1);
		
		/*do { printf("   Edad: "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if(validacionEdad(buffer)) { edad = atoi(buffer); break; }
		else printf("   -> Error: Ingrese una edad valida (1-120).\n");
		} while(1);*/
		
		// --- CAMBIO AQUI: Validar Nacimiento ---
		do { 
			printf("   Fecha Nacimiento (DD-MM-YYYY): "); 
			fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
			
			if(validarFechaNacimiento(buffer)) { // Usamos la nueva función
				strcpy(fechaNac, buffer); break; 
			} else {
				printf("   -> Error: Formato invalido (DD-MM-YYYY) o fecha imposible.\n");
			}
		} while(1);
		
		edad = calcularEdad(fechaNac);
		
		int opEsp;
		printf("\n   Seleccione Especialidad:\n"); 
		for(int k=0; k<9; k++) printf("   [%d] %s\n", k+1, esps[k]); 
		do {
			printf("   Opcion: "); fgets(buffer, 100, stdin); buffer[strcspn(buffer, "\n")] = '\0';
			if(numVerify(buffer)==1) {
				opEsp=atoi(buffer);
				if(opEsp>=1 && opEsp<=9) { strcpy(especialidad, esps[opEsp-1]); break; }
			}
			printf("   -> Error: Opcion invalida.\n");
		} while(1);
		
		do { printf("   Horario (HH:MM-HH:MM): "); fgets(buffer, 100, stdin); buffer[strcspn(buffer,"\n")]='\0';
		if(esHorarioValido(buffer)) { strcpy(horario, buffer); break; }
		else printf("   -> Error: Formato incorrecto.\n");
		} while(1);
		
		FILE *fm = fopen(fileMed, "a");
		if(!fm) fm = fopen(fileMed, "w");
		if(fm) { fprintf(fm, "%s|%s|%d|%s|%s|%s|1\n", u.username, nombre, edad, fechaNac, especialidad, horario); fclose(fm); }
	}
	
	FILE *fa = fopen(fileAuth, "a");
	if (!fa) fa = fopen(fileAuth, "w");
	if (fa) { fprintf(fa, "%s|%s|%d\n", u.username, u.password, u.rol); fclose(fa);
	printf("\n   [OK] Registro completado exitosamente.\n"); }
	pausa();
}


// --- INICIO DE SESIÓN ---
static int iniciarSesion(char *usuarioLogueadoBuffer)
{
	char usuarioIngresado[20], passwordIngresado[20], linea[100];
	char uLeido[20], pLeido[20];
	int rLeido, intentos = 3;
	
	while (intentos > 0) {
		dibujarEncabezado("INICIAR SESION");
		printf("\n   Intentos restantes: %d\n", intentos);
		
		printf("   Usuario (Cedula): ");
		if(fgets(usuarioIngresado, sizeof(usuarioIngresado), stdin) == NULL) return 0;
		usuarioIngresado[strcspn(usuarioIngresado, "\n")] = '\0';
		
		printf("   Contrasena: ");
		pedirPasswordSeguro(passwordIngresado, 20);
		
		// 1. VERIFICACIÓN DE ADMINISTRADOR
		if (strcmp(usuarioIngresado, ADMIN_USER) == 0 && 
			strcmp(passwordIngresado, ADMIN_PASS) == 0) {
			
			strcpy(usuarioLogueadoBuffer, "ADMIN"); 
			printf("\n   [OK] Acceso de ADMINISTRADOR concedido.\n");
			pausa();
			return 3; 
		}
		
		// 2. VERIFICACIÓN DE USUARIOS NORMALES
		FILE *archivo = fopen(fileAuth, "r");
		int loginExitoso = 0, rolDetectado = 0;
		int usuarioEncontrado = 0;
		
		if (archivo) {
			while (fgets(linea, sizeof(linea), archivo)) {
				if (sscanf(linea, "%19[^|]|%19[^|]|%d", uLeido, pLeido, &rLeido) == 3) {
					if (strcmp(usuarioIngresado, uLeido) == 0) {
						usuarioEncontrado = 1;
						if (strcmp(passwordIngresado, pLeido) == 0) {
							loginExitoso = 1;
							rolDetectado = rLeido;
							strcpy(usuarioLogueadoBuffer, uLeido);
							break;
						}
					}
					
				}
			}
			fclose(archivo);
		}
		
		if (loginExitoso) {
			printf("\n   [OK] Bienvenido!\n"); pausa();
			return rolDetectado;
		} else if (!usuarioEncontrado) {
			printf("\n   [!] Usuario no encontrado.\n");
			printf("   Desea crear una nueva cuenta? (s/n): ");
			
			char opcion[5];
			fgets(opcion, sizeof(opcion), stdin);
			opcion[strcspn(opcion, "\n")] = '\0';
			
			if (strcmp(opcion, "s") == 0 || strcmp(opcion, "S") == 0) {
				registrarUsuario();
				return 0; // volver
			}
			
			pausa();
		}else{
			printf("\n   [!] Contraseña incorrecta.\n");
			intentos--;
			pausa();
		}
	}
	return 0;
}


// --- SISTEMA AUTENTICACIÓN PÚBLICO ---
int sistemaAutenticacion(char *usuarioLogueadoBuffer)
{
	int opcion;
	char buffer[20];
	do
	{
		dibujarEncabezado("BIENVENIDO AL SISTEMA MÉDICO");
		printf("\n   [1] Iniciar Sesión");
		//printf("\n   [2] Registrarse (Nuevo Usuario)");
		printf("\n   [0] Salir");
		printf("\n\n   Opción: ");
		
		if(fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
		buffer[strcspn(buffer, "\n")] = '\0';
		
		if (numVerify(buffer) == 1)
		{
			opcion = strtol(buffer, NULL, 10);
			switch(opcion)
			{
			case 1:
			{ 
				int rol = iniciarSesion(usuarioLogueadoBuffer); 
				if (rol > 0) return rol; 
				break; 
			}
			case 2: registrarUsuario(); break;
			case 0: return 0; 
			default: printf("   Opción no válida.\n"); pausa();
			}
		}
	} while (opcion != 0);
	return 0;
}
