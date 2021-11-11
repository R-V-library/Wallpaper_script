# Wallpaper_script
Script that changes desktop background with the bing.com daily wallpaper.

## Overview
When taking a closer look at the html header of the bing.com website,
the URL of the daily wallpaper can be found in the metadata.

> `<meta property="og:image" content="..." />`

The script extracts the URL from the metadata via a regex
and tweaks the found URL to obtain the image without bing.com watermark.
The wallpaper is saved to the raspbian default dir.
Consequently, the wallpaper is changed 

## Setting up the script    


### Notes
