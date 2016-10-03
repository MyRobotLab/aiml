
def rest():
	if IsInmoovArduino==1:
		head.neck.setSpeed(NeckSpeed)
		head.rothead.setSpeed(RotHeadSpeed)
		HeadSide.setSpeed(PistonSideSpeed)
		i01.setHandSpeed("left", 1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
		i01.setArmSpeed("left", 1.0, 1.0, 1.0, 1.0)
		i01.setHandSpeed("right", 1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
		i01.setArmSpeed("right", 1.0, 1.0, 1.0, 1.0)
		head.neck.setSpeed(NeckSpeed)
		head.rothead.setSpeed(RotHeadSpeed)
		i01.setTorsoSpeed(1.0, 1.0, 1.0)
		head.rest()
		leftHand.rest()
		rightHand.rest()
		leftArm.rest()
		rightArm.rest()
		torso.rest()
		HeadSide.rest()
		sleep(2)
		i01.detach()
		HeadSide.detach()
		
		
def No(data):
	global MoveHeadRandom
	MoveHeadRandom=0
	# WE MOVE THE ROTHEAD OR PISTONMOD
	if IsInmoovArduino==1:
		head.neck.setSpeed(NeckSpeed+0.37)
		head.rothead.setSpeed(RotHeadSpeed+0.37)
		if random.randint(0,1)==1:
			#i01.attach()
			i01.moveHead(NeckRest-10,RotHeadRest+30)
			sleep(0.7)
			i01.moveHead(NeckRest-5,RotHeadRest-30)
			sleep(1.4)
			i01.moveHead(NeckRest,RotHeadRest+30)
			sleep(1.5)
			i01.moveHead(NeckRest+5,RotHeadRest-30)
			sleep(1.5)
			i01.moveHead(NeckRest+2,RotHeadRest+30)
			sleep(0.7)
			i01.moveHead(NeckRest,RotHeadRest)
			i01.head.jaw.rest()
		else:
			HeadSide.setSpeed(PistonSideSpeed+0.37)
			HeadSide.moveTo(50)
			sleep(0.5)
			HeadSide.moveTo(120)
			sleep(1)
			HeadSide.moveTo(HeadSideRest)
			i01.head.jaw.rest()
		head.neck.setSpeed(NeckSpeed)
		head.rothead.setSpeed(RotHeadSpeed)
		HeadSide.setSpeed(PistonSideSpeed)
		

def Yes(data):
	global MoveHeadRandom
	MoveHeadRandom=0
	if IsInmoovArduino==1:
		#i01.attach()
		head.neck.setSpeed(NeckSpeed+0.37)
		head.rothead.setSpeed(RotHeadSpeed+0.37)
		i01.moveHead(NeckRest+50,RotHeadRest)
		sleep(0.7)
		i01.moveHead(NeckRest-40,RotHeadRest+5)
		sleep(1.5)
		i01.moveHead(NeckRest+40,RotHeadRest)
		sleep(1.6)
	#Light(0,1,1)
		i01.moveHead(NeckRest-30,RotHeadRest-2)
		sleep(1.3)
		i01.moveHead(NeckRest+30,RotHeadRest)
		sleep(1.2)
		i01.moveHead(NeckRest-20,RotHeadRest+5)
		sleep(1.2)
		i01.moveHead(NeckRest+60,RotHeadRest)
		sleep(1.2)
	#Light(1,1,1)
		i01.moveHead(NeckRest,RotHeadRest)
		i01.head.jaw.rest()
		head.neck.setSpeed(NeckSpeed)
		head.rothead.setSpeed(RotHeadSpeed)
		
		
	

def MoveHand(side,thumb,index,majeure,ringFinger,pinky):
	print side
	if side=="left":
		if thumb != -1:
			i01.leftHand.thumb.attach()
			i01.leftHand.thumb.moveTo(thumb)
			
		if index != -1:
			i01.leftHand.index.attach()
			i01.leftHand.index.moveTo(index)
			
		if majeure != -1:
			i01.leftHand.majeure.attach()
			i01.leftHand.majeure.moveTo(majeure)
			
		if ringFinger != -1:
			i01.leftHand.ringFinger.attach()
			i01.leftHand.ringFinger.moveTo(ringFinger)
			
		if pinky != -1:
			i01.leftHand.pinky.attach()
			i01.leftHand.pinky.moveTo(pinky)
		
		sleep(1)
		i01.leftHand.detach()
			
	if side=="right":
		if thumb != -1:
			i01.rightHand.thumb.attach()
			i01.rightHand.thumb.moveTo(thumb)
			
		if index != -1:
			i01.rightHand.index.attach()
			i01.rightHand.index.moveTo(index)
			
		if majeure != -1:
			i01.rightHand.majeure.attach()
			i01.rightHand.majeure.moveTo(majeure)
			
		if ringFinger != -1:
			i01.rightHand.ringFinger.attach()
			i01.rightHand.ringFinger.moveTo(ringFinger)
			
		if pinky != -1:
			i01.rightHand.pinky.attach()
			i01.rightHand.pinky.moveTo(pinky)
			
		sleep(1)
		i01.rightHand.detach()

def LookAtTheSky():
	global MoveHeadRandom
	MoveHeadRandom=0
	head.neck.setSpeed(NeckSpeed)
	head.rothead.setSpeed(RotHeadSpeed)
	i01.moveHead(0,RotHeadRest)
	sleep(5)
	i01.moveHead(NeckRest,RotHeadRest)
	
	
def LookAtYourFeet():
	global MoveHeadRandom
	MoveHeadRandom=0
	head.neck.setSpeed(NeckSpeed)
	head.rothead.setSpeed(RotHeadSpeed)
	i01.moveHead(180,RotHeadRest)
	sleep(5)
	i01.moveHead(NeckRest,RotHeadRest)
	
	
def LookAtYourLeft():
	global MoveHeadRandom
	MoveHeadRandom=0
	head.neck.setSpeed(NeckSpeed)
	head.rothead.setSpeed(RotHeadSpeed)
	i01.moveHead(NeckRest,0)
	sleep(5)
	i01.moveHead(NeckRest,RotHeadRest)
	
def LookAtYourRight():
	global MoveHeadRandom
	MoveHeadRandom=0
	head.neck.setSpeed(NeckSpeed)
	head.rothead.setSpeed(RotHeadSpeed)
	i01.moveHead(NeckRest,180)
	sleep(5)
	i01.moveHead(NeckRest,RotHeadRest)

	
	
def LookAroundYou():
	global MoveHeadRandom
	MoveHeadRandom=0
	head.neck.setSpeed(NeckSpeed)
	head.rothead.setSpeed(RotHeadSpeed)
	i01.moveHead(160,160)
	sleep(1)
	i01.moveHead(160,20)
	sleep(1)
	i01.moveHead(20,20)
	sleep(1)
	i01.moveHead(20,160)
	sleep(1)
	i01.moveHead(NeckRest,RotHeadRest)