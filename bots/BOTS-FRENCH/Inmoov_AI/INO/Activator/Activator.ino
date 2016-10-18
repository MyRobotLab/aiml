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
 * Compilateur : Arduino version 1.0.6                             *
 *                                                                 *
 *                                                                 *
 * Commande sur port série:                                        *
 *                                                                 *
 * A: animation                                                    *
 * B: état et valeur tension batterie                              *
 * C: couleur et animation du ring                                 *
 * E: état système                                                 *
 * S: commande module audio                                        *
 * V: version du programme                                         *
 *                                                                 *
 *                                                                 *
 * Fonctionnement:                                                 *
 *                                                                 *
 * A la mise sous tension, Activator est en standby et attend une  *
 * action sur PRESENCE_SENSOR (mise à la masse)                    *
 * Une fois activé, activator active l'alimentation des moteurs,   *
 * puis l'électronique, et enfin le PC.                            *
 * Nous sommes en mode réveil.                                     *
 * Dés que le PC est démarré avec MRL et tout les services, celui- *
 * ci doit envoyé "E:1," à Activator.                              *
 * Celui ci démarre alors en mode normal.                          *
 * On active le ring par C:2, ou C:3, ou C:4, ...                  *
 * Le servo Jaw est active.                                        *
 * Pour éteindre InMoov, MRL doit prévoir de fermer proprement     *
 * tout les services et envoyer E:5, a activator                   *
 * Celui ci va couper les alimentations 30s aprés avoir reçu la    *
 * commandes puis se met en veille.                                *
 *                                                                 *
 *                                                                 *
 * Résumé des commandes d'état:                                    *
 * E:1, mode normal                                                *
 * E:2, mode shutdown                                              *
 * E:3, bouche ouverte                                             *
 * E:4, bouche fermée                                              *
 * E:5, procedure de shutdown                                      *
 * E:6, désactive l'audio pour la bouche                           *
 * E:7, réactive l'audio pour le bouche                            *
 *                                                                 *
 * A chaque commande accepté, Activator répond commande:val,OK     *                                                                *
 * A chaque commande refusé, Activator reponds commande:ERROR      *                                                                 *
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
 *                                                                 *
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

const String version = "0.8.0";
// Mettre 1 pour débugger sur port série
#define DEBUG               1
// Mettre 1 si le MAX9744 est connecté
#define MAX9744             0
// Définition des pins du nano
#define PRESENCE_SENSOR_PIN 2    // PIN D2
#define MAX9744_SHTDOWN_PIN 4    // PIN D4
#define MAX9744_MUTE_PIN    5    // PIN D5
#define NEO_PIN             6    // PIN D6
#define JAW_SERVO_PIN       7    // PIN D7
#define ON_ELEC_PIN         8    // PIN D8
#define OFF_ELEC_PIN        9    // PIN D9
#define ON_MOT_PIN          10   // PIN D10
#define OFF_MOT_PIN         11   // PIN D11
#define PC_POWER_PIN        12   // PIN D12
#define LED_VEILLEUSE       13   // PIN D13
#define AUDIO_PIN           0    // PIN A0
#define AUDIO_SEUIL         1    // PIN A1
#define BAT1_PIN            2    // PIN A2
#define BAT2_PIN            3    // PIN A3

// Nombre de pixels du ring
#define NUMPIXELS           16

// Min et max du servo à régler en fonction de InMoov
#define SERVO_MIN           30  // Bouche fermé
#define SERVO_MAX           60  // Bouche ouverte

// Définition des seuils à partir duquel on signal l'erreur batterie
#define BAT1_SEUIL          1000
#define BAT2_SEUIL          1000

// Adresse I2C du MAX9744
#define MAX9744_I2CADDR     0x4B

// Déclaration de l'objet NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
// Déclaration de l'objet servo
Servo jawServo;

// Déclaration des variables globals
int sensorVal = 0;
int bat1Val = 0;
int bat2Val = 0;
int audioVal = 0;
int seuilVal = 0;
//int serialCount = 0;             
int cptWait = 0;
int cptTimer = 0;
byte rVal = 0;
byte gVal = 0;
byte bVal = 0;
byte srVal = 0;
byte sgVal = 0;
byte sbVal = 0;

float bat1Value = 0.0;
float bat2Value = 0.0;
int cptBat1Failed = 0;
int cptBat2Failed = 0;
int animRequest = 0;
int icptWait = 0;

unsigned long long processTime;

boolean servoIsEnable = false;
boolean inmoovIsOn = false;
boolean batteryEleIsFailed = false;
boolean batteryMotIsFailed = false;
boolean neoState = false;
boolean wakeUp = false;
boolean shutdownPC = false;
boolean closeIsRun = false;
boolean bUpdateRing = false;
boolean bJawIsOpen = false;
boolean closeRequest = false;
boolean openRequest = false;

int inbyte;
int pixelCpt = 0;
int ucCpt = 0;
int ucCpt1 = 0;

int8_t volAudio = 31;


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
  delay(100);
  digitalWrite(ON_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'électronique générale
 * état OFF
 */
void PowerElecOFF()
{
  digitalWrite(OFF_ELEC_PIN, HIGH);
  delay(100);
  digitalWrite(OFF_ELEC_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état ON
 */
void PowerMotON()
{
  digitalWrite(ON_MOT_PIN, HIGH);
  delay(100);
  digitalWrite(ON_MOT_PIN, LOW);
}

/** 
 * Commande du relais bistable de l'alimentation des moteurs
 * état OFF
 */
void PowerMotOFF()
{
  digitalWrite(OFF_MOT_PIN, HIGH);
  delay(100);
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
  
  if ((inmoovIsOn) || (wakeUp))
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

  if (wakeUp)
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
    if (inmoovIsOn)
    {
      // Lecture des valeurs analogiques
      bat1Val = analogRead(BAT1_PIN);
      bat2Val = analogRead(BAT2_PIN);
      
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
      bat1Value = map(bat1Val, 0, 1023, 0, 5000) / 1000;
      bat2Value = map(bat2Val, 0, 1023, 0, 5000) / 1000;

      /*if (DEBUG)
      {
        //on affiche la valeur lue sur la liaison série
        Serial.print("bat1Val = ");
        Serial.println(bat1Val);
        Serial.print("bat2Val = ");
        Serial.println(bat2Val);
        
        // On affiche la tension calculée en V
        Serial.print("Tension batterie 1 = ");
        Serial.print(bat1Value, 2);
        Serial.println(" V");
        Serial.print("Tension batterie 2 = ");
        Serial.print(bat2Value, 2);
        Serial.println(" V");
        
        // On saute une ligne entre deux affichages
        Serial.println();
      }*/
      
      // Gestion de la mise en veille aprés avoir reçu l'ordre du PC
      if (shutdownPC)
      {
        cptWait++;
        if (cptWait >= 6)
        {
          // Aprés 30s, le PC est bien coupé (normalement)
          
          // Attention au mise à jour du PC...
          // A voir si 30s suffit...
          
          cptWait = 0;
          shutdownPC = false;
          inmoovIsOn = false;
          servoIsEnable = false;
          wakeUp = false;
          
          shutdownProcess();
        }
      }
    }
    else
    {
      if (!wakeUp)
      {
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

  /* Vérification du servo de la bouche
  jawServo.attach(JAW_SERVO_PIN);
  jawServo.write(SERVO_MIN);
  jawServo.write(SERVO_MAX);*/

  delay(2000);
  // Etape 5
  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

  // réveil du module audio
  digitalWrite(MAX9744_SHTDOWN_PIN, HIGH);
  // Coupe la sortie audio pendant le démarrage du PC
  digitalWrite(MAX9744_MUTE_PIN, HIGH);

  // Fin du startup
  digitalWrite(LED_VEILLEUSE, LOW);
  MsTimer2::start(); 

  wakeUp = true;
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
  PowerElecON();
  delay(2000);

  // Etape 2
  for (i = 0; i < 8; i++)
  {
    pixels.setPixelColor(i, pixels.Color(255,0,0));
  }
  pixels.show();

  // Coupure alimentation des moteurs
  PowerMotON();
  delay(2000);

  for (i = 0; i < pixels.numPixels(); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();

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
 * Récuppère un caractère sur le port série
 */
char getSerialChar()
{
  char serialdata = 0;
  
  if (Serial.available() > 0)
  {
    while (inbyte != ':')
    {
      inbyte = Serial.read(); 
      if (inbyte > 0 && inbyte != ':')
      {
        serialdata = inbyte;
      }
    }
    inbyte = 0;
  }

  return serialdata;
}

/** 
 * Récuppère un nombre sur le port série
 */
long getSerialNum()
{
  long serialdata = 0;
  
  while (inbyte != ',')
  {
    inbyte = Serial.read(); 
    if (inbyte > 0 && inbyte != ',')
    {
      serialdata = serialdata * 10 + inbyte - '0';
    }
  }
  inbyte = 0;

  return serialdata;
}

/** 
 * Réglage du volume audio MAX9744
 */
boolean setVolume(int8_t v) 
{
  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;
  
  if (MAX9744)
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

  // Configure la référence des ADCs
  analogReference(DEFAULT);   // 5V
  //analogReference(INTERNAL);  // 1.1V
  //analogReference(EXTERNAL);  // AREF
  
  // Initialisation des états
  PowerElecOFF();
  PowerMotOFF();
  
  // Initialise la librairie NeoPixel.
  pixels.begin();           
  // Eteint tout les pixels.
  pixels.show();

  // Initialisation interruption Timer 2
  // période 100ms 
  MsTimer2::set(100, ProcessTimer); 
  // active Timer2 
  MsTimer2::start(); 
   
  // Init timing
  processTime = superMillis();
  
  servoIsEnable = false;
  inmoovIsOn = false;
  batteryEleIsFailed = false;
  batteryMotIsFailed = false;

  Serial.begin(9600);  
  
  if (DEBUG)
  {
    Serial.println("--- Program startup ---");
    Serial.println(); 
  }

  if (MAX9744)
  {
    // Démarrage I2C
    Wire.begin();
    
    // Vérifie la présence du module audio
    // en mettant le volume par défaut
    if (!setVolume(volAudio)) 
    {
      Serial.println("MAX9744 non trouvé!");
    }
  }
}

/** 
 * Boucle principal
 */
void loop() 
{
  int i = 0;
  long serialdata;
  
  char val = getSerialChar();
  if (val != 0)
  {
    switch(val)
    {
      
      case 'A':
      {
        serialdata = getSerialNum();
        switch (serialdata)
        {
          case 0:
          {
            // Animation par défaut
            animRequest = 0;
            Serial.println("A:0,OK");
            break;
          }
          case 1:
          {
            animRequest = 1;
            Serial.println("A:1,OK");
            break;
          }
          case 2:
          {
            animRequest = 2;
            Serial.println("A:2,OK");
            break;
          }
          default:
          {
            Serial.println("A:ERROR");
            break;
          }
        }
        break; 
      }
      case 'B':
      {
        serialdata = getSerialNum();
        switch (serialdata)
        {
          case 1:
          {
            // Valeur brut
            Serial.print("B:");
            Serial.println(bat1Val);
            break;
          }
          case 2:
          {
            // Valeur brut
            Serial.print("B:");
            Serial.println(bat2Val);
            break;
          }
          case 3:
          {
            // Valeur en volt
            Serial.print("B:");
            Serial.println(bat1Value, 2);
            break;
          }
          case 4:
          {
            // Valeur en volt
            Serial.print("B:");
            Serial.println(bat2Value, 2);
            break;
          }
          default:
          {
            Serial.println("B:ERROR");
            break;
          }
        }
        break; 
      }
      case 'C':
      {
        serialdata = getSerialNum();
        switch (serialdata)
        {
          case 0:
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
            Serial.println("C:0,OK");
            break;
          }
          case 1:
          {
            // Jaune
            rVal = 100;
            gVal = 100;
            bVal = 0;
            Serial.println("C:1,OK");
            break;
          }
          case 2:
          {
            // Rouge
            rVal = 100;
            gVal = 0;
            bVal = 0;
            Serial.println("C:2,OK");
            break;
          }
          case 3:
          {
            // Vert
            rVal = 0;
            gVal = 100;
            bVal = 0;
            Serial.println("C:3,OK");
            break;
          }
          case 4:
          {
            // Bleu
            rVal = 0;
            gVal = 0;
            bVal = 100;
            Serial.println("C:4,OK");
            break;
          }
          case 5:
          {
            // Cyan
            rVal = 0;
            gVal = 100;
            bVal = 100;
            Serial.println("C:5,OK");
            break;
          }
          case 6:
          {
            // Rose
            rVal = 100;
            gVal = 0;
            bVal = 100;
            Serial.println("C:6,OK");
            break;
          }
          case 10:
          {
            // Custom
            rVal = getSerialNum();
            gVal = getSerialNum();
            bVal = getSerialNum();
            Serial.println("C:10,OK");
            break;
          }
          default:
          {
            Serial.println("C:ERROR");
            break;
          }
        }
        break; 
      }
      case 'E':
      {
        serialdata = getSerialNum();
        switch (serialdata)
        {
          case 1:
          {
            // Le PC est prêt
            wakeUp = false;
            inmoovIsOn = true;
            servoIsEnable = true;
            rVal = 0;
            gVal = 0;
            bVal = 0;
            animRequest = 0;
            // Active la sortie audio
            digitalWrite(MAX9744_MUTE_PIN, LOW);
            Serial.println("E:1,OK");
            break;
          }
          case 2:
          {
            // Met le nano en veille
            inmoovIsOn = false;
            servoIsEnable = false;
            for (i = 0; i < NUMPIXELS; i++)
            {
              pixels.setPixelColor(i, pixels.Color(0,0,0));
            }
            pixels.show();
            jawServo.detach();
            Serial.println("E:2,OK");
            break;
          }
          case 3:
          {
            // Demande d'ouverture de la bouche
            servoIsEnable = false;
            jawServo.attach(JAW_SERVO_PIN);
            jawServo.write(SERVO_MAX);
            Serial.println("E:3,OK");
            break;
          }
          case 4:
          {
            // Demande de fermeture de la bouche
            servoIsEnable = true;
            jawServo.attach(JAW_SERVO_PIN);
            jawServo.write(SERVO_MIN);
            Serial.println("E:4,OK");
            break;
          }
          case 5:
          {
            // Le PC va s'arrété
            shutdownPC = true;
            Serial.println("E:5,OK");
            break;
          }
          case 6:
          {
            // Bloque la gestion du servo en fonction de l'audio
            servoIsEnable = false;
            Serial.println("E:6,OK");
            break;
          }
          case 7:
          {
            // Débloque la gestion du servo en fonction de l'audio
            servoIsEnable = true;
            Serial.println("E:7,OK");
            break;
          }
          default:
          {
            Serial.println("E:ERROR");
            break;
          }
        }
        break;
      }
      case 'S':
      {
        serialdata = getSerialNum();
        switch (serialdata)
        {
          case 1:
          {
            // Réglage du volume
            volAudio = getSerialNum();
            if (setVolume(volAudio))
            {
              Serial.println("S:1,OK");
            }
            else
            {
              Serial.println("S:ERROR");
            }
            break;
          }
          case 2:
          {
            // return la valeur du volume
            Serial.print("S:");
            Serial.println(volAudio);
            break;
          }
          case 3:
          {
            // réveil du module audio
            digitalWrite(MAX9744_SHTDOWN_PIN, HIGH);
            Serial.println("S:3,OK");
            break;
          }
          case 4:
          {
            // shutdown du module audio
            digitalWrite(MAX9744_SHTDOWN_PIN, LOW);
            Serial.println("S:4,OK");
            break;
          }
          case 5:
          {
            // mute ON
            digitalWrite(MAX9744_MUTE_PIN, HIGH);
            Serial.println("S:3,OK");
            break;
          }
          case 6:
          {
            // mute OFF
            digitalWrite(MAX9744_MUTE_PIN, LOW);
            Serial.println("S:4,OK");
            break;
          }
          default:
          {
            Serial.println("S:ERROR");
            break;
          }
        }
        break; 
      }
      case 'V':
      {
        Serial.print("V:");
        Serial.println(version);
        break; 
      }
      default:
      {
        // mauvaise commande, flush buffer
        Serial.flush();
      }
    }
  }

  if (inmoovIsOn)
  {
    // InMoov est actif
    ///////////////////
    
    // Gestion du servo de la bouche
    if (servoIsEnable)
    {
      // Lecture des valeurs analogiques audios
      audioVal = analogRead(A0);
      seuilVal = analogRead(A1);
      
      if (audioVal > seuilVal)
      {
        closeRequest = false;

        if (!bJawIsOpen)
        {
          if (closeIsRun)
          {
            ucCpt1++;
            if (ucCpt1 >= 20)
            {
              srVal = rVal;
              sgVal = gVal;
              sbVal = bVal;
              if (rVal != 0)
              {
                rVal = 255;
              }
              if (gVal != 0)
              {
                gVal = 255;
              }
              if (bVal != 0)
              {
                bVal = 255;
              }
              closeIsRun = false;
              ucCpt1 = 0;
              jawServo.attach(JAW_SERVO_PIN);
              jawServo.write(SERVO_MAX);
              bJawIsOpen = true;
            }
          }
          else
          {
            jawServo.attach(JAW_SERVO_PIN);
            jawServo.write(SERVO_MAX);
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
      }
      
      if (closeRequest)
      {
        ucCpt++;
        if (ucCpt >= 20)
        {
          ucCpt = 0;
          jawServo.attach(JAW_SERVO_PIN);
          jawServo.write(SERVO_MIN);
          bJawIsOpen = false;
          closeRequest = false;
          closeIsRun = true;
          rVal = srVal;
          gVal = sgVal;
          bVal = sbVal;
        }
      }
    }
    
    // Mise à jour du ring
    if ((superMillis() - processTime) >= 10)
    {
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
        
        if (animRequest == 1)
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
        else if (animRequest == 2)
        {
          // Animations 2 demandé
          ///////////////////////
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
              pixels.setPixelColor(pixelCpt, pixels.Color(rVal,gVal,bVal));
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
    if (!wakeUp)
    {
      // Lit l'état du capteur de présence ou bouton ON
      if (digitalRead(PRESENCE_SENSOR_PIN) == LOW)
      {
        delay(50);
        
        // 2éme lecture pour éviter les parasites
        if (digitalRead(PRESENCE_SENSOR_PIN) == LOW)
        {
          // InMoov se réveille...
          wakeUp = true;
          // Démarrage...
          startupProcess();
        }
      }
    }
  }
}




