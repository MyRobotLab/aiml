# ##############################################################################
# TEST SCRIPT BASE
#  MOUTHCONTROL + MARYTTS + SOUND
# ##############################################################################
try:
	shutil.rmtree(oridir+"ProgramAB/bots/test/aimlif")
except: 
	pass
		



	
i01 = Runtime.create("i01", "InMoov")
left = Runtime.create("i01.left", "Arduino")
jaw = Runtime.createAndStart("i01.head.jaw","Servo")
eyex = Runtime.createAndStart("i01.head.eyeX","Servo")
i01 = Runtime.start("i01","InMoov")

AudioFile = Runtime.createAndStart("AudioFile", "AudioFile")
i01.mouth = Runtime.createAndStart("i01.mouth", "MarySpeech")

i01.startMouth()


i01 = Runtime.start("i01","InMoov")
i01.startMouth()


mouth=i01.mouth
mouth.setLanguage("fr")
mouth.setVoice("upmc-pierre-hsmm")



#IT IS A PERSONNAL TWEAK TO REDIRECT MOUTHCONTROL TO ANOTHER PIN
#JUST DONT PAY ATTENTION TO IT
jawPIN=26
eyexPIN=22
i01.startLeftHand("COM3")
i01.detach()
eyex.setMinMax(90,120) 
eyex.attach("i01.left",eyexPIN,90,0)
jaw.attach("i01.left",jawPIN,90,0)
jaw.rest()
i01.startMouthControl("COM3")
i01.mouthControl.setmouth(90,120)
#END OF MOUTHCONTROL TWEAK




#we send fake sound to MaryTTS and we playin sound in same time
mouth.setAudioEffects("Volume(amount=0)")
mouth.speak("Hello sir, I am C3po unicyborg relations")
AudioFile.playFileBlocking("sound\\Hello sir, I am C3po unicyborg relations.mp3")
mouth.setAudioEffects("Volume(amount=1.70)")




