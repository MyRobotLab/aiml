# -*- coding: utf-8 -*- 

# arduino attach
if Activator==1 and IsInmoovArduino==1:
	if ActivatorControler=="left":
		ActivatorControler=left
	else:
		ActivatorControler=right

  #activator arduino configuration
	ActivatorArduino = Runtime.createAndStart("ActivatorArduino","Arduino")
	ActivatorArduino.setBoard(BoardTypeActivator)
	ActivatorArduino.connect(ActivatorControler,"Serial1")
	
	
		
	
	#recepetion
	def FonctionReceptionData(data):
	  print "J ai reçu des données les voici" , data
    
	ActivatorArduino.addListener("publishCustomMsg","python","FonctionReceptionData")
 

#exemple d'integration
 

#example NeoPixel(iron_man)
def NeoPixel(action):
	if Activator==1:
		print 0
		#ActivatorArduino.envoyerquelquechose(neopixel,action)
	if MrlNeopixel==1:
		# >>> neopixel.setAnimation("Theater Chase", 255, 0, 0, 1) #running Theater Chase with color red at full speed
		print 0

#example monter le volume SoundControl(30)
def SoundControl(action):
	if Activator==1:
		# >>> ActivatorArduino.envoyerquelquechose(volume_control,action)
		print 0
	 
#example machoire : JawAction(100) 
def JawAction(action):
	if Activator==1:
		print 0
		# >>> ActivatorArduino.envoyerquelquechose(jawaction,action)
	if MRLmouthControl==1:
		print 0
		# >>> head.jaw.moveTo(action)