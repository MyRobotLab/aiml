PaupiereServoGauche = Runtime.start("PaupiereServoGauche","Servo")
PaupiereServoGauche.setMinMax(PaupiereGaucheMIN , PaupiereGaucheMAX)
PaupiereServoDroite = Runtime.start("PaupiereServoDroite","Servo")
PaupiereServoDroite.setMinMax(PaupiereDroiteMIN , PaupiereDroiteMAX)

if PaupiereArduino=="left":
	PaupiereServoGauche.attach(left, PaupiereGaucheServoPin)
else:
	PaupiereServoGauche.attach(right, PaupiereGaucheServoPin)

if PaupiereArduino=="left" and IhaveEyelids==2:
	PaupiereServoDroite.attach(left, PaupiereDroiteServoPin)
else:
	PaupiereServoDroite.attach(right, PaupiereDroiteServoPin)



clock = Runtime.start("clock","Clock")
clock.setInterval(1000)
# define a ticktock method
def clignement(timedata):
	PaupiereServoGauche.moveTo(PaupiereGaucheMIN)
	PaupiereServoDroite.moveTo(PaupiereDroiteMIN)
	sleep(0.12)
	PaupiereServoGauche.moveTo(PaupiereGaucheMAX)
	PaupiereServoDroite.moveTo(PaupiereDroiteMAX)
	
#on fait un double clignement ou pas
	if random.randint(0,1)==1:
		sleep(0.2)
		PaupiereServoGauche.moveTo(PaupiereGaucheMIN)
		PaupiereServoDroite.moveTo(PaupiereDroiteMIN)
		sleep(0.12)
		PaupiereServoGauche.moveTo(PaupiereGaucheMAX)
		PaupiereServoDroite.moveTo(PaupiereDroiteMAX)
#on redefini une valeur aleatoire pour le prochain clignement
	clock.setInterval(random.randint(10000,30000))
#create a message routes
clock.addListener("pulse", python.name, "clignement")
# start the clock
clock.startClock()