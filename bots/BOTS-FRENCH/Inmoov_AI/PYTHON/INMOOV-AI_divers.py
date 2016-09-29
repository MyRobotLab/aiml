# -*- coding: utf-8 -*- 

def NeoPixelF(valNeo):
	if Neopixel!="COMX":
		serial.write(valNeo)
	else:
		print(valNeo)

def talk(data):
	if data[0:2]=="l ":
		data=data.replace("l ", "l'")
	data=data.replace(" l ", " l'")
	
	ear.startListening() #fix onclick micro
	
	if data!="":
		mouth.speak(unicode(data,'utf-8'))
		
	if IsInmoovArduino==1:
		if random.randint(1,3)==1:
			i01.head.eyeX.moveTo(0)
			sleep(2)
			i01.head.eyeX.moveTo(180)
			sleep(1)
			i01.head.eyeX.moveTo(90)

def talkBlocking(data):
		
	if data!="":
		mouth.speakBlocking(unicode(data,'utf-8'))
		
def Parse(utfdata):
	#Light(1,1,0)
	utfdata = urllib2.urlopen(utfdata).read()
	utfdata = utfdata.replace("&#039;", "'").replace("http://fr.answers.yahoo.com/question/ind...", "")
	try:
		utfdata = utfdata.decode( "utf8" ).replace(" : ", random.choice(troat))
	except: 
		pass
	#print utfdata
	#Light(1,1,1)
	return utfdata;

##############################################################
# We listen when the robot is starting to speak to avoid ear listening
# If you click on the webkit mic icon, this trick is broken
##############################################################
def onEndSpeaking(text):
	global IcanStartToEar
	global IcanEarOnlyKnowsWords
	print "End speaking debug"
	global MoveHeadRandom
	MoveHeadTimer.stopClock()
	global Ispeak
	Ispeak=0
	global TimeNoSpeak
	VieAleatoire.startClock()
	TimeNoSpeak="OFF"
	#Light(0,0,0)
	if IsInmoovArduino==1:
		i01.moveHead(90,90,90,90,90)
	MoveHeadRandom=1
	
	if IcanStartToEar==1:
		try:
			ear.startListening()
		except: 
			pass
	WebkitSpeachReconitionFix.startClock()
	IcanStartToEar=1
	StopListenTimer.stopClock()
	IcanEarOnlyKnowsWords=-1
	global RobotIsSleepingSoft
	if RobotIsSleepingSoft==1:
		RobotIsSleepingSoft=0
		PaupiereAttach(1)
		sleep(0.1)
		PositionPaupiere(180,180,0.3)
		sleep(3)
		clockPaupiere.startClock()
	StopListenTimer.startClock()
	#sleep(0.2)
	
def onStartSpeaking(text):

	#sleep(0.2)
	print "Start speaking debug"
	global Ispeak
	Ispeak=1
	WebkitSpeachReconitionFix.stopClock()
	global MoveHeadRandom
	if 'non' in text or 'no' in text:
		No('no')
		MoveHeadRandom=0
		#print("no detected")
	if 'oui' in text or 'yes' in text:
		Yes('yes')
		#print("yes detected")
		MoveHeadRandom=0
	if MoveHeadRandom==1:
		MoveHeadTimer.startClock()
	try:
		ear.stopListening()
	except: 
		pass
	global TimeNoSpeak
	TimeNoSpeak="OFF"
	VieAleatoire.stopClock()
	#Light(1,1,1)
	
##############################################################
#We intercept what the robot is listen to change some values
#here we replace ' by space because AIML doesn't like '
##############################################################
def onText(text):
	#print text.replace("'", " ")
	
	global Ispeak
	global ParrotMod
	if Ispeak==0:
		if ParrotMod==0:
			chatBot.getResponse(text.replace("'", " "))
		else:
			chatBot.getResponse("SAY " + text)
		 #we close pictures
	image.exitFS()
	image.closeAll()
	
def WebkitSpeachReconitionON(timedata):
	sleep(0.2)
	global Ispeak
	if Ispeak==0:
		try:
			ear.startListening()
		except: 
			pass
			
def Light(ROUGE_V,VERT_V,BLEU_V):
	if IhaveLights==1 and IsInmoovArduino==1:
		print 0
    
##############################################################
# Cette fonction permet d'afficher une date personnalis(mardi, le 10 juin, 1975, 12h38 .....)	
##############################################################
def getDate(query, ID):
	answer = ( wdf.getTime(query,ID,"day") +" " +wdf.getTime(query,ID,"month") + " " + wdf.getTime(query,ID,"year"))
	#print " La date est : " + answer
	chatBot.getResponse("say Le " + answer)
		

def DisplayPic(pic):
	r=0
	try:
		r=image.displayFullScreen(pic,1)
	except: 
		chatBot.getResponse("PICTUREPROBLEM")
		pass
	time.sleep(0.1)
	try:
		r=image.displayFullScreen(pic,1)
	except:
		pass
	
def UpdateBotName(botname):
	if str(chatBot.getPredicate("default","bot_id"))=="unknown":
		bot_id=hashlib.md5(str(time.time()).encode('utf-8')).hexdigest()
	else:
		bot_id=str(chatBot.getPredicate("default","bot_id"))
	RetourServer=Parse("http://www.myai.cloud/shared_memory.php?action=UpdateBotName&bot_id="+urllib2.quote(bot_id)+"&botname="+urllib2.quote(botname.replace("'", " ")))
	#print "http://www.myai.cloud/shared_memory.php?action=UpdateBotName&bot_id="+urllib2.quote(bot_id)+"&botname="+urllib2.quote(botname.replace("'", " "))
	chatBot.setPredicate("default","bot_id",bot_id)
	chatBot.setPredicate("default","botname",botname)
	chatBot.savePredicates()
	
def CheckVersion():
	RetourServer=Parse("http://www.myai.cloud/version.html")
	#print str(RetourServer)+' '+str(version)
	if str(RetourServer)==str(version):
		print "software is OK"
		#chatBot.getResponse("IAMUPDATED")
	else:
		chatBot.getResponse("INEEDUPDATE")
		sleep(3)
		
def PlayUtub(q,num):
	if q=="stop" and num==0:
		subprocess.Popen("taskkill /F /T /PID %i"%proc1.pid , shell=True)
		sleep(2)
		webgui.startBrowser("http://localhost:8888/#/service/i01.ear")
	else:
		webgui.startBrowser("http://www.myai.cloud/utub/?num="+str(num)+"&q="+str(q).encode('utf-8'))
		#print "http://www.myai.cloud/utub/?num="+str(num)+"&q="+str(q).encode('utf-8')
		
def ShutDown():
	talkBlocking("Extinction")
	MoveHeadRandom=0
	sleep(1)
	if IsInmoovArduino==1:
		i01.setHeadSpeed(RotHeadSpeed+0.1, NeckSpeed+0.1)
		i01.moveHead(0,180)
		HeadSide.moveTo(90)
		clockPaupiere.stopClock()
		sleep(0.2)
		PositionPaupiere(0,0,0.5)
	sleep(5)
	PaupiereServoGauche.detach()
	PaupiereServoDroite.detach()
	HeadSide.detach()
	i01.detach()
	sleep(1)
	runtime.exit()
	
def IdontUnderstand():
	global IcanEarOnlyKnowsWords
	if IcanEarOnlyKnowsWords<=0:
		chatBot.getResponse("IDONTUNDERSTAND")
	else:
		print "robot doesnt understand"
	#runtime.shutdown()

