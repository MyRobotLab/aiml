MoveHeadTimer = Runtime.start("MoveHeadTimer","Clock")
MoveHeadTimer.setInterval(1000)


def MoveHead(timedata):

	if IsInmoovArduino==1:
		
		head.neck.setSpeed(NeckSpeed)
		head.rothead.setSpeed(RotHeadSpeed)
		HeadSide.setSpeed(PistonSideSpeed)
		i01.moveHead(random.randint(50,130),random.randint(50,130))
		HeadSide.moveTo(random.randint(50,130))
		MoveHeadTimer.setInterval(1000)
		
		
	
MoveHeadTimer.addListener("pulse", python.name, "MoveHead")
