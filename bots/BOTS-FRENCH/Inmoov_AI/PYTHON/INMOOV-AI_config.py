#Robot Name
#please, there is no verifications ! don't use an other used robot name ( usefull to use Inmoov Messenger )
myBotname=""
myAimlFolder="inmoovai" # dossier ou se trouvent vos aiml pas d'accent svp
# LANGUAGE ( FR/EN )
lang="FR"
global Voice
Voice="Antoine" # ryan in english
# To fetch weather etc [ adresse du serveur fetcher meteo etc ...]
BotURL="http://myai.cloud/bot1.php"
units="metric" # or : imperial
# ##
#
# ARDUINO CONFIG IF YOU DIDNT HAVE MOTORS set inmoov=0 / Si vous n'avez pas de servo : inmoov=0
IsInmoovLeft=0
IsInmoovRight=0
MRLmouthControl=0
BoardType="atmega2560" # atmega168 | atmega328 | atmega328p | atmega2560 | atmega1280 | atmega32u4
leftPort = "COM3"
rightPort = "COM4"
# other things
Neopixel = "COMX"
DiyServo1 = "COMX"
DiyServo2 = "COMX"
DiyServo3 = "COMX"
DiyServo4 = "COMX"


#Eyeleads / paupieres
IhaveEyelids=0
PaupiereServoPin = 27

# Ligh if you have / led RGB with pins
IhaveLights = 0

ROUGE=29
VERT=28
BLEU=30

#tracking for testing
tracking=0


# !!! IMPORTANT !!! Your Servos Values :

global PaupiereMIN
global PaupiereMAX
PaupiereMIN=10
PaupiereMAX=50
MinRotHead=0
MaxRotHead=180
RotHeadInverted=0
MinNeck=0
MaxNeck=180
NeckInverted=0
JawMIN=0
JawMAX=180
JawInverted=0
EyeXMIN=0
EyeXMAX=180
EyeYMIN=0
EyeYMAX=180
BicepsLeftMIN=0
BicepsLeftMAX=180
ShoulderLeftMIN=0
ShoulderLeftMAX=180
RotateLeftMIN=0
RotateLeftMAX=180
OmoplateLeftMIN=0
OmoplateLeftMAX=180
ThumbLeftMIN=0
ThumbLeftMAX=180
ThumbLeftInverted=0
IndexLeftMIN=0
IndexLeftMAX=180
majeureLeftMIN=0
majeureLeftMAX=180
ringFingerLeftMIN=0
ringFingerLeftMAX=180
pinkyLeftMIN=0
pinkyLeftMAX=0
WristLeftMIN=0
WristLeftMAX=180
BicepsRightMIN=0
BicepsRightMAX=180
ShoulderRightMIN=0
ShoulderRightMAX=180
RotateRightMIN=0
RotateRightMAX=180
OmoplateRightMIN=0
OmoplateRightMAX=180
ThumbRightMIN=0
ThumbRightMAX=180
ThumbRightInverted=0
IndexRightMIN=0
IndexRightMAX=180
majeureRightMIN=0
majeureRightMAX=180
ringFingerRightMIN=0
ringFingerRightMAX=180
pinkyRightMIN=0
pinkyRightMAX=0
WristRightMIN=0
WristRightMAX=180
# translator : https://datamarket.azure.com/dataset/bing/microsofttranslator 
global Azure_client_id
global Azure_client_secret
Azure_client_id = '7da9defb-7d86-46e4-8607-5b82f280f430'
Azure_client_secret = 'IgCffuxo0QgHDzKMK1k5bf9/9GpP9ZwWwAaonacAcW0'
 
param1=0
param2=0
param3=0
param4=0
param5=0
param6=0
param7=0
param8=0
param9=0
param10=0
