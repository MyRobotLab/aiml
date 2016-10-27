# -*- coding: utf-8 -*- 
# ##############################################################################
# 					    *** Librairie de fonction pour Activator ***
# ##############################################################################

	 
# Réception
# A FAIRE
if Activator==1 and IsInmoovArduino==1:
	def FonctionReceptionData(data):
	  print "J ai reçu des données les voici", data
    
	ActivatorArduino.addListener("publishCustomMsg","python","FonctionReceptionData")

# Commande à envoyer quand le PC a terminé de charger tout les services  
def pcIsReady():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(2,1)

# Commande à envoyer quand on veut endormir InMoov
def shudownRequest():
  if Activator==1 and IsInmoovArduino==1:
		ActivatorArduino.digitalWrite(3,1)
    
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

#exemple d'integration

#example NeoPixel(iron_man)
#def NeoPixel(action):
#	if Activator==1:
#		print 0
#		#ActivatorArduino.envoyerquelquechose(neopixel,action)
#	if MrlNeopixel==1:
#		# >>> neopixel.setAnimation("Theater Chase", 255, 0, 0, 1) #running Theater Chase with color red at full speed
#		print 0

#example monter le volume SoundControl(30)
#def SoundControl(action):
#	if Activator==1:
#		# >>> ActivatorArduino.envoyerquelquechose(volume_control,action)
#		print 0
	 
#example machoire : JawAction(100) 
#def JawAction(action):
#	if Activator==1:
#		print 0
#		# >>> ActivatorArduino.envoyerquelquechose(jawaction,action)
#	if MRLmouthControl==1:
#		print 0
#		# >>> head.jaw.moveTo(action)
