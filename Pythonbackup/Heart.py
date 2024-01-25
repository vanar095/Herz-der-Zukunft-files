import time
import pygame
import RPi.GPIO as GPIO
import serial

pygame.init()
pygame.mixer.init()

cprPin = 26      #Pin for receiving compress
statePin = 20    #Pin for receiving alternating puzzle state

previousInput = 0     #Last recorded input of the state pin
currentState = 0      #State variable, 0 = panic, 1 = death, 2 
cpr = False           #cpr initiated
baudrate = 115200	

beatDelay = 1.9       #Length of file heartbeat

GPIO.setmode(GPIO.BCM)
GPIO.setup(statePin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.setup(cprPin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
	
try:
	while True:
		#print(GPIO.input(statePin))

		if currentState >= 3:
			currentState = 0
			
		if currentState == 0:
			pygame.mixer.music.load("/home/pi/Downloads/sounds/fastbeat.mp3")
			pygame.mixer.music.play()
		
		if currentState == 1:
			if GPIO.input(cprPin) == 1 and not cpr:
				pygame.mixer.music.load("/home/pi/Downloads/sounds/bump1.mp3")
				pygame.mixer.music.play()
				cpr = True
			if GPIO.input(cprPin) == 0 and cpr:
				pygame.mixer.music.load("/home/pi/Downloads/sounds/bump2.mp3")
				pygame.mixer.music.play()
				cpr = False
				
			time.sleep(0.1)
		
		if currentState == 2:
			pygame.mixer.music.load("/home/pi/Downloads/sounds/normalbeat.mp3")
			pygame.mixer.music.play()
			time.sleep(beatDelay)
		
		
		while pygame.mixer.music.get_busy():
			if GPIO.input(statePin) != previousInput:
				previousInput = GPIO.input(statePin)
				currentState += 1
				playOnce = True
				cpr = False
				pygame.mixer.music.stop()
				print(currentState)
				break
			
		if GPIO.input(statePin) != previousInput:
			previousInput = GPIO.input(statePin)
			currentState += 1
			playOnce = True
			cpr = False
			pygame.mixer.music.stop()
			print(currentState)
		
except KeyboardInterrupt:
	print("\nExiting program")





