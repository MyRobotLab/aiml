/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Programme INMOOV_NANO                                           *
 *                                                                 *
 * Créé le : 22/09/16                                              *
 * Auteur  : Dominique Gyselinck                                   *
 *                                                                 *
 * Ce système permet de :                                          *
 * - Gérer le servo de la bouche en fonction du niveau sonore      *
 * - Gérer le NEO ring                                             *
 * - Superviser les niveaux des batteries moteurs et électronique  *
 * - Gérer les démarrages et extinctions de INMOOV                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 * Suivi des modifications :                                       *
 * Date       | Auteur        | Description                        *
 * --------------------------------------------------------------- *
 * 22/09/2016 | D. Gyselinck  | Création du fichier                *
 *								                                                 *
 *                                                                 *
 * V0.1.0	: - Pré release pour test avant version 1.0.0            *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 * Librairies utilisées :                                          *
 *                                                                 *
 * Timer2, Adafruit_NeoPixel, Servo.                               *
 *                                                                 *
 * Compilateur : Arduino version 1.0.6                             *
 *                                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <MsTimer2.h>
#include <Servo.h> 

// Mettre 1 pour débugger sur port série
#define DEBUG               1

#define CMD_BIT1_PIN        1
#define CMD_BIT0_PIN        2
#define PRESENCE_SENSOR_PIN 5
#define CMD_BIT2_PIN        7
#define CMD_BIT3_PIN        8
#define NEO_PIN             9
#define JAW_SERVO_PIN       10
#define ON_ELEC_PIN         11
#define OFF_ELEC_PIN        12
#define ON_MOT_PIN          13
#define OFF_MOT_PIN         14
#define PC_POWER_PIN        15
#define LED_VEILLEUSE       16
#define AUDIO_R_PIN         19
#define AUDIO_L_PIN         20
#define BAT1_PIN            21
#define BAT2_PIN            22
// Réservé pour extension futur
#define I2C_SDA_PIN         23
#define I2C_SCL_PIN         24

// Nombre de pixels du ring
#define NUMPIXELS           16

// Commandes sur 4 bits
#define CMD_STANDBY         0
#define CMD_1               1
#define CMD_2               2
#define CMD_3               3
#define CMD_4               4
#define CMD_5               5
#define CMD_6               6
#define CMD_7               7
#define CMD_8               8
#define CMD_9               9
#define CMD_10              10
#define CMD_11              11
#define CMD_12              12
#define CMD_13              13
#define CMD_14              14
#define CMD_15              15

// Min et max du servo à régler en fonction de InMoov
#define SERVO_MIN           65
#define SERVO_MAX           100

#define BAT1_SEUIL          1000
#define BAT2_SEUIL          1000

// Déclaration de l'objet NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
// Déclaration de l'objet servo
Servo jawServo;

int i = 0;
int sensorVal = 0;
int bat1Val = 0;
int bat2Val = 0;
int audioLVal = 0;
int audioRVal = 0;
int command = 0;
int saveCmd = 0;
int ringColor = 0;
int cptWait = 0;

float bat1Value = 0.0;
float bat2Value = 0.0;
int cptBat1Failed = 0;
int cptBat2Failed = 0;

unsigned long long processTime;

boolean servoIsEnable = false;
boolean inmoovIsOn = false;
boolean batteryEleIsFailed = false;
boolean batteryMotIsFailed = false;
boolean neoState = false;
boolean wakeUp = false;
boolean shutdownPC = false;

int     MIN = 10;   //value when sound is detected
int     MAX = 500;  //max value when sound is detected
int     SecondDetection = 2; 
int     val = 0;    
//int     i = 0;
//int     posMax = 100;    
//int     posMin = 65;
int     pos = SERVO_MIN;
int     BoucheStatus = 0;
int     ActionBouche = 0;
int     Repos = 0;
int     CompteurRepos = 0;
String  dbg;


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
 * Cette procédure est appelée toutes les 5 secondes
 * par l'interruption du Timer 2
 */
void ProcessTimer()
{
  if (inmoovIsOn)
  {
    // Lecture des valeurs analogiques
    bat1Val = analogRead(BAT1_PIN);
    bat2Val = analogRead(BAT2_PIN);
    
    if (bat1Value < BAT1_SEUIL)
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
    }

    if (DEBUG)
    {
      // Tension batterie en V
      bat1Value = map(bat1Val, 0, 1023, 0, 5000) / 1000;
      bat2Value = map(bat2Val, 0, 1023, 0, 5000) / 1000;
      
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
    }
    
    // Gestion de la mise en veille aprés avoir reçu l'ordre du PC
    if (shutdownPC)
    {
      cptWait++;
      if (cptWait >= 6)
      {
        // Aprés 30s, le PC est bien coupé (normalement)
        
        // Attention au mise à jour du PC...
        // A voir si 30s suffise...
        
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
    if (wakeUp)
    {
      if (DEBUG)
      {
        Serial.println("InMoov se réveille...");
      }
    }
    else
    {
      if (DEBUG)
      {
        Serial.println("InMoov dort...");
      }

      // Flash de la veilleuse toutes les 5 secondes
      // Indique que InMoov dort...
      digitalWrite(LED_VEILLEUSE, HIGH);
      delay(500);
      digitalWrite(LED_VEILLEUSE, LOW);
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
 * Commandes envoyés par le cerveau d'InMoov afin
 * que le nano puisse effectuer des actions sur le ring,
 * servo, état général...
 */
void ProcessCmd()
{
  switch (command)
  {
    case CMD_1:
    {
      // Le PC est prêt
      inmoovIsOn = true;
      servoIsEnable = true;
      break;
    }
    case CMD_2:
    {
      // Demande d'ouverture de la bouche
      jawServo.write(SERVO_MAX);
      break;
    }
    case CMD_3:
    {
      // Demande de fermeture de la bouche
      jawServo.write(SERVO_MIN);
      break;
    }
    case CMD_4:
    {
      // Le PC va s'arrété
      shutdownPC = true;
      break;
    }
    case CMD_5:
    {
      // Bloque la gestion du servo en fonction de l'audio
      servoIsEnable = false;
      break;
    }
    case CMD_6:
    {
      // Débloque la gestion du servo en fonction de l'audio
      servoIsEnable = true;
      break;
    }
    case CMD_7:
    {
      // Couleur jaune claire
      ringColor = 1;
      break;
    }
    case CMD_8:
    {
      // Couleur rouge
      ringColor = 2;
      break;
    }
    case CMD_9:
    {
      // Couleur vert
      ringColor = 3;
      break;
    }
    case CMD_10:
    {
      // Couleur bleu
      ringColor = 4;
      break;
    }
    case CMD_11:
    {
      // Couleur blanc
      ringColor = 5;
      break;
    }
    // .....
    
    default:
    {
    }
  }
}

/** 
 * Gestion du démarrage de InMoov
 * Active les 2 batteries, allume le PC.
 * A ce stade, on attend une confirmation d'une commande PC allumé.
 */
void startupProcess()
{
  digitalWrite(LED_VEILLEUSE, HIGH);
  
  // Pas besoin du timer pendant le startup
  MsTimer2::stop(); 

  // Utilisation du ring pour visualiser l'état de l'avancement
  // Etape 1
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,255));
  }
  pixels.show();

  // Alimentation des moteurs
  PowerMotON();
  delay(2000);

  // Etape 2
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 4), pixels.Color(0,0,255));
  }
  pixels.show();

  // Alimentation électronique générale
  PowerElecON();
  delay(2000);

  // Etape 3
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 8), pixels.Color(0,0,255));
  }
  pixels.show();

  // Allumage du PC
  PcPowerPulse();
  delay(2000);

  // Etape 4
  for (i = 0; i < 4; i++)
  {
    pixels.setPixelColor((i + 12), pixels.Color(0,0,255));
  }
  pixels.show();

  // Vérification du servo de la bouche
  jawServo.attach(JAW_SERVO_PIN);
  jawServo.write(SERVO_MIN);
  delay(2000);
  jawServo.write(SERVO_MAX);
  jawServo.detach();

  // Fin du startup
  digitalWrite(LED_VEILLEUSE, LOW);
  MsTimer2::start(); 
}

/** 
 * Gestion de l'arrêt de InMoov
 * Le PC s'éteint tout seul.
 * Coupure des batteries.
 */
void shutdownProcess()
{
  digitalWrite(LED_VEILLEUSE, HIGH);

  // Utilisation du ring pour visualiser l'état de l'avancement
  // Etape 1
  for (i = 0; i < 8; i++)
  {
    pixels.setPixelColor((i + 8), pixels.Color(0,0,255));
  }
  pixels.show();

  // Coupure alimentation électronique générale
  PowerElecON();
  delay(2000);

  // Etape 2
  for (i = 0; i < 8; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0,0,255));
  }
  pixels.show();

  // Coupure alimentation des moteurs
  PowerMotON();
  delay(2000);

  // Fin du shutdown
  digitalWrite(LED_VEILLEUSE, LOW);
}

/** 
 * Lecture des 4 bits constituant la commande
 */
int readCommand()
{
  int cmd = 0;
  int temp = 0;
  
  // Bit 3
  cmd = digitalRead(CMD_BIT3_PIN);
  cmd <<= 3;
  // Bit 2
  temp = digitalRead(CMD_BIT2_PIN);
  temp <<= 2;
  cmd |= temp;
  // Bit 1
  temp = digitalRead(CMD_BIT1_PIN);
  temp <<= 1;
  cmd |= temp;
  temp = digitalRead(CMD_BIT0_PIN);
  cmd |= temp;
  
  return cmd;
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
  
  // Config des pins numériques en entrées
  pinMode(PRESENCE_SENSOR_PIN, INPUT_PULLUP);
  pinMode(CMD_BIT0_PIN, INPUT);
  pinMode(CMD_BIT1_PIN, INPUT);
  pinMode(CMD_BIT2_PIN, INPUT);
  pinMode(CMD_BIT3_PIN, INPUT);

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
  // période 5s 
  MsTimer2::set(5000, ProcessTimer); 
  // active Timer2 
  MsTimer2::start(); 
  
  // Init timing
  processTime = superMillis();
  
  servoIsEnable = false;
  inmoovIsOn = false;
  batteryEleIsFailed = false;
  batteryMotIsFailed = false;

  if (DEBUG)
  {
    Serial.begin(9600);  
    Serial.println("--- Démarrage du programme ---");
    Serial.println(); 
  }
}

/** 
 * Boucle principal
 */
void loop() 
{
  if (DEBUG)
  {
    if (Serial.available()) 
    {
      command = Serial.read();
    }
  }
  else
  {
    command = readCommand();
  }
  
  // Test si une commande est demandée
  if (command != 0)
  {
    if (DEBUG)
    {
      Serial.write(command); 
    }

    if (inmoovIsOn)
    {
      // Toutes les commandes sont autorisées
      
      // Test pour éviter de traiter plusieurs fois la même commande
      if (saveCmd != command)
      {
        // Sauvegarde de la commande reçu
        saveCmd = command;
      
        ProcessCmd();
      }
    }
    else
    {
      if (DEBUG)
      {
        // Toutes les commandes sont autorisées pour débugger
        
        // Test pour éviter de traiter plusieurs fois la même commande
        if (saveCmd != command)
        {
          // Sauvegarde de la commande reçu
          saveCmd = command;
        
          ProcessCmd();
        }
      }
      else
      {
        // Seule la première commande est autorisée
        // A voir a l'utilisation et ce qui est alimenté
        // par l'alimentation du NANO... A discuter.
        if (command <= CMD_1)
        {
          // Test pour éviter de traiter plusieurs fois la même commande
          if (saveCmd != command)
          {
            // Sauvegarde de la commande reçu
            saveCmd = command;
          
            ProcessCmd();
          }
        }
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
      audioLVal = analogRead(AUDIO_L_PIN);
      
      // Pas utilisé pour le moment....
      audioRVal = analogRead(AUDIO_R_PIN);
      

      /////////////////////////////////////////////////////////////////////////
      //////////////////////// TEST CODE ANTHONY //////////////////////////////
      /////////////////////////////////////////////////////////////////////////


      // Action à effectuer sur le servo en fonction des valeurs obtenues
      pos = map(audioLVal, MIN, MAX, SERVO_MIN, SERVO_MAX); 
  
      if (val > MIN ) // if values detected : speaker voltage
      {
        i++;
      }
      else  
      {
        BoucheStatus = 1; // closed mouth
      }
  
      if (i >= SecondDetection)
      {
        i=0;
        BoucheStatus = 0;
      }
  
      if (BoucheStatus == 0 && ActionBouche == 0)
      {
        if (Repos==0)
        {
          jawServo.attach(JAW_SERVO_PIN);
          delay(1);
        }

        if (DEBUG)
        {
          Serial.println(val);
        }
      
        ActionBouche = 1;
        jawServo.write(pos);
        CompteurRepos=0;
        delay(1);
      }

      if (BoucheStatus == 1 && ActionBouche == 1)
      {
        Repos = 0;
        CompteurRepos = 0;
        ActionBouche =0;
      }

      if (CompteurRepos == 100 && Repos == 0)
      {
        jawServo.write(SERVO_MIN); 
        delay(10);
        jawServo.detach(); 
        Repos=1;
      }

      CompteurRepos += 1;    
      delay(1);

      /////////////////////////////////////////////////////////////////////////
      //////////////////////// TEST CODE DE MATT //////////////////////////////
      /////////////////////////////////////////////////////////////////////////

      /*if (VOICEdelayFlag == 0) 
      {
        if (audioLVal < 630 || audioLVal > 680 ) // recherche valeur entre 665 et 680 ( a ajuster si besoin )
        {
          VOICEBoucheStatus = 1; // bouche fermée
        }
        else // tant que
        {
          VOICEi++; // la valeur entre 630 et 680 est pas trouvée
        }

        if (VOICEi >= VOICESecondDetection)
        {
          VOICEi = 0;
          VOICEBoucheStatus = 0;
        }
      }
      
      if (VOICEBoucheStatus == 0 && VOICEActionBouche == 0) 
      {
        if (VOICEdelayFlag == 0) 
        {
          VOICEmyservo.write(95);
          VOICEtime = millis();
          VOICEdelayFlag = 1;
        }
        delay(40); //1

        if (VOICEtime + VOICEdelay < millis() and VOICEdelayFlag == 1) 
        {
          VOICEmyservo.write(55);
          VOICEtime = millis();
          VOICEdelayFlag = 2;
        }
        delay(40); //2

        if (VOICEtime + VOICEdelay < millis() and VOICEdelayFlag == 2) 
        {
          VOICEActionBouche = 1;
          VOICEtime = millis();
          VOICEdelayFlag = 0;
        }  
      }

      if ((VOICEBoucheStatus == 1 && VOICEActionBouche == 1) or VOICEdelayFlag == 3)
      {
        if (VOICEdelayFlag == 0) 
        {
          VOICEdelayFlag = 3;
          VOICEmyservo.write(55);
          VOICEtime = millis();
          delay(40); // 3
        }
        
        if (VOICEtime + VOICEdelay < millis()) 
        {
          VOICEActionBouche = 0;
          VOICEtime = millis();
          VOICEdelayFlag = 0;
        }
      }*/

    }
    
    // Mise à jour du ring toutes les secondes
    if ((superMillis() - processTime) >= 1000)
    {
      if (batteryEleIsFailed)
      {
        // La batterie de l'electronique est vide
        if (neoState)
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(150,255,0));
          }
        }
        else
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0,0,0));
          }
        }

        pixels.show();
        neoState = !neoState;
      }
      else if (batteryMotIsFailed)
      {
        // La batterie de la motorisation est vide
        if (neoState)
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(255,150,0));
          }
        }
        else
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0,0,0));
          }
        }

        pixels.show();
        neoState = !neoState;
      }
      else if (batteryMotIsFailed)
      {
        // Les 2 batteries sont vident
        if (neoState)
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(255,0,0));
          }
        }
        else
        {
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0,0,0));
          }
        }

        pixels.show();
        neoState = !neoState;
      }
      else
      {
        // Pas de problème batteries
        if (ringColor == 1)
        {
          // Jaune clair
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(150,150,0));
          }
        }
        else if (ringColor == 2)
        {
          // Rouge
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(255,0,0));
          }
        }
        else if (ringColor == 3)
        {
          // Vert
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0,255,0));
          }
        }
        else if (ringColor == 4)
        {
          // Bleu
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0,0,255));
          }
        }
        else if (ringColor == 5)
        {
          // blanc
          for (i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(255,255,255));
          }
        }
        else
        {
          // Couleur en fonction de l'audio
          // TODO
        }
        // Mise à jour état des pixels
        pixels.show();
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

