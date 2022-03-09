#!/bin/bash
# use bash

# init argument flags 
q=0
v=0
d=/usr/share/rpd-wallpaper

# Help function 
Help()
{
   # Display Help
   echo ""
   echo "Example usage $(basename $0)" 2>&1
   echo ""
   echo "Syntax: $(basename $0) [-hHqQvV]<-d|D path>"
   echo "options:"
   echo "-d | -D	Change output directory."
   echo "-h | -H	Print this Help."
   echo "-q | -Q	Quiet mode, no output shown."
   echo "-v | -V	Verbose mode."
   exit 0
}

# Handle arguments via getopts
while getopts "d:D:hHqQvV" option; do
	case $option in 
		d|D) # change output directory
			d=${OPTARG};;
		h|H) # display help
			Help;;
		q|Q) # set quiet flag, higher priority then verbose flag
			q=1;;
		v|V) # set verbose flag
			v=1;;
		\?) # invalid argument
			Help;;
	esac 
done 

# change directory to directory containing wallpapers
cd $d

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
		sudo wget bing.com -O Wallpaper.html
	else 
		sudo wget bing.com -q -O Wallpaper.html
	fi
			
	# filename contained in html header (<meta property="og:image" .... >)
	# use regex to extract file url from header
	filename2='Wallpaper.html'
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
				echo "Image URL: $image" 
			fi
			
			# break after regex match
			break 
		fi
	
	done < $filename2 # feed wallpaper.html file into loop 
	
	filename='Wallpaper.html'
	regex2='"Description":(.*),"Image":'
	
	if [[ $v == 1 && $q == 0 ]]
	then
		echo "Description regex: $regex2" # display Regex used 
	fi 
	
	while read line; # loop through file
	do 
		if [[ $line =~ $regex2 ]]
		then 
						
			# change url to download full hd image 
			description="${BASH_REMATCH[1]}"
			
			if [[ $q == 0 ]]
			then
				if [[ $v == 1 ]]
				then 
					# signal regex match
					echo "Image description found" 
				fi
				
				# display image URL
				echo "Image description: $description" 
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
	sudo rm Wallpaper.html
	if [[ $v == 1 && $q == 0 ]]
	then 
		echo "Removing html file"
	fi	
		
else 
		
	# Message if wallpaper up to date
	if [[ $q == 0 ]]
	then
			
		echo "Wallpaper up to date"
		echo "Last changed on: $(date +%F)"
									
	fi	
fi

