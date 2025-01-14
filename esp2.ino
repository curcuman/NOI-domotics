#include <WiFi.h>
#include "ThingSpeak.h" 

//AGGIUNGERE DI RITENTARE se falliscce (se stato è 0)
//tenta tante ricezioni e pochi invii

#ifdef ENABLE_DEBUG
       #define DEBUG_ESP_PORT Serial
       #define NODEBUG_WEBSOCKETS
       #define NDEBUG
#endif 
#include <Arduino.h>

//DATI WIFI
const char* ssid = "NOI extender";                  // network SSID (name) 
const char* password = "circolonoi";   //  network password

WiFiClient  client;

//DATI THINGSPEAK
unsigned long myChannelNumber = 1945358;
const char * myWriteAPIKey = "5D00IJ3X86A8ZC43";


bool stato_wifi;
int ledPin = 13;
int buttonPin = 26;

void setup() {
  Serial.begin(115200);  //Initialize serial

  delay(2000);
  Serial.print("\n----Starting Program----\n");

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak


  Serial.println();
}

byte comando = 0;
int comandoPrima=0; //assumo all'inizio tutto spento.. da sistemare
void loop() {
  /*
  Serial.println("######## Lettura e invio #########");
  delay(1000);
  int dato = digitalRead(buttonPin);
  Serial.print("Dato letto dall'esp: ");
  Serial.println(dato);
  stato_wifi = InviaDato(dato);
  
  Serial.print("Stato invio tramite wifi: ");
  Serial.println(stato_wifi);
  delay(15000);
  */

  for (int i=0; i<15; i++){
    Serial.println("--- Ricezione e azione ---");
    comandoPrima = comando;
    comando = ricezioneComando();
    Serial.print("Stato ricezione tramite wifi: ");
    Serial.println(stato_wifi);

    if(comandoPrima != comando){
      //applico l'impulso
    digitalWrite(ledPin, 1);
    delay(800);
    digitalWrite(ledPin, 0);
   
    }
   delay(1000); 
  }


}

int ricezioneComando(){
   //controllo wifi, che se non c'è spegne il dispositivo
   if(WiFi.status() != WL_CONNECTED){
    stato_wifi = false;
    return 0;
   }

   //ricevo il dato sul comando da thingspeak (e colgo occasione per vedere se quello caricato va bene)
   float  valore_caricato = ThingSpeak.readFloatField(myChannelNumber, 1);
   int comando = ThingSpeak.readFloatField(myChannelNumber, 2);
   Serial.println("Leggo i dati che sono online (prima l'ultimo valore caricato e poi il comando appena ricevuto):");
   Serial.println(valore_caricato);
   Serial.println(comando);

   return comando;
}

bool InviaDato(float valore){
    int tentativi=0;
    // Connect or reconnect to WiFi: OTTIMO modo se si stacca ritenta
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Waiting to connect");
      while(WiFi.status() != WL_CONNECTED && tentativi<10){
        WiFi.begin(ssid, password); 
        tentativi++;
        delay(5000);     
      } 
      Serial.println("\nConnected, now I send");
    }else{
      Serial.println("\nAlready connected, now I send");
    }

    int x = ThingSpeak.writeField(myChannelNumber, 1, valore, myWriteAPIKey);
    Serial.print("valore inviato: ");
    Serial.println(valore);
    if(x == 200) return true;
    else return false;
  
}
