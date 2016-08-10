
# ##############################################################################
# 							*** SETUP / INSTALLATION ***
# ##############################################################################
# STABLE FILES : https://github.com/MyRobotLab/pyrobotlab/tree/master/home/moz4r  [ AIML + PYTHON ]
# UPDATED DEV FILES :  https://github.com/MyRobotLab/aiml/tree/master/bots/ [ AIML + PYTHON ]
# -----------------------------------
# - Inmoov-AI Version 1.7.2 By Moz4r
# - Credit :
# - Rachel the humanoïde
# - Wikidatafetcher By Beetlejuice
# - Azure translator by Papaoutai
# - Grog / Kwatters / and All MRL team
# - HairyGael
# - Heisenberg
# - Grattounet
# - Lecagnois
# -----------------------------------
# !!! INSTALL : !!!
# !!! PLEASE copy all aiml files to : develop\ProgramAB\bots\YOUR_BOT_NAME\aiml !!!
# !!! AND https://github.com/MyRobotLab/aiml/tree/master/bots/BOTS-FRENCH/INMOOV_AI/TXT to the root of MRL
# !!! + https://github.com/MyRobotLab/aiml/tree/master/bots/BOTS-ENGLISH/INMOOV_AI/TXT
#
# 
# I use realTime voice syncronisation but you can check mouthcontrol=1 in 2-INMOOV-AI_config.py 
# https://github.com/MyRobotLab/pyrobotlab/blob/master/home/moz4r/mouthcontrol_hardware.ino
# -
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  !!!!!!!!!!!!!!!! CONFIG INSIDE THIS FILE !!! / ENTREZ VOS PARAMETRES DANS CE FICHIER  !!!!!!!!!!
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# 						2-INMOOV-AI_config.py


# ###
# 
# ##############################################################################
#  						*** END SETUP ***
# ##############################################################################




version=17
IcanStartToEar=0

#Python libraries

import urllib2
from java.lang import String
import random
import threading
import io
import itertools
import random
import time
import textwrap
import codecs
import socket
import os
import shutil
import hashlib
import subprocess
import json
from subprocess import Popen, PIPE



#check runing folder
oridir=os.getcwd().replace("\\", "/")


# check if a config file exist or create default one
if os.path.isfile(os.getcwd().replace("develop", "").replace("\\", "/")+'2-INMOOV-AI_config.py'):
	print("ok")
else:
	shutil.copyfile(os.getcwd().replace("develop", "").replace("\\", "/")+'2-INMOOV-AI_config.py.default',os.getcwd().replace("develop", "").replace("\\", "/")+'2-INMOOV-AI_config.py')
execfile('../2-INMOOV-AI_config.py')
	
gesturesPath = os.getcwd().replace("develop", "").replace("\\", "/")+"gestures"
BotURL=BotURL+"?lang="+lang+"&FixPhpCache="+str(time.time())

#fix programab aimlif problems : remove all aimlif files
print oridir+'/ProgramAB/bots/'+myAimlFolder+'/aimlif'
try:
	shutil.rmtree(oridir+'/ProgramAB/bots/'+myAimlFolder+'/aimlif')
except: 
	pass

#some voice emotions
laugh = [" #LAUGH01# ", " #LAUGH02# ", " #LAUGH03# ", " ", " "]
troat = [" #THROAT01# ", " #THROAT02# ", " #THROAT03# ", " ", " ", " "]


#service pictures
image=Runtime.createAndStart("ImageDisplay", "ImageDisplay")

#service aiml
Runtime.createAndStart("chatBot", "ProgramAB")

#service wikidata
wdf=Runtime.createAndStart("wdf", "WikiDataFetcher")

#service inmoov
i01 = Runtime.createAndStart("i01", "InMoov")

#disable autocheck
i01.setMute(1)

#start acapela and webkit ear
i01.startMouth()
i01.startEar()
ear = i01.ear
mouth = i01.mouth

#start webgui
webgui = Runtime.create("WebGui","WebGui")
webgui.autoStartBrowser(False)
webgui.startService()

#r=image.displayFullScreen("https://i.ytimg.com/vi/tIk1Mc170yg/maxresdefault.jpg",1)
sleep(0.1)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\loading.jpg',1)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\loading.jpg',1)
#webgui.start()

# inmoov init
# check arduino right
if IsInmoovRight==1:

	right = Runtime.start("i01.right", "Arduino")
	right.publishState()
	right.connect(rightPort)
	sleep(1)
	if IhaveLights==1:

		right.pinMode(ROUGE, Arduino.OUTPUT)
		right.pinMode(VERT, Arduino.OUTPUT)
		right.pinMode(BLEU, Arduino.OUTPUT)
		

		right.digitalWrite(ROUGE,1)
		right.digitalWrite(VERT,0)
		right.digitalWrite(BLEU,1)
	
	
	i01.startRightArm(rightPort)
	i01.startRightHand(rightPort,BoardType)
	
# check arduino left	
if IsInmoovLeft==1:	
	
	left = Runtime.start("i01.left", "Arduino")
	left.publishState()
	left.connect(leftPort)
	sleep(1)
	i01.startLeftHand(leftPort,"")
	head = Runtime.create("i01.head","InMoovHead")
	if MRLmouthControl==1:
		head.jaw.setMinMax(JawMIN,JawMAX)
		head.jaw.map(0,180,JawMIN,JawMAX)
		head.jaw.setRest(0)
		i01.startMouthControl(leftPort)
		i01.mouthControl.setmouth(0,180)
	i01.setHeadSpeed(0.5, 0.5)
	i01.startHead(leftPort,BoardType)
	i01.startLeftArm(leftPort)
		
	i01.leftHand.thumb.setMinMax(ThumbLeftMIN,ThumbLeftMAX) 
	i01.leftHand.index.setMinMax(IndexLeftMIN,IndexLeftMAX) 
	i01.leftHand.majeure.setMinMax(majeureLeftMIN,majeureLeftMAX) 
	i01.leftHand.ringFinger.setMinMax(ringFingerLeftMIN,ringFingerLeftMAX) 
	i01.leftHand.pinky.setMinMax(pinkyLeftMIN,pinkyLeftMAX) 
	i01.leftHand.thumb.map(0,180,ThumbLeftMIN,ThumbLeftMAX) 
	i01.leftHand.index.map(0,180,IndexLeftMIN,IndexLeftMAX) 
	i01.leftHand.majeure.map(0,180,majeureLeftMIN,majeureLeftMAX) 
	i01.leftHand.ringFinger.map(0,180,ringFingerLeftMIN,ringFingerLeftMAX) 
	i01.leftHand.pinky.map(0,180,majeureLeftMIN,majeureLeftMAX) 

	
	torso = i01.startTorso(leftPort)
	
	i01.head.neck.setMinMax(MinNeck,MaxNeck)
	if param1==1: #param1 = inversion du servo du cou
		i01.head.neck.map(0,180,MaxNeck,MinNeck)
	else:
		i01.head.neck.map(0,180,MinNeck,MaxNeck)
	i01.head.neck.setMinMax(0,180)
	
	i01.head.rothead.setMinMax(MinRotHead,MinRotHead)
	if param2==1: #param2 = inversion du servo du cou
		i01.head.rothead.map(0,180,MaxRotHead,MinRotHead)
	else:
		i01.head.rothead.map(0,180,MinRotHead,MaxRotHead)
		
	i01.head.rothead.setMinMax(0,180)
	i01.moveHead(80,86,40,78,76)
	i01.head.eyeX.setMinMax(EyeXMIN,EyeXMAX)
	i01.head.eyeX.map(0,180,EyeXMIN,EyeXMAX)
	i01.head.eyeX.setMinMax(0,180)
	i01.head.eyeY.setMinMax(EyeYMIN,EyeYMAX)
	i01.head.eyeY.map(0,180,EyeYMIN,EyeYMAX)
	i01.head.eyeY.setMinMax(0,180)
	i01.head.eyeX.setRest(90)
	i01.head.eyeY.setRest(90)
	i01.head.eyeY.rest()
	i01.head.eyeX.rest()
	i01.startEyesTracking(leftPort)
	i01.startHeadTracking(leftPort)
	i01.eyesTracking.pid.setPID("eyeX",20.0,5.0,0.1)
	i01.eyesTracking.pid.setPID("eyeY",20.0,5.0,0.1)
	i01.headTracking.pid.setPID("rothead",12.0,5.0,0.1)
	i01.headTracking.pid.setPID("neck",12.0,5.0,0.1)
		
	
# start opencv service
if IsInmoovLeft==1 or IsInmoovRight==1:
	opencv = i01.opencv
	


Runtime.createAndStart("htmlFilter", "HtmlFilter")


voiceType=Voice


if lang=="FR":
   WikiFile="prop.txt"
   wdf.setLanguage("fr")
   wdf.setWebSite("frwiki")
else:
   WikiFile="propEN.txt"
   wdf.setLanguage("en")
   wdf.setWebSite("enwiki")


sleep(1)
mouth.setVoice(voiceType)
mouth.setLanguage(lang)




chatBot.startSession("ProgramAB", "default", myAimlFolder)
chatBot.addTextListener(htmlFilter)
htmlFilter.addListener("publishText", python.name, "talk") 


			

#var to set when robot is speaking
 
global Ispeak
Ispeak=1
global MoveHeadRandom
MoveHeadRandom=1

chatBot.startSession("ProgramAB", "default", myAimlFolder)
#ear.addTextListener(chatBot)
chatBot.addTextListener(htmlFilter)
htmlFilter.addListener("publishText", python.name, "talk") 

if Neopixel!="COMX":
	serial = Runtime.createAndStart("serial","Serial")
	serial.connect(Neopixel, 9600, 8, 1, 0)

def NeoPixelF(valNeo):
	if Neopixel!="COMX":
		serial.write(valNeo)
	else:
		print(valNeo)

NeoPixelF(3)

def No(data):
	global MoveHeadRandom
	MoveHeadRandom=0
	if IsInmoovLeft==1:
		#i01.attach()
		i01.setHeadSpeed(0.98, 0.98)
		i01.moveHead(80,130)
		sleep(0.5)
		i01.moveHead(80,90)
		sleep(0.5)
		i01.moveHead(80,50)
		sleep(0.5)
	#Light(0,1,1)
	if IsInmoovLeft==1:
		i01.moveHead(81,90)
		sleep(0.5)
		i01.moveHead(79,130)
	if IsInmoovLeft==1:
		i01.moveHead(80,90)
		sleep(0.5)
		i01.moveHead(83,50)
	sleep(0.5)
	#Light(1,1,1)
	if IsInmoovLeft==1:
		i01.moveHead(80,90)
	if IsInmoovLeft==1:
		i01.head.jaw.rest()

def Yes(data):
	global MoveHeadRandom
	MoveHeadRandom=0
	if IsInmoovLeft==1:
		#i01.attach()
		i01.setHeadSpeed(0.98, 0.98)
		i01.moveHead(130,90)
		sleep(0.5)
		i01.moveHead(50,93)
		sleep(0.5)
		i01.moveHead(130,90)
		sleep(0.5)
	#Light(0,1,1)
	if IsInmoovLeft==1:
		i01.moveHead(60,91)
		sleep(0.5)
		i01.moveHead(120,88)
	if IsInmoovLeft==1:
		i01.moveHead(70,90)
		sleep(0.5)
		i01.moveHead(95,90)
	sleep(0.5)
	#Light(1,1,1)
	if IsInmoovLeft==1:
		i01.moveHead(90,90)
	if IsInmoovLeft==1:
		i01.head.jaw.rest()
	
			
def talk(data):
	
	if data!="":
		try:
			ear.stopListening()
		except: 
			pass
		mouth.speak(unicode(data,'utf-8'))
		
	if IsInmoovLeft==1:
		if random.randint(1,3)==1:
			i01.head.eyeX.moveTo(0)
			sleep(2)
			i01.head.eyeX.moveTo(180)
			sleep(1)
			i01.head.eyeX.moveTo(90)

def talkBlocking(data):
		
	if data!="":
		try:
			ear.stopListening()
		except: 
			pass
		
		mouth.speakBlocking(unicode(data,'utf-8'))

#We include all InmoovAI mods
# -- coding: utf-8 --
execfile('../INMOOV-AI_memory.py')
if IhaveEyelids==1:
	execfile('../INMOOV-AI_paupieres_eyeleads.py')
execfile('../INMOOV-AI_vie_aleatoire-standby_life.py')
if IsInmoovLeft==1:
	execfile('../INMOOV-AI_opencv.py')
execfile('../INMOOV-AI_move_head_random.py')
execfile('../INMOOV-AI_azure_translator.py')
execfile('../INMOOV-AI_messenger.py')
execfile('../INMOOV-AI_wikidata.py')
execfile('../INMOOV-AI_games.py')
execfile('../INMOOV-AI_gestures.py')
execfile('../INMOOV-AI_domotique.py')

# We listen when the robot is starting to speak to avoid ear listening
# If you click on the webkit mic icon, this trick is broken


def onEndSpeaking(text):
	global MoveHeadRandom
	MoveHeadTimer.stopClock()
	global Ispeak
	Ispeak=0
	global TimeNoSpeak
	VieAleatoire.startClock()
	TimeNoSpeak="OFF"
	#Light(0,0,0)
	if IsInmoovLeft==1:
		i01.moveHead(90,90,90,90,90)
	MoveHeadRandom=1
	sleep(1)
	if IcanStartToEar==1:
		try:
			ear.startListening()
		except: 
			pass
	WebkitSpeachReconitionFix.startClock()

	
def onStartSpeaking(text):

	WebkitSpeachReconitionFix.stopClock()
	global MoveHeadRandom
	if 'non' in text or 'no' in text:
		No('no')
		MoveHeadRandom=0
		print("no detected")
	if 'oui' in text or 'yes' in text:
		Yes('yes')
		print("yes detected")
		MoveHeadRandom=0
	if MoveHeadRandom==1:
		MoveHeadTimer.startClock()
	global Ispeak
	Ispeak=1
	try:
		ear.stopListening()
	except: 
		pass
	global TimeNoSpeak
	TimeNoSpeak="OFF"
	VieAleatoire.stopClock()
	#Light(1,1,1)
	
	
#We intercept what the robot is listen to change some values
#here we replace ' by space because AIML doesn't like '
def onText(text):
	ear.stopListening()
	talk(" ")
	print text.replace("'", " ")
	global Ispeak
	if Ispeak==0:
		chatBot.getResponse(text.replace("'", " "))
	

	
python.subscribe(mouth.getName(),"publishStartSpeaking")
python.subscribe(mouth.getName(),"publishEndSpeaking")


#Timer function to autostart webkit microphone every 10seconds
WebkitSpeachReconitionFix = Runtime.start("WebkitSpeachReconitionFix","Clock")
WebkitSpeachReconitionFix.setInterval(10000)

def WebkitSpeachReconitionON(timedata):
	
	global Ispeak
	if Ispeak==0:
		try:
			ear.startListening()
		except: 
			pass
			
WebkitSpeachReconitionFix.addListener("pulse", python.name, "WebkitSpeachReconitionON")




		
def Parse(utfdata):
	#Light(1,1,0)
	utfdata = urllib2.urlopen(utfdata).read()
	utfdata = utfdata.replace("&#039;", "'").replace("http://fr.answers.yahoo.com/question/ind...", "")
	try:
		utfdata = utfdata.decode( "utf8" ).replace(" : ", random.choice(troat))
	except: 
		pass
	print utfdata
	#Light(1,1,1)
	return utfdata;


		
def Light(ROUGE_V,VERT_V,BLEU_V):
	if IhaveLights==1 and IsInmoovRight==1:
		right.digitalWrite(ROUGE,ROUGE_V)
		right.digitalWrite(VERT,VERT_V)
		right.digitalWrite(BLEU,BLEU_V)



	
def getDate(query, ID):# Cette fonction permet d'afficher une date personnalisée (mardi, le 10 juin, 1975, 12h38 .....)
	answer = ( wdf.getTime(query,ID,"day") +" " +wdf.getTime(query,ID,"month") + " " + wdf.getTime(query,ID,"year"))
	print " La date est : " + answer
	chatBot.getResponse("say Le " + answer)
	
def FindImage(image):
	try:
		image = image.decode( "utf8" )
	except: 
		pass
	mouth.speak(image)
	#PLEASE USE REAL LANGUAGE PARAMETER :
	#lang=XX ( FR/EN/RU/IT etc...)
	#A FAKE LANGUAGE WORKS BUT DATABASE WILL BROKE
	a = Parse(BotURL+"&type=pic&pic="+urllib2.quote(image).replace(" ", "%20"))
	
	DisplayPic(a)
	print BotURL+"&type=pic&pic="+urllib2.quote(image).replace(" ", "%20")
	#Light(1,1,1)
			
	



	
def UpdateBotName(botname):
	if str(chatBot.getPredicate("default","bot_id"))=="unknown":
		bot_id=hashlib.md5(str(time.time()).encode('utf-8')).hexdigest()
	else:
		bot_id=str(chatBot.getPredicate("default","bot_id"))
	RetourServer=Parse("http://www.myai.cloud/shared_memory.php?action=UpdateBotName&bot_id="+urllib2.quote(bot_id)+"&botname="+urllib2.quote(botname.replace("'", " ")))
	print "http://www.myai.cloud/shared_memory.php?action=UpdateBotName&bot_id="+urllib2.quote(bot_id)+"&botname="+urllib2.quote(botname.replace("'", " "))
	chatBot.setPredicate("default","bot_id",bot_id)
	chatBot.setPredicate("default","botname",botname)
	chatBot.savePredicates()
	


	
def CheckVersion():
	RetourServer=Parse("http://www.myai.cloud/version.html")
	print str(RetourServer)+' '+str(version)
	if str(RetourServer)==str(version):
		print "software is OK"
		#chatBot.getResponse("IAMUPDATED")
	else:
		chatBot.getResponse("INEEDUPDATE")
	
		
def Meteo(data):
	a = Parse(BotURL+"&type=meteo&units="+units+"&city="+urllib2.quote(data).replace(" ", "%20"))
	print BotURL+"&type=meteo&units="+units+"&city="+urllib2.quote(data).replace(" ", "%20")
	mouth.speakBlocking(a)
	
#this is broken now need fix on myai.cloud
def question(data):
	chatBot.getResponse("FINDTHEWEB")
	a = Parse(BotURL+"&type=question&question="+urllib2.quote(data).replace(" ", "%20"))
	print BotURL+"&type=question&question="+urllib2.quote(data).replace(" ", "%20")
	if a[0]=="0":
		return("IDONTUNDERSTAND")
	elif a[0:299]<>"":
		#return(a[0:299])
		return("IDONTUNDERSTAND")
	else:
		return("IDONTUNDERSTAND")


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
	time.sleep(10)
	image.exitFS()
	image.closeAll()


def trackHumans():
	#i01.headTracking.findFace()
	#i01.opencv.SetDisplayFilter
	i01.headTracking.faceDetect()
	i01.eyesTracking.faceDetect()
	print "test"

def TakePhoto(messagePhoto):
	talkBlocking(messagePhoto)
	global FaceDetected
	global FaceDetectedCounter
	global startTimerFunction
	FaceDetectedCounter=0
	FaceDetected=0
	Light(0,0,0)
	startTimerFunction=0
	NoFaceDetectedTimer.startClock()
	#opencv.setInputSource("camera")
	#opencv.setCameraIndex(0)
	#opencv.addFilter("pdown","PyramidDown")
	#opencv.setDisplayFilter("pdown")
	#opencv.capture()
	#sleep(1)
	#photoFileName = opencv.recordSingleFrame()
	#print "name file is" , photoFileName

def PhotoProcess(messagePhoto):
	global FaceDetected
	Light(1,1,1)
	FaceDetectedCounter=0
	FaceDetected=1
	NoFaceDetectedTimer.stopClock()
	NeoPixelF(3)
	talkBlocking(messagePhoto)
	Light(1,1,1)
	talkBlocking("chi i i i i i i i i ize")
	sleep(0.5)
	Light(0,0,0)
	sleep(0.1)
	Light(1,1,1)
	sleep(0.1)
	Light(0,0,0)
	sleep(0.1)
	Light(1,1,1)
	sleep(0.1)
	i01.stopTracking()
	opencv.removeFilters()
	opencv.stopCapture()
	sleep(1)
	opencv.setInputSource("camera")
	opencv.setCameraIndex(0)
	opencv.capture()
	sleep(0.5)
	Light(0,0,0)
	photoFileName = opencv.recordSingleFrame()
	print "name file is" , os.getcwd()+'\\'+str(photoFileName)
	Light(1,1,1)
	NeoPixelF(1)
	DisplayPic(os.getcwd()+'\\'+str(photoFileName))
	opencv.removeFilters()
	opencv.stopCapture()
	i01.startEyesTracking(leftPort)
	i01.startHeadTracking(leftPort)
	i01.eyesTracking.faceDetect()
	

def PlayUtub(q,num):
	if q=="stop" and num==0:
		subprocess.Popen("taskkill /F /T /PID %i"%proc1.pid , shell=True)
		sleep(2)
		webgui.startBrowser("http://localhost:8888/#/service/i01.ear")
	else:
		webgui.startBrowser("http://www.myai.cloud/utub/?num="+str(num)+"&q="+str(q).encode('utf-8'))
		print "http://www.myai.cloud/utub/?num="+str(num)+"&q="+str(q).encode('utf-8')
	
# ##########################################################	

#gestures



	
	
	
#######


# program start :

Light(1,1,0)
ClearMemory()
if myBotname!="":
	UpdateBotName(myBotname)
#print gesturesPath
CheckVersion()
chatBot.getResponse("WAKE_UP")


rest()
if IsInmoovLeft==1:
	i01.head.attach()
if IsInmoovLeft==1 and tracking==1:
	trackHumans()

sleep(4)


proc1 = subprocess.Popen("%programfiles(x86)%\Google\Chrome\Application\chrome.exe", stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
sleep(0.5)
webgui.startBrowser("http://localhost:8888/#/service/i01.ear")
Light(1,1,1)
if lang=="FR":
   ear.setLanguage("fr-FR")
python.subscribe(ear.getName(),"publishText")
IcanStartToEar=1
WebkitSpeachReconitionFix.startClock()
#r=image.displayFullScreen("http://vignette2.wikia.nocookie.net/worldsofsdn/images/7/7a/Tyrell-corp.jpg",1)

if str(chatBot.getPredicate("default","botname"))!="unknown" and str(chatBot.getPredicate("default","botname"))!="default" and str(chatBot.getPredicate("default","botname"))!="":
	UpdateBotName(str(chatBot.getPredicate("default","botname")))
#if str(chatBot.getPredicate("default","bot_id"))!="unknown":
	#chatBot.getResponse("MESSAGESCHECK")
sleep(0.5)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\logo.jpg',1)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\logo.jpg',1)
Light(1,1,1)
NeoPixelF(1)
talk(" ")
GetUnreadMessageNumbers("0")
