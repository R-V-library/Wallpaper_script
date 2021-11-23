
# use bash
#!/usr/bin/env bash

# init argument flags 
d=0
q=0
v=0

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

# Download description if non existent 
getDescription(){
	
	if [[ $v == 1 && $q == 0 ]]
	then 
		echo "Downloading html file"
		sudo wget bing.com -O Wallpaper.html
	else 
		sudo wget bing.com -q -O Wallpaper.html
	fi
	
	regex='<meta property="og:description" content="(.*)" /><title>'
	#echo "$regex"
		
	while read line; # loop through file
	do 
		if [[ $line =~ $regex ]]
		then 
			#echo "$line"
						
			# change url to download full hd image 
			description="${BASH_REMATCH[1]}"
									
			if [[ ($v == 1 || $d == 1) && $q == 0 ]]
			then
			
				# signal regex match
				echo "Image description found" 
				
				# show image description
				echo "Image description: $description"		
			fi
			
			# save description to a file
			# description can be called if needed afterwards
			# only output when -q && description.txt not found
			printf "Image description: $description \n" | sudo tee $PWD/description.txt 
						
			# break after regex match
			break 
		fi
	
	done < $PWD/Wallpaper.html # feed wallpaper.html file into loop 
	
	# remove html file
	sudo rm $PWD/Wallpaper.html
	
}

# Handle arguments via getopts
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
		sudo wget bing.com -O Wallpaper.html
	else 
		sudo wget bing.com -q -O Wallpaper.html
	fi
	
		
	# filename contained in html header (<meta property="og:image" .... >)
	# use regex to extract file url from header
	filename='Wallpaper.html'
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
	
	# fetch description of image
	getDescription
		
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
		echo "Removing html file"
	fi
	
	#sudo rm Wallpaper.html
	#sudo rm Wallpaper.jpg # for debugging purposes only

else 
	# set wallpaper every time
	pcmanfm --set-wallpaper /usr/share/rpd-wallpaper/Wallpaper.jpg #pcmanfm only works !without! sudo 
	
	# Message if wallpaper up to date
	if [[ $q == 0 ]]
	then
	
		fi 
		
		echo "Wallpaper up to date"
		echo "Last changed on: $(date +%F)"
				
		# Show description if file found
		if [[ $d == 1 || $v == 1 ]]
		then
		
			if [[ -e $PWD/description.txt ]]
			then
				cat $PWD/description.txt
						
			else
				echo "No description found, fetching description now"
				getDescription
			fi
		fi
	fi
		
fi

