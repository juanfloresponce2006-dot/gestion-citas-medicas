\#🏥 Sistema de Gestión de Citas Médicas



\## 📋 Descripción

Proyecto integrador desarrollado en \*\*Lenguaje C\*\* para la administración de turnos médicos, pacientes y personal de salud. Aplica conceptos de modularidad, persistencia en archivos y control de versiones.



\## 👥 Integrantes

\* \*\*Juan Flores\*\*

\* \*\*Andrés De La Torre\*\*



\## 🏗️ Arquitectura del Sistema

El programa está compuesto por los siguientes módulos:

\* `main.c`: Punto de entrada del sistema.

\* `autenticacion.c/h`: Gestión de usuarios y acceso.

\* `citas.c/h`: Control de estados y observaciones.

\* `pacientes.c/h` y `medicos.c/h`: Administración de registros.

\* `fecha.c/h` y `historial.c/h`: Utilidades de tiempo y bitácora.

\* `util.c/h`: Funciones que se utilizaron recurrentemente



\## 🚀 Compilación y Ejecución

Para compilar el sistema completo, use el siguiente comando:

gcc main.c autenticacion.c citas.c fecha.c historial.c medicos.c pacientes.c utils.c -o sistema\_medico



Para ejecutar:

./sistema\_medico



\## 📸 Evidencia

!\[Captura de pantalla](./captura.png)

