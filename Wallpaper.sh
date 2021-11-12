
# use bash
#!/usr/bin/env bash

# Help function 
Help()
{
   # Display Help
   echo ""
   echo "Example usage $(basename $0)" 2>&1
   echo ""
   echo "Syntax: $(basename $0) [-dDhHqQvV]"
   echo "options:"
   echo "-d | -D	display description of downloaded wallpaper."
   echo "-h | -H   	Print this Help."
   echo "-q | -Q	Quiet mode, no output shown."
   echo "-v | -V	Verbose mode."
   exit 0
}

# init argument flags 
d=0
q=0
v=0

# Handle arguments via getoptst
while getopts "dDhHqQvV" option; do
	case $option in 
		d|D) # set description flag
			#echo "d set"
			d=1;;
		h|H) # display help
			#echo "h set"
			Help;;
		q|Q) # set quiet flag, higher priority then verbose flag
			#echo "q set"
			q=1;;
		v|V) # set verbose flag
			#echo "v set"
			v=1;;
		\?) # invalid argument
			Help;;
	esac 
done 

# change directory to directory containing wallpapers
cd /usr/share/rpd-wallpaper

if [[ $v == 1 && $q == 0 ]]
then
	echo "Working directory: $PWD" # show working directory
fi

File=$PWD/Wallpaper.jpg

# check if Wallpaper.jpg exists
# if file Wallpaper.jpg does not exist or date different from current date => get new wallpaper
if [[ !(-e $File) || $(stat -c '%y' $File | cut -d ' ' -f1) != $(date +%F) ]]; 
then 
	if [[ $q == 0 ]]
	then
		echo "Wallpaper out of date"
		echo "Fetching new wallpaper"
	fi 		
	
	# use wget to download Bing.com webpage
	# save to file wallpaper.html
	if [[ $v == 1 && $q == 0 ]]
	then 
		echo "Downloading html file"
		sudo wget bing.com -O wallpaper.html
	else 
		sudo wget bing.com -q -O wallpaper.html
	fi
	
		
	# filename contained in html header (<meta property="og:image" .... >)
	# use regex to extract file url from header
	filename='wallpaper.html'
	regex='<meta property="og:image" content="(.*)_tmb.jpg'
	
	if [[ $v == 1 && $q == 0 ]]
	then
		echo "URL Regex: $regex" # display Regex used 
	fi 
	
	while read line; # loop through file
	do 
		if [[ $line =~ $regex ]]
		then 
						
			# change url to download full hd image 
			image="${BASH_REMATCH[1]}"
			image="${image}_1920x1080.jpg" 
			
			if [[ $v == 1 && $q == 0 ]]
			then
				# signal regex match
				echo "Image URL Found" 
			
				# display image URL
				echo "image URL: $image" 
			fi
			
			# break after regex match
			break 
		fi
	
	done < $filename # feed wallpaper.html file into loop 
		
	# raspbian wallpaper directory : /usr/share/rpd-wallpaper
	# use wget to download wallpaper and save in wallpaper directory
	if [[ $v == 1 && $q == 0 ]]
	then 
		echo "Downloading image"
		sudo wget $image -O Wallpaper.jpg
	else
		sudo wget $image -q -O Wallpaper.jpg
	fi
	
	# change wallpaper via pcmanfm
	path="${PWD}/Wallpaper.jpg"
	
	if [[ $v == 1 && $q == 0 ]]
	then
		echo "Saving image to: $path"
	fi
	
	if [[ $q == 0 ]]
	then
		echo "Changing wallpaper"
	fi 
	
	pcmanfm --set-wallpaper /usr/share/rpd-wallpaper/Wallpaper.jpg #pcmanfm only works !without! sudo 
	
	# remove wallpaper.htlm file
	if [[ $v == 1 && $q == 0 ]]
	then 
		echo "Cleaning html file"
	fi
	sudo rm wallpaper.html
	# sudo rm Wallpaper.jpg # for debugging purposes only

else 
	
	# Message if wallpaper up to date
	if [[ $q == 0 ]]
	then
		echo "Wallpaper up to date"
		echo "Last changed on: $(date +%F)"
	fi
		
fi
