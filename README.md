# Wallpaper_script
Script that changes desktop background with the bing.com daily wallpaper.

## Overview
The bing.com search engine displays a different background every day.
This image is available for download through the website with a watermark only.
However, when taking a closer look at the html header of the bing.com website,
the URL and other information of the daily wallpaper can be found in the metadata.

> `<meta property="og:image" content="..." />`

The script extracts the URL from the metadata via a regex 
and tweaks the found URL to obtain the image without bing.com watermark.
The wallpaper is saved to the raspbian default dir.
Consequently, the wallpaper is changed 

![Example wallpaper on Raspbian desktop](/img/Wallpaper_screenshot.png)

## Setting up the script    


### Notes
