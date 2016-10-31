#EN : SHUTDOWN THE EAR ACTION AFTER 1mn INACTIVITY
#FR : ON COUPE VIRTUELEMENT LE MICRO APRES 1 MINUTE ( mode pause )
StopListenTimer = Runtime.create("StopListenTimer","Clock")
StopListenTimer.setInterval(60000)
StopListenTimer = Runtime.start("StopListenTimer","Clock")

def StopListenTimerFunc(timedata):
	global PleaseRobotDontSleep
	if PleaseRobotDontSleep==0:
		global IcanEarOnlyKnowsWords
		global RobotIsSleepingSoft
		IcanEarOnlyKnowsWords=IcanEarOnlyKnowsWords+1
		if DEBUG==1:
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
			head.detach()
		
	

StopListenTimer.addListener("pulse", python.name, "StopListenTimerFunc")
# start the clock
StopListenTimer.startClock()


LedWebkitListen = Runtime.create("LedWebkitListen","Clock")
LedWebkitListen.setInterval(8000)
LedWebkitListen = Runtime.start("LedWebkitListen","Clock")

global LedWebkitListenFuncFix
LedWebkitListenFuncFix=0


def LedWebkitListenFunc(timedata):
	global LedWebkitListenFuncFix
	if LedWebkitListenFuncFix==1:
		Light(1,1,1)
		LedWebkitListenFuncFix=0
		LedWebkitListen.stopClock()
	else:
		LedWebkitListenFuncFix+=1
		
	

LedWebkitListen.addListener("pulse", python.name, "LedWebkitListenFunc")
# start the clock


def WebkitSpeachReconitionON(timedata):
	global LedWebkitListenFuncFix
	global Ispeak
	if Ispeak==0:
		ear.clearLock()
		ear.resumeListening()
		LedWebkitListenFuncFix=0
		LedWebkitListen.startClock()
		Light(1,1,0)

			

#RANDOM TIME ACTIONS
VieAleatoire = Runtime.start("VieAleatoire","Clock")
VieAleatoire.setInterval(120000)
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
	
	VieAleatoire.setInterval(random.randint(220000,700000))
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

# Timer pour le watchdog
# Envoie un refresh watchdog toutes les 2s
def sendRefresh(timedata):
  watchdogRefresh()
  
watchdogTimer = Runtime.start("watchdogTimer","Clock")
watchdogTimer.setInterval(5000)
watchdogTimer.addListener("pulse", python.name, "sendRefresh")

def startWatchdogTimer():
  watchdogTimer.startClock()




