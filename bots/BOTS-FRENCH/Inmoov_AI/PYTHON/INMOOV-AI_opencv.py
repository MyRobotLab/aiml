global FaceDetectedCounter
FaceDetectedCounter=0
global FaceDetected
FaceDetected=1
global startTimerFunction

python.subscribe(opencv.getName(),"publishOpenCVData")

NoFaceDetectedTimer = Runtime.start("NoFaceDetectedTimer","Clock")
NoFaceDetectedTimer.setInterval(20000)

def onOpenCVData(data):
	#print FaceDetectedCounter
	global FaceDetectedCounter
	global FaceDetected
	if data.getBoundingBoxArray() != None:
		if not data.getBoundingBoxArray():
			FaceDetectedCounter=0
		else:
			FaceDetectedCounter+=1
			if FaceDetectedCounter>50 and FaceDetected==0:
				NoFaceDetectedTimer.stopClock()
				FaceDetected=1
				chatBot.getResponse("SYSTEM FACEDETECTED")
				
				
		
		
	
		
def NoFaceDetectedTimerFunction(timedata):
	global startTimerFunction
	startTimerFunction+=1
	if startTimerFunction==2:
		FaceDetected=1
		chatBot.getResponse("SYSTEM FACENOTDETECTED")
		
		
				
NoFaceDetectedTimer.addListener("pulse", python.name, "NoFaceDetectedTimerFunction")
# start the clock

def trackHumans():
	#i01.headTracking.findFace()
	#i01.opencv.SetDisplayFilter
	i01.headTracking.faceDetect()
	i01.eyesTracking.faceDetect()
	print "test"

def TakePhoto(messagePhoto):
	i01.startEyesTracking(leftPort,22,24)
	i01.eyesTracking.faceDetect()	
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
	#print "name file is" , os.getcwd()+'\\'+str(photoFileName)
	Light(1,1,1)
	NeoPixelF(1)
	DisplayPic(os.getcwd()+'\\'+str(photoFileName))
	opencv.removeFilters()
	opencv.stopCapture()
	#i01.startEyesTracking(leftPort,22,24)
	#i01.startHeadTracking(leftPort)
	