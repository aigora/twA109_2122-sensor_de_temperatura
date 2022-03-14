#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int menu_ppal(void)
{
	float T; // La T de temperatura viene dada por el sensor
	int tiempo;

	for (tiempo =0; tiempo<100000; tiempo++)
	{
		if (tiempo % 30 == 0)
		{
			scanf_s("%f", &T);
			if (T < 22)
			{
				printf("La temperatura al tiempo %d es %.2f", tiempo, T);
				set("LED_ROJO OFF");
				set("LED_VERDE ON");
			}
			else
			{
				printf("ALERTA! Se ha superado la temperatura recomendada");
				set("LED_VERDE OFF");
				set("LED_ROJO ON");
			}

			//ALMACENAMOS LA INFORMACION
		}
	}
}
