
#include "stdio.h"
#include "argp.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/stat.h"
#include "regex.h"
#include "limits.h"
#include "time.h"
#include "string.h"

/* Struct which stores argument values */
struct arguments{
	char * args[4]; 
	int q,v,p; // quiet & verbose flag
	char * set_directory; // directory chosen by user
	char wallpaper_file[PATH_MAX]; // filename chosen by user
};

/*	Function definitions	*/
void update_wallpaper(const struct arguments);
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
			//printf("path: %s\n",parguments->set_directory);
			break;
	
		case 'p' : case 'P':
			parguments->p = 1;
			/*if (parguments->set_directory == NULL){
				printf("No path found\n");
			}
			else{
				printf("path: %s\n",parguments->set_directory);
			}*/
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


// logging : nok
// os / system : ok
// regex : ok
// date time : ok 
//downloads using wget: ok


int main(int argc, char **argv){
	
	char wallpaper[PATH_MAX] = ""; // avoid segmentation fault by using max path size
	
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
	printf("Arguments: \n V: %d \n Q: %d \n P: %s \n D: %s \n",parguments.v,parguments.q,parguments.set_directory,parguments.set_directory);
	
	/*	change dir according to flags	*/
	if (chdir(parguments.set_directory) != 0){
		perror("Chdir command failed");
	}
	else{
			printf("New dir: %s\n",parguments.set_directory);
	}
	
	/* Define Wallpaper file name */
	sprintf(parguments.wallpaper_file,"%s/Wallpaper.jpg",parguments.set_directory);
	
	/*	Display dir if -d or -p used and not quiet	*/
	if ((parguments.set_directory || parguments.p)  && !parguments.q){
		printf("Chosen directory: %s\n",parguments.set_directory);
		printf("Wallpaper file: %s\n",parguments.wallpaper_file);
	}
	
	/*	Check if file exists	*/
	struct stat filestat;
	int exist = stat(parguments.wallpaper_file,&filestat);
	if (exist == 0){
		printf("File found\n");
		
		/*	Check if up to date	*/
		char t_wallpaper[PATH_MAX];
		char t_now[PATH_MAX];
		time_t t = time(NULL);
		
		strftime(t_wallpaper, PATH_MAX, "%Y-%m-%d", localtime(&filestat.st_mtime));
		strftime(t_now, PATH_MAX, "%Y-%m-%d", localtime(&t));
		printf("Time updated: %s\n",t_wallpaper);
		printf("Time today: %s\n",t_now);
		
		if (strcmp(t_now,t_wallpaper) == 0){
			printf("File up to date\n");
		}
		
		/*	If out of date download new	*/
		else {
			printf("Fetching new Wallpaper\n");
			update_wallpaper(parguments);
		}
	}
	
	/*	If file not found then download new	*/
	else{
		printf("File not found\n");
		printf("Fetching new Wallpaper\n");
		update_wallpaper(parguments);
	}
	
	/*	Exit program	*/
	exit(0);
}

void update_wallpaper(const struct arguments parguments){
		
	if (parguments.v){
		printf("Downloading HTML file\n");
		system("sudo wget bing.com -O Wallpaper.html");
	}
	else {
		system("sudo wget bing.com -q -O Wallpaper.html");
	}
	
	char * rgx = "<meta property=\"og:image\" content=\"(.*)_tmb.jpg";
	regex_t re;
	int status;
	status = regcomp(&re,rgx,REG_EXTENDED|REG_NEWLINE);
	if (!status){
		printf("Regex valid\n");
	}
	else{
		perror("Regex invalid\n");
	}
	
	printf("Executing regex search\n");
	
	/*	Open file	*/
	char html_file[PATH_MAX];
	sprintf(html_file,"%s/Wallpaper.html",parguments.set_directory);
	
	FILE * file = fopen(html_file,"r");
	char * line;
	size_t len = 0;
	ssize_t read;
	
	int ready = 1;
	char result[PATH_MAX];
	while((read = getline(&line,&len,file)) != -1 && ready != 0){
		printf("Retrieved line of length %zu:\n", read);
        
        //printf("%s\n", line);
		ready = match_regex(&re, line, result);
		printf("found = %s\n\r",result);
		
	}
	fclose(file);
	free(line);
	
	regfree(&re);
	
	char url[PATH_MAX];
	snprintf(url,PATH_MAX,"%s_1920x1080.jpg",result);
	printf("Found image URL: %s",url);
	
	// download html file
	printf("Downloading new image\n\r");
	char command[PATH_MAX];
	sprintf(command, "sudo wget %s -q -O Wallpaper.jpg",url);
	system(command);
	printf("Command: %s\n\r",command);
	
	// remove html file
	sprintf(command, "sudo rm ./Wallpaper.html");
	system(command);
	
	// update wallpaper
	printf("Updating wallpaper");
	sprintf(command,"pcmanfm --set-wallpaper %s/Wallpaper.jpg",parguments.set_directory);
	printf(command);
	system(command);
	
	
}

/*
  Match the string in <to_match> against the compiled regular
  expression in <rx> and store result in <result>.
  source:https://www.lemoda.net/c/unix-regex/
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
            printf ("No matches found.\n\r");
            return 0;
        }
        for (i = 0; i < n_matches; i++) {
            int start;
            int finish;
            if (matches[i].rm_so == -1) {
                break;
            }
            start = matches[i].rm_so + (previous - to_match);
            finish = matches[i].rm_eo + (previous - to_match);
            if (i == 0) {
                printf ("$& is ");
            }
            else {
                printf ("$%d is ", i);
            }
            printf ("'%.*s' (bytes %d:%d)\n\r", (finish - start),
                    to_match + start, start, finish);
            snprintf(result,PATH_MAX,"%.*s",(finish-start), to_match + start);
        }
        previous += matches[0].rm_eo;
        break;
    }
    
    return 0;
}

