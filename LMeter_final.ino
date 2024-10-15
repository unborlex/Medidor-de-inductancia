
#include <EEPROM.h>


int pin = 13;     //entrada de pulsos
int pout = 14;    //salida pulso
float   frecuencia, inductor, inductor0, ind_promedio;
float   capacitor = 0.000002;                    //ESTE ES EL COMPONENTE PATRON Y DEBE SER MEDIDO BIEN O DEBE SER DE BUENA CALIDAD
float   ind_default = 0.000020;
char    menu_in;
int     vuelta;
long    ultimo_tiempo;
char    cap_in[]= {32,32,32,32,32};
int     cap_inint;
float   cap_default; 
float   ind_ee;

void setup()
{
            
            //inductor0 = 0.000029;            
            Serial.begin(115200);
            pinMode(pin, INPUT);
            pinMode(pout,OUTPUT);
            EEPROM.begin(20);
            cal_inductor();
            cal_capacitor();
            presentacion();
            limpiar();
            menu_in = '0';
            menu();
            ultimo_tiempo = millis();
}

void loop()
{
  
      recepcion();
      if(millis() - ultimo_tiempo > 8000 )
      {
                           ultimo_tiempo = millis();
                           Serial.println("");
                           Serial.println("");
                           Serial.println("");
                           Serial.println("");
                           Serial.println("");
                           Serial.println("");
                           Serial.println("");
                           menu();
      }
      while(menu_in == '1')
      {
                  Serial.println ("");
                  Serial.println ("");
                  Serial.println ("");
                 
                  Serial.print("Ingrese nuevo valor de capacitor patron en nF (100-10000):");
                  while(Serial.available() == 0)
                  {
                                             
                           delay(50);
                                            
                  }
                  delay(300);
                  while(Serial.available() > 0)
                  {

                              if( cap_in[4] != '\r')
                              {
                                        cap_in[0] = cap_in[1];
                                        cap_in[1] = cap_in[2];
                                        cap_in[2] = cap_in[3];
                                        cap_in[3] = cap_in[4];
                                        cap_in[4] = Serial.read();
                                                              
                              }
                              else
                              {
                                        Serial.read();            // vacio buffer
                              }                
                  }
                  for(vuelta = 0; vuelta<4 ; vuelta++)
                  {
                                Serial.print(cap_in[vuelta]);
                  }
                  if( ((cap_in[3]-48) >=0) && ((cap_in[3]-48) <=9))
                  {
                                cap_inint = (int)(cap_in[3]-48);
                                if( ((cap_in[2]-48) >=0) && ((cap_in[2]-48) <=9))
                                {
                                          cap_inint = cap_inint + ((int)(cap_in[2]-48) * 10);
                                          if( ((cap_in[1]-48) >=0) && ((cap_in[1]-48) <=9))
                                          {
                                                cap_inint = cap_inint + ((int)(cap_in[1]-48) * 100);
                                                if( ((cap_in[0]-48) >=0) && ((cap_in[0]-48) <=9))
                                                {
                                                        cap_inint = cap_inint + ((int)(cap_in[0]-48) * 1000);
                                                }
                                          }
                                
                                }                        
                                if((cap_inint<=10000)&&(cap_inint>=100))
                                {     
                                        Serial.print("  Valor guardado = ");
                                        Serial.print(cap_inint);
                                        capacitor = ((float)(cap_inint)* 0.000000001);
                                        EEPROM.put(10, capacitor );
                                        EEPROM.commit();
                                        
                                }
                                else
                                {
                                         Serial.println ("  Valor fuera de rango volver a ingresarlo!!");
                                }
                                
                                
                  }
                  else
                  {
                                 Serial.println ("  Valor incorrecto volver a ingresarlo!!");
                                 delay(1000);   
                  }
           

                  cap_in[0] = 32;
                  cap_in[1] = 32;
                  cap_in[2] = 32;
                  cap_in[3] = 32;
                  cap_in[4] = 32;
                  
                  Serial.println ("");
                  Serial.println ("");
                  Serial.println ("");
                  delay(1000);   
                  menu_in = '0';
                  menu();
                  ultimo_tiempo = millis();
      }
      while(menu_in == '2')
      {

                  
                  Serial.println ("");
                  Serial.println ("");
                  Serial.println ("");
                 
                  Serial.print("Calibrando");
                  for(vuelta=0; vuelta<5; vuelta++)
                  {
                          Serial.print(".");
                          delay(500);
                          medir();
                          ind_promedio = ind_promedio + inductor;
                  }
                  ind_promedio = ind_promedio / 5;
                  Serial.println("");
                  if((ind_promedio > 0.000005)&&(ind_promedio < 0.000200  ))
                  {
                                Serial.print ("Inductor interno calibrado = ");
                                Serial.print((long)(ind_promedio * 1000000000));
                                Serial.println("nHy");
                                Serial.println("Grabando en EEPROM");
                                EEPROM.put(0, ind_promedio);
                                EEPROM.commit(); 
                                inductor0 = ind_promedio;
                  }
                  else
                  {
                                Serial.println("Valores fuera de escala para el inductor interno ( 5-200 mHy )");
                  }          
                  
                  delay(4000);
                
                  Serial.println ("");
                  Serial.println ("");
                  Serial.println ("");  
                  menu_in = '0';
                  menu();
                  ultimo_tiempo = millis();
      }
      while(menu_in == '3')
      {
                  medir();
                  if(inductor != 0)
                  {
                          Serial.println (""); 
                          Serial.print("Inductor = ");
                          Serial.print((long)((inductor-inductor0) * 1000000000.0)); 
                          Serial.println("nHy");
                  }
                  else
                  {
                          Serial.println("Conecte un inductor entre los terminales de medicion!! ");
                  }
                  Serial.println ("");
                  Serial.println ("");
                  Serial.println ("");  
                          
                  menu_in = '0';
                  menu();
                  ultimo_tiempo = millis();
      }
      while(menu_in == '4')
      {                       
              medir();
              if(inductor != 0)
              {
                    Serial.print("Inductor = ");
                    Serial.print((long)((inductor-inductor0) * 1000000000.0)); 
                    Serial.println("nHy");
              }
              else
              {
                    Serial.println("Conecte un inductor entre los terminales de medicion!! ");
              }   
              
              recepcion();
              if(menu_in != '4')
              {
                     menu();
              }
              delay(100); 
              ultimo_tiempo = millis();               
      }      

      
}

void      cal_inductor()
{
              
              EEPROM.get(0, ind_ee);   
              if((ind_ee > 0.0000001)&&(ind_ee < 0.000200  ))
              {
                          inductor0 = ind_ee;
              }
              else
              {                          
                          EEPROM.put(0, ind_default);
                          EEPROM.commit(); 
                          inductor0 = ind_default;                         
              }

              
              
}
void    cal_capacitor()
{
              EEPROM.get(10, cap_default);   
              if((cap_default >= 0.0000001)&&(cap_default <= 0.00001  ))
              {
                          capacitor = cap_default;
              }
              else
              {
                                                                 
                          EEPROM.put(10, capacitor);
                          EEPROM.commit();
                          
              }
}
void    presentacion()
{
              delay(1000);
              Serial.println("");
              Serial.println("");
              for(vuelta=5; vuelta>=0; vuelta--)
              {
                        Serial.print(vuelta);
                        Serial.print(".");
                        delay(1000);
              }
              Serial.println("");
              Serial.println("Iniciando......");              
              delay(1000);
}
void   medir()
{

     
      unsigned long  duracionL, duracionH, tiempo_total;
      float  tiempo;
    

      
      digitalWrite(pout,HIGH);
      delay(1);
      digitalWrite(pout,LOW);
      delayMicroseconds(15);      
      duracionL = pulseIn(pin, LOW, 500);      // mide el ancho del semicilo LOW demora max 500 us
      delay(30);

      digitalWrite(pout,HIGH);
      delay(1);
      digitalWrite(pout,LOW);
      delayMicroseconds(15);      
      duracionH = pulseIn(pin, HIGH, 500);      // mide el ancho del semicilo HIGH demora max 500 us
    
      tiempo_total = duracionH + duracionL;
      tiempo = (float)tiempo_total * 0.000001;
      frecuencia = 1 / tiempo;
      
      inductor =(1 / (frecuencia * frecuencia * 39.4786 * capacitor));      // en Hy
  
}
void     limpiar()
{
      int a;
      for(a=0; a<100; a++)
      { 
                Serial.println("");        
      }  
}
void      recepcion()
{
      
                if(Serial.available() > 0)
                {
                           menu_in = Serial.read();          // leo el primer caracter
                           delay(400);                        //espero fin de recepcion
                           while( Serial.available() > 0 )   // vacio el buffer
                           {
                                    Serial.read();
                           }                            
                           Serial.println(menu_in);         //muestro seleccion
                           delay(700);      
                }
}

  
void    menu()
{
      Serial.print("    1 - Cambiar valor del capacitor patron -> ");
      Serial.print((long)(capacitor * 1000000000.0));
      Serial.println(" nF");
      Serial.print("    2 - CALIBRACION.  Cortocircuite la entrada de medicion ( bobina de referencia actual -> ");
      Serial.print((long)(inductor0 * 1000000000.0));
      Serial.println(" nHy)");
      Serial.println("    3 - MEDIR. Solo una medicion");
      Serial.println("    4 - MEDIR CONTINUO. Para finalizar envie '0'");
      Serial.println("");
      Serial.print("    Ingrese opcion: ");
      
}
