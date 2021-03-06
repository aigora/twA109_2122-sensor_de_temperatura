
#include <DHT.h>      // INCLUIR LIBRERIA
#include <LiquidCrystal.h>           //importar libreria

enum Estados {MODO_MANUAL, MODO_ALARMA_ON,MODO_ALARMA_OFF};
Estados estado=MODO_MANUAL;

const int DHTPin = 10;     // what digital pin we're connected to
const int LedPin = 11;
LiquidCrystal lcd(8, 7, 6, 5, 4, 3, 2);    //crear el objeto con los pines que se van a usarxxxxxxxxxx
DHT dht(10, DHT11);    // CREAR UN OBJETO PASANDO PIN Y TIPO DE SENSOR POR PARAMETROS

String mensaje_entrada;
String mensaje_salida;

void setup() 
{
   //delay(6000); //6 segundos de espera
   Serial.begin(9600);
   dht.begin();        // INICIALIZAR EL DHT
   lcd.begin(16, 1); // inicializar la pantalla con la cantidad de (2) fila y (16)columna que esta tiene
   pinMode(LedPin, OUTPUT);
}

void loop ()
{
  delay(200);
  procesar_mensajes();
  switch (estado)
  {
    case MODO_MANUAL:
        break;
    case MODO_ALARMA_ON:
        modo_alarma(5.0,15.0,2000,1); //<-- ENTRADAS PARA LA FUNCION DE ALARMA??
        break;
    case MODO_ALARMA_OFF:
        modo_alarma(5.0,15.0,2000,0);
        estado=MODO_MANUAL;
        break;
  }
}

void procesar_mensajes(void)
{
  if( Serial.available()> 0) 
  {
    mensaje_entrada = Serial.readStringUntil('\n');
    if (mensaje_entrada.compareTo("GET_TEMPERATURA")==0)
    {

      //PARTE COPIADA DEL CODIGO VIDEO_21
      //:::::::::::::::::::::::::::::::::::::::::::://
      float TemC = dht.readTemperature();            // GRADOS CELCIUS
   
      if (isnan(TemC)) // SI LO QUE DEBUELVE NO ES UN NUMERO, IMPRIMIR MENSAJE DE ERRROR
      {          
          lcd.setCursor(0, 0); //COLOCAR EN POSICION
          lcd.print("Revisar conexion");
      }
      else
      {
          lcd.setCursor(0, 0); //COLOCAR EN POSICION
          lcd.print("Tem:" + String(TemC, 1) + "C  ");
      }
      delay(5000);   //ESPERAR 5 SEGUNDOS
    //:::::::::::::::::::::::::::::::::::::::::::://
      
      
      mensaje_salida=String("Tem:" + String(TemC, 1) + "C  " );
    }
    else
      if (mensaje_entrada.compareTo("SET_MODO_ALARMA")==0)
      {
        if (estado==MODO_ALARMA_OFF || estado==MODO_MANUAL)
        {
            estado=MODO_ALARMA_ON;
            mensaje_salida=String("ALARMA_OPERATIVA");
            lcd.print("ALARMA_OPERATIVA");
        }
        else
        {
          estado=MODO_ALARMA_OFF;
          mensaje_salida=String("ALARMA_INACTIVA");
          lcd.print("ALARMA_INACTIVA");
         }
      }
      else
        mensaje_salida="COMANDO DESCONOCIDO";
        lcd.print("COMANDO DESCONOCIDO");

    Serial.println(mensaje_salida);
    lcd.print(mensaje_salida);
  }
}



void modo_alarma(float tem_max,int ms_sup, int ms_led, int modo)
{
  static int  estado_alarma=0; // Estado actual
  static unsigned long  tiempo_alarma=0; // Ref. de tiempo alarma
  static unsigned long tiempo_led_on=0; // Ref. de tiempo led on
  float tem=0.0; // Temperatura captada
  int i,rango; 

  if (modo==0)
  {
    estado_alarma=0;
    digitalWrite(LedPin,LOW);
  }
  else
    {
      float tem = dht.readTemperature();
      rango = tem>=tem_max?1:0;// ??Hay obst??culo en el rango?
      switch (estado_alarma)
      {
        case 0: // Estado inicial
         if (rango==1) // Si la temperatura supera la tcrit
         {
           tiempo_alarma=millis(); // Referencia temporal
           estado_alarma=1; // Cambia al estado 1
         }
         break;


         case 1: // Se acaba de superar recientemente la tcrit
           if (millis()-tiempo_alarma>ms_sup) // Si la superaci??n ha permanecido el tiempo m??nimo requerido
           {
             estado_alarma=2; // Cambio de estado
             digitalWrite(LedPin,HIGH); // Activa el led de alarma
             Serial.println("ATENCION: SE HA SUPERADO LA TEMPERATURA CRITICA");
             lcd.print("ATENCION: SE HA SUPERADO LA TEMPERATURA CRITICA");
           }
           else
           if (rango==0)  // No se ha superado la temperatura durante el tiempo minimo
             estado_alarma=0;  // 
        break;


        case 2: // Alarma activa
         if (rango==0)  // Si deja de superarse la temperatura
         {
           tiempo_led_on=millis(); // Inicia temporizaci??n para mantener el led 
           estado_alarma=3; // Cambio de estado
         }
         break;


         case 3: // Se ha dejado de superar la tcrit pero el led continua encendio
         if (millis()-tiempo_led_on>ms_led) // Si ha pasado el tiempo del led on
         {
           digitalWrite(LedPin,LOW); //Apaga led
           estado_alarma=0; //Regresa a estado inicial
           Serial.println("ATENCION: YA NO HAY RIESGO POR SOBRECALENTAMIENTO");           
         }
         else
          if (rango==1) // Si se vuelve a superar tcrit
            estado_alarma=2; // Regresa a estado 2
      }
    }
  
}
