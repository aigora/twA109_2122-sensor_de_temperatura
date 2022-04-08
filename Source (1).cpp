#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "SerialClass/SerialClass.h"

#define MAX_TEMP 20
#define PAUSA_MS 200

void verifica_sensores(Serial* Arduino, char* port);
int menu(void);
float leer_sensor_temperatura(Serial* Arduino);


int menu(void)
{
	int opcion;

	printf("\n");
	printf("Menu Principal\n");
	printf("==============\n");
	printf("1-Verificar sensores.\n");
	printf("2-Salir la aplicación\n");
	printf("Opción:");
	scanf_s("%d", &opcion);
	return opcion;
}


int main(void)
{
	Serial* Arduino;
	char puerto[] = "COM6";
	int opcion_menu;

	setlocale(LC_ALL, "es-ES");
	Arduino = new Serial((char*)puerto);
	do
	{
		opcion_menu = menu();
		switch (opcion_menu)
		{
		case 1:
			verifica_sensores(Arduinoo, puerto);
			break;
		case 2:
			break;
		default: printf("\nOpción incorrecta\n");
		}
	} while (opcion_menu != 2);
	return 0;
}



void verifica_sensores(Serial* Arduino, char* port)
{
	float temperatura;

	if (Arduino->IsConnected())
	{
		distancia = leer_sensor_temperatura(Arduino);
		if (temperatura != -1)
			printf("\nTemperatura: %f\n", temperatura);
	}
	else
	{
		printf("\nNo se ha podido conectar con Arduino.\n");
		printf("Revise la conexión, el puerto %s y desactive el monitor serie del IDE de Arduino.\n", port);
	}
}


float leer_sensor_temperatura(Serial* Arduino)
{
	float temperatura;
	int bytesRecibidos;
	char mensajes_recibido[MAX_TEMP]

	bytesRecibidos = Enviar_y_Recibir(Arduino, "GET_TEMP\n", mensaje_recibido);

	if (bytesRecibidos <= 0)
	{
		printf("\nNo se ha recibido respuesta a la peticion\n");
		temperatura = -1;
	}
	else
	{
		printf("\nLa respuesta recibida tiene %d bytes. Recibido=%s\n", bytesRecibidos, mensaje_recibido);
		temperatura = float_from_cadena(mensaje_recibido);
	}
	return temperatura;
}