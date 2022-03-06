
#include "stdio.h"
#include "argp.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/stat.h"
#include "regex.h"
#include "limits.h"
#include "time.h"
#include "string.h"
#include "stdbool.h"

/* Struct which stores argument values */
struct arguments{
	char * args[4]; 
	bool q,v,p; // quiet & verbose flag
	char * set_directory; // directory chosen by user
	char wallpaper_file[PATH_MAX]; // filename chosen by user
};

/*	Function definitions	*/
int update_wallpaper(const struct arguments);
int match_regex(regex_t *, char *, char *);


/*		 ~~~Argparser~~~		 */
const char *argp_program_version = "Wallpaper script 1.0";

static char doc[] = "Wallpaper script arguments";
static char args_doc[] = "Wallpaper script -dDqQpPvV";

/* Possible options */
static struct argp_option options[] = { // name, key, arg, flags, doc, group
	{"directory",'d',"<DIR>",0,"Set wallpaper directory"},
	{0,'D',0,OPTION_ALIAS}, // alias of d
	{"get_directory",'p',0,0,"Get wallpaper directory"},
	{0,'P',0,OPTION_ALIAS}, // alias of p	
	{"quiet",'q',0,0, "No output"},
	{0,'Q', 0, OPTION_ALIAS}, //alias of q
	{"verbose",'v',0,0,"Verbose output"},
	{0,'V',0,OPTION_ALIAS}, // alias of v
	{ 0 }
};

static error_t parse_opt(int key, char* arg, struct argp_state *state){
	struct arguments *parguments = state->input;

	switch(key){
	
		case 'd' : case 'D':
			parguments->p = 1;
			parguments->set_directory = arg;
			break;
	
		case 'p' : case 'P':
			parguments->p = 1;
			break;
	
		case 'q' : case 'Q':
			parguments->q = 1;
			parguments->v = 0;
			break;
	
		case 'v' : case 'V':
			if (parguments->q != 1){
				parguments->v = 1;
			}
			break;
	
		case ARGP_KEY_ARG:
			if (state->arg_num >= 4){
				argp_usage(state);
			}
			parguments->args[state->arg_num] = arg;
			break;
	
		default:
			return ARGP_ERR_UNKNOWN;
		}
	return 0;
}

static struct argp argparser = {options,parse_opt,args_doc,doc};

int main(int argc, char **argv){
	
	/*	Specify default path	*/ 
	char * default_path = "/usr/share/rpd-wallpaper";
	
	/*	Make arguments struct + initialize argument parser	*/
	struct arguments parguments;
	parguments.v = 0;
	parguments.q = 0;
	parguments.p = 0;
	parguments.set_directory = default_path;
	argp_parse(&argparser,argc,argv,0,0,&parguments);
	
	/*	Debugging purposes	*/ 
	//printf("Arguments: \n V: %d \n Q: %d \n P: %s \n D: %s \n",parguments.v,parguments.q,parguments.set_directory,parguments.set_directory);
	
	/*	change dir according to flags	*/
	if (chdir(parguments.set_directory) != 0){
		perror("Chdir command failed\n\r");
		exit(EXIT_FAILURE);
	}
	else if (!parguments.q){
			printf("Working directory: %s\n\r",parguments.set_directory);
	}
	
	/* Define Wallpaper file name */
	sprintf(parguments.wallpaper_file,"%s/Wallpaper.jpg",parguments.set_directory);
	
	/*	Display dir if -d or -p used and not quiet	*/
	if ((parguments.p || parguments.v) && !parguments.q){
		printf("Chosen directory: %s\n\r", parguments.set_directory);
		printf("Wallpaper file: %s\n\r", parguments.wallpaper_file);
	}
	
	/*	Check if file exists	*/
	struct stat filestat;
	if (stat(parguments.wallpaper_file,&filestat) == 0){
		
		if (parguments.v && !parguments.q){
			printf("File found\n\r");
		}
		
		/*	Check if up to date	*/
		char t_wallpaper[PATH_MAX];
		char t_now[PATH_MAX];
		time_t t = time(NULL);
		
		strftime(t_wallpaper, PATH_MAX, "%Y-%m-%d", localtime(&filestat.st_mtime));
		strftime(t_now, PATH_MAX, "%Y-%m-%d", localtime(&t));
		if (parguments.v && !parguments.q){
			printf("Date updated: %s\n\r",t_wallpaper);
			printf("Date today: %s\n\r",t_now);
		}
		
		if (strcmp(t_now,t_wallpaper) == 0){
			if (!parguments.q){
				printf("File up to date\n\r");
			}
		}
		
		/*	If out of date download new	*/
		else {
			if (!parguments.q){
				printf("Fetching new Wallpaper\n\r");
			}
			if (update_wallpaper(parguments) != 0){
				exit(EXIT_FAILURE);
			}
		}
	}
	
	/*	If file not found then download new	*/
	else{
		if (parguments.v && !parguments.q){
			perror("File not found\n\r");
			printf("Fetching new Wallpaper\n\r");
		}
		if (update_wallpaper(parguments) != 0){
			exit(EXIT_FAILURE);
		}
	}
	
	/*	Exit program	*/
	exit(EXIT_SUCCESS);
}

int update_wallpaper(const struct arguments parguments){
	/*	Download HTML page to extract data	*/	
	if (parguments.v && !parguments.q){
		printf("Downloading HTML file\n\r");
		system("sudo wget bing.com -v -O Wallpaper.html");
	}
	else {
		system("sudo wget bing.com -q -O Wallpaper.html");
	}
	
	char * rgx = "<meta property=\"og:image\" content=\"(.*)_tmb.jpg";
	regex_t re;
	/*	Check if regex valid	*/ 
	if (regcomp(&re,rgx,REG_EXTENDED|REG_NEWLINE) == 0){
		if (parguments.v && !parguments.q){
		printf("Regex valid\n\r");
		}
	}
	else{
		perror("Regex invalid\n\r");
	}

	if (parguments.v && !parguments.q){
		printf("Executing regex search\n\r");
	}
		
	/*	Open file + search for regex	*/
	char html_file[PATH_MAX];
	sprintf(html_file,"%s/Wallpaper.html",parguments.set_directory);
	
	FILE * file = fopen(html_file,"r");
	char * line;
	size_t len = 0;
	ssize_t read;
	bool ready = false;
	char result[PATH_MAX] = "";
	while((read = getline(&line,&len,file)) != -1 && !ready){
		ready = !match_regex(&re, line, result);
		if (parguments.v && !parguments.q){
			printf("Retrieved line of length %zu:\n\r", read);
			printf("found = %s\n\r",result);
		}		
	}
	
	
	/*	Close file + free mem	*/ 
	fclose(file);
	free(line);
	regfree(&re);
		
	/*	Construct URL	*/
	char url[PATH_MAX];
	snprintf(url,PATH_MAX,"%s_1920x1080.jpg",result);
	if (parguments.v && !parguments.q){
		printf("Found image URL: %s\n\r",url);
	}
	
	/* Download html file	*/
	char command[PATH_MAX];
	if (parguments.v){
		sprintf(command, "sudo wget %s -v -O Wallpaper.jpg",url);
	}
	else{
		sprintf(command, "sudo wget %s -q -O Wallpaper.jpg",url);
	}
	system(command);
	if (!parguments.q){
		printf("Downloading new image\n\r");
		if (parguments.v){
			printf("Command: %s\n\r",command);
		}
	}
	
	/*	Display image description	*/
	if (!parguments.q){
		
		char * rgx2 ="\"Description\":(\".*\"),\"Image\":"; 
		/*	Check if regex valid	*/ 
		if (regcomp(&re,rgx2,REG_EXTENDED|REG_NEWLINE) == 0){
			if (parguments.v){
				printf("Regex 2 valid\n\r");
			}
		}
		else{
			perror("Regex 2 invalid \n\r");
		}

		if (parguments.v){
			printf("Executing regex search\n\r");
		}
		
		FILE * file = fopen(html_file,"r");
		char * line;
		size_t len = 0;
		ssize_t read;
		ready = false;
		char result[PATH_MAX];
		while((read = getline(&line,&len,file)) != -1 && !ready){
			ready = !match_regex(&re, line, result);
			if (parguments.v){
				printf("Retrieved line of length %zu:\n\r", read);
			}
			
		}
		
		printf("Image description: %s\n\r",result);
		
		/*	Close file + free mem	*/ 
		fclose(file);
		free(line);
		regfree(&re);
	}
	
	/* remove html file	*/
	sprintf(command, "sudo rm ./Wallpaper.html");
	system(command);
	if (parguments.v && !parguments.q){
		printf("Removing HTML page\n\r");
	}
	
	/* update wallpaper */
	sprintf(command,"pcmanfm --set-wallpaper %s/Wallpaper.jpg",parguments.set_directory);
	system(command);
	if (!parguments.q){
		printf("Updating wallpaper\n\r");
		if (parguments.v){
			printf(command);
		}
	}
	return EXIT_SUCCESS;	
}

/*
  Match the string in <to_match> against the compiled regular
  expression in <rx> and store result in <result>.
  inspiration/source: https://www.lemoda.net/c/unix-regex/
 */
int match_regex (regex_t * rx, char * to_match, char * result)
{
    /* "previous" is a pointer into the string which points to the end of the
       previous match. */
    char * previous = to_match;
    /* "N_matches" is the maximum number of matches allowed. */
    const int n_matches = 2;
    /* "matches" contains the matches found. */
    regmatch_t matches[n_matches];

    while (1) {
        int i = 0;
        int nomatch = regexec (rx, previous, n_matches, matches, 0);
        if (nomatch) {
            //printf ("No matches found.\n\r");
            return 1;
        }
        for (i = 0; i < n_matches; i++) {
            int start;
            int finish;
            if (matches[i].rm_so == -1) {
                break;
            }
            start = matches[i].rm_so + (previous - to_match);
            finish = matches[i].rm_eo + (previous - to_match);
            snprintf(result,PATH_MAX,"%.*s",(finish-start), to_match + start);
        }
        previous += matches[0].rm_eo;
        break;
    }
    
    return 0;
}

