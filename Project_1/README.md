# Project 1 - Beaglebone Green Multithreading Application

## This project realises the concepts of the multithreading on the Beaglebone Green.

Please visit for the source documentation <https://hardiksenjaliya.github.io/apes_project_1/>

**Buildroot is used for building a custom kernel for beaglebone green.**

### *Project Description:*
  The main requirement of the project was to interface Light and Temperature sensor using the same I2C module on the Beaglebone Green. Thus learning a concept of data sharing using synchronization constructs in a multithreading application.
  
 **Four Tasks/Threads in the application and their description:**
  
**Main Task:**
  1. Creates four child threads, temperature sensor thread, light sensor thread, logger thread and socket thread.
  2. Creates five posix message queues
  3. qMain - responsible for reading heartbeat responses from all child threads
  4. qLight - responsible for receiving heartbeat and socket requests 
  5. qTemp - responsible for receiving heartbeat and socket requests
  6. qLogger - responsible for receiving heartbeat request and log messages
  7. qSocket - responsible for receiving socket request responses
  8. qMain and qSocket are BLOCKING queues while all other are NON_BLOCKING queues.

**Heartbeat Functionality:**
Main task requests heartbeat status from each thread periodically every 5 sec. After receiving the request all the thread sends required response. 
If it fails to receive a heartbeat response it sends EXIT command to the threads to exit the application.

**Startup Test:**
Main task requests startup test after spawning required threads. Light sensor threads reads the value of id_register to check I2C functionality while Temperature sensor thread writes value to the tlow register and reads the same value. Logger and Socket tasks just sends the response back as required.

**Light Task:**
Light task runs every 25ms and reads the lux value. After reading lux value it checks for the state of the light and accordingly logs the message into the log file by sending a message to logger queue.
It also responds to the heartbeat and socket requests from the main thread and socket thread.
After initialization, the task runs into the while loop waiting for the sem_post from the timer handler and starts execution after receiving the semaphore.
Inside while loop, it runs in a state machine and handles current state based on the requests received and sends response back to the requester.


**Temperature Task:**
Temperature task runs every 10ms and reads the temperature value. After reading the value it logs the message into the log file by sending a message to logger queue.
It also responds to the heartbeat and socket requests from the main thread and socket thread.
After initialization, the task runs into the while loop waiting for the sem_post from the timer handler and starts execution after receiving the semaphore.
Inside while loop, it runs in a state machine and handles current state based on the requests received and sends response back to the requester.

**Logger Task:**
Logger task receives log messages from all the queues and writes the message into the logfile.
It creates a new file with the user given command line path and name or takes a back up if a file is already presents.
It responses to the heartbeat request whenever requested and exit command if requested.

**Socket Task:**
It reads the requests from the remote client and sends requests to the light and temp thread for required data and send the data as requested to the client and again starts listening for new requests.
