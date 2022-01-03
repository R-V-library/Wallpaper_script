#!/usr/bin/env python3
# use python3 

# Libraries 
import argparse
import datetime
import logging
import os
import re

# Argument parsing 
parser= argparse.ArgumentParser(prog="Wallpaper script",description="Wallpaper script arguments")
parser.add_argument("-q", "-Q", action="store_true",default=False,help="Quiet mode")
parser.add_argument("-v", "-V",action="store_true",default=False,help="Verbose mode")
parser.add_argument("-d", "-D",action="store",type=str,default="/usr/share/rpd-wallpaper",help="Set wallpaper directory")
parser.add_argument("-p","-P",action="store_true",default=False,help= "Get wallpaper directory")
args = parser.parse_args()


# Actual wallpaper fetching
def update_wallpaper(logger):
	
	logger.debug("Downloading html file")
				
	if args.v:
		command = "sudo wget bing.com -O Wallpaper.html"
		logger.debug(" Command used: %s"% (command)) 
		os.system("sudo wget bing.com -O Wallpaper.html")
		
	else:
		command = "sudo wget bing.com -q -O Wallpaper.html"
		os.system(command)
	
	
	regex = r"<meta property=\"og:image\" content=\"(.*)_tmb.jpg"	
	logger.debug("Regex used: "+regex)
	
	logger.debug(" Trying to open file: %s"% (os.getcwd() + "/Wallpaper.html"))
	try:
		with open("Wallpaper.html",'r') as f:
			logger.debug("Succesfully opened file: %s"% (os.getcwd() + "/Wallpaper.html"))
			for line in f:
				m = re.search(regex,line.strip())
				if m:
					logger.debug("Regex match found")
					url = m.group(1)+ "_1920x1080.jpg"
					logger.info("Found image URL: "+ url)
					break
			logger.debug("Closing and removing file: %s "% (os.getcwd() + "/Wallpaper.html"))			
			f.close()
			command = "sudo rm " + os.getcwd() + "/Wallpaper.html"
			logger.debug(" Command used: %s"% (command))
			os.system(command)
				
	except Exception as e:
		logger.error("An error occured during execution")
		logger.error(e)
		
	logger.debug("Downloading image")
	if args.v:
		command = "sudo wget " + url + " -O Wallpaper.jpg"
	else:
		command = "sudo wget " + url + " -q -O Wallpaper.jpg"
		
	os.system(command)
	logger.debug("File saved in: " + os.getcwd())
		
	logger.info("Updating wallpaper")
	command = "pcmanfm --set-wallpaper " + os.getcwd() + "/Wallpaper.jpg"
	os.system(command)
	
def main():
	
	# Set default logging level
	logging.basicConfig(level=logging.INFO)
	logger = logging.getLogger()
	
	# Quiet mode higher priority then verbose mode	
	if args.q:
		args.v = False
		logger.setLevel(level=logging.ERROR)

	if args.v:
		logger.setLevel(level=logging.DEBUG)

	# Display directory
	if args.p or args.d:
		logger.info("Wallpaper directory: "+ args.d)
		os.chdir(args.d)

	if not os.path.exists(os.getcwd()+"/Wallpaper.jpg"):
		logger.debug("File does not exist")
		update_wallpaper(logger)
	
	else:
		logger.debug("File exists")
		t = os.path.getmtime(os.getcwd() + "/Wallpaper.jpg")
		# ~ t_mod = datetime.date.fromtimestamp(t).isoformat()
		# ~ print(t_mod,datetime.date.today().isoformat(),t_mod == datetime.date.today().isoformat())
		if datetime.date.fromtimestamp(t).isoformat() != datetime.date.today().isoformat(): 
			logger.info("File out of date")
			update_wallpaper(logger)
	
		else:
			logger.info("File up to date")
	
	
	logger.info("Script finished without errors")
	
main()
