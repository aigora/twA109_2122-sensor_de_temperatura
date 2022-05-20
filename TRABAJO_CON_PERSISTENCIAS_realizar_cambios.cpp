

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include "SerialClass/SerialClass.h"
#define MAX_BUFFER 200
#define PAUSA_MS 200
#define LONGCAD 50
#define N 1000

typedef struct
{
	int dia;
	int mes;
	int anio;
}fecha;

typedef struct
{
	fecha f[LONGCAD];
	int hora;
	int minutos;
	int temperatura;
}registro;

// Funciones prototipo
int menu(void);
void verifica_sensores(Serial*, char*);
void monitorizar_sensor_temperatura(Serial*);
void activar_alarma_temperatura(Serial* Arduino);
void comprobar_mensajes(Serial*);
float leer_sensor_temperatura(Serial*);
int Enviar_y_Recibir(Serial*, const char*, char*);
float float_from_cadena(char* cadena);
void leer_fichero_registro(registro t[], int* ptemp, int longitud);
//registro leer_datos_temperatura();
//int mostrar_datos_temperatura(registro r);

int main(void)
{
	Serial* Arduino;
	char puerto[] = "COM6"; // Puerto serie al que está conectado Arduino
	int opcion_menu;

	registro r[N]; //<------!!!!!!!!!!!!!!!

	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	do
	{
		comprobar_mensajes(Arduino);								////////1////////
		opcion_menu = menu();										////////2////////
		switch (opcion_menu)
		{
		case 0: break;
		case 1:
			verifica_sensores(Arduino, puerto);
			break;
		case 2:
			monitorizar_sensor_temperatura(Arduino);
			break;
		case 3:
			activar_alarma_temperatura(Arduino);
			break;
		case 4:
			break;
		default: printf("\nOpción incorrecta\n");
		}
	} while (opcion_menu != 4);
	return 0;
}



int menu(void)														////////2////////
{
	static int opcion = -1;

	if (opcion != 0)
	{
		printf("\n");
		printf("Menú Principal\n");
		printf("==============\n");
		printf("1 - Verificar sensores.\n");
		printf("2 - Monitorizar sensores.\n");
		printf("3 - Activar/Desactivar alarma por distancia\n");
		printf("4 - Salir de la aplicación\n");
		printf("Opción:");
	}

	if (_kbhit())
	{
		opcion = (int)_getch() - '0';
		printf("%d\n", opcion);
	}
	else
		opcion = 0;

	return opcion;
}




void verifica_sensores(Serial* Arduino, char* port)								////////2.1////////
{
	float temperatura;

	if (Arduino->IsConnected())
	{
		temperatura = leer_sensor_temperatura(Arduino);
		if (temperatura != -1)
			printf("\nTemperatura: %f\n", temperatura);
	}
	else
	{
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", port);
	}
}




float leer_sensor_temperatura(Serial* Arduino)								////////2.2.1////////o///////2.1.1/////
{
	float temperatura;
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_DISTANCIA\n", mensaje_recibido);

	if (bytesRecibidos <= 0)
	{
		printf("\nNo se ha recibido respuesta a la petición\n");
		temperatura = -1;
	}
	else
	{
		printf("\nLa respuesta recibida tiene %d bytes. Recibido=%s\n", bytesRecibidos, mensaje_recibido);
		temperatura = float_from_cadena(mensaje_recibido);
	}
	return temperatura;
}



float float_from_cadena(char* cadena)							/////////2.2.1.2 o 2.1.1.2//////////////
{
	float numero = 0;
	int i, divisor = 10, estado = 0;


	for (i = 0; cadena[i] != '\0' && estado != 3 && i < MAX_BUFFER; i++)
		switch (estado)
		{
		case 0:// Antes del número
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = cadena[i] - '0';
				estado = 1;
			}
			break;
		case 1:// Durante el número
			if (cadena[i] >= '0' && cadena[i] <= '9')
				numero = numero * 10 + cadena[i] - '0';
			else
				if (cadena[i] == '.' || cadena[i] == ',')
					estado = 2;
				else
					estado = 3;
			break;
		case 2: // Parte decimal
			if (cadena[i] >= '0' && cadena[i] <= '9')
			{
				numero = numero + (float)(cadena[i] - '0') / divisor;
				divisor *= 10;
			}
			else
				estado = 3;
			break;
		}
	return numero;
}



int Enviar_y_Recibir(Serial* Arduino, const char* mensaje_enviar, char* mensaje_recibir)        //////2.2.1.1 o 2.1.1.1/////
{																									////////2.3.1////////
	int bytes_recibidos = 0, total = 0;
	int intentos = 0, fin_linea = 0;


	Arduino->WriteData((char*)mensaje_enviar, strlen(mensaje_enviar));
	Sleep(PAUSA_MS);

	bytes_recibidos = Arduino->ReadData(mensaje_recibir, sizeof(char) * MAX_BUFFER - 1);

	while ((bytes_recibidos > 0 || intentos < 5) && fin_linea == 0)
	{
		if (bytes_recibidos > 0)
		{
			total += bytes_recibidos;
			if (mensaje_recibir[total - 1] == 13 || mensaje_recibir[total - 1] == 10)
				fin_linea = 1;
		}
		else
			intentos++;
		Sleep(PAUSA_MS);
		bytes_recibidos = Arduino->ReadData(mensaje_recibir + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
		mensaje_recibir[total - 1] = '\0';

	//printf("LOG: %d bytes -> %s\nIntentos=%d - EOLN=%d\n", total, mensaje_recibir,intentos,fin_linea);
	return total;
}



void monitorizar_sensor_temperatura(Serial* Arduino)								////////2.2////////
{
	float frecuencia, temperatura;
	char tecla;
	do
	{
		printf("Establezca frecuencia de muestreo (0,5 Hz - 2,0 Hz):");
		scanf_s("%f", &frecuencia);
	} while (frecuencia < 0.5 || frecuencia>2.0);

	printf("Pulse una tecla para finalizar la monitorización\n");
	do
	{
		if (Arduino->IsConnected())
		{
			temperatura = leer_sensor_temperatura(Arduino);
			if (temperatura != -1)
				printf("%.2f ", temperatura);
			else
				printf("XXX ");
		}
		else
			printf("\nNo se ha podido conectar con Arduino.\n");
		if ((1 / frecuencia) * 1000 > PAUSA_MS)
			Sleep((1 / frecuencia) * 1000 - PAUSA_MS);
	} while (_kbhit() == 0);
	tecla = _getch();
	return;
}




void activar_alarma_temperatura(Serial* Arduino)								////////2.3////////
{
	int bytesRecibidos;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Enviar_y_Recibir(Arduino, "SET_MODO_ALARMA\n", mensaje_recibido);
	if (bytesRecibidos <= 0)
		printf("\nNo se ha recibido confirmación\n");
	else
		printf("\n%s\n", mensaje_recibido);
}




void comprobar_mensajes(Serial* Arduino)									////////1////////
{
	int bytesRecibidos, total = 0;
	char mensaje_recibido[MAX_BUFFER];

	bytesRecibidos = Arduino->ReadData(mensaje_recibido, sizeof(char) * MAX_BUFFER - 1);
	while (bytesRecibidos > 0)
	{
		Sleep(PAUSA_MS);
		total += bytesRecibidos;
		bytesRecibidos = Arduino->ReadData(mensaje_recibido + total, sizeof(char) * MAX_BUFFER - 1);
	}
	if (total > 0)
	{
		mensaje_recibido[total - 1] = '\0';
		printf("\nMensaje recibido: %s\n", mensaje_recibido);
	}
}





void leer_fichero_registro(registro t[N], int* ptemp, int longitud)
{
	FILE* fichero; // Puntero para manipular el fichero
	int ntemp = 0; // Variable auxiliar para numero de usuarios leídos
	int i, pos; // Variable bucle y posicion final cadena
	errno_t cod_error; // Código de error tras el proceso de apertura.
	char intro[2];

	cod_error = fopen_s(&fichero, "Registro.txt", "rt"); // Se intenta abrir el fichero de texto
	if (cod_error != 0) // Si el fichero no se ha podido abrir
		*ptemp = 0; // La lista estará vacía
	else // Si el fichero ha podido abrirse
	{
		fscanf_s(fichero, "%d", &ntemp); // Se lee la cantidad de registros
		if (ntemp == 0) // Si esa cantidad es cero
			*ptemp = 0; // La lista estará vacía
		else // Si hay registros para leer (según el entero leído)
		{
			if (ntemp > longitud) // Si no hay memoria suficiente
			{
				printf("Memoria insuficiente para almacenar los datos del fichero\n");
				*ptemp = 0;
			}
			else // Si hay memoria suficiente
			{
				fgets(intro, 2, fichero); // Saltamos el intro que hay tras el número (Ascii 10 y 13)
				for (i = 0; i < ntemp; i++) // Se leen los registros uno por uno
				{
					fgets((t + i)->f, LONGCAD, fichero);
					pos = strlen((t + i)->f); // Calcula la longitud del nombre para ubicar el \n.
					(t + i)->f[pos - 1] = '\0';
					fscanf_s(fichero, "%d", &(t + i)->hora);
					fscanf_s(fichero, "%d", &(t + i)->minutos);
					fscanf_s(fichero, "%f", &(t + i)->temperatura);
					fgets(intro, 2, fichero); // Saltamos el intro que hay tras el número (Ascii 10 y 13)
				}
				*ptemp = ntemp;
			}
		}
		fclose(fichero); // Se cierra el fichero
	}
}



int escribir_fichero_registro(registro* lista, int ntemp)
{
	int i;
	FILE* fichero;
	errno_t err;
	err = fopen_s(&fichero, "Temperatura.txt", "w");
	if (err == 0) // Si el fichero se ha podido crear
	{
		fprintf(fichero, "%d\n", ntemp); // Se graba en el fichero el número de usuarios
		for (i = 0; i < ntemp; i++)
		{
			fprintf(fichero, "%s\n", (lista + i)->f);
			fprintf(fichero, "%d\n", (lista + i)->hora);
			fprintf(fichero, "%d\n", (lista + i)->minutos);
			fprintf(fichero, "%f\n", (lista + i)->temperatura);
		}
		fclose(fichero);
	}
	else
		printf("Se ha producido un problema a la hora de grabar el fichero de usuarios\n");
	return err;
}
*/






registro leer_datos_temperatura()
{
	registro r;
	char intro;

	printf("Nuevo registro\n");
	printf("Dia:");
	scanf_s("%d", &r.f.dia);
	printf("Mes:");
	scanf_s("%d", &r.f.mes);
	printf("Anio:");
	scanf_s("%d", &r.f.anio);
	printf("hora:");
	scanf_s("%d", &r.hora);
	printf("minutos:");
	scanf_s("%d", &r.minutos);
	printf("Temperatura:");
	scanf_s("%d", &r.temperatura);
	intro = getchar();

	return r;
}

int mostrar_datos_temperatura(registro r)
{
	printf("%d\t%d\t%d\t", r.f.dia, r.f.mes, r.f.anio);
	printf("%d\t%d\t", r.hora, r.minutos);
	printf("%d\n", r.temperatura);
}
