from java.lang import String

######################################################################
# Lloyd is an AIML based MRL powered implementation for the InMoov
# Robot.
# Lloyd defines all custom python in this script and custom 
# Aiml files to drive the python methods and speech. 
######################################################################
 
aimlDir = "c:/dev/workspace.kmw/aiml/bots"
userName = "kevin"
botName = "lloyd"
 

######################################################################
# A helper function to print the recognized text to the python window.
# semi-useful for debugging.
######################################################################
def heard(data):
  print "Speech Recognition:" + str(data)
 
######################################################################
# Create ProgramAB chat bot
######################################################################
lloyd = Runtime.createAndStart("lloyd", "ProgramAB")
# start the session for the chat bot
lloyd.startSession(aimlDir, userName, botName)

######################################################################
# create the speech recognition service
# Speech recognition is based on WebSpeechToolkit API
# So this just means we need the web gui, it's part of the programAB 
# service now.
######################################################################
# Start the REST API for MRL
webgui = Runtime.createAndStart("webgui","WebGUI")

######################################################################
# create the html filter to filter the output of program ab
######################################################################
htmlfilter = Runtime.createAndStart("htmlfilter", "HtmlFilter")
 
######################################################################
# create the speech to text service (named the same as the inmoov's)
# TODO: consider a different voice?
######################################################################
mouth = Runtime.createAndStart("i01.mouth", "MarySpeech")
# mouth.setGoogleURI("http://thehackettfamily.org/Voice_api/api2.php?voice=Ryan&txt=")
 
######################################################################
# MRL Routing   webgui (speech recognition) -> program ab -> htmlfilter -> inmoov
######################################################################
# Add route from Program AB to html filter
lloyd.addTextListener(htmlfilter)
# Add route from html filter to mouth
htmlfilter.addTextListener(mouth)
 
# make sure the ear knows if it's speaking.
# TODO: how does this jive with webspeech ?!
# sphinx.attach(mouth)


# Now we want to start and calibrate the InMoov .. 


# once the inmoov is started we will define the custom python fucntion
# so they can be called easily from AIML OOB tags


