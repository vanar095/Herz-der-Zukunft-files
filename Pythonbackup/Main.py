import subprocess
import time
import os
import RPi.GPIO as GPIO

resetPin = 0     #Pin to connect to 3.3v for raspberri pi reboot
shutdownPin = 1  #Pin to connect to 3.3v for raspberri pi shutdown

GPIO.setmode(GPIO.BCM)
GPIO.setup(resetPin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)
GPIO.setup(shutdownPin, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

time.sleep(10)    #Wait a bit upon rebooting

def run_script(script_path):
	subprocess.Popen(['python', script_path])
	
def main():
	
	run_script('/home/pi/Downloads/Heart.py')
	run_script('/home/pi/Downloads/BS.py')
	run_script('/home/pi/Downloads/Voice.py')
	
	while True:
		if GPIO.input(resetPin) == 1:
			print("reset")
			os.system("sudo reboot")
		if GPIO.input(shutdownPin) == 1:
			print("shutdown")
			os.system("sudo shutdown")
		
		time.sleep(0.01)
		
if __name__=="__main__":
	main()
	


	
			
			

#ls /dev/tty*
# sudo crontab -e
#@reboot python /home/pi/Downloads/Heart.py
