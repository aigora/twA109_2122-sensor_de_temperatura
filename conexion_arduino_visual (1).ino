void setup() 
{
  Serial.begin(9600);
  while(!Serial){;}  
}

void loop() 
{
  if(Serial.available()>0)
  {
    String str = Serial.readStringUntil('\n');

    if(str.compareTo("GET_TEMPERATURA")==0)
      str="TEMPERATURA=25.378";
    else
      str="COMANDO DESCONOCIDO";
    Serial.println(str);    
   }
}
//////////////////
