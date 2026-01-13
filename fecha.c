#include "fecha.h"

char fechaActual[11];

void getFechaActual(void) {
	time_t ahora = time(NULL);
	struct tm *t = localtime(&ahora);
	
	strftime(fechaActual, sizeof(fechaActual), "%d-%m-%Y", t);
}
