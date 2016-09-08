
def rest():
	if IsInmoovLeft==1:
		i01.setHandSpeed("left", 1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
		i01.setArmSpeed("left", 1.0, 1.0, 1.0, 1.0)
		i01.setHeadSpeed(0.8, 0.8)
		i01.setTorsoSpeed(1.0, 1.0, 1.0)
		i01.moveHead(80,86,82,78,76)
		i01.moveArm("left",5,90,0,10)
		i01.moveHand("left",2,2,2,2,2,90)
		i01.moveTorso(80,90,80)
		
	if IsInmoovRight==1:
		i01.setHandSpeed("right", 1.0, 1.0, 1.0, 1.0, 1.0, 1.0)
		i01.setArmSpeed("right", 1.0, 1.0, 1.0, 1.0)
		i01.moveArm("right",5,90,0,12)
		i01.moveHand("right",2,2,2,2,2,90)
	
	if IsInmoovLeft==1 or IsInmoovRight==1:
		i01.detach()

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
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(00,90)
	sleep(5)
	i01.setHeadSpeed(0.1, 0.1)
	i01.moveHead(90)
	
	
def LookAtYourFeet():
	global MoveHeadRandom
	MoveHeadRandom=0
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(180,90)
	sleep(5)
	i01.setHeadSpeed(0.1, 0.1)
	i01.moveHead(90)
	
	
def LookAtYourLeft():
	global MoveHeadRandom
	MoveHeadRandom=0
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(120,20)
	sleep(5)
	i01.setHeadSpeed(0.1, 0.1)
	i01.moveHead(90,90)
	
def LookAtYourRight():
	global MoveHeadRandom
	MoveHeadRandom=0
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(120,160)
	sleep(5)
	i01.setHeadSpeed(0.1, 0.1)
	i01.moveHead(90,90)

	
	
def LookAroundYou():
	global MoveHeadRandom
	MoveHeadRandom=0
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(160,160)
	sleep(1)
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(160,20)
	sleep(1)
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(20,20)
	sleep(1)
	i01.setHeadSpeed(0.2, 0.2)
	i01.moveHead(20,160)
	sleep(1)
	i01.setHeadSpeed(0.1, 0.1)
	i01.moveHead(90,90)