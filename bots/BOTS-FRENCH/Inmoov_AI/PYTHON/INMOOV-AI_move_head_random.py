MoveHeadTimer = Runtime.start("MoveHeadTimer","Clock")
MoveHeadTimer.setInterval(10)


def MoveHead(timedata):
	if IsInmoovArduino==1:
		i01.setHeadSpeed(RotHeadSpeed+0.3, NeckSpeed+0.3)
		HeadSide.setSpeed(PistonSideSpeed+0.3)
		i01.moveHead(random.randint(50,130),random.randint(50,130))
		HeadSide.moveTo(random.randint(50,130))
		MoveHeadTimer.setInterval(1000)
		
	
MoveHeadTimer.addListener("pulse", python.name, "MoveHead")
