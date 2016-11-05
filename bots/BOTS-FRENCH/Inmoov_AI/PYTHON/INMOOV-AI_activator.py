# -*- coding: utf-8 -*- 
# ##############################################################################
# 					    *** Librairie de fonction pour Activator ***
# ##############################################################################

	 
# Réception
# A FAIRE
if Activator==1 and IsInmoovArduino==1:
	def receiveActivatorData(data):
	  print "J ai reçu des données les voici", data
    # A VOIR CE QUI A DANS DATA
    #BatteryElectValue=0
    #BatteryMotorValue=0
    #AudioVolume=30

# Fonction d'écoute pour la réception des données Activator    
if Activator==1 and IsInmoovArduino==1:
	ActivatorArduino.addListener("publishCustomMsg","python","receiveActivatorData")

# Commande à envoyer quand le PC a terminé de charger tout les services
# Attention, le watchdog est automatiquement activé.
def pcIsReady():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(2,1)

# Commande à envoyer quand on veut endormir InMoov
def shudownRequest():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(3,1)
    
# Commande à envoyer périodiquement pour le watchdog  
def watchdogRefresh():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(17,1)

# Permet de désactiver le watchdog
# pour debug par exemple
def disableWatchdog():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(17,0)

# Demande la mise à jour des valeurs batteries
def updateBatterieRequest(bat):
  if Activator==1 and IsInmoovArduino==1:
    if bat==1:
      ActivatorArduino.analogWrite(1,0)
    else:
      ActivatorArduino.analogWrite(2,0)

# Demande le niveau audio en cours
def updateAudioLevel():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.analogWrite(10,0)

# Commande machoire : JawAction(action)
# action = "ouvre"
# action = "ferme"
def JawAction(action):
  if Activator==1 and IsInmoovArduino==1:
    if action=="ouvre":
      ActivatorArduino.digitalWrite(4,1)
    else:
      ActivatorArduino.digitalWrite(4,0)
	#if MRLmouthControl==1:
	#	print 0
		# >>> head.jaw.moveTo(action)

# Commande machoire : JawBlock()
# Désactive les mouvements avec l'audio
def JawBlock():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(5,1)

# Commande machoire : JawDeBlock()
# Active les mouvements avec l'audio (par défaut)
def JawDeBlock():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(5,0)

# Commande machoire : JawDetach()
# Coupe le signal PWM sur le servo JAW
def JawDetach():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(14,1)

# NeoPixelColor("rouge")
def NeoPixelColor(action):
  if Activator==1 and IsInmoovArduino==1:
    # Animation par défaut
    ActivatorArduino.digitalWrite(12,0)
    # Couleur
    if action=="noir":
      ActivatorArduino.digitalWrite(6,0)
    if action=="jaune":
      ActivatorArduino.digitalWrite(6,1)
    if action=="rouge":
      ActivatorArduino.digitalWrite(7,1)
    if action=="vert":
      ActivatorArduino.digitalWrite(8,1)
    if action=="bleu":
      ActivatorArduino.digitalWrite(9,1)
    if action=="Cyan":
      ActivatorArduino.digitalWrite(10,1)
    if action=="rose":
      ActivatorArduino.digitalWrite(11,1)
	#if MrlNeopixel==1:
		# >>> neopixel.setAnimation("Theater Chase", 255, 0, 0, 1) #running Theater Chase with color red at full speed
		#print 0
    
# NeoPixelCustomColor(100,200,50)
def NeoPixelCustomColor(r, g, b):
  if Activator==1 and IsInmoovArduino==1:
    # Animation par défaut
    ActivatorArduino.setTrigger(r,g,b)
    
# NeoPixelAnimation(action)
# Action = 0, 1 ou 2
def NeoPixelAnimation(action):
  if Activator==1 and IsInmoovArduino==1:
    if action==0:
      ActivatorArduino.digitalWrite(12,0)
    if action==1:
      ActivatorArduino.digitalWrite(12,1)
    if action==2:
      ActivatorArduino.digitalWrite(13,1)
	#if MrlNeopixel==1:
		# >>> neopixel.setAnimation("Theater Chase", 255, 0, 0, 1) #running Theater Chase with color red at full speed
		#print 0

# Règle le volume audio SoundControl(30)
# Volume audio de 0 à 63
def SoundControl(action):
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(20,action)
    
# Fonction SoundMute()
def SoundMute():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(16,1)

# Fonction SoundMuteOff()
def SoundMuteOff():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(16,0)

# Fonction SoundDisable()
def SoundDisable():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(15,1)

# Fonction SoundEnable()
def SoundEnable():
  if Activator==1 and IsInmoovArduino==1:
    ActivatorArduino.digitalWrite(15,0)

