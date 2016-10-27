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
 * Timer2, Adafruit_NeoPixel, Servo, Wire.                         *
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
 *			                                           *
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
 *     - Refonte total du programme avec le protocole MrlComm      *
 * 25/10/2016 | D. Gyselinck  | V1.1.0 release                     *
 *     - Paramètre min et max du servo en EEPROM                   *
 *     - Correction de bug                                         *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <MsTimer2.h>
#include <Servo.h> 
#include <Wire.h> 
#include "MrlComm.h"

// Numéro de version Activator
#define V1                  1
#define V2                  1
#define V3                  0

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
#define SERVO_MAX           60  // Bouche ouverte

// Nombre de pixels du ring
#define NUMPIXELS           16

// Définition des seuils à partir duquel on signal l'erreur batterie
#define BAT1_SEUIL          1000
#define BAT2_SEUIL          1000

// Adresse I2C du MAX9744
#define MAX9744_I2CADDR     0x4B

// Déclaration de l'objet NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
// Déclaration de l'objet servo
Servo jawServo;
// Objet static MrlComm
MrlComm mrlComm;

// Déclaration des variables globals
int sensorVal = 0;
int audioVal = 0;
int seuilVal = 0;
int cptWait = 0;
int cptTimer = 0;
byte srVal = 0;
byte sgVal = 0;
byte sbVal = 0;

int cptBat1Failed = 0;
int cptBat2Failed = 0;
int icptWait = 0;
int cptDetach = 0;

unsigned long long processTime;

boolean batteryEleIsFailed = false;
boolean batteryMotIsFailed = false;
boolean neoState = false;
boolean closeIsRun = false;
boolean bUpdateRing = false;
boolean bJawIsOpen = false;
boolean closeRequest = false;
boolean openRequest = false;
boolean openJawRequest = false;

int pixelCpt = 0;
int ucCpt = 0;
int ucCpt1 = 0;


///////////////////////////////
// Déclaration des fonctions //
///////////////////////////////

/** 
 * Commande du relais bistable de l'électronique générale
 * état ON
 */
void PowerElecON()
{
  digitalWrite(ON_ELEC_PIN, HIGH);
  delay(200);
  digitalWrite(ON_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'électronique générale
 * état OFF
 */
void PowerElecOFF()
{
  digitalWrite(OFF_ELEC_PIN, HIGH);
  delay(200);
  digitalWrite(OFF_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état ON
 */
void PowerMotON()
{
  digitalWrite(ON_MOT_PIN, HIGH);
  delay(200);
  digitalWrite(ON_MOT_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état OFF
 */
void PowerMotOFF()
{
  digitalWrite(OFF_MOT_PIN, HIGH);
  delay(200);
  digitalWrite(OFF_MOT_PIN, LOW);
}

/** 
 * Pulse de démarrage du PC
 */
void PcPowerPulse()
{
  digitalWrite(PC_POWER_PIN, HIGH);
  delay(1000);
  digitalWrite(PC_POWER_PIN, LOW);
}

/** 
 * Effectue une mesure de tension des 2 batteries
 * et change l'état du neo ring en conséquence
 * Si veille, flash de la led.
 * Cette procédure est appelée toutes les 100ms
 * par l'interruption du Timer 2
 */
void ProcessTimer()
{
  static boolean state = false;
  static int cpt = 0;
  
  int i, j;
  
  if ((mrlComm.inmoovIsOn) || (mrlComm.wakeUp))
  {
    // Permet de visualiser le mode RUN
    cpt++;
    if (cpt >= 5)
    {
      cpt = 0;
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

  if (mrlComm.wakeUp)
  {
    // Permet de visualiser le mode RUN
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
  
  cptTimer++;
  if (cptTimer >= 50)
  {
    cptTimer = 0;
    
    // 5s
    if (mrlComm.inmoovIsOn)
    {
      // Lecture des valeurs analogiques
      mrlComm.bat1Val = analogRead(A2);
      mrlComm.bat2Val = analogRead(A3);
      
      // TODO
      // reglage des seuils en fonction des batteries utilisées
      
      /*if (bat1Value < BAT1_SEUIL)
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
        
      if (bat2Value < BAT2_SEUIL)
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
      }*/

      // Tension batterie en V
      mrlComm.bat1Value = map(mrlComm.bat1Val, 0, 1023, 0, 4000) / 1000;
      mrlComm.bat2Value = map(mrlComm.bat2Val, 0, 1023, 0, 4000) / 1000;

      // Gestion de la mise en veille aprés avoir reçu l'ordre du PC
      if (mrlComm.shutdownPC)
      {
        cptWait++;
        if (cptWait >= 6)
        {
          // Aprés 30s, le PC est bien coupé (normalement)
          
          // Attention au mise à jour du PC...
          // A voir si 30s suffit...
          
          cptWait = 0;
          mrlComm.setServoEnable(false);
          mrlComm.setInmoovIsOn(false);
          mrlComm.setShutdownPC(false);
          mrlComm.setWakeUp(false);
          
          shutdownProcess();
        }
      }
      else
      {
        cptWait = 0;
      }
    }
    else
    {
      if (!mrlComm.wakeUp)
      {
        cptWait++;
        if (cptWait >= 2)
        {
          cptWait = 0;
          
          // Flash de la veilleuse toutes les 5 secondes
          // Indique que InMoov dort...
          for (j = 0; j < 80; j++)
          {
            for (i = 0; i < pixels.numPixels(); i++)
            {
              pixels.setPixelColor(i, pixels.Color(0,0,j));
            }
            pixels.show();
            delay(10);
          }
          
          for (j = 80; j >= 0; j--)
          {
            for (i = 0; i < pixels.numPixels(); i++)
            {
              pixels.setPixelColor(i, pixels.Color(0,0,j));
            }
            pixels.show();
            delay(10);
          }
        }
      }
    }
  }
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
unsigned long long superMillis() 
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
 * Gestion du démarrage de InMoov
 * Active les 2 batteries, allume le PC.
 * A ce stade, on attend une confirmation d'une commande PC allumé.
 */
void startupProcess()
{
  int i = 0;
  
  digitalWrite(LED_VEILLEUSE, HIGH);
  
  // Pas besoin du timer pendant le startup
  MsTimer2::stop(); 

  // Utilisation du ring pour visualiser l'état de l'avancement
  // Etape 1
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,100,0));
  }
  pixels.show();

  // Alimentation des moteurs
  PowerMotON();
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
  mrlComm.setDetachRequest(true);

  delay(2000);
  // Etape 5
  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  // réveil du module audio
  mrlComm.enableAudio();
  // Coupe la sortie audio pendant le démarrage du PC
  mrlComm.setMuteOn();

  // Fin du startup
  digitalWrite(LED_VEILLEUSE, LOW);
  MsTimer2::start(); 

  mrlComm.setWakeUp(true);
  pixelCpt = 0;
}

/** 
 * Gestion de l'arrêt de InMoov
 * Le PC s'éteint tout seul.
 * Coupure des batteries.
 */
void shutdownProcess()
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

  mrlComm.disableAudio();

  // Fin du shutdown
  digitalWrite(LED_VEILLEUSE, LOW);
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
boolean writeFirstConfig()
{
  if (EEPROM.read(0) != 0xAA)
  {
    // La config n'a jamais été sauvegardé
    EEPROM.write(0, 0xAA);
    
    // Version
    EEPROM.write(0, V1);
    EEPROM.write(1, V2);
    EEPROM.write(2, V3);
    
    // Valeur min/max de la bouche
    EEPROM.write(10, mrlComm.servoMin);
    EEPROM.write(11, mrlComm.servoMax);
  }
}

/** 
 * Lecture de la config EEPROM
 */
void readConfig()
{
  mrlComm.servoMin = EEPROM.read(10);
  mrlComm.servoMax = EEPROM.read(11);
}

/** 
 * Réglage du volume audio MAX9744
 */
boolean setVolume(int8_t v) 
{
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
  
  if (mrlComm.max9744IsOK)
  {
    Wire.beginTransmission(MAX9744_I2CADDR);
    Wire.write(v);

    if (Wire.endTransmission() == 0)
      return true;
    else
      return false;
  }
  else
  {
    return false;
  }
}

/** 
 * Initialisation du programme
 */
void setup() 
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

  // Initialisation interruption Timer 2
  // période 100ms 
  MsTimer2::set(100, ProcessTimer); 
  // active Timer2 
  MsTimer2::start(); 
   
  // Démarrage I2C
  Wire.begin();
  mrlComm.max9744IsOK = false;
  
  // Vérifie la présence du module audio
  // en mettant le volume par défaut
  if (setVolume(mrlComm.volAudio)) 
  {
    mrlComm.max9744IsOK = true;
  }
  
  // Init des valeurs en EEPROM
  mrlComm.servoMin = SERVO_MIN;
  mrlComm.servoMax = SERVO_MAX;
  writeFirstConfig();
  readConfig();

  mrlComm.publishVersion();
  mrlComm.publishBoardInfo();

  // Init timing
  processTime = superMillis();

  // Init variables globals
  mrlComm.inmoovIsOn = false;
  mrlComm.wakeUp = false;
  mrlComm.shutdownPC = false;
  mrlComm.servoIsEnable = false;
  mrlComm.animRequest = 0;
  mrlComm.max9744IsOK = false;
}

/** 
 * Boucle principal
 */
void loop() 
{
  int i = 0;

  // increment how many times we've run
  // TODO: handle overflow here after 32k runs, i suspect this might blow up?
  mrlComm.loopCount++;
  // get a command and process it from the serial port (if available.)
  mrlComm.readCommand();
  // update devices
  //mrlComm.updateDevices();
  // send back load time and memory
  mrlComm.publishBoardStatus();

  if (mrlComm.inmoovIsOn)
  {
    // InMoov est actif
    ///////////////////

    // Gestion du servo de la bouche
    if (mrlComm.servoIsEnable)
    {
      // Lecture des valeurs analogiques audios
      audioVal = analogRead(A0);
      seuilVal = analogRead(A1);
      
      /*if (audioVal > seuilVal)
      {
        closeRequest = false;

        if (!bJawIsOpen)
        {
          if (closeIsRun)
          {
            ucCpt1++;
            if (ucCpt1 >= 20)
            {
              srVal = mrlComm.rVal;
              sgVal = mrlComm.gVal;
              sbVal = mrlComm.bVal;
              if (mrlComm.rVal != 0)
              {
                mrlComm.rVal = 200;
              }
              if (mrlComm.gVal != 0)
              {
                mrlComm.gVal = 200;
              }
              if (mrlComm.bVal != 0)
              {
                mrlComm.bVal = 200;
              }
              closeIsRun = false;
              ucCpt1 = 0;
              jawServo.attach(JAW_SERVO_PIN);
              jawServo.write(mrlComm.servoMax);
              bJawIsOpen = true;
            }
          }
          else
          {
            jawServo.attach(JAW_SERVO_PIN);
            jawServo.write(mrlComm.servoMax);
            bJawIsOpen = true;
          }
        }
      }
      else
      {
        if (bJawIsOpen)
        {
          closeRequest = true;
        }
      }*/
      
      if (closeRequest)
      {
        ucCpt++;
        if (ucCpt >= 20)
        {
          ucCpt = 0;
          jawServo.attach(JAW_SERVO_PIN);
          jawServo.write(mrlComm.servoMin);
          bJawIsOpen = false;
          closeRequest = false;
          closeIsRun = true;
          mrlComm.rVal = srVal;
          mrlComm.gVal = sgVal;
          mrlComm.bVal = sbVal;
        }
      }
    }
    else
    {
      if (mrlComm.openJawRequest)
      {
        if (!bJawIsOpen)
        {
          jawServo.attach(JAW_SERVO_PIN);
          jawServo.write(mrlComm.servoMax);
          bJawIsOpen = true;
          mrlComm.setDetachRequest(true);
        }
      }
      else
      {
        if (bJawIsOpen)
        {
          jawServo.attach(JAW_SERVO_PIN);
          jawServo.write(mrlComm.servoMin);
          bJawIsOpen = false;
          mrlComm.setServoEnable(true);
          mrlComm.setDetachRequest(true);
        }
      }
    }

    // Mise à jour du ring
    if ((superMillis() - processTime) >= 10)
    {
      // Gestion du détachement du servo
      //////////////////////////////////
      if (mrlComm.servoDetachIsRequest)
      {
        cptDetach++;
        if (cptDetach >= 200)
        {
          cptDetach = 0; 
          jawServo.detach();
          mrlComm.setDetachRequest(false);
        }
      }

      // Gestion du volume audio
      //////////////////////////
      if (mrlComm.updateAudio)
      {
        mrlComm.updateAudio = false;
        setVolume(mrlComm.volAudio);
      }
      
      // Les alertes batteries sont prioritaires
      //////////////////////////////////////////
      if ((batteryMotIsFailed) && (batteryEleIsFailed))
      {
        // Les 2 batteries sont vident
        //////////////////////////////
        icptWait++;
        if (icptWait > 100)
        {
          icptWait = 0;
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
        icptWait++;
        if (icptWait > 100)
        {
          icptWait = 0;
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
        icptWait++;
        if (icptWait > 100)
        {
          icptWait = 0;
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
        
        if (mrlComm.animRequest == 1)
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
        else if (mrlComm.animRequest == 2)
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
          icptWait++;
          if (icptWait > 5)
          {
            icptWait = 0;
            
            if (neoState)
            {
              pixels.setPixelColor(pixelCpt, pixels.Color(mrlComm.rVal,mrlComm.gVal,mrlComm.bVal));
            }
            else
            {
              pixels.setPixelColor(pixelCpt, pixels.Color(0,0,0));
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
      }

      processTime = superMillis(); 
    }
  }
  else
  {
    // InMoov est en veille et n'est pas entrain de se réveiller...
    ///////////////////////////////////////////////////////////////
    if (!mrlComm.wakeUp)
    {
      // Lit l'état du capteur de présence ou bouton ON
      if (digitalRead(PRESENCE_SENSOR_PIN) == LOW)
      {
        delay(50);
        
        // 2éme lecture pour éviter les parasites
        if (digitalRead(PRESENCE_SENSOR_PIN) == LOW)
        {
          // Démarrage...
          startupProcess();
        }
      }
    }
  }
}




