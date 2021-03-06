#include <OneWire.h> // Inclusion de la librairie OneWire
#include <VirtualWire.h>

uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
 
#define DS18B20 0x28     // Adresse 1-Wire du DS18B20
#define BROCHE_ONEWIRE 7 // Broche utilisée pour le bus 1-Wire
 
OneWire ds(BROCHE_ONEWIRE); // Création de l'objet OneWire ds
 
// Fonction récupérant la température depuis le DS18B20
// Retourne true si tout va bien, ou false en cas d'erreur
boolean getTemperature(float *temp){
  byte data[9], addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté
 
  if (!ds.search(addr)) { // Recherche un module 1-Wire
    ds.reset_search();    // Réinitialise la recherche de module
    return false;         // Retourne une erreur
  }
   
  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
    return false;                        // Si le message est corrompu on retourne une erreur
 
  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur
 
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
   
  ds.write(0x44, 1);      // On lance une prise de mesure de température
  delay(800);             // Et on attend la fin de la mesure
   
  ds.reset();             // On reset le bus 1-Wire
  ds.select(addr);        // On sélectionne le DS18B20
  ds.write(0xBE);         // On envoie une demande de lecture du scratchpad
 
  for (byte i = 0; i < 9; i++) // On lit le scratchpad
    data[i] = ds.read();       // Et on stock les octets reçus
   
  // Calcul de la température en degré Celsius
  *temp = ((data[1] << 8) | data[0]) * 0.0625; 
   
  // Pas d'erreur
  return true;
}
 
void setup() {
  Serial.begin(9600); // Initialisation du port série
  vw_setup(2000);
}
 
void loop() {
  float temp;
   
  if(getTemperature(&temp)) {
    
    Serial.println("Envoi"); 
    
    Serial.println(temp); 
    char bufFloat[sizeof(float)];
    memcpy(bufFloat, &temp, sizeof(float));
    
    char id = 'T';
    char dir = 'R';  
    char bufTemp[sizeof(float)+2];

    bufTemp[0] = dir;
    bufTemp[1] = id;
    for (int i = 2; i < 6; i++){
      bufTemp[i] = bufFloat[i-2];
    }
    
    for (int i = 0; i < 6; i++){
      Serial.print((char)bufTemp[i]);
    }
    Serial.println("Envoie...");

    vw_send((uint8_t *)bufTemp, sizeof(bufTemp));
    vw_wait_tx();
    Serial.println("Done !"); 
    delay(1000); 
    
  }
}
