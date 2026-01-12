/* File: utils.c */
//--- LIBRERIAS ---
#include "utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>


// --- Funciones de Validación ---
int numVerify(char number[]) {
	int i = 0;
	int decimalCount = 0;
	
	if (strlen(number) == 0) return -1;
	
	if (number[0] == '-') {
		if (strlen(number) == 1) return -1;
		i = 1;
	}
	
	for (; number[i] != '\0'; i++) {
		if (number[i] == '.') {
			decimalCount++;
			if (decimalCount > 1) return -1;
		} 
		else if (!isdigit((unsigned char)number[i])) { 
			return -1;
		}
	}
	
	if (decimalCount == 1) return 2; // Float
	return 1; // Integer
}

void cleanBuffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}
	
	int esSoloNumeros(const char *txt) {
		if (strlen(txt) == 0) return 0;
		for (int i = 0; txt[i]; i++)
			if (!isdigit((unsigned char)txt[i])) return 0;
		return 1;
	}
	
	int esTextoConEspacios(const char *txt) {
		if (strlen(txt) < 3) return 0;
		
		for (int i = 0; txt[i]; i++) {
			unsigned char c = (unsigned char)txt[i];
			
			// 1. Si es un caracter extendido (como ñ, á, é, ü), lo dejamos pasar.
			if (c > 127) continue; 
			
			// 2. Si es ASCII normal, validamos que sea letra o espacio
			if (!isalpha(c) && !isspace(c)) return 0;
		}
		return 1;
	}
	
	int esHorarioValido(const char *h) {
		int h1, m1, h2, m2;
		if (strlen(h) != 11) return 0;
		if (h[2] != ':' || h[5] != '-' || h[8] != ':') return 0;
		
		if (sscanf(h, "%d:%d-%d:%d", &h1, &m1, &h2, &m2) != 4) return 0;
		
		if (h1 < 0 || h1 > 23 || h2 < 0 || h2 > 23) return 0;
		if (m1 < 0 || m1 > 59 || m2 < 0 || m2 > 59) return 0;
		if (h1 * 60 + m1 >= h2 * 60 + m2) return 0;
		
		return 1;
	}
	
	int esHoraValida(const char *h) {
		int hh, mm;
		
		if (strlen(h) != 5) return 0;
		if (h[2] != ':') return 0;
		if (sscanf(h, "%d:%d", &hh, &mm) != 2) return 0;
		if (hh < 0 || hh > 23) return 0;
		if (mm < 0 || mm > 59) return 0;
		
		return 1;
	}
	
	
	static int validarDiasMes(int dia, int mes, int anio) {
		if (mes < 1 || mes > 12) return 0;
		if (dia < 1 || dia > 31) return 0;
		
		// Meses con 30 días
		if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) return 0;
		
		// Febrero
		if (mes == 2) {
			int esBisiesto = 0;
			if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0)) esBisiesto = 1;
			
			if (dia > 29) return 0;
			if (dia == 29 && !esBisiesto) return 0;
		}
		return 1;
	}
	
	// 1. VALIDAR NACIMIENTO (Rango: 1900 - 2025)
	int validarFechaNacimiento(const char *fecha) {
		size_t len = strlen(fecha);
		
		// Estricto: DD-MM-YYYY (10 chars)
		if (len != 10) return 0;
		if (fecha[2] != '-' || fecha[5] != '-') return 0;
		
		// Verificar que el resto sean números
		for(int i=0; i<10; i++) {
			if(i==2 || i==5) continue;
			if(!isdigit(fecha[i])) return 0;
		}
		
		int dia, mes, anio;
		sscanf(fecha, "%d-%d-%d", &dia, &mes, &anio);
		
		// Rango de años para nacimiento
		if (anio < 1900 || anio > 2025) return 0; 
		
		return validarDiasMes(dia, mes, anio);
	}
	
	// 2. VALIDAR CITA (Rango : 2024 - 2100)
	int validarFechaCita(const char *fecha) {
		size_t len = strlen(fecha);
		
		// Estricto: DD-MM-YYYY (10 chars)
		if (len != 10) return 0;
		if (fecha[2] != '-' || fecha[5] != '-') return 0;
		
		for(int i=0; i<10; i++) {
			if(i==2 || i==5) continue;
			if(!isdigit(fecha[i])) return 0;
		}
		
		int dia, mes, anio;
		sscanf(fecha, "%d-%d-%d", &dia, &mes, &anio);
		
		// Rango de años para citas futuras
		if (anio < 2024 || anio > 2100) return 0;
		
		return validarDiasMes(dia, mes, anio);
	}
	
	int validacionEdad(char *buffer) {
		if (numVerify(buffer) != 1) return 0;
		int edad = strtol(buffer, NULL, 10);
		if (edad >= 1 && edad <= 120) return 1;
		return 0;
	}
	
	// --- Funciones de UI ---
	
	void limpiarPantalla() {
		system("cls"); 
	}
	
	void pausa() {
		char *m = "                                    ";
		printf("\n%sPresione cualquier tecla para continuar...", m);
		_getch(); 
		printf("\n");
	}
	
	void imprimirLinea(char caracter) {
		for (int i = 0; i < ANCHO_CONSOLA; i++) {
			printf("%c", caracter);
		}
		printf("\n"); 
	}
	
	void imprimirCentrado(const char *texto) {
		int largo = strlen(texto); 
		int espacios = (ANCHO_CONSOLA - largo) / 2; 
		if (espacios < 0) espacios = 0;
		
		for (int i = 0; i < espacios; i++) {
			printf(" ");
		}
		printf("%s\n", texto);
	}
	
	void dibujarEncabezado(char *titulo) {
		limpiarPantalla(); 
		printf("\n"); 
		
		imprimirLinea('='); 
		imprimirCentrado("SISTEMA DE GESTIÓN DE CITAS MÉDICAS");
		imprimirLinea('=');
		
		char subTitulo[256]; 
		snprintf(subTitulo, sizeof(subTitulo), ":: %s ::", titulo); 
		imprimirCentrado(subTitulo);
		
		imprimirLinea('-');
	}
