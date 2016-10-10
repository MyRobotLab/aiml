#EN : SHUTDOWN THE EAR ACTION AFTER 1mn INACTIVITY
#FR : ON COUPE VIRTUELEMENT LE MICRO APRES 1 MINUTE
StopListenTimer = Runtime.create("StopListenTimer","Clock")
StopListenTimer.setInterval(60000)
StopListenTimer = Runtime.start("StopListenTimer","Clock")

def StopListenTimerFunc(timedata):
	global IcanEarOnlyKnowsWords
	global RobotIsSleepingSoft
	IcanEarOnlyKnowsWords=IcanEarOnlyKnowsWords+1
	print "dbg : IcanEarOnlyKnowsWords=",IcanEarOnlyKnowsWords
	if IcanEarOnlyKnowsWords==1:
		print "Sleeping mode ON"
		RobotIsSleepingSoft=1
		try:
			clockPaupiere.stopClock()
		except: 
			pass
		PositionPaupiere(90,90,0.4)
		sleep(3)
		PaupiereAttach(0)
		rest()
		
	

StopListenTimer.addListener("pulse", python.name, "StopListenTimerFunc")
# start the clock
StopListenTimer.startClock()

def WebkitSpeachReconitionON(timedata):
	
	global Ispeak
	if Ispeak==0:
		try:
			ear.stopListening()
			sleep(0.2)
			ear.startListening()
		except: 
			pass
			

#RANDOM TIME ACTIONS
VieAleatoire = Runtime.start("VieAleatoire","Clock")
VieAleatoire.setInterval(60000)
chatBot.getResponse("SAVEPREDICATES")
global TimeNoSpeak
TimeNoSpeak="OFF"
TuTeTais=0
def OnBalanceUnePhare_Aleatoire(timedata):
	global TimeNoSpeak
	global TuTeTais
	global RamdomSpeak
	global RobotIsStarted
	if RobotIsStarted==1:
		RamdomSpeak=1
	
	VieAleatoire.setInterval(random.randint(60000,600000))
	if TimeNoSpeak=="ON":
		if random.randint(0,1)==1:
			chatBot.getResponse("ALEATOIRE")
		else:
			chatBot.getResponse("ALEATOIRE2")
		
	if TuTeTais==0:
		TimeNoSpeak="ON"
	

def TuTeTais_OuPas(value):
	global TuTeTais
	TuTeTais=value
	
#create a message routes
VieAleatoire.addListener("pulse", python.name, "OnBalanceUnePhare_Aleatoire")
# start the clock
VieAleatoire.startClock()