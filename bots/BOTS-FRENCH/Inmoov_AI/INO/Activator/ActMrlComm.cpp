#include "ActMrlComm.h"


/** 
 * Constructeur
 */
ActMrlComm::ActMrlComm() 
{
	softReset();
	byteCount = 0;

	mrlCmd[0] = new MrlCmd(MRL_IO_SERIAL_0);
  
  // Efface le buffer de réception
	for (unsigned int i = 1; i < (sizeof(mrlCmd) / sizeof(MrlCmd*)); i++) 
  {
		mrlCmd[i] = NULL;
	}
}

/** 
 * Destructeur
 */
ActMrlComm::~ActMrlComm() 
{
	for (unsigned int i = 0; i < (sizeof(mrlCmd) / sizeof(MrlCmd*)); i++) 
  {
		if (mrlCmd[i] != NULL) 
    {
			delete mrlCmd[i];
		}
	}
}

/***********************************************************************
 * UTILITY METHODS BEGIN
 */
void ActMrlComm::softReset() 
{
	while (deviceList.size() > 0) 
  {
		delete deviceList.pop();
	}
  
  /*while (pinList.size() > 0) 
  {
    delete pinList.pop();
  }*/
  
	//resetting var to default
	loopCount = 0;
	publishBoardStatusModulus = 10000;
	enableBoardStatus = false;
	Device::nextDeviceId = 1; // device 0 is Arduino
	debug = false;
  
	for (unsigned int i = 1; i < (sizeof(mrlCmd) / sizeof(MrlCmd*)); i++) 
  {
		if (mrlCmd[i] != NULL) 
    {
			mrlCmd[i]->end();
			delete mrlCmd[i];
			mrlCmd[i] = NULL;
		}
	}
  
	heartbeat = false;
	heartbeatEnabled = false;
	lastHeartbeatUpdate = 0;
  
	for (unsigned int i = 0; i < MAX_MSG_SIZE; i++) 
  {
	  customMsg[i] = 0;
	}
	customMsgSize = 0;

  rVal = 0;
  gVal = 0; 
  bVal = 0;
  openJawRequest = false;
  servoDetachIsRequest = false;
  volAudio = 31;
  updateAudio = false;
  watchdogCpt = 0;
  sendValueRequest = 0;
  servoPowerIsOn = false;
  servoControlRequest = false;
}

/***********************************************************************
 * PUBLISH_BOARD_STATUS
 * This function updates the average time it took to run the main loop
 * and reports it back with a publishBoardStatus MRLComm message
 *
 * TODO: avgTiming could be 0 if loadTimingModule = 0 ?!
 *
 * MAGIC_NUMBER|7|[loadTime long0,1,2,3]|[freeMemory int0,1]
 */
void ActMrlComm::publishBoardStatus() 
{
	// protect against a divide by zero in the division.
	if (publishBoardStatusModulus == 0) 
  {
		publishBoardStatusModulus = 10000;
	}

	unsigned int avgTiming = 0;
	unsigned long now = micros();

	avgTiming = (now - lastMicros) / publishBoardStatusModulus;

	// report board status
	if (enableBoardStatus && (loopCount % publishBoardStatusModulus == 0)) 
  {
    // send the average loop timing.
		MrlMsg msg(PUBLISH_BOARD_STATUS);
		msg.addData16(avgTiming);
		msg.addData16(getFreeRam());
		msg.addData16(deviceList.size());
		msg.sendMsg();
	}
	// update the timestamp of this update.
	lastMicros = now;
}

int ActMrlComm::getFreeRam() 
{
	// KW: In the future the arduino might have more than an 32/64k of ram. an int might not be enough here to return.
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/***********************************************************************
 * PUBLISH DEVICES BEGIN
 *
 * All serial IO should happen here to publish a MRLComm message.
 * TODO: move all serial IO into a controlled place this this below...
 * TODO: create MRLCommMessage class that can just send itself!
 *
 */
/**
 * Publish the MRLComm message
 * MAGIC_NUMBER|2|MRLCOMM_VERSION
 */
void ActMrlComm::publishVersion() 
{
	MrlMsg msg(PUBLISH_VERSION);
	msg.addData(MRLCOMM_VERSION);
	msg.sendMsg();
}

/**
 * publishBoardInfo()
 * MAGIC_NUMBER|2|PUBLISH_BOARD_INFO|BOARD
 * return the board type (mega/uno) that can use in javaland for the pin layout
 */
void ActMrlComm::publishBoardInfo() 
{
	MrlMsg msg(PUBLISH_BOARD_INFO);
	msg.addData(BOARD);
	msg.sendMsg();
}

/**
 * Publish the acknowledgement of the command received and processed.
 * MAGIC_NUMBER|2|PUBLISH_MESSAGE_ACK|FUNCTION
 */
void ActMrlComm::publishCommandAck(int function) 
{
	MrlMsg msg(PUBLISH_MESSAGE_ACK);
	// the function that we're ack-ing
	msg.addData(function);
	msg.sendMsg();
}

/**
 * PUBLISH_ATTACHED_DEVICE
 * MSG STRUCTURE
 * PUBLISH_ATTACHED_DEVICE | NEW_DEVICE_INDEX | NAME_STR_SIZE | NAME
 *
 */
void ActMrlComm::publishAttachedDevice(int id, int nameSize, unsigned char* name) 
{
	MrlMsg msg(PUBLISH_ATTACHED_DEVICE, id);
	msg.addData(name, nameSize, true);
	msg.sendMsg();
}

/***********************************************************************
 * SERIAL METHODS BEGIN
 */
void ActMrlComm::readCommand() 
{
	for (unsigned int i = 0; i < (sizeof(mrlCmd) / sizeof(MrlCmd*)); i++) 
  {
		if (mrlCmd[i] != NULL) 
    {
			if (mrlCmd[i]->readCommand()) 
      {
				processCommand(i + 1);
			}
		}
	}
}

// This function will switch the current command and call
// the associated function with the command
/**
 * processCommand() - once the main loop has read an mrlcomm message from the 
 * serial port, this method will be called.
 */
void ActMrlComm::processCommand(int ioType) 
{
	unsigned char* ioCmd = mrlCmd[ioType - 1]->getIoCmd();
  
	if (ioType != MRL_IO_SERIAL_0) 
  {
		MrlMsg msg = MrlMsg(MSG_ROUTE);
		msg.addData(ioType);
		msg.addData(ioCmd, mrlCmd[ioType - 1]->getMsgSize());
		msg.sendMsg();
		// MrlMsg::publishDebug("not from Serial, ioType" + String(ioType));
		return;
	}
  
	// MrlMsg::publishDebug("not from Serial:" + String(ioCmd[0]));
	switch (ioCmd[0]) 
  {
    // === system pass through begin ===
    case DIGITAL_WRITE:
    {
      switch (ioCmd[1])
      {
        case 2:
        {
          if (ioCmd[2] == 1)
          {
            // Le PC est prêt
            inmoovIsOn = true;
            shutdownPC = false;
            servoIsEnable = true;
            servoDetachIsRequest = true;
            watchdogCpt = 0;

            // Active la sortie audio
            setMuteOff();
          }
          break;
        }
        case 3:
        {
          if (ioCmd[2] == 1)
          {
            // Le PC va s'arrété
            shutdownPC = true;
            watchDogIsEnable = false;
            watchdogCpt = 0;
          }
          break;
        }
        case 4:
        {
          if (ioCmd[2] == 1)
          {
            // Demande d'ouverture de la bouche
            servoIsEnable = false;
            openJawRequest = true;
          }
          else
          {
            // Demande de fermeture de la bouche
            openJawRequest = false;
          }
          break;
        }
        case 5:
        {
          if (ioCmd[2] == 1)
          {
            // Bloque la gestion du servo en fonction de l'audio
            servoIsEnable = false;
          }
          else
          {
            // Débloque la gestion du servo en fonction de l'audio
            servoIsEnable = true;
          }
          break;
        }
        case 6:
        {
          if (ioCmd[2] == 1)
          {
            // Jaune
            rVal = 100;
            gVal = 100;
            bVal = 0;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 7:
        {
          if (ioCmd[2] == 1)
          {
            // Rouge
            rVal = 100;
            gVal = 0;
            bVal = 0;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 8:
        {
          if (ioCmd[2] == 1)
          {
            // Vert
            rVal = 0;
            gVal = 100;
            bVal = 0;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 9:
        {
          if (ioCmd[2] == 1)
          {
            // Bleu
            rVal = 0;
            gVal = 0;
            bVal = 100;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 10:
        {
          if (ioCmd[2] == 1)
          {
            // Cyan
            rVal = 0;
            gVal = 100;
            bVal = 100;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 11:
        {
          if (ioCmd[2] == 1)
          {
            // Rose
            rVal = 100;
            gVal = 0;
            bVal = 100;
          }
          else
          {
            // Noir
            rVal = 0;
            gVal = 0;
            bVal = 0;
          }
          break;
        }
        case 12:
        {
          if (ioCmd[2] == 1)
          {
            animRequest = 1;
          }
          else
          {
            animRequest = 0;
          }
          break;
        }
        case 13:
        {
          if (ioCmd[2] == 1)
          {
            animRequest = 2;
          }
          else
          {
            animRequest = 0;
          }
          break;
        }
        case 14:
        {
          if (ioCmd[2] == 1)
          {
            servoDetachIsRequest = true;
          }
          break;
        }
        case 15:
        {
          if (ioCmd[2] == 1)
          {
            disableAudio();
          }
          else
          {
            enableAudio();
          }
          break;
        }
        case 16:
        {
          if (ioCmd[2] == 1)
          {
            setMuteOn();
          }
          else
          {
            setMuteOff();
          }
          break;
        }
        case 17:
        {
          if (ioCmd[2] == 1)
          {
            // Commande de rafraichissement du watchdog
            watchDogIsEnable = true;
          }
          else
          {
            // Désactivation du watchdog
            watchDogIsEnable = false;
          }
          watchdogCpt = 0;
          break;
        }
        case 18:
        {
          if (ioCmd[2] == 1)
          {
            // Demande l'activation de l'alimentation des servos
            servoPowerIsOn = true;
          }
          else
          {
            // Demande l'arrêt de l'alimentation des servos
            servoPowerIsOn = false;
          }
          servoControlRequest = true;
          break;
        }
        default:
        {
          // Erreur
          MrlMsg::publishError(ERROR_UNKOWN_CMD);
          break;
        }
      }
      break; 
    }
    case ANALOG_WRITE: 
    {
      switch (ioCmd[1])
      {
        case 0:
        {
          volAudio = ioCmd[2];
          updateAudio = true;
          break;
        }
        case 1:
        {
          servoMin = ioCmd[2];
          EEPROM.write(10, servoMin);
          break;
        }
        case 2:
        {
          servoMax = ioCmd[2];
          EEPROM.write(11, servoMax);
          break;
        }
        default:
        {
          MrlMsg::publishError(ERROR_UNKOWN_CMD);
          break;
        }
      }
      break;
    }
    case PIN_MODE: 
    case SERVO_ATTACH: 
    case SERVO_SWEEP_START:
    case SERVO_SWEEP_STOP:
    case SERVO_WRITE:
    case SERVO_WRITE_MICROSECONDS:
    case SERVO_DETACH: 
    {
      break;
    }
    case ENABLE_BOARD_STATUS:
    {
      enableBoardStatus = true;
      publishBoardStatusModulus = (unsigned int) MrlMsg::toInt(ioCmd, 1);
      if (debug)
        MrlMsg::publishDebug(
            "modulus is " + String(publishBoardStatusModulus));
      break;
    }
    // ENABLE_PIN_EVENTS | ADDRESS | PIN TYPE 0 = DIGITAL | 1 = ANALOG
    case ENABLE_PIN: 
    case DISABLE_PIN: 
    case DISABLE_PINS: 
    {
      break;
    }
    case DISABLE_BOARD_STATUS:
    {
      enableBoardStatus = false;
      break;
    }
    case SET_PWMFREQUENCY:
    case PULSE:
    case PULSE_STOP:
    {
      break;
    }
    case SET_TRIGGER:
    {
      // Couleur custom du ring
      rVal = ioCmd[1];
      gVal = ioCmd[2];
      bVal = ioCmd[3];
      break;
    }
    case SET_DEBOUNCE:
    case SET_DIGITAL_TRIGGER_ONLY:
    case SET_SERIAL_RATE:
    {
      break;
    }
    case GET_VERSION:
    {
      publishVersion();
      break;
    }
    case SET_SAMPLE_RATE:
    {
      break;
    }
    case SOFT_RESET:
    {
      softReset();
      break;
    }
    case SENSOR_POLLING_START:
    {
      break;
    }
    case DEVICE_ATTACH:
    {
      deviceAttach(ioCmd);
      break;
    }
    case DEVICE_DETACH:
    {
      deviceDetach(ioCmd[1]);
      break;
    }
    case SENSOR_POLLING_STOP:
    case I2C_READ:
    case I2C_WRITE:
    case I2C_WRITE_READ:
    {
      break;
    }
    case SET_DEBUG:
    {
      debug = ioCmd[1];
      if (debug) 
      {
        MrlMsg::publishDebug(F("Debug logging enabled."));
      }
      break;
    }
    case PUBLISH_BOARD_INFO:
    {
      publishBoardInfo();
      break;
    }
    case NEO_PIXEL_WRITE_MATRIX:
    case NEO_PIXEL_SET_ANIMATION:
    {
      break;
    }
    case CONTROLLER_ATTACH:
    {
      mrlCmd[ioCmd[1] - 1] = new MrlCmd(ioCmd[1]);
      break;
    }
    case MSG_ROUTE: 
    {
      MrlMsg msg(ioCmd[2]);
      msg.addData(ioCmd + 3, mrlCmd[ioType - 1]->getMsgSize() - 3);
      msg.begin(ioCmd[1], 115200);
      msg.sendMsg();
      break;
    }
    case SERVO_SET_MAX_VELOCITY: 
    case SERVO_SET_VELOCITY: 
    {
      break;
    }
    case HEARTBEAT: 
    {
      heartbeatEnabled = true;
      break;
    }
    case CUSTOM_MSG: 
    {
      if (ioCmd[1] <= 10)
      {
        sendValueRequest = ioCmd[1];
      }
      else
      {
        MrlMsg::publishError(ERROR_UNKOWN_CMD);
      }
      break;
    }
    default:
    {
      MrlMsg::publishError(ERROR_UNKOWN_CMD);
      break;
    }
	} // end switch
  
	// ack that we got a command (should we ack it first? or after we process the command?)
	heartbeat = true;
	lastHeartbeatUpdate = millis();
	publishCommandAck(ioCmd[0]);
	// reset command buffer to be ready to receive the next command.
	// KW: we should only need to set the byteCount back to zero. clearing this array is just for safety sake i guess?
	// GR: yup
	//memset(ioCmd, 0, sizeof(ioCmd));
	//byteCount = 0;
} // process Command

/**********************************************************************
 * ATTACH DEVICES BEGIN
 *
 *<pre>
 *
 * MSG STRUCTURE
 *                    |<-- ioCmd starts here                                        |<-- config starts here
 * MAGIC_NUMBER|LENGTH|ATTACH_DEVICE|DEVICE_TYPE|NAME_SIZE|NAME .... (N)|CONFIG_SIZE|DATA0|DATA1 ...|DATA(N)
 *
 * ATTACH_DEVICE - this method id
 * DEVICE_TYPE - the mrlcomm device type we are attaching
 * NAME_SIZE - the size of the name of the service of the device we are attaching
 * NAME .... (N) - the name data
 * CONFIG_SIZE - the size of the folloing config
 * DATA0|DATA1 ...|DATA(N) - config data
 *
 *</pre>
 *
 * Device types are defined in org.myrobotlab.service.interface.Device
 * TODO crud Device operations create remove (update not needed?) delete
 * TODO probably need getDeviceId to decode id from Arduino.java - because if its
 * implemented as a ptr it will be 4 bytes - if it is a generics id
 * it could be implemented with 1 byte
 */
void ActMrlComm::deviceAttach(unsigned char* ioCmd) 
{
	// TOOD:KW check free memory to see if we can attach a new device. o/w return an error!
	// we're creating a new device. auto increment it
	// TODO: consider what happens if we overflow on this auto-increment. (very unlikely. but possible)
	// we want to echo back the name
	// and send the config in a nice neat package to
	// the attach method which creates the device
	//unsigned char* ioCmd = mrlCmd->getIoCmd();
	int nameSize = ioCmd[2];

	// get config size
	int configSizePos = 3 + nameSize;
	int configSize = ioCmd[configSizePos];
	int configPos = configSizePos + 1;
	config = ioCmd + configPos;
	// MAKE NOTE: I've chosen to have config & configPos globals
	// this is primarily to avoid the re-allocation/de-allocation of the config buffer
	// but part of me thinks it should be a local var passed into the function to avoid
	// the dangers of global var ... fortunately Arduino is single threaded
	// It also makes sense to pass in config on the constructor of a new device
	// based on device type - "you inflate the correct device with the correct config"
	// but I went on the side of globals & hopefully avoiding more memory management and fragmentation
	// CAL: change config to a pointer in ioCmd (save some memory) so config[0] = ioCmd[configPos]

	int type = ioCmd[1];
	Device* devicePtr = 0;
	// KW: remove this switch statement by making "attach(int[]) a virtual method on the device base class.
	// perhaps a factory to produce the devicePtr based on the deviceType..
	// currently the attach logic is embeded in the constructors ..  maybe we can make that a more official
	// lifecycle for the devices..
	// check out the make_stooge method on https://sourcemaking.com/design_patterns/factory_method/cpp/1
	// This is really how we should do this.  (methinks)
	// Cal: the make_stooge method is certainly more C++ like, but essentially do the same thing as we do,
	// it just move this big switch to another place

	// GR: I agree ..  "attach" should be a universal concept of devices, yet it does not need to be implmented
	// in the constructor .. so I'm for making a virtualized attach, but just like Java-Land the attach
	// needs to have size sent in with the config since it can be variable array
	// e.g.  attach(int[] config, configSize)

	switch (type) 
  {
    case DEVICE_TYPE_ARDUINO: 
    {
      //devicePtr = attachAnalogPinArray();
      break;
    }
    /*
     case SENSOR_TYPE_DIGITAL_PIN_ARRAY: {
     //devicePtr = attachDigitalPinArray();
     break;
     }
     case SENSOR_TYPE_PULSE: {
     //devicePtr = attachPulse();
     break;
     }
     */
    case DEVICE_TYPE_ULTRASONIC: 
    {
      //devicePtr = attachUltrasonic();
      break;
    }
    case DEVICE_TYPE_STEPPER: 
    {
      //devicePtr = attachStepper();
      break;
    }
    case DEVICE_TYPE_MOTOR: 
    {
      //devicePtr = attachMotor();
      break;
    }
    case DEVICE_TYPE_SERVO: 
    {
      //devicePtr = new MrlServo(); //no need to pass the type here
      break;
    }
    case DEVICE_TYPE_I2C: 
    {
      //devicePtr = new MrlI2CBus();
      break;
    }
    case DEVICE_TYPE_NEOPIXEL: 
    {
      //devicePtr = new MrlNeopixel();
      break;
    }
    default: 
    {
      // TODO: publish error message
      MrlMsg::publishDebug(F("Unknown Message Type."));
      break;
    }
	}

	// if we have a device - then attach it and call its attach method with config passed in
	// and send back a publishedAttachedDevice with its name - so Arduino-Java land knows
	// it was successfully attached
	if (devicePtr) 
  {
		if (devicePtr->deviceAttach(config, configSize)) 
    {
			addDevice(devicePtr);
			publishAttachedDevice(devicePtr->id, nameSize, ioCmd + 3);
		}
    else 
    {
			MrlMsg::publishError(ERROR_UNKOWN_SENSOR, F("DEVICE not attached"));
			delete devicePtr;
		}
	}
}

/**
 * deviceDetach - get the device
 * if it exists delete it and remove it from the deviceList
 */
void ActMrlComm::deviceDetach(int id) 
{
	ListNode<Device*>* node = deviceList.getRoot();
	int index = 0;
  
	while (node != NULL) 
  {
		if (node->data->id == id) 
    {
			delete node->data;
			deviceList.remove(index);
      break;
		}
		node = node->next;
		index++;
	}
}

/**
 * getDevice - this method will look up a device by it's id in the device list.
 * it returns null if the device isn't found.
 */
Device* ActMrlComm::getDevice(int id) 
{
	ListNode<Device*>* node = deviceList.getRoot();
  
	while (node != NULL)
  {
		if (node->data->id == id) 
    {
			return node->data;
		}
		node = node->next;
	}
	MrlMsg::publishError(ERROR_DOES_NOT_EXIST);
	return NULL; //returning a NULL ptr can cause runtime error
	// you'll still get a runtime error if any field, member or method not
	// defined is accessed
}

/**
 * This adds a device to the current set of active devices in the deviceList.
 * 
 * FIXME - G: I think dynamic array would work better
 * at least for the deviceList
 * TODO: KW: i think it's pretty dynamic now.
 * G: the nextDeviceId & Id leaves something to be desired - and the "index" does
 * not spin through the deviceList to find it .. a dynamic array of pointers would only
 * expand if it could not accomidate the current number of devices, when a device was
 * removed - the slot could be re-used by the next device request
 */
void ActMrlComm::addDevice(Device* device) 
{
	deviceList.add(device);
}

/***********************************************************************
 * UPDATE DEVICES BEGIN
 * updateDevices updates each type of device put on the device list
 * depending on their type.
 * This method processes each loop. Typically this "back-end"
 * processing will read data from pins, or change states of non-blocking
 * pulses, or possibly regulate a motor based on pid values read from
 * pins
 */
void ActMrlComm::updateDevices() 
{
	// update self - the first device which
	// is type Arduino
	update();

	ListNode<Device*>* node = deviceList.getRoot();
	// iterate through our device list and call update on them.
	while (node != NULL) 
  {
		node->data->update();
		node = node->next;
	}
}

/** 
 * Réveil du module audio
 */
void ActMrlComm::enableAudio()
{
  digitalWrite(MAX9744_SHTDOWN_PIN, HIGH);
}

/** 
 * Met le MAX9744 en veille
 */
void ActMrlComm::disableAudio()
{
  digitalWrite(MAX9744_SHTDOWN_PIN, LOW);
}

/** 
 * Coupe la sortie audio pendant le démarrage du PC
 */
void ActMrlComm::setMuteOn()
{
  digitalWrite(MAX9744_MUTE_PIN, LOW);
}

/** 
 * Active la sortie audio pendant le démarrage du PC
 */
void ActMrlComm::setMuteOff()
{
  digitalWrite(MAX9744_MUTE_PIN, HIGH);
}

