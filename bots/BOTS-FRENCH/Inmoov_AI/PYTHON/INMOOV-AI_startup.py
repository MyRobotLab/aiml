# ##############################################################################
# 							         *** SETUP / INSTALLATION ***
# ##############################################################################
# 
# - Inmoov-AI Version 2.1.5 By Moz4r
# - Credit :
# - Rachel the humanoïde
# - Wikidatafetcher By Beetlejuice
# - Azure translator by Papaoutai
# - Grog / Kwatters / Calamity and All MRL team
# - HairyGael
# - Heisenberg
# - Grattounet
# - Lecagnois
# - Dom
# ------------------------------------------------------------------------------
#									multilingual base python script
#							( you just need translate all the aiml :)
# ------------------------------------------------------------------------------
# !!! INSTALL : ( if you get this from github ) !!!
# !!! PLEASE copy all aiml files to : ProgramAB\bots\YOUR_BOT_NAME\aiml !!!
# !!! AND https://github.com/MyRobotLab/aiml/tree/master/bots/BOTS-FRENCH/INMOOV_AI/BDD + SOUND + PICTURES to the root of MRL
#
# 
# I use realTime voice syncronisation but you can check mouthcontrol=1 in INMOOV-AI_config.py 
# https://github.com/MyRobotLab/pyrobotlab/blob/master/home/moz4r/mouthcontrol_hardware.ino
# -
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  !!!!!!!!!!!!!!!! CONFIG INSIDE THIS FILE !!! / ENTREZ VOS PARAMETRES DANS CE FICHIER  !!!!!!!!!!
#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
# 						INMOOV-AI_config.py
# 
# ##############################################################################
#  						               *** END SETUP ***
# ##############################################################################

# Quelle est cette version
version=21

# ##############################################################################
# Variables global
# ##############################################################################
global PaupiereGaucheMIN
global PaupiereGaucheMAX
global PaupiereDroiteMIN
global PaupiereDroiteMAX
global IhaveEyelids
global PaupiereDroiteServoPin
global PaupiereGaucheServoPin
global Voice
global openCvModule
#EN : We wait startup before robot can start to ear
global IcanStartToEar
IcanStartToEar=0
#EN : After timer we don't want the robot listen everything we say
global IcanEarOnlyKnowsWords
IcanEarOnlyKnowsWords=-1
#Robot state
global RobotIsStarted
RobotIsStarted=0
global PleaseRobotDontSleep
PleaseRobotDontSleep=0
global RobotIsSleepingSoft
RobotIsSleepingSoft=0
global ParrotMod
ParrotMod=0
global RamdomSpeak
RamdomSpeak=0
#var to set when robot is speaking
global Ispeak
Ispeak=1
global MoveHeadRandom
MoveHeadRandom=1
# Variable pour Activator
global BatteryElectValue
BatteryElectValue=0
global BatteryMotorValue
BatteryMotorValue=0
global AudioVolume
AudioVolume=30

# Some voice emotions
laugh = [" #LAUGH01# ", " #LAUGH02# ", " #LAUGH03# ", " ", " "]
troat = [" #THROAT01# ", " #THROAT02# ", " #THROAT03# ", " ", " ", " "]

# ##############################################################################
# Python libraries
# ##############################################################################
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
import time
import csv
from datetime import datetime
from subprocess import Popen, PIPE
from org.myrobotlab.service import Servo

# ##############################################################################
# Check running folder
# ##############################################################################
oridir=os.getcwd().replace("\\", "/")+"/"

# ##############################################################################
# check if a config file exist or create default one
# ##############################################################################
if os.path.isfile(oridir + 'CONFIG/INMOOV-AI_config.py'):
	print("config file ok")
else:
	shutil.copyfile(oridir + 'CONFIG/INMOOV-AI_config.py.default',oridir + 'CONFIG/INMOOV-AI_config.py')
	
if os.path.isfile(oridir + 'CONFIG/INMOOV-AI_ServoParam.py'):
	print("servo file ok")
else:
	shutil.copyfile(oridir + 'CONFIG/INMOOV-AI_ServoParam.py.default',oridir + 'CONFIG/INMOOV-AI_ServoParam.py')

# ##############################################################################
# Chargement des paramètres de config
# ##############################################################################

# -- coding: utf-8 --
execfile(u'CONFIG/INMOOV-AI_config.py')
execfile(u'CONFIG/INMOOV-AI_ServoParam.py')

# ##############################################################################
# Chargement des paramètres personnalisés
# ##############################################################################

# ##############################################################################
# Initialisation hardware
# ##############################################################################
execfile(u'INMOOV-AI_InitHardware.py')
	
# ##############################################################################
# Gesture
# ##############################################################################
gesturesPath = (oridir)+"gestures"

# ##############################################################################
# API MyAI ( partage de connaisances / messenger / Recherche d images )
# ##############################################################################
BotURL=BotURL+"?lang="+lang+"&FixPhpCache="+str(time.time())

# ##############################################################################
# Fix programab aimlif problems : remove all aimlif files
# ##############################################################################
try:
	shutil.rmtree(oridir+'ProgramAB/bots/'+myAimlFolder+'/aimlif')
except: 
	pass

# ##############################################################################
# Service pictures
# ##############################################################################
image=Runtime.createAndStart("ImageDisplay", "ImageDisplay")
r=image.displayFullScreen('pictures\loading.jpg',1)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\loading.jpg',1)

# ##############################################################################
# Service aiml
# ##############################################################################
Runtime.createAndStart("chatBot", "ProgramAB")

# ##############################################################################
# Service wikidata
# ##############################################################################
Runtime.createAndStart("wdf", "WikiDataFetcher")

# ##############################################################################
# Open cv lattepanda tweak
# ##############################################################################
opencv = Runtime.create("i01.opencv", "OpenCV")
if LATTEPANDA==1:
	opencv.setFrameGrabberType("org.myrobotlab.opencv.SarxosFrameGrabber")
opencv = Runtime.start("i01.opencv", "OpenCV")

# ##############################################################################
# Service WebGui
# ##############################################################################
webgui = Runtime.create("WebGui","WebGui")
webgui.autoStartBrowser(False)
webgui.startService()

#r=image.displayFullScreen("https://i.ytimg.com/vi/tIk1Mc170yg/maxresdefault.jpg",1)
sleep(0.1)

Runtime.createAndStart("htmlFilter", "HtmlFilter")

voiceType=Voice

if lang=="FR":
   WikiFile="BDD/WIKI_prop.txt"
   wdf.setLanguage("fr")
   wdf.setWebSite("frwiki")
else:
   WikiFile="BDD/WIKI_propEN.txt"
   wdf.setLanguage("en")
   wdf.setWebSite("enwiki")

sleep(0.1)
mouth.setVoice(voiceType)
mouth.setLanguage(lang)

chatBot.startSession("ProgramAB", "default", myAimlFolder)
chatBot.addTextListener(htmlFilter)
htmlFilter.addListener("publishText", python.name, "talk") 

chatBot.startSession("ProgramAB", "default", myAimlFolder)
#ear.addTextListener(chatBot)
chatBot.addTextListener(htmlFilter)
htmlFilter.addListener("publishText", python.name, "talk") 

# ##############################################################################
# We include all InmoovAI mods
# ##############################################################################
# -- coding: utf-8 --
execfile(u'INMOOV-AI_activator.py')
execfile('INMOOV-AI_divers.py')
execfile('INMOOV-AI_memory.py')
execfile('INMOOV-AI_gestures.py')
if IhaveEyelids>0:
	execfile('INMOOV-AI_paupieres_eyeleads.py')
execfile(u'INMOOV-AI_timers.py')
if IsInmoovArduino==1:
	execfile('INMOOV-AI_opencv.py')
execfile('INMOOV-AI_move_head_random.py')
execfile('INMOOV-AI_azure_translator.py')
execfile('INMOOV-AI_messenger.py')
execfile('INMOOV-AI_KnowledgeFetchers.py')
execfile('INMOOV-AI_games.py')
execfile('INMOOV-AI_reminders.py')
execfile('INMOOV-AI_domotique.py')
execfile(u'INMOOV-AI_dictionaries.py')
execfile(u'INMOOV-AI_WeatherMap_Meteo.py')
execfile(u'INMOOV-AI_jeanneton.py')
#execfile(u'INMOOV-AI_demo_halleffect.py')

# ##############################################################################
# We start a function that do actions after voice start / stop
# On creer un connecteur qui va nous permettre de savoir quand le robot commence a parler. Et quand il a fini
# ##############################################################################
python.subscribe(mouth.getName(),"publishStartSpeaking")
python.subscribe(mouth.getName(),"publishEndSpeaking")

# ##############################################################################
# Timer function to autostart webkit microphone every 10seconds
# ##############################################################################
WebkitSpeachReconitionFix = Runtime.start("WebkitSpeachReconitionFix","Clock")
WebkitSpeachReconitionFix.setInterval(15000)
WebkitSpeachReconitionFix.addListener("pulse", python.name, "WebkitSpeachReconitionON")

# ##############################################################################
# LED RGB EXTERNE
# ##############################################################################
Light(1,1,0)

#on remet à zero certaines variables de l'aiml ( sujets de discussion... )
ClearMemory()
if myBotname!="":
	UpdateBotName(myBotname)

# on met le robot dans sa position repos ( les servos ne sont plus solicités )
rest()

if IsInmoovArduino==1:
	i01.head.attach()
	HeadSide.attach()
	
if IsInmoovArduino==1 and tracking==1:
	trackHumans()

Light(1,1,1)

#r=image.displayFullScreen("http://vignette2.wikia.nocookie.net/worldsofsdn/images/7/7a/Tyrell-corp.jpg",1)

if str(chatBot.getPredicate("default","botname"))!="unknown" and str(chatBot.getPredicate("default","botname"))!="default" and str(chatBot.getPredicate("default","botname"))!="":
	UpdateBotName(str(chatBot.getPredicate("default","botname")))

#if str(chatBot.getPredicate("default","bot_id"))!="unknown":
	#chatBot.getResponse("MESSAGESCHECK")

#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\logo.jpg',1)
#r=image.displayFullScreen(os.getcwd().replace("develop", "")+'pictures\logo.jpg',1)

# ##############################################################################
# Je suppose pour voir la progression avec RGB externe
# ##############################################################################
Light(1,1,1)

# ##############################################################################
# Vérification divers
# ##############################################################################
CheckVersion()
anniversaire("0")
chatBot.getResponse("WAKE_UP")
GetUnreadMessageNumbers("0")
sleep(4)

# ##############################################################################
# Browser
# ##############################################################################
webgui.startBrowser("http://localhost:8888/#/service/i01.ear")
#petit fix pour dire au robot qu'il eut commencer à écouter

if lang=="FR":
   ear.setLanguage("fr-FR")
python.subscribe(ear.getName(),"publishText")

WebkitSpeachReconitionFix.startClock()

# ##############################################################################
# System is ready
# ##############################################################################
RobotIsStarted=1
pcIsReady()
sleep(0.5)
startWatchdogTimer()
# A METTRE SI ON VEUT PAS DECLENCHER LE WATCHDOG
#disableWatchdog() 
sleep(0.5)
updateBatterieRequest(1)
sleep(1)
updateBatterieRequest(2)
sleep(1)

# ##############################################################################
# Mettre ici les différents tests
# Anthony, peux tu déplacer les tests qui n'ont rien à voir avec le startup
# après ceci...
# ##############################################################################

#StartSensorDemo()

#NeoPixelColor("rouge")
#sleep(5)
#NeoPixelCustomColor(10,10,10)
#sleep(5)
NeoPixelAnimation(1)
sleep(5)
NeoPixelAnimation(0)

