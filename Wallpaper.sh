
# use bash
#!/usr/bin/env bash

# change directory to directory containing wallpapers
cd /usr/share/rpd-wallpaper
echo $PWD # show working directory

File=$PWD/Wallpaper.jpg
FileDate=$(stat -c '%y' $File | cut -d ' ' -f1)

# check if Wallpaper.jpg exists
# if file Wallpaper.jpg does not exist or date different from current date => get new wallpaper
if [[ !(-f $File) || $FileDate != $(date +%F) ]] 
then 
	#if [[ '(date +%F)' == '(stat -c '%y' $File | cut -d ' ' -f1)' ]]
	#if [[ $FileDate == $(date +%F) ]]
	#then 
		echo "Wallpaper out of date"
		echo "Fetching new wallpaper ..."
		
		# use wget to download Bing.com webpage
		# save to file wallpaper.html
		sudo wget bing.com -q -O wallpaper.html
		
		# filename contained in html header (<meta property="og:image" .... >)
		# use regex to extract file url from header
		filename='wallpaper.html'
		n=1
		regex='<meta property="og:image" content="(.*)_tmb.jpg'
		echo "Regex: $regex"

		while read line; # loop through file
		do 
		#echo "Line No. $n : $line"
		#n=$((n+1))
	
		if [[ $line =~ $regex ]]
		then 
			echo "Found"
			image="${BASH_REMATCH[1]}"
			image="${image}_1920x1080.jpg" # change url to download full hd image 
			echo "$image"
		break
		fi
	
		done < $filename # feed wallpaper.html file into loop 
		
		# wallpaper directory : 
		# use wget to download wallpaper and save in wallpaper directory
		sudo wget $image -q -O Wallpaper.jpg

		# change wallpaper via pcmanfm
		path="${PWD}/Wallpaper.jpg"
		echo "$path"
		pcmanfm --set-wallpaper /usr/share/rpd-wallpaper/Wallpaper.jpg #pcmanfm only works !without! sudo 

		# remove wallpaper.htlm file
		sudo rm wallpaper.html

else 
	echo "Wallpaper up to date"
	echo "Last changed on: $FileDate"
		
	#fi
fi

# use wget to download Bing.com webpage
# save to file wallpaper.html
sudo wget bing.com -q -O wallpaper.html

# filename contained in html header (<meta property="og:image" .... >)
# use regex to extract file url from header
filename='wallpaper.html'
n=1
regex='<meta property="og:image" content="(.*)_tmb.jpg'
echo "Regex: $regex"

while read line; # loop through file
do 
	#echo "Line No. $n : $line"
	#n=$((n+1))
	
	if [[ $line =~ $regex ]]
	then 
		echo "Found"
		image="${BASH_REMATCH[1]}"
		image="${image}_1920x1080.jpg" # change url to download full hd image 
		echo "$image"
		break
	fi
	
done < $filename # feed wallpaper.html file into loop 


#while read line; do
#echo "Line No. $n : $line"
#n=$((n+1))
	#if [[$line =~ $regex]]
	#then 
		#echo "OK"
	#fi
	##for w in $line;
	##do 
		##if [[$line =~ $regex ]]
		##then 
			##foto="${BASH_REMATCH[1]}"
			##echo "${foto}_1920x1080.jpg" # concatenate string to form URL
			##foto="${foto}_1920x1080.jpg" # store URL to variable
		##else
			##echo "$w doesn't match" >&2 # error output in shell
	##done 
#done < $filename

 

# wallpaper directory : 
# use wget to download wallpaper and save in wallpaper directory
sudo wget $image -q -O Wallpaper.jpg

# change wallpaper via pcmanfm
path="${PWD}/Wallpaper.jpg"
echo "$path"
pcmanfm --set-wallpaper /usr/share/rpd-wallpaper/Wallpaper.jpg #pcmanfm only works !without! sudo 


# remove wallpaper.htlm file
sudo rm wallpaper.html
