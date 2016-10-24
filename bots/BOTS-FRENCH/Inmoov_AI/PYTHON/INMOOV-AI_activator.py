# -*- coding: utf-8 -*- 
#witch arduino attach
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
 
