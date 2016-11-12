/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Programme ACTIVATOR pour INMOOV                                 *
 *                                                                 *
 * Créé le : 22/09/16                                              *
 * Auteur  : Dominique Gyselinck                                   *
 *                                                                 *
 * Ce système permet de :                                          *
 * - Gérer le servo de la bouche en fonction du niveau sonore      *
 * - Gérer le NEO ring                                             *
 * - Superviser les niveaux des batteries moteurs et électronique. *
 *   Attention, seul 2 batteries sont gérées en attendant celle des*
 *   jambes.                                                       *
 * - Gérer les démarrages et extinctions de INMOOV                 *
 * - Gére l'amplificateur, controle volume et shutdown             *
 *                                                                 *
 * Librairies utilisées :                                          *
 *                                                                 *
 * Adafruit_NeoPixel, Servo, Wire.                                 *
 *                                                                 *
 * Compilateur : Arduino version 1.6.12                            *
 *                                                                 *
 * Fonctionnement:                                                 *
 *                                                                 *
 * A la mise sous tension, Activator est en standby et attend une  *
 * action sur PRESENCE_SENSOR (mise à la masse)                    *
 * Une fois activé, activator active l'alimentation des moteurs,   *
 * puis l'électronique, et enfin le PC.                            *
 * Nous sommes en mode réveil.                                     *
 * Dés que le PC est démarré avec MRL et tout les services, celui- *
 * ci doit envoyé une commande à Activator.                        *
 * Celui ci démarre alors en mode normal.                          *
 * Le servo Jaw est active.                                        *
 * Pour éteindre InMoov, MRL doit prévoir de fermer proprement     *
 * tout les services et envoyer une commande a activator           *
 * Celui ci va couper les alimentations 30s aprés avoir reçu la    *
 * commandes puis se met en veille.                                *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 * Suivi des modifications :                                       *
 * Date       | Auteur        | Description                        *
 * --------------------------------------------------------------- *
 * 22/09/2016 | D. Gyselinck  | Création du fichier                *
 *			                                                           *
 * 09/10/2016 | D. Gyselinck  | V0.5.0 beta test                   *
 * 10/10/2016 | D. Gyselinck  | V0.6.0 beta test                   *
 *     - Ajout lecture tension batterie B:1, ou B:2,               *
 *     - Ajout ring en fonction de l'audio                         *
 *     - Ajout de la commande V: pour connaître le num de version  *
 *     - Correction bug                                            *
 * 11/10/2016 | D. Gyselinck  | V0.7.0 beta test                   *
 *     - Ajout animation coloré (source Vincent)                   *
 *     - Prise en charge du module MAX9744                         *
 *     - Optimisation du code                                      *
 * 13/10/2016 | D. Gyselinck  | V0.8.0 beta test                   *
 *     - Ajout animation pendant le démarrage du PC                *
 * 21/10/2016 | D. Gyselinck  | V1.0.0 beta test                   *
 *     - Refonte total du programme avec le protocole actMrlComm   *
 * 25/10/2016 | D. Gyselinck  | V1.1.0 release                     *
 *     - Paramètre min et max du servo en EEPROM                   *
 *     - Correction de bug                                         *
 * 28/10/2016 | D. Gyselinck  | V1.2.0 release                     *
 *     - Ajout de la fonctionnalité watchdog                       *
 *     - Ajout de commande MRL                                     *
 *     - Correction de bug                                         *
 * 31/10/2016 | D. Gyselinck  | V1.3.0 release                     *
 *     - Correction bug NeoRing fonction de l'audio                *
 *     - Correction bug servo Jaw en fonction de l'audio           *
 * 12/11/2016 | D. Gyselinck  | V1.4.0 release                     *
 *     - Controle de l'alimentation des servos par Python          *
 *     - Choix de la couleur des pixels au sommeil (a mettre dans  *
 *       le define)                                                *
 *     - Suppression du timer2 car l'interruption perturber la com *
 *                                                                 *
 *                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <Adafruit_NeoPixel.h>
#include <Servo.h> 
#include <Wire.h> 

#include "ActMrlComm.h"

// Numéro de version Activator
#define V1                  1
#define V2                  4
#define V3                  0

// Couleur du ring pendant le sommeil
// 0 = noir
// 1 = jaune
// 2 = rouge
// 3 = vert
// 4 = bleu
// 5 = cyan
// 6 = rose
// 7 = blanc
#define PIXEL_COLOR         4

// Puissance d'éclairement pendant le sommeil
// supérieur à 5 et inférieur à 255
#define COLOR_POWER         80

// Tension de référence AREF en mV
#define REF_VOLTAGE         4000

// Définition des pins du nano
#define PRESENCE_SENSOR_PIN 2    // PIN D2
#define NEO_PIN             6    // PIN D6
#define JAW_SERVO_PIN       7    // PIN D7
#define ON_ELEC_PIN         8    // PIN D8
#define OFF_ELEC_PIN        9    // PIN D9
#define ON_MOT_PIN          10   // PIN D10
#define OFF_MOT_PIN         11   // PIN D11
#define PC_POWER_PIN        12   // PIN D12
#define LED_VEILLEUSE       13   // PIN D13

// Valeur par défaut sauvegardé en EEPROM
// Min et max du servo à régler en fonction de InMoov
#define SERVO_MIN           30  // Bouche fermé
#define SERVO_MAX           90  // Bouche ouverte
#define SERVO_AUDIO_MAX     60  // Bouche ouverte avec audio

// Nombre de pixels du ring
#define NUMPIXELS           16

// Définition des seuils à partir duquel on signal l'erreur batterie
#define BAT1_SEUIL          12050
#define BAT2_SEUIL          6050

// Adresse I2C du MAX9744
#define MAX9744_I2CADDR     0x4B

// Déclaration de l'objet NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
// Déclaration de l'objet servo
Servo jawServo;
// Objet static actMrlComm
ActMrlComm actMrlComm;

// Déclaration des variables globals
int sensorVal = 0;
int audioVal = 0;
int seuilVal = 0;
byte srVal = 0;
byte sgVal = 0;
byte sbVal = 0;

int cptBat1Failed = 0;
int cptBat2Failed = 0;
int pixelColorInSleep = 0;

unsigned long long processTime;

boolean wakeUp = false;
boolean batteryEleIsFailed = false;
boolean batteryMotIsFailed = false;
boolean neoState = false;
boolean bUpdateRing = false;
boolean bJawIsOpen = false;
boolean closeRequest = false;
boolean openRequest = false;
boolean openJawRequest = false;
boolean state = false;

float bat1Value = 0.0;
float bat2Value = 0.0;

// Compteurs
int cptDetach = 0;
int pixelCpt = 0;
int icptSeconde = 0;
int iCptVeille = 0;
int iCpt5 = 0;
int iCpt100 = 0;
int iCpt500 = 0;

///////////////////////////////
// Déclaration des fonctions //
///////////////////////////////

/** 
 * Commande du relais bistable de l'électronique générale
 * état ON
 */
void PowerElecON(void)
{
  digitalWrite(ON_ELEC_PIN, HIGH);
  delay(200);
  digitalWrite(ON_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'électronique générale
 * état OFF
 */
void PowerElecOFF(void)
{
  digitalWrite(OFF_ELEC_PIN, HIGH);
  delay(200);
  digitalWrite(OFF_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état ON
 */
void PowerMotON(void)
{
  digitalWrite(ON_MOT_PIN, HIGH);
  delay(200);
  digitalWrite(ON_MOT_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état OFF
 */
void PowerMotOFF(void)
{
  digitalWrite(OFF_MOT_PIN, HIGH);
  delay(200);
  digitalWrite(OFF_MOT_PIN, LOW);
}

/** 
 * Pulse de démarrage du PC
 */
void PcPowerPulse(void)
{
  digitalWrite(PC_POWER_PIN, HIGH);
  delay(1000);
  digitalWrite(PC_POWER_PIN, LOW);
}

/** 
 * Animation durant le mode veille
 * Indique que InMoov dort...
 */
void sleepAnimation(int val, int power)
{
  int i, j;
  
  digitalWrite(LED_VEILLEUSE, HIGH);

  for (j = 0; j < power; j++)
  {
    for (i = 0; i < pixels.numPixels(); i++)
    {
      if (val == 1)
      {
        pixels.setPixelColor(i, pixels.Color(j,j,0));
      }
      else if (val == 2)
      {
        pixels.setPixelColor(i, pixels.Color(j,0,0));
      }
      else if (val == 3)
      {
        pixels.setPixelColor(i, pixels.Color(0,j,0));
      }
      else if (val == 4)
      {
        pixels.setPixelColor(i, pixels.Color(0,0,j));
      }
      else if (val == 5)
      {
        pixels.setPixelColor(i, pixels.Color(0,j,j));
      }
      else if (val == 6)
      {
        pixels.setPixelColor(i, pixels.Color(j,0,j));
      }
      else if (val == 7)
      {
        pixels.setPixelColor(i, pixels.Color(j,j,j));
      }
      else
      {
        pixels.setPixelColor(i, pixels.Color(0,0,0));
      }
    }
    pixels.show();
    delay(10);
  }

  digitalWrite(LED_VEILLEUSE, LOW);
  
  for (j = power; j >= 0; j--)
  {
    for (i = 0; i < pixels.numPixels(); i++)
    {
      if (val == 1)
      {
        pixels.setPixelColor(i, pixels.Color(j,j,0));
      }
      else if (val == 2)
      {
        pixels.setPixelColor(i, pixels.Color(j,0,0));
      }
      else if (val == 3)
      {
        pixels.setPixelColor(i, pixels.Color(0,j,0));
      }
      else if (val == 4)
      {
        pixels.setPixelColor(i, pixels.Color(0,0,j));
      }
      else if (val == 5)
      {
        pixels.setPixelColor(i, pixels.Color(0,j,j));
      }
      else if (val == 6)
      {
        pixels.setPixelColor(i, pixels.Color(j,0,j));
      }
      else if (val == 7)
      {
        pixels.setPixelColor(i, pixels.Color(j,j,j));
      }
      else
      {
        pixels.setPixelColor(i, pixels.Color(0,0,0));
      }
    }
    pixels.show();
    delay(10);
  }
}

/** 
 * Watchdog Interrupt Service est exécité lors d'un timeout du WDT
 */
ISR(WDT_vect) 
{
  sleepAnimation(pixelColorInSleep, COLOR_POWER);
}

/** 
 * Initialisation du watchdog sur 8s
 */
void setup_watchdog(void) 
{
  // Clear the reset flag.
  MCUSR &= ~(1<<WDRF);
  // In order to change WDE or the prescaler, we need to
  // set WDCE (This will allow updates for 4 clock cycles).
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout prescaler value 8s
  WDTCSR = 1<<WDP0 | 1<<WDP3;
  // Enable the WD interrupt (note no reset).
  WDTCSR |= _BV(WDIE);
}

/** 
 * Interruption présence ou bouton
 */
void presenceSensorInterrupt(void)
{
  // Désactive toutes les interruptions
  cli();
  
  delay(50);
  
  wakeUp = false;
  
  // 2éme lecture pour être sur que l'on veux
  // réveiller InMoov...
  if (digitalRead(PRESENCE_SENSOR_PIN) == LOW)
  {
    // Démarrage...
    wakeUp = true;
  }
  else
  {
    // Réactive toutes les interruptions
    sei();
  }
}

/** 
 * Mets le processeur en mode sommeil
 */
void sleepNow(void) 
{
  // Activation du mode veille
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
 
  // Interruption quand D2 passe à 0
  attachInterrupt(0, presenceSensorInterrupt, LOW); 
  // Interruption watchdog
  setup_watchdog(); 

  // Mode veille, le microcontroleur ne tourne plus
  sleep_mode();

  // Réveille
  sleep_disable();
  // Désactive l'interruption de D2
  detachInterrupt(0);
  // Désactive le watchdog
  wdt_disable();
}

/** 
 * Retourne le nombre de millisecondes depuis le démarrage du programme.
 *
 * @return Le nombre de millisecondes depuis le démarrage du programme sous la forme d'un
 * nombre entier sur 64 bits (unsigned long long).
 * Permet d'éviter le débordement de la fonction millis() aprés 50 jours.
 * le débordement aura lieu dans:
 * 584 942 417 années, 129 jours, 14 heures, 25 minutes, 51 secondes et 616 millisecondes
 */
unsigned long long superMillis(void) 
{
  static unsigned long nbRollover = 0;
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  
  if (currentMillis < previousMillis)
  {
    nbRollover++;
  }
  previousMillis = currentMillis;

  unsigned long long finalMillis = nbRollover;
  finalMillis <<= 32;
  finalMillis +=  currentMillis;
  return finalMillis;
}

/** 
 * Input a value 0 to 255 to get a color value.
 * The colours are a transition r - g - b - back to r.
 */
uint32_t Wheel(byte WheelPos) 
{
  if (WheelPos < 85) 
  {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if (WheelPos < 170) 
  {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else 
  {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

/** 
 * Ecriture de la première config en EEPROM
 */
boolean writeFirstConfig(void)
{
  if (EEPROM.read(0) != 0xAA)
  {
    // La config n'a jamais été sauvegardé
    EEPROM.write(0, 0xAA);
    
    // Version
    EEPROM.write(1, V1);
    EEPROM.write(2, V2);
    EEPROM.write(3, V3);
    
    // Valeur min/max de la bouche
    EEPROM.write(10, actMrlComm.servoMin);
    EEPROM.write(11, actMrlComm.servoMax);
    EEPROM.write(12, pixelColorInSleep);
  }
}

/** 
 * Lecture de la config EEPROM
 */
void readConfig(void)
{
  actMrlComm.servoMin = EEPROM.read(10);
  actMrlComm.servoMax = EEPROM.read(11);
  pixelColorInSleep = EEPROM.read(12);
}

/** 
 * Réglage du volume audio MAX9744
 */
boolean setVolume(int8_t v) 
{
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
  
  Wire.beginTransmission(MAX9744_I2CADDR);
  Wire.write(v);

  if (Wire.endTransmission() == 0)
    return true;
  else
    return false;
}

/** 
 * Gestion du démarrage de InMoov
 * Active les 2 batteries, allume le PC.
 * A ce stade, on attend une confirmation d'une commande PC allumé.
 */
void startupProcess(void)
{
  int i = 0;
  
  digitalWrite(LED_VEILLEUSE, HIGH);
  // Utilisation du ring pour visualiser l'état de l'avancement
  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  // Etape 1
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,100,0));
  }
  pixels.show();

  // Reférence externe à 4V pour ADC
  analogReference(EXTERNAL);  // AREF
  // Démarrage I2C
  Wire.begin();
  actMrlComm.max9744IsOK = false;
  
  // Vérifie la présence du module audio
  // en mettant le volume par défaut
  if (setVolume(actMrlComm.volAudio)) 
  {
    actMrlComm.max9744IsOK = true;
  }
  delay(2000);

  // Etape 2
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 4), pixels.Color(0,100,0));
  }
  pixels.show();

  // Alimentation électronique générale
  PowerElecON();
  delay(2000);

  // Etape 3
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 8), pixels.Color(0,100,0));
  }
  pixels.show();

  // Allumage du PC
  PcPowerPulse();
  delay(2000);

  // Etape 4
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 12), pixels.Color(0,100,0));
  }
  pixels.show();

  // Ferme la bouche
  jawServo.attach(JAW_SERVO_PIN);
  jawServo.write(SERVO_MIN);
  actMrlComm.servoDetachIsRequest = true;
  delay(2000);
  
  // Etape 4
  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  // réveil du module audio
  actMrlComm.enableAudio();
  // Coupe la sortie audio pendant le démarrage du PC
  actMrlComm.setMuteOn();

  // Fin du startup
  digitalWrite(LED_VEILLEUSE, LOW);
  // Init timing
  processTime = superMillis();
  wakeUp = true;
  pixelCpt = 0;
}

/** 
 * Gestion de l'arrêt de InMoov
 * Le PC s'éteint tout seul.
 * Coupure des batteries.
 */
void shutdownProcess(void)
{
  int i = 0;
  
  digitalWrite(LED_VEILLEUSE, HIGH);

  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  // Utilisation du ring pour visualiser l'état de l'avancement
  // Etape 1
  for (i = 0; i < 8; i++)
  {
    pixels.setPixelColor((i + 8), pixels.Color(255,0,0));
  }
  pixels.show();

  // Coupure alimentation électronique générale
  PowerElecOFF();
  delay(2000);

  // Etape 2
  for (i = 0; i < 8; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255,0,0));
  }
  pixels.show();

  // Coupure alimentation des moteurs
  PowerMotOFF();
  delay(2000);

  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  actMrlComm.disableAudio();

  // Fin du shutdown
  digitalWrite(LED_VEILLEUSE, LOW);
  wakeUp = false;
}

/** 
 * Gestion du servo et du ring en fonction de l'audio
 */
void ProcessJaw(void)
{
  static int iCpt = 0;
  
  // Gestion du servo de la bouche
  if (actMrlComm.servoIsEnable)
  {
    // Lecture des valeurs analogiques audios
    audioVal = analogRead(A0);
    seuilVal = analogRead(A1);
    
    // Convertion en mV
    audioVal = map(audioVal, 0, 1023, 0, REF_VOLTAGE);
    seuilVal = map(seuilVal, 0, 1023, 0, REF_VOLTAGE);
    
    // Ajout d'un test pour éviter les erreurs
    // Le niveau audio doit être supérieur à 800mv
    if ((audioVal > 800) && (audioVal > seuilVal))
    {
      if (!bJawIsOpen)
      {
        bJawIsOpen = true;
        
        // Pour le ring
        srVal = actMrlComm.rVal;
        sgVal = actMrlComm.gVal;
        sbVal = actMrlComm.bVal;
        actMrlComm.rVal = 200;
        actMrlComm.gVal = 200;

        jawServo.attach(JAW_SERVO_PIN);
        jawServo.write(SERVO_AUDIO_MAX);
      }
    }
    else
    {
      if (bJawIsOpen)
      {
        bJawIsOpen = false;
        
        // Pour le ring
        actMrlComm.rVal = srVal;
        actMrlComm.gVal = sgVal;
        actMrlComm.bVal = sbVal;

        closeRequest = true;
      }
    }
    
    if (closeRequest)
    {
      iCpt++;
      if (iCpt >= 20)
      {
        iCpt = 0;
        jawServo.attach(JAW_SERVO_PIN);
        jawServo.write(actMrlComm.servoMin);
        closeRequest = false;
        actMrlComm.servoDetachIsRequest = true;
      }
    }
  }
  else
  {
    if (actMrlComm.openJawRequest)
    {
      if (!bJawIsOpen)
      {
        jawServo.attach(JAW_SERVO_PIN);
        jawServo.write(actMrlComm.servoMax);
        bJawIsOpen = true;
        actMrlComm.servoDetachIsRequest = true;
      }
    }
    else
    {
      if (bJawIsOpen)
      {
        jawServo.attach(JAW_SERVO_PIN);
        jawServo.write(actMrlComm.servoMin);
        bJawIsOpen = false;
        //actMrlComm.servoIsEnable = true;
        actMrlComm.servoDetachIsRequest = true;
      }
    }
  }
}

/** 
 * Lecture de la tension batterie et test des niveaux
 */
void ProcessBattery(void)
{
  unsigned int bat1Val;
  unsigned int bat2Val;

  // Lecture des valeurs analogiques
  bat1Val = analogRead(A2);
  bat2Val = analogRead(A3);

  // Tension batterie en mV
  bat1Value = map(bat1Val, 0, 1023, 0, REF_VOLTAGE);
  bat2Value = map(bat2Val, 0, 1023, 0, REF_VOLTAGE);
  // Prise en compte du diviseur de tension
  // Pour la batterie1 12V: V = 0.3125U
  // Pour la batterie2 6V: V = 0.617U
  bat1Value = 12100;//(actMrlComm.bat1Value / 0.3125);
  bat2Value = 6100;//(actMrlComm.bat2Value / 0.383);
  
  if (bat1Value <= BAT1_SEUIL)
  {
    cptBat1Failed++;
    if (cptBat1Failed >= 12)
    {
      // Aprés 1mn on déclare la batterie vide.
      batteryEleIsFailed = true;
    }
  }
  else
  {
    cptBat1Failed = 0;
    batteryEleIsFailed = false;
  }
    
  if (bat2Value <= BAT2_SEUIL)
  {
    cptBat2Failed++;
    if (cptBat2Failed >= 12)
    {
      // Aprés 1mn on déclare la batterie vide.
      batteryMotIsFailed = true;
    }
  }
  else
  {
    batteryMotIsFailed = false;
    cptBat2Failed = 0;
  }
}

/** 
 * Initialisation du programme
 */
void setup(void) 
{
  // Config des pins numériques en sortie
  pinMode(ON_ELEC_PIN, OUTPUT);  
  pinMode(OFF_ELEC_PIN, OUTPUT);
  pinMode(ON_MOT_PIN, OUTPUT);
  pinMode(OFF_MOT_PIN, OUTPUT);
  pinMode(PC_POWER_PIN, OUTPUT);
  pinMode(LED_VEILLEUSE, OUTPUT);
  pinMode(JAW_SERVO_PIN, OUTPUT);
  pinMode(MAX9744_SHTDOWN_PIN, OUTPUT);
  pinMode(MAX9744_MUTE_PIN, OUTPUT);
  // Config des pins numériques en entrées
  pinMode(PRESENCE_SENSOR_PIN, INPUT_PULLUP);
  // Reférence externe à 4V pour ADC
  analogReference(EXTERNAL);  // AREF
  
  // Initialisation des états
  PowerElecOFF();
  PowerMotOFF();
  batteryEleIsFailed = false;
  batteryMotIsFailed = false;

  // Initialise la librairie NeoPixel.
  pixels.begin();           
  // Eteint tout les pixels.
  pixels.show();

  // Démarrage I2C
  Wire.begin();
  actMrlComm.max9744IsOK = false;
  
  // Vérifie la présence du module audio
  // en mettant le volume par défaut
  if (setVolume(actMrlComm.volAudio)) 
  {
    actMrlComm.max9744IsOK = true;
  }
  
  // Init des valeurs en EEPROM
  actMrlComm.servoMin = SERVO_MIN;
  actMrlComm.servoMax = SERVO_MAX;
  pixelColorInSleep = PIXEL_COLOR;
  writeFirstConfig();
  readConfig();

  actMrlComm.publishVersion();
  actMrlComm.publishBoardInfo();
  delay(100);

  // Init variables globals
  actMrlComm.inmoovIsOn = false;
  actMrlComm.shutdownPC = false;
  actMrlComm.servoIsEnable = false;
  actMrlComm.animRequest = 0;
  actMrlComm.watchDogIsEnable = false;

  wakeUp = false;
}

/** 
 * Boucle principal
 */
void loop(void) 
{
  int i = 0;
  int rTemp = 0;
  int bTemp = 0;
  int gTemp = 0;

  if (!wakeUp)
  {
    // Si pas réveillé, alors on dort...
    ////////////////////////////////////
    sleepNow();

    if (wakeUp)
    {
      startupProcess();
    }
  }
  else
  {
    // increment how many times we've run
    // TODO: handle overflow here after 32k runs, i suspect this might blow up?
    actMrlComm.loopCount++;
    // get a command and process it from the serial port (if available.)
    actMrlComm.readCommand();
    // update devices
    //actMrlComm.updateDevices();
    // send back load time and memory
    actMrlComm.publishBoardStatus();
    
    // Répond à la demande d'informations
    /////////////////////////////////////
    if (actMrlComm.sendValueRequest != 0)
    {
      actMrlComm.sendValueRequest = 0;
      
      MrlMsg msg(PUBLISH_CUSTOM_MSG);
      msg.countData();
      msg.addData16((unsigned int)bat1Value);
      msg.addData16((unsigned int)bat2Value);
      msg.addData(actMrlComm.volAudio);
      msg.sendMsg();
    }

    // Controle de l'alimentation des servos par Python
    ///////////////////////////////////////////////////
    if (actMrlComm.servoControlRequest)
    {
      actMrlComm.servoControlRequest = false;
      
      if (actMrlComm.servoPowerIsOn)
      {
        PowerMotON();
      }
      else
      {
        PowerMotOFF();
      }
    }

    if (actMrlComm.inmoovIsOn)
    {
      // Gestion du volume audio
      //////////////////////////
      if (actMrlComm.updateAudio)
      {
        actMrlComm.updateAudio = false;
        setVolume(actMrlComm.volAudio);
      }

      // Gestion de la bouche si InMoov est actif
      ///////////////////////////////////////////
      ProcessJaw();
    }

    // Gestion tous les 10ms
    ////////////////////////
    if ((superMillis() - processTime) >= 10)
    {
      // Gestion du détachement du servo
      //////////////////////////////////
      if (actMrlComm.servoDetachIsRequest)
      {
        // Détachement du servo aprés 2s
        cptDetach++;
        if (cptDetach >= 200)
        {
          cptDetach = 0; 
          jawServo.detach();
          actMrlComm.servoDetachIsRequest = false;
        }
      }

      if (actMrlComm.inmoovIsOn)
      {
        // Les alertes batteries sont prioritaires
        //////////////////////////////////////////
        if ((batteryMotIsFailed) && (batteryEleIsFailed))
        {
          // Les 2 batteries sont vident
          //////////////////////////////
          icptSeconde++;
          if (icptSeconde > 100)
          {
            icptSeconde = 0;
            if (neoState)
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(255,255,255));
              }
            }
            else
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(0,0,0));
              }
            }
    
            pixels.show();
            neoState = !neoState;
          }
        }
        else if (batteryEleIsFailed)
        {
          // La batterie de l'electronique est vide
          /////////////////////////////////////////
          icptSeconde++;
          if (icptSeconde > 100)
          {
            icptSeconde = 0;
            if (neoState)
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(255,0,0));
              }
            }
            else
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(0,0,0));
              }
            }
    
            pixels.show();
            neoState = !neoState;
          }
        }
        else if (batteryMotIsFailed)
        {
          // La batterie de la motorisation est vide
          //////////////////////////////////////////
          icptSeconde++;
          if (icptSeconde > 100)
          {
            icptSeconde = 0;
            if (neoState)
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(255,150,0));
              }
            }
            else
            {
              for (i = 0; i < pixels.numPixels(); i++)
              {
                pixels.setPixelColor(i, pixels.Color(0,0,0));
              }
            }
    
            pixels.show();
            neoState = !neoState;
          }
        }
        else
        {
          // Pas de problème batteries
          ////////////////////////////
          
          if (actMrlComm.animRequest == 1)
          {
            // Animations 1 demandé
            ///////////////////////
            for (i = 0; i < pixels.numPixels(); i++) 
            {
              pixels.setPixelColor(i, Wheel((i+pixelCpt) & 255));
            }
            pixels.show();

            pixelCpt++;
            if (pixelCpt > 255)
            {
              pixelCpt = 0;
            }
          }
          else if (actMrlComm.animRequest == 2)
          {
            // Animations 2 demandées
            /////////////////////////
            for (i = 0; i < pixels.numPixels(); i++) 
            {
              pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + pixelCpt) & 255));
            }
            pixels.show();

            pixelCpt++;
            if (pixelCpt > 255)
            {
              pixelCpt = 0;
            }
          }
          else
          {
            // Animations par défaut
            ////////////////////////
            if (neoState)
            {
              pixels.setPixelColor(pixelCpt, pixels.Color(actMrlComm.rVal,actMrlComm.gVal,actMrlComm.bVal));
            }
            else
            {
              if (actMrlComm.rVal == 0)
              {
                rTemp = 0;
              }
              else
              {
                rTemp = 40;
              }

              if (actMrlComm.gVal == 0)
              {
                gTemp = 0;
              }
              else
              {
                gTemp = 40;
              }
              
              if (actMrlComm.bVal == 0)
              {
                bTemp = 0;
              }
              else
              {
                bTemp = 40;
              }
              pixels.setPixelColor(pixelCpt, pixels.Color(rTemp, gTemp, bTemp));
            }
        
            pixelCpt++;
            if (pixelCpt >= pixels.numPixels())
            {
              pixelCpt = 0;
              neoState = !neoState;
            }
            
            // Mise à jour état des pixels
            pixels.show();
          }
        }
      }  // fin de if (actMrlComm.inmoovIsOn)

      // Gestion tous les 100ms
      /////////////////////////
      iCpt100++;
      if (iCpt100 >= 10)
      {
        iCpt100 = 0;

        if (!actMrlComm.inmoovIsOn)
        {
          // Animation en attendant que l'on reçois la commande PC OK
          pixels.setPixelColor(pixelCpt, pixels.Color(100,0,0));
          if (pixelCpt == 0)
          {
            pixels.setPixelColor(pixels.numPixels() - 1, pixels.Color(0,0,0));
          }
          else
          {
            pixels.setPixelColor((pixelCpt - 1), pixels.Color(0,0,0));
          }
          pixels.show();
              
          pixelCpt++;
          if (pixelCpt >= pixels.numPixels())
          {
            pixelCpt = 0;
          }
        }
      }

      // Gestion tous les 500ms
      /////////////////////////
      iCpt500++;
      if (iCpt500 >= 50)
      {
        iCpt500 = 0;

        if (actMrlComm.inmoovIsOn)
        {
          if (state)
          {
            digitalWrite(LED_VEILLEUSE, LOW);
            state = false;
          }
          else
          {
            digitalWrite(LED_VEILLEUSE, HIGH);
            state = true;
          }
        }
      }
      
      // Gestion tous les 5s
      //////////////////////
      iCpt5++;
      if (iCpt5 >= 500)
      {
        iCpt5 = 0;

        if (actMrlComm.inmoovIsOn)
        {
          // Lecture tensions batteries
          /////////////////////////////
          ProcessBattery();
          
          // Gestion du watchdog
          //////////////////////
          if (actMrlComm.watchDogIsEnable)
          {
            actMrlComm.watchdogCpt++;
            if (actMrlComm.watchdogCpt > 60)
            {
              // Aprés 5mn, on a rien reçu
              // le système est planté...
              actMrlComm.watchdogCpt = 0;
              
              // Redémarrage du système...
              actMrlComm.inmoovIsOn = false;
              actMrlComm.shutdownPC = false;
              actMrlComm.servoIsEnable = false;
              actMrlComm.animRequest = 0;
              actMrlComm.watchDogIsEnable = false;

              shutdownProcess();
              delay(5000);
              startupProcess();
            }
          }

          // Gestion de la mise en veille aprés avoir reçu l'ordre du PC
          //////////////////////////////////////////////////////////////
          if (actMrlComm.shutdownPC)
          {
            iCptVeille++;
            if (iCptVeille >= 6)
            {
              // Aprés 30s, le PC est bien coupé (normalement)
              
              // Attention au mise à jour du PC...
              // A voir si 30s suffit...
              
              iCptVeille = 0;
              actMrlComm.inmoovIsOn = false;
              actMrlComm.servoIsEnable = false;
              actMrlComm.shutdownPC = false;
              
              shutdownProcess();
            }
          }
          else
          {
            iCptVeille = 0;
          }
        }  // fin de if (actMrlComm.inmoovIsOn)
      }

      processTime = superMillis(); 
    }  // fin de if ((superMillis() - processTime) >= 10)
  }
}




