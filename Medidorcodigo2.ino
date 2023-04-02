 /**
 * FreeRTOS
 */
#include "app.h"
#include "EmonLib.h"  
#include <SSD1306.h>

EnergyMonitor emon1; 

float KWh=0;
float potencia=0;
int tensao = 220;
float y=0;
float x=0;
int controle = 0;
unsigned long lastmillis = millis();
unsigned long millisFirebase = millis();




// Pinos do display (comunicação i2c)
 const int DISPLAY_ADDRESS_PIN = 0x3c;
 const int DISPLAY_SDA_PIN = 21;
 const int DISPLAY_SCL_PIN = 22;
 const int DISPLAY_RST_PIN = 16;
 //const int DISPLAY_SDA_PIN = 4;
// const int DISPLAY_SCL_PIN = 15;
// const int DISPLAY_RST_PIN = 16;


// Altura da fonte (correspondente a fonte ArialMT_Plain_8)
const int fontHeight = 5; 

// Objeto do display
SSD1306 display(DISPLAY_ADDRESS_PIN, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);

// Função que inicializa o display
bool displayBegin()
{
  // Reiniciamos o display
  pinMode(DISPLAY_RST_PIN, OUTPUT);
  digitalWrite(DISPLAY_RST_PIN, LOW);
  delay(1);
  digitalWrite(DISPLAY_RST_PIN, HIGH);
  delay(1);

  return display.init(); 
}

// Função que faz algumas configuções no display
void displayConfig()
{
  // Invertemos o display verticalmente
  display.flipScreenVertically();
  // Setamos a fonte
  display.setFont(ArialMT_Plain_10);
  // Alinhamos a fonta à esquerda
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}



void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando");
  emon1.current(35,2.5219);             // Current: input pin, calibration. 0.8387 anterior e posterior 
  config(915);

  // Iniciamos o display
  if(!displayBegin())
  {
    // Se não deu certo, exibimos falha de display na serial
    Serial.println("Display failed!");
    // E deixamos em loop infinito
    while(1);
  }

  // Configuramos o posicionamento da tela, fonte e o alinhamento do texto
  displayConfig();
}




void medidor() {

  double Irms = emon1.calcIrms(1480);  // Calculate Irms only 


  // Variável usada para indicar em qual linha o cursor deverá estar quando uma mensagem no display for exibida
  int line;

  // Limpamos o display
   display.clear();
  
  // Iniciamos na primeira linha (zero)
  line = 0;

  // Escrevemos a mensagem "Sending packet: " na primeira linha
   display.drawString(0, line, "Potencia: "); 
 
  if(Irms<0.4 )
  {
  Irms=0; 
  potencia=0;
  } 

  
  potencia = Irms*tensao;
 // Serial.print(" ");
  //Serial.print(potencia);         // Apparent power

  line=line+2;
  display.drawString(0, line * fontHeight,String(potencia));
  display.drawString(60, line * fontHeight,"W");
  line=line+2;
  display.drawString(0, line * fontHeight,"Corrente:");
  line=line+2;
  display.drawString(0, line * fontHeight,String(Irms));
  display.drawString(60, line * fontHeight,"A");
  
  
  //Serial.print("kWh: ");
 // Serial.print(KWh, 4);


  line=line+2;
  display.drawString(0, line * fontHeight,"Consumo KWh:");
  line=line+2;
  display.drawString(0, line * fontHeight,String(KWh));
  display.drawString(60, line * fontHeight,"KWh");
   

  if((millis() - millisFirebase) > 5000){
     String corrente= "";
     String consumo = "";
     consumo.concat(KWh);
     corrente.concat(Irms);

     String message =  String(KWh) + "?" + String(Irms);
 
     //getRTC();
     enviarLoRa(timestamp, "arCondConsumoKWhCorrenteA", message);
     unQueueLoRa();
    
     millisFirebase = millis();   
  }

  // Exibimos as alterações no display
  display.display();

 // Serial.print(" ");
 // Serial.println(Irms);          // Irms
  
  KWh = KWh + potencia*(millis()-lastmillis)/3600000000.0;
  lastmillis = millis();
  delay(1000);
}

void loop() {

  medidor();
 
  
}
