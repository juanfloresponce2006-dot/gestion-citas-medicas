/* File: utils.h */
#ifndef UTILS_H
#define UTILS_H

#define ANCHO_CONSOLA 120

// Prototipos de validación
int numVerify(char number[]);
void cleanBuffer();
int esTextoConEspacios(const char *txt);
int esHorarioValido(const char *h); // Valida rangos HH:MM-HH:MM
int esSoloNumeros(const char *txt);
int esHoraValida(const char *h);    // Valida horas sueltas HH:MM
int validarFechaNacimiento(const char *fecha); // Rango 1900-2025
int validarFechaCita(const char *fecha);       // Rango 2024-2100
int validacionEdad(char *buffer);


// Prototipos de UI
void limpiarPantalla();
void pausa();
void imprimirCentrado(const char *texto);
void imprimirLinea(char caracter);
void dibujarEncabezado(char *titulo);

#endif
