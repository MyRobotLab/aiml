global FaceDetectedCounter #FaceDetectedCounter is to prevent artefact face reconition
FaceDetectedCounter=0
global FaceDetected #open cv detected a face
FaceDetected=1
global startTimerFunction
global posxSquare #position of the left box
global WidthSquare #size of the face ( small is far )
WidthSquare=0
global FaceHadMoved
FaceHadMoved=0
from decimal import Decimal
posxSquare=Decimal(0)
python.subscribe(opencv.getName(),"publishOpenCVData")


NoFaceDetectedTimer = Runtime.start("NoFaceDetectedTimer","Clock")
NoFaceDetectedTimer.setInterval(20000)

def onOpenCVData(data):
#This is opencv functions that do jobs

	global posxSquare
	global openCvModule
	global WidthSquare
	global FaceHadMoved
	global FaceDetectedCounter
# openCvModule=="photo" : just detect one face
	if openCvModule=="photo":

		
		
		if data.getBoundingBoxArray() != None:
			if not data.getBoundingBoxArray():
				FaceDetectedCounter=0
			else:
				FaceDetectedCounter+=1
				if FaceDetectedCounter>50 and FaceDetected==0:
					NoFaceDetectedTimer.stopClock()
					FaceDetected=1
					chatBot.getResponse("SYSTEM FACEDETECTED")

# openCvModule=="123" : just detect if detected face is mooving in the space. 1.2.3 soleil :)
	if openCvModule=="123":
		
		if data.getBoundingBoxArray() != None:
			if data.getBoundingBoxArray():
				FaceDetectedCounter+=1
				rect = data.getBoundingBoxArray().get(0)
				print FaceDetectedCounter,posxSquare,posxSquare-Decimal(rect.x)
				if FaceDetectedCounter>10:
					if posxSquare != 0 and posxSquare-Decimal(rect.x) !=0 and (posxSquare-Decimal(rect.x) <= 0.2) and (posxSquare-Decimal(rect.x) >= -0.2):
						print "tu as bougez ",posxSquare-Decimal(rect.x)
								 # Store the information in rect
					posxSquare = Decimal(rect.x)                                        # Get the x position of the corner
					posy = rect.y                                        # Get the y position of the corner
					WidthSquare = rect.width                                       # Get the width
					h = rect.height  				# Get the height
				
			else:
				FaceDetectedCounter=0
				
				
				
			
	
		
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
	global openCvModule
	openCvModule = "photo"
	global FaceDetected
	global FaceDetectedCounter
	global startTimerFunction
	FaceDetectedCounter=0
	FaceDetected=0
	Light(0,0,0)
	startTimerFunction=0
	NoFaceDetectedTimer.startClock()


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
	