PaupiereServoGauche = Runtime.start("PaupiereServoGauche","Servo")
PaupiereServoGauche.setMinMax(PaupiereGaucheMIN , PaupiereGaucheMAX)
PaupiereServoDroite = Runtime.start("PaupiereServoDroite","Servo")
PaupiereServoDroite.setMinMax(PaupiereDroiteMIN , PaupiereDroiteMAX)

if PaupiereArduino=="left":
  if IhaveEyelids==1 or IhaveEyelids==2:
    PaupiereServoGauche.attach(left, PaupiereGaucheServoPin)
else:
  if IhaveEyelids==1 or IhaveEyelids==2:
    PaupiereServoGauche.attach(right, PaupiereGaucheServoPin)

if PaupiereArduino=="left":
  if IhaveEyelids==2:
    PaupiereServoDroite.attach(left, PaupiereDroiteServoPin)
else:
  if IhaveEyelids==2:
    PaupiereServoDroite.attach(right, PaupiereDroiteServoPin)



clock = Runtime.start("clock","Clock")
clock.setInterval(1000)
# define a ticktock method
def clignement(timedata):
	PaupiereServoGauche.moveTo(PaupiereGaucheMIN)
	PaupiereServoDroite.moveTo(PaupiereDroiteMAX)
	sleep(0.12)
	PaupiereServoGauche.moveTo(PaupiereGaucheMAX)
	PaupiereServoDroite.moveTo(PaupiereDroiteMIN)
	
#on fait un double clignement ou pas
	if random.randint(0,1)==1:
		sleep(0.2)
		PaupiereServoGauche.moveTo(PaupiereGaucheMIN)
		PaupiereServoDroite.moveTo(PaupiereDroiteMAX)
		sleep(0.12)
		PaupiereServoGauche.moveTo(PaupiereGaucheMAX)
		PaupiereServoDroite.moveTo(PaupiereDroiteMIN)
#on redefini une valeur aleatoire pour le prochain clignement
	clock.setInterval(random.randint(10000,30000))
#create a message routes
clock.addListener("pulse", python.name, "clignement")
# start the clock
clock.startClock()