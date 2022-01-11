
#include "stdio.h"
#include "argp.h"
#include "stdlib.h"
#include "unistd.h"
#include "regex.h"
#include "limits.h"
#include "time.h"

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
	char *args[4]; // 4 args max allowed
	int q,v; // quiet & verbose flag
	char *set_directory; // directory chosen by user
};

static error_t parse_opt(int key, char* arg, struct argp_state *state){
	struct arguments *parguments = state->input;

	switch(key){
	
		case 'd':case 'D':
			parguments->set_directory = arg;
			break;
	
		case 'p' : case 'P':
			if (getcwd(parguments->set_directory,PATH_MAX) == NULL){
				printf("No path found\n");
			}
			else{
				printf("path: %s",parguments->set_directory);
			}
			break;
	
		case 'q':case 'Q':
			parguments->q = 1;
			break;
	
		case 'v' : case 'V':
			parguments-> v = 1;
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

int main(int argc, char **argv){
	
	
	char * default_path = "/usr/share/rpd-wallpaper";
	
	struct arguments parguments;
	parguments.v = 0;
	parguments.q = 0;
	parguments.set_directory = default_path;
	
	argp_parse(&argparser,argc,argv,0,0,&parguments);
	
	printf("Arguments: \n V: %d \n Q: %d \n P: %s \n D: %s \n ",parguments.v,parguments.q,parguments.set_directory,parguments.set_directory);
	exit(0);
}
