
#include "stdio.h"
#include "argp.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/stat.h"
#include "regex.h"
#include "limits.h"
#include "time.h"
#include "string.h"

// argparser
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

struct arguments{
	char * args[4]; // 4 args max allowed
	int q,v,p; // quiet & verbose flag
	char * set_directory; // directory chosen by user
	char wallpaper_file[PATH_MAX];
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


// logging
// os / system
// regex
// date time
//downloads using wget

void update_wallpaper();

int main(int argc, char **argv){
	
	//char * command = "";
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
			printf("new dir: %s\n",parguments.set_directory);
	}
	/*sprintf(command,"cd %s",parguments.set_directory);
	system(command);*/
	
	/*	Display dir if -d or -p used	*/
	printf("Chosen directory: %s\n",parguments.set_directory);
	
	/*	Check if file exists	*/ 
		/*getcwd(parguments.set_directory,PATH_MAX);
		printf("getcwd path: %s",parguments.set_directory);*/
	
	sprintf(parguments.wallpaper_file,"%s/Wallpaper.jpg",parguments.set_directory);
	sprintf(wallpaper,"%s/Wallpaper.jpg",parguments.set_directory);
	printf("Wallpaper file: %s\n",wallpaper);
	
	/*if (access(wallpaper,F_OK) != -1){
		printf("File found");
	}
	else{
		perror("Error");
	}*/
	
	struct stat filestat;
	
	int exist = stat(wallpaper,&filestat);
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
	
	
	/*FILE *file;
	if (file = fopen(wallpaper,"r")){
		fclose(file);
		printf("File exists");
	}
	else{
		printf("File does not exist");
	}*/
	
	
	/*printf("Access check: %d",access(wallpaper,R_OK));
	if ( access(wallpaper,X_OK) != -1){
		printf("File found");
	}
	else{
		printf("File not found");
	}*/
	
	/*	Exit program	*/
	exit(0);
}

void update_wallpaper(const struct arguments parguments){
	printf("Downloading HTML file\n");
	
	
	if (parguments.v){
		system("sudo wget bing.com -O Wallpaper.html");
	}
	else {
		system("sudo wget bing.com -q -O Wallpaper.html");
	}
	
	char * rgx = "<meta property=\"og:image\" content=\"(.*)_tmb.jpg";
	regex_t re;
	regmatch_t matches[1]; // only 1 regex
	if (regcomp(&re,rgx,REG_EXTENDED|REG_NOSUB) != 0){
		perror("Regex problem:");
	}
	printf("Executing regex search\n");
	
	/*	Open file	*/
	char html_file[PATH_MAX];
	sprintf(html_file,"%s/Wallpaper.html",parguments.set_directory);
	
	FILE * file = fopen(html_file,"r");
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int status;
	int numchars;
	char * found;
	
	int i = 0;
	while((read = getline(&line,&len,file)) != -1){
		//printf("%d: %s\n",i,line);
		
		
		/*	regex not working	*/
		status = regexec(&re,line,0,matches,0);
		if (status != 0){
			printf("%d: Problem with regex\n",i);
		}
		else{
			printf("%d: Regex match found\n",i);
			numchars =(int) matches[0].rm_eo - (int) matches[0].rm_so;
			found = (char *) malloc((numchars+1)/sizeof(char));
			strncpy(found,line+matches[0].rm_so,numchars);
			found[numchars] = '\0';
			printf("From %d to %d (%s) \n", (int) matches[0].rm_so, (int) matches[0].rm_eo, found);
			printf("Found match: %s\n",found);
			//break;
		}
		i = i+1;
	}
	fclose(file);
	free(line);
	free(found);
	
	//int status = regexec(&re,string,0,NULL,0);
	regfree(&re);
	
	
}
