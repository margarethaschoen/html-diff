/** 
 * @file main.c
 * @brief Main source of program.
 *
 * @mainpage HTML Diff
 * \image html logo.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * @par Project:
 * This program was maded with final bachelor project. On Charles University in Prague - MFF (IT):
 * http://www.mff.cuni.cz.
 * \image html logo2.jpg "Facultas Mathematica-physicaque"
 *
 * @par Description:
 * A tool that compares HTML files and shows found differences in user-friendly manner.\n
 * The tool is able to compare files not only on the source level, but compare shown text.\n
 * Output is one HTML file - revision of diferences input files.
 */

#include "lib/getopt.h"
#include "config.h"
#include "last_modification.h"

/* Filtres headers. */
#include "filter_delete_comment.h"
#include "filter_separate_tags.h"
#include "filter_delete_space.h"
#include "filter_change_specchar.h"
#include "filter_append_src.h"
#include "filter_delete_src.h"
#include "filter_change_src.h"
#include "filter_append_txt.h"
#include "filter_delete_txt.h"
#include "filter_change_txt.h"

/** Count of filters. */
#define COUNT_OF_FILTRES 10

/** List of command line options. */
static char const shortopts[] = "hl:fF:D:P:stdWLR:c:C:nO:xy";

static struct option long_options[] =
{
	{"help",               no_argument,       NULL, 'h'},
	{"log-level",          required_argument, NULL, 'l'},
	{"log-to-file",        no_argument,       NULL, 'f'},
	{"log-file-name",      required_argument, NULL, 'F'},
	{"use-diff",           required_argument, NULL, 'D'},
	{"use-patch",          required_argument, NULL, 'P'},
	{"compare-source",     no_argument,       NULL, 's'},
	{"compare-text",       no_argument,       NULL, 't'},
	{"debug",              no_argument,       NULL, 'd'},
	{"compare-lines",      no_argument,       NULL, 'W'},
	{"compare-words",      no_argument,       NULL, 'L'},
	{"revision-file-name", required_argument, NULL, 'R'},
	{"css",                required_argument, NULL, 'c'},
	{"css-file-name",      required_argument, NULL, 'C'},
	{"show-filters",       no_argument,       NULL, 'n'},
	{"turn-off-filters",   required_argument, NULL, 'O'},
	{"minimal",            no_argument,       NULL, 'x'},
	{"speed-large-files",  no_argument,       NULL, 'y'},
};

/** For holding information aboute input files. */
SIDE struct_left_side, struct_right_side;	/**< Allocate struct of comparation files. */
SIDE *left_side = &struct_left_side;	/**< Information about first (origin) input file. */
SIDE *right_side = &struct_right_side;	/**< Information about second (modife) input file. */

/** New typ struct included pointers to filtres function. */
typedef struct filter FILTER;

/** Struct of filters function for preprocessing and postprocessing function. */
struct filter
{
	int (*fce_filter)(SIDE*);		/**< Normalization function	(preprocessing). */
	int (*fce_re_filter)(SIDE*);	/**< Function after compare (postprocessing). */
	char filter_name[30];			/**< Name of filter. */
	compare_typ compare;			/**< Typ of comparation - (text|source|botuh). */
	int turn_off;					/**< For turning off filter. */
	int filter_can_off;				/**< Would be truning off. */
};

/** For each filtr this array holds pointer to filtres main function. */
FILTER filter_array[COUNT_OF_FILTRES];

/** Configuration of hole program. */
CONFIGURATION config;

/** Name of this programm. */
char *program_name;

/**
 * Saving pointers to filters to array struct.
 * @return (None).
 */
void load_filtres()
{
	/* Deleting comment. */
	filter_array[0].fce_filter = filter_delete_comment;
	filter_array[0].fce_re_filter = NULL;
	strcpy(filter_array[0].filter_name, "Delete comment");
	filter_array[0].compare = both;
	filter_array[0].turn_off = 0;
	filter_array[0].filter_can_off = 1;

	/* Separate HTML tags on separate lines. */
	filter_array[1].fce_filter = filter_separate_tags;
	filter_array[1].fce_re_filter = NULL;
	strcpy(filter_array[1].filter_name, "Separate HTML tags");
	filter_array[1].compare = both;
	filter_array[1].turn_off = 0;
	filter_array[1].filter_can_off = 0;

	/* Deleting extra white spaces. */
	filter_array[2].fce_filter = filter_delete_space;
	filter_array[2].fce_re_filter = NULL;
	strcpy(filter_array[2].filter_name, "Delete spaces");
	filter_array[2].compare = both;
	filter_array[2].turn_off = 0;
	filter_array[2].filter_can_off = 0;

	/* Change special chars to entites. */
    filter_array[3].fce_filter = filter_change_specchar;
    filter_array[3].fce_re_filter = NULL;
	strcpy(filter_array[3].filter_name, "Change special character");
	filter_array[3].compare = source;
	filter_array[3].turn_off = 0;
	filter_array[3].filter_can_off = 0;

	/* Modife of operation delete from patch file. Source level. */
	filter_array[4].fce_filter = NULL;
	filter_array[4].fce_re_filter = filter_delete_src;
	strcpy(filter_array[4].filter_name, "Delete blocks");
	filter_array[4].compare = source;
	filter_array[4].turn_off = 0;
	filter_array[4].filter_can_off = 0;

	/* Modife of operation append from patch file. Source level. */
	filter_array[5].fce_filter = NULL;
	filter_array[5].fce_re_filter = filter_append_src;
	strcpy(filter_array[5].filter_name, "Append blocks");
	filter_array[5].compare = source;
	filter_array[5].turn_off = 0;
	filter_array[5].filter_can_off = 0;

	/* Modife of operation change from patch file. Source level. */
	filter_array[6].fce_filter = NULL;
	filter_array[6].fce_re_filter = filter_change_src;
	strcpy(filter_array[6].filter_name, "Change blocks");
	filter_array[6].compare = source;
	filter_array[6].turn_off = 0;
	filter_array[6].filter_can_off = 0;
	
	/* Modife of operation delete from patch file. Shown text level. */
	filter_array[7].fce_filter = NULL;
	filter_array[7].fce_re_filter = filter_delete_txt;
	strcpy(filter_array[7].filter_name, "Delete blocks");
	filter_array[7].compare = text;
	filter_array[7].turn_off = 0;
	filter_array[7].filter_can_off = 0;

	/* Modife of operation append from patch file. Shown text level. */
	filter_array[8].fce_filter = NULL;
	filter_array[8].fce_re_filter = filter_append_txt;
	strcpy(filter_array[8].filter_name, "Append blocks");
	filter_array[8].compare = text;
	filter_array[8].turn_off = 0;
	filter_array[8].filter_can_off = 0;

	/* Modife of operation change from patch file. Shown text level. */
	filter_array[9].fce_filter = NULL;
	filter_array[9].fce_re_filter = filter_change_txt;
	strcpy(filter_array[9].filter_name, "Change blocks");
	filter_array[9].compare = text;
	filter_array[9].turn_off = 0;
	filter_array[9].filter_can_off = 0;
}

/**
 * Load configuration of programm.
 * Order of akcepting configuration:
 *	- first place where programm search is command line
 *	- if isn't on command line then searching in config file (CONFIG_FILE_NAME)
 *	- else in (config.h) the default configuration constants
 * @return (None).
 */
void load_config()
{
	int is_config_file = 1;

	/* Config file. Used struct SIDE for it. */
	SIDE cfg_file;

	/* For test of input. */
	int test_i = 0;

	/* For allocation of strings for inicialization from config file. */
	char *tmp_log_file_name;
	char *tmp_diff;
	char *tmp_patch;
	char *tmp_revision_name;
	char *tmp_css_name;

	/* Load default from constants from config.h. */
	config.debug = DEBUG;
	config.compare = COMPARE;
	config.compare_lvl = COMPARE_LVL;	
	config.log_level = LOG_LEVEL;
	config.log_to_file = LOG_TO_FILE;
	config.log_file_name = LOG_FILE_NAME;
	config.diff = DIFF;
	config.patch = PATCH;
	config.revision_name = NULL;
	config.log_file = NULL;
	config.diff_minimal = DIFF_MINIMAL;
	config.diff_speed_large_files = DIFF_SPEED_LARGE_FILES;
	config.css = CSS;
	config.css_name = CSS_NAME;

	/* Open config file. */
	if((cfg_file.temp_file_from = fopen(CONFIG_FILE_NAME, "r")) == NULL)
	{
		printf("Cannot open file '%s'... load configuration from command line then from default constants. \n", CONFIG_FILE_NAME);
		is_config_file = 0;
	}

	/* Log config from config file. */
	if(is_config_file)
	{
		/* Inicialization of struct - allocate strings. */
		if(!new_side(&cfg_file))
		{
			/* Read first character from input file bacause checking end of file. */
			cfg_file.character = getc(cfg_file.temp_file_from);

			while(cfg_file.character != EOF)
			{
				copy_word(&cfg_file);		/* Read word. */
				copy_whitespace(&cfg_file);	/* Read following white spaces. */

				if(cfg_file.word[0] != 0)
				{
					if(!strcmp(cfg_file.word,"DEBUG"))
					{
						copy_word(&cfg_file);
						config.debug = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.debug = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"COMPARE"))
					{
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.compare = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"DIFF_MINIMAL"))
					{
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.diff_minimal = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"DIFF_SPEED_LARGE_FILES"))
					{
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.diff_speed_large_files = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"COMPARE_LVL"))
					{
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.compare_lvl = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"LOG_LEVEL"))
					{
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);

						/* Test of input. */
						if(test_i > 0 && test_i < 5){
							config.log_level = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"LOG_TO_FILE"))
					{
						copy_word(&cfg_file);
						config.log_to_file = atoi(cfg_file.word);

						/* Test of input. */
						if(test_i == 0 || test_i == 1){
							config.log_to_file = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"LOG_FILE_NAME")){
						/* Copy value of variable. */
						copy_word(&cfg_file);
						copy_whitespace(&cfg_file);

						/* If isn't variable empty. */
						if(cfg_file.word[0] != 0){
							/* Alloc new tmp string. */
							tmp_log_file_name = (char*) malloc(strlen(cfg_file.word)+1);

							/* Copy to tmp string. */
							strcpy(tmp_log_file_name, cfg_file.word);

							/* Take pointer from tmp string. */
							config.log_file_name = tmp_log_file_name;

							/* Is logged to file. */
							config.log_to_file = 1;
						}
					}
					else if(!strcmp(cfg_file.word,"DIFF")){
						/* Copy value of variable. */
						copy_word(&cfg_file);
						copy_whitespace(&cfg_file);

						/* If isn't variable empty. */
						if(cfg_file.word[0] != 0){
							/* Alloc new tmp string. */
							tmp_diff = (char*) malloc(strlen(cfg_file.word)+1);

							/* Copy to tmp string. */
							strcpy(tmp_diff, cfg_file.word);

							/* Take pointer from tmp string. */
							config.diff = tmp_diff;
						}
					}
					else if(!strcmp(cfg_file.word,"PATCH")){
						/* Copy value of variable. */
						copy_word(&cfg_file);
						copy_whitespace(&cfg_file);

						/* If isn't variable empty. */
						if(cfg_file.word[0] != 0){
							/* Alloc new tmp string. */
							tmp_patch = (char*) malloc(strlen(cfg_file.word)+1);

							/* Copy to tmp string. */
							strcpy(tmp_patch, cfg_file.word);

							/* Take pointer from tmp string. */
							config.patch = tmp_patch;
						}
					}
					else if(!strcmp(cfg_file.word,"REVISION_NAME")){
						/* Copy value of variable. */
						copy_word(&cfg_file);
						copy_whitespace(&cfg_file);

						/* If isn't variable empty. */
						if(cfg_file.word[0] != 0){
							/* Alloc new tmp string. */
							tmp_revision_name = (char*) malloc(strlen(cfg_file.word)+1);

							/* Copy to tmp string. */
							strcpy(tmp_revision_name, cfg_file.word);

							/* Take pointer from tmp string. */
							config.revision_name = tmp_revision_name;
						}
					}
					else if(!strcmp(cfg_file.word,"CSS")){
						copy_word(&cfg_file);
						test_i = atoi(cfg_file.word);
						
						/* Test of input. */
						if(test_i == 1 || test_i == 2 || test_i == 3){
							config.css = atoi(cfg_file.word);
						}
					}
					else if(!strcmp(cfg_file.word,"CSS_NAME")){
						/* Copy value of variable. */
						copy_word(&cfg_file);
						copy_whitespace(&cfg_file);

						/* If isn't variable empty. */
						if(cfg_file.word[0] != 0){
							/* Alloc new tmp string. */
							tmp_css_name = (char*) malloc(strlen(cfg_file.word)+1);

							/* Copy to tmp string. */
							strcpy(tmp_css_name, cfg_file.word);

							/* Take pointer from tmp string. */
							config.css_name = tmp_css_name;
						}
					}
					else
					{
						/* Skip this line. */
						while((cfg_file.character = getc(cfg_file.temp_file_from)) != '\n' && cfg_file.character != EOF);
					}
				}
			}
		}
		fclose(cfg_file.temp_file_from);

		/* Deallocate memory. */
		free(cfg_file.word);
		free(cfg_file.white_space);
	}
}

/**
 * Set to config highlights tags depended on option from command line.
 * @param cmd_highlight Option for switching from command line.
 * @return (None).
 */
void load_highlight_tags(int cmd_highlight)
{
	/* Set highlighting tags. */
	config.hl.append_middle = APPEND_MIDDLE;
	config.hl.append_end = APPEND_END;
	config.hl.delete_middle = DELETE_MIDDLE;
	config.hl.delete_end = DELETE_END;
	config.hl.change_middle = CHANGE_MIDDLE;
	config.hl.change_end = CHANGE_END;
	config.hl.change_append_end = CHANGE_APPEND_END;
	config.hl.change_delete_end = CHANGE_DELETE_END;
	config.hl.blank_middle = BLANK_MIDDLE;
	config.hl.blank_end = BLANK_END;

	/*With css class name. */
	if(cmd_highlight == 2 || cmd_highlight == 3 ){
		config.hl.append_start = APPEND_START;
		config.hl.delete_start = DELETE_START;
		config.hl.change_start = CHANGE_START;
		config.hl.change_append_start = CHANGE_APPEND_START;
		config.hl.change_delete_start = CHANGE_DELETE_START;
		config.hl.blank_start = BLANK_START;
	}
	/* Style direct to tags. */
	else if(cmd_highlight == 1){
		config.hl.append_start = APPEND_START_DIRRECT;
		config.hl.delete_start = DELETE_START_DIRRECT;
		config.hl.change_start = CHANGE_START_DIRRECT;
		config.hl.change_append_start = CHANGE_APPEND_START_DIRRECT;
		config.hl.change_delete_start = CHANGE_DELETE_START_DIRRECT;
		config.hl.blank_start = BLANK_START_DIRRECT;
	}
}

/**
 * Generate name of revision file.
 * If is seted option config.revision_name then used else generate new name.
 * @param origin_file Name of origin file.
 * @return Revision name.
 */
char* generate_revision_name(char *origin_file)
{
	/* String leght of origin file name. */
	int strlen_origin = 0;

	/* Sighn if origin file contain ".html". */
	int html = 0;

	char* revision_name = NULL;

	if(config.revision_name == NULL || config.revision_name[0] == 0){
		strlen_origin = strlen(origin_file);

		/* Test if origin file contain ".html". */
		if(!strcmp(&origin_file[strlen_origin-5], ".html")){
			/* +4 because first "tmp_revision" and +1 because last character '\0'. */
			revision_name = (char*) malloc(strlen_origin+13+1);
			html = 1;
		}
		else{
			/* +5 because last ".html", +13 because first "tmp_revision_" and +1 because last character '\0'. */
			revision_name = (char*) malloc(strlen_origin+5+13+1);
			html = 0;
		}
		
		/* Test if was allocated. */
		if(revision_name  == NULL){
			return NULL;
		}

		/* Revision name starts same as origin input file. */
		strcpy(revision_name, "tmp_revision_");
		strcat(revision_name, origin_file);

		/* If not .html in the end of revision name added. */
		if(!html){
			strcat(revision_name, ".html");
		}

		return revision_name;
	}
	else{
		/* Add prefix "tmp_". */
		revision_name = (char*) malloc(strlen(config.revision_name)+1+4);

		/* Test if was allocated. */
		if(revision_name  == NULL){
			return NULL;
		}

		/* Create new name. */
		strcpy(revision_name, "tmp_");
		strcat(revision_name, config.revision_name);

		/* Deallocate old name. */
		free(config.revision_name);

		/* Repointer to new revision name. */
		config.revision_name = revision_name;

		return config.revision_name;
	}
}

/**
 * Write how to use this program
 * @param status Which param is require to write.
 * @return (None).
 */
void usage(int status)
{
	if (status != 0)	{
		fprintf (stderr, "Try `%s -h' for more information.\n", program_name);
	}
	else
	{
	  printf ("\n\
Usage: %s FILE1 FILE2 [OPTION]... \n", program_name);
      printf ("\n\
  Default comparation is on the source level.\n\
  -d --debug : don't delete temporary files using by programm\n\
  -D DIFF_NAME --use-diff=DIFF_NAME : name of external Diff programm (default \"diff.exe\")\n\
  -x --minimal : GNU Diff Performance Tradeoffs - diff to use a modified algorithm that sometimes produces a smaller set of differences\n\
  -y --speed-large-files : GNU Diff Performance Tradeoffs - speeds up the comparisons without changing the output; diff might produce a larger set of differences\n\
  -l x --log-level=X : level of logging (X = 0 to 4) (default 3)\n\
  -f --log-to-file : turn on logging to file (default off = to stderr)\n\
  -h --help : print this help\n\
  -F log_file_name --log-file-name=LOG_FILE_NAME : name of file for logging (default \"ladandiff.log\")\n\
  -P PATCH_NAME --use-patch=PATCH_NAME : name of external Patch programm (default \"patch.exe\")\n\
  -s --compare-source : comparation files on the source level\n\
  -t --compare-text : comparation files on the shown text level (default)\n\
  -L --compare-words : comparation files on line by line level (default)\n\
  -W --compare-lines : comparation files on word by word level\n\
  -R REVISION_NAME --revision-file-name=REVISION_NAME : name of revision file (default \"revision_of_(name_of_origin_file).html\")\n\
  -c X --css=X : css style is added to tags directly = 1 or eternal css = 2 or embedded = 3 (default embedded = 3)\n\
  -C CSS_FILE_NAME --css-file-name=CSS_FILE_NAME: name of external css style-sheet  (default \"ladandiff.css\")\n\
  -n --show-filters : show filtr for normalization with information which is able to turn off\n\
  -O X,Y,... --turn-off-filters=X,Y,... : turn off filtre(s) with number(s) X(Y,...)\n\n");
	}
}

/**
 * Write how to use this program
 * @return (None).
 */
void show_filtres()
{
	int i, y = 1;

	printf("\
  - level = comapration level with is used (shown text | source level | both)\n\
  - number = is number for turning off filter with option -O (use help -h)\n");
  

	printf("\nFiltres for preprocessing:\n");
	printf("       |  level |number|  filter name\n");
	for(i=0; i<COUNT_OF_FILTRES; i++)
	{
		if(filter_array[i].fce_filter != NULL)
		{
			printf("    %i. |", y);

			/* Type of comparation. */
			switch(filter_array[i].compare)
			{
				case text: printf(" text   |"); break;
				case source: printf(" source |"); break;
				case both: printf(" both   |"); break;
			}

			/* If would be turning off write number for option -o. */
			if(filter_array[i].filter_can_off){printf("  %i   |", i);}
			else{printf("      |");}

			/* Name of filter. */
			printf("  %s\n", filter_array[i].filter_name);

			y++;
		}
	}

	y = 1;

	printf("\nFiltres for postprocessing:\n");
	printf("       |  level |number|  filter name\n");
	for(i=COUNT_OF_FILTRES-1; i>-1; i--)
	{
		if(filter_array[i].fce_re_filter != NULL)
		{
			printf("    %i. |", y);

			/* Type of comparation. */
			switch(filter_array[i].compare)
			{
				case text: printf(" text   |"); break;
				case source: printf(" source |"); break;
				case both: printf(" both   |"); break;
			}
			
			/* If would be turning off write number for option -o. */
			if(filter_array[i].filter_can_off){printf("  %i   |", i);}
			else{printf("      |");}


			/* Name of filter. */
			printf("  %s\n", filter_array[i].filter_name);

			y++;
		}
	}
} 


/**
 * Clean programm if is fatal error or end of programm.
 * The programm will close yourself if is fatal error then have to clean hard drive from temp file.
 * If programm reach end of programm have to free alloc memmory.
 * @return (None).
 */
void clean_programm()
{
	/* Close log file. */
	if(config.log_to_file)
	{
		fclose(config.log_file);
	}
}

/**
 * Load input parameters from command line.
 * @param argc Count of arguments from command line.
 * @param argv Array of arguments.
 * @return int Sign if end the programm (0 = end; 1 = continue).
 */
int load_cmd_parameters(int argc, char *argv[])
{
	/* For testing of input options from command line. */
	int test_i = 0;
	
	/* For reading singel input parameters. */
	int option_char;

	/* Array of number of turning off filtres. */
	char *turn_off_filtres[COUNT_OF_FILTRES];

	/* Forcycle for reading list of turning off filtres. */
	int loop;

	/* Reading of input parameters and save configuration from command line. */
	while(option_char = getopt_long(argc, argv, shortopts, long_options, NULL), option_char != EOF)
	{
		switch(option_char)
		{
			/* Explain options. */
			case 'h':
				usage(0);
				return 0;
				break;
			
			/* Source level comparation. */
			case 's':
				config.compare = source;
				break;
			
			/* Shown text level comparation. */
			case 't':
				config.compare = text;
				break;

			/* Line by line comparation. */
			case 'L':
				config.compare_lvl = lines;
				break;
			
			/* Words by words comparation. */
			case 'W':
				config.compare_lvl = words;
				break;

			/* Debug - don't erase temp file. */
			case 'd':
				config.debug = 1;
				break;

			/* Log level. */
			case 'l':
				test_i = atoi(optarg);

				/* Test of input. */
				if(test_i >= 0 && test_i <= 4){
					config.log_level = atoi(optarg);
				}
				break;

			/* Log to file. */
			case 'f':
				config.log_to_file = 1;
				break;

			/* Log to file and log filename. */
			case 'F':
				config.log_to_file = 1;
				config.log_file_name = optarg;
				break;

			/* Name of external diff programm. */
			case 'D':
				config.diff = optarg;
				break;

			/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
			case 'x':
				config.diff_minimal = 1;
				break;

			/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
			case 'y':
				config.diff_speed_large_files = 1;
				break;

			/* Name of external patch programm. */
			case 'P':
				config.patch = optarg;
				break;

			/* Name of revision. */
			case 'R':
				config.revision_name = optarg;
				break;

			/* Highlights mode - css style is added to tags directly = 1 or eternal css = 2 or embedded = 3. */
			case 'c':
				test_i = atoi(optarg);

				/* Test of input. */
				if(test_i == 1 || test_i == 2 || test_i == 3){
					config.css = atoi(optarg);
				}
				break;

			/*  Name of external css style sheet. */
			case 'C':
				config.css = 2;
				config.css_name = optarg;
				break;

			/* Shows filters. */
			case 'n':
				show_filtres();
				return 0;
				break;

			/* Turn off filters. */
			case 'O':
				turn_off_filtres[0] = strtok(optarg, ",");

				/* No input number. */
				if(turn_off_filtres[0]==NULL){break;}
				else
				{
					/* Revert to integer. */
					test_i = atoi(turn_off_filtres[0]);

					/* Test range of integer. */
					if(test_i >= 0 && test_i < COUNT_OF_FILTRES){
						
						/* Turn off filter if can be disabled. */
						if(filter_array[test_i].filter_can_off){
							filter_array[test_i].turn_off = 1;
						}
					}
				}

				for(loop=1; loop<COUNT_OF_FILTRES; loop++)
				{
					turn_off_filtres[loop] = strtok(NULL, ",");

					if(turn_off_filtres[loop] == NULL){break;}
					else{
						/* Revert to integer. */
						test_i = atoi(turn_off_filtres[loop]);

						/* Test range of integer. */
						if(test_i >= 0 && test_i < COUNT_OF_FILTRES){

							/* Turn off filter if can be disabled. */
							if(filter_array[test_i].filter_can_off){
								filter_array[test_i].turn_off = 1;
							}
						}
					}
				}
				break;

			default:
				usage(2);
		}
	}
	return 1;
}

/**
 * Preprocesing of compare.
 * Every filter make new file which is used for its output.\n
 * Output from previous filter is input for folowing filter.\n
 * Output from the last filter is normalized file.
 * @param side Struct of side.
 * @return int Sign if preprocesing was ok.
 */
int filtres(SIDE *side){
	/* For moving betwen filtres. */
	int i;
    
	/* Sign if first run of forcycle of filtres. */
	int first = 1;

	program_log(4, "Open file '%s' as input for preprocess filter.", side->input_file_name);

	/* Open input file. */
	if((side->temp_file_to = fopen(side->input_file_name, "r")) == NULL)
	{
		program_log(1, "Cannot open file '%s' for preprocessing.", side->input_file_name);
		return 1;
	}

	/* Inicialization of struct - allocate strings. */
	if(new_side(side)){
		return 1;
	}
	
	/* Process normalization filtres from first to last. */
	for(i=0; i<COUNT_OF_FILTRES; i++)
	{
		/* If the filter is implemented and is for actual comparation. */
		if(filter_array[i].fce_filter != NULL && (filter_array[i].compare == config.compare || filter_array[i].compare == both))
		{
			if(!filter_array[i].turn_off)
			{
				/* If is first run of cycle then input file is file from command line - not tmpfile*/
				if(first){
					program_log(4, "File '%s' is now input for next preprocess filter.", side->input_file_name);
				}
				else
				{
					program_log(4, "File '%s' is now input for next preprocess filter.", side->temp_filename_to);

					/* Previous output file is now input file for next filter. */
					side->temp_filename_from = side->temp_filename_to;

					/* Move pointer file to begening of file because now is in the end of file from previous filter. */
					fseek(side->temp_file_to, 0, SEEK_SET);
				}

				/* Previous output file is now input file for next filter. */
				side->temp_file_from = side->temp_file_to;
			
				/* Make name of new temporery file as output for next filter. */
				side->temp_filename_to = my_tempnam(NULL);

				program_log(4, "Open file '%s' as output for next preprocess filter.", side->temp_filename_to);

				/* Open temp file as input for next filter. */
				if((side->temp_file_to = fopen(side->temp_filename_to, "w+")) == NULL)
				{
					program_log(1, "Cannot open file '%s' for preprocessing.", side->temp_filename_to);
					return (1+i);
				}

				/* Read first character from input file bacause checking end of file. */
				side->character = getc(side->temp_file_from);

				program_log(3, "Process filter: %s.", filter_array[i].filter_name);

				/* Process filter. */
				if(filter_array[i].fce_filter(side)){
					return 1;
				}

				/* Close previous temp file which was used now as an input. */
				if(fclose(side->temp_file_from)){
					if(first){
						program_log(2, "Cannot close file '%s' from preprocessing.", side->input_file_name);
					}else{
						program_log(2, "Cannot close file '%s' from preprocessing.", side->temp_filename_from);
					}
				}

				/* Erase previous temp file which was used now as an input if wasnt origin or modife file (input files to this programm). */
				if(!first && !config.debug){
					remove(side->temp_filename_from);
				}

				if(first){first=0;}
			}
			else{
				program_log(3, "Process filter: %s [IS DISABLED].", filter_array[i].filter_name);
			}
		}
	}

	/* Close output file - normalized file. */
	fclose(side->temp_file_to);

	/* Save name of normalized file to right variable. */
	side->norm_filename = (char*) malloc(strlen(side->temp_filename_to)+1);
	strcpy(side->norm_filename, side->temp_filename_to);

	/* Everithing was ok. */
	return 0;
}

/**
 * Postpocesing of compare.
 * Makeing of new patch file for revision.\n
 * "Temp_filenam_(to|from)" in SIDE is used for temp file of new patch file.\n
 * Output from last filter is output patch file.
 * @param side Struct of side.
 * @return int Sign if postprocesing was ok.
 */
int re_filtres(SIDE *side){
	/* For moving betwen filtres. */
	int i;

	/* Sign of first run in for-cycle for check if patch file is empty. */
	int first = 1;

	/* Open input file. */
	if((side->temp_file_to = fopen(side->temp_filename_to, "r")) == NULL)
	{
		program_log(1, "Cannot open input file for preprocessing '%s'.", side->temp_filename_to);
		return 1;
	}

	/* Process normalization filtres from last to first. Revers order unlike preprocessing. */
	for(i=COUNT_OF_FILTRES-1; i>-1; i--)
	{
		/* If the filter is implemented. */
		if(filter_array[i].fce_re_filter != NULL && (filter_array[i].compare == config.compare || filter_array[i].compare == both))
		{
			if(!filter_array[i].turn_off)
			{
				program_log(4, "File '%s' is now input for next postpreprocess filter.", side->temp_filename_to);

				/* Previous output file is now input file for next filter. */
				side->temp_filename_from = side->temp_filename_to;
				side->temp_file_from = side->temp_file_to;

				/* Move pointer file to begening of file because now is in the end of file from previous filter. */
				fseek(side->temp_file_from, 0, SEEK_SET);

				/* Read first character from input file bacause checking end of file. */
				side->character = getc(side->temp_file_from);

				/* Check if patch file is empty = no changes in file. */
				if(first && side->character == EOF){
					fclose(side->temp_file_to);
					return 3;
				}
				first = 0;

				/* Make name of new temporery file as output for next filter. */
				side->temp_filename_to = my_tempnam(NULL);

				program_log(4, "Open file '%s' as input for next postpreprocess filter.", side->temp_filename_to);
				
				/* Open temp file as input for next filter. */
				if((side->temp_file_to = fopen(side->temp_filename_to, "w+")) == NULL){
					program_log(1, "Cannot open output file for preprocessing '%s'.", side->temp_filename_to);
					return 1;
				}

				program_log(3, "Process filter: %s.", filter_array[i].filter_name);

				/* Process filter. */
				filter_array[i].fce_re_filter(side);

				/* Close previous temp file which was used now as an input. */
				if(fclose(side->temp_file_from)){
					program_log(2, "Cannot close input file for preprocessing '%s'.", side->temp_filename_to);
				}

				/* Erase previous temp file which was used now as an input. */
				if(!config.debug){
					remove(side->temp_filename_from);
				}
			}
			else{
				program_log(3, "Process filter: %s [IS DISABLED].", filter_array[i].filter_name);
			}
		}
	}

	/* Close output file - new patch file. */
	fclose(side->temp_file_to);

	/* Everithing was ok. */
	return 0;
}

/** Main function. */
int main(int argc, char *argv[])
{
	/* Output patch file from diff which is modife. */
	char *patch_file_name;

	/* Name of revision file. */
	char *revision_name;
	
	/* Command for external diff. */
	char *command_diff;

	/* Leght of command diff - command lenhght is variable debend on input option -d and -b. */
	int command_diff_lenght;

	/* Command for external patch. */
	char *command_patch;

	/* Sign if the file are same. */
	int no_changes = 0;

	/* Revision file - use only if file are same. */
	FILE *revision = NULL;

	/* For checking if exists diff and patch. */
	FILE *exists_diff, *exists_patch;

	/* Save name of this program. */
	program_name = argv[0];

	/* Log to consol (isnt open log file). */
	printf("Start of loading configuration.\n");	

	/* Load config from default constant and then from config file. */
	load_config();

	/* Load filtres function to array. */
	load_filtres();

	/* Load input parameters. */
	if(!load_cmd_parameters(argc, argv)){return 0;}

	/* Load to config highlight tags depended on comparation level. */
	load_highlight_tags(config.css);

	/* Log to consol (isnt open log file). */
	printf("End of loading configuration.\n");

	/* Open log file if is required. */
	if(config.log_to_file)
	{
		if((config.log_file = fopen(config.log_file_name, "w+")) == NULL){
			config.log_to_file = 0;
			printf("Cannot open file '%s' for logging. Programm progress is logging to terminal.\n", config.log_file_name);
		}
		else{
			printf("Programm progress is logging to file '%s'.\n", config.log_file_name);
		}
	}
	printf("\n");

	program_log(0, "START OF PROGRAMM: %s.", program_name);
	if(config.compare == source){
		program_log(0, " - COMPARATION ON SOURCE LEVEL -\n", "");
	}else{
		program_log(0, " - COMPARATION ON SHOWN TEXT LEVEL -\n", "");
	}

	/* Test if exists diff. If not then close program. */
	if((exists_diff = fopen(config.diff, "r" )) == NULL )
	{
		program_log(1, "Diff: \"%s\" doesn't exists.", config.diff);
		clean_programm();
		return 1;
	}
	else{fclose(exists_diff);}
	
	/* Test if exists patch. If not then close program. */
	if((exists_patch = fopen(config.patch, "r" )) == NULL )
	{
		program_log(1, "Patch: \"%s\" doesn't exists.", config.patch);
		clean_programm();
		return 1;
	}
	else{fclose(exists_patch);}

	/* Error - missing input files. Optind = count of parameters (counted from 2: no parameter optind=1). */
	if(optind + 2 != argc)
	{
		program_log(1, "Missing input files.", "");
		usage(2);

		/* Before close of programm have to clean it up. */
		clean_programm();

		return 1;
	}

	/* Read name of origin file. */
	left_side->input_file_name = argv[optind];
	optind++;
	
	/* Read name of modife file. */
	right_side->input_file_name = argv[optind];
	optind++;		

	program_log(3, "Start of preprocessing of origin file '%s'.", left_side->input_file_name);

	/* Preprocessing of origin file. */
	if(filtres(left_side))
    {
		program_log(1, "Fail in preprocessing file '%s'.", left_side->temp_filename_to);

		/* Before close of programm have to clean it up. */
		clean_programm();

        return 1;
    }

	program_log(4, "Output normalized file from preprocessing is '%s'.", left_side->norm_filename);
	program_log(3, "End of preprocessing of origin file '%s'.\n", left_side->input_file_name);

	program_log(3, "Start of preprocessing of modife file '%s'.", right_side->input_file_name);

	/* Preprocessing of modife file. */
	if(filtres(right_side))  //modifikovany
	{
		program_log(1, "Fail in preprocessing file '%s'.", right_side->temp_filename_to);
		
		/* Before close of programm have to clean it up. */
		clean_programm();
        
		return 1;
    }

	program_log(4, "Output normalized file from preprocessing is '%s'.", right_side->norm_filename);
	program_log(3, "End of preprocessing of modife file '%s'.\n", right_side->input_file_name);

		/* File name of patch file from diff. */
		patch_file_name = my_tempnam(NULL);

		/* Command for diff. */
		command_diff_lenght = strlen(config.diff)+
								strlen(left_side->temp_filename_to)+
								strlen(right_side->temp_filename_to)+
								strlen(patch_file_name)+6;
		
		if(config.diff_minimal){command_diff_lenght += 3;}
		if(config.diff_speed_large_files){command_diff_lenght += 20;}

		command_diff = (char*) malloc(command_diff_lenght);

		/* Making command of diff. */
		strcpy(command_diff, config.diff);
		strcat(command_diff, " ");
		strcat(command_diff, left_side->temp_filename_to);	/* Origin normalizated file. */
		strcat(command_diff, " ");
		strcat(command_diff, right_side->temp_filename_to);	/* Modife normalizated file. */
		
		if(config.diff_minimal){
			strcat(command_diff, " -d");
		}
		
		if(config.diff_speed_large_files){
			strcat(command_diff, " --speed-large-files");
		}
		
		strcat(command_diff, " > ");
		strcat(command_diff, patch_file_name);

	program_log(3, "Execute diff on normalized files: '%s'.", command_diff);

	/* Execute diff on normalized file. */
	system(command_diff);

	free(command_diff);

		/* For postprocessing are the temp file used for modifing patchfile. */
		strcpy(left_side->temp_filename_to, patch_file_name);
		strcpy(right_side->temp_filename_to, patch_file_name);

	program_log(3, "Start of postprocessing (make patch file for revision).", "");

	/* Postprocessing - making new patch file. */
	no_changes = re_filtres(left_side);

	/* Error in postprocessing. */
	if(no_changes && no_changes != 3)
	{
		program_log(1, "Fail in preprocessing file '%s'.", left_side->temp_filename_to);
		
		/* Before close of programm have to clean it up. */
		clean_programm();
	        
		return 1;
    }

	program_log(3, "End of postprocessing.\n", "");

	/* Generate name of revision file. */
	revision_name = generate_revision_name(left_side->input_file_name);

	/* Diferences in input file. */
	if(no_changes != 3){
		/* Command for patch - making output revision. */
		command_patch = (char*) malloc(
								strlen(config.patch)+
								strlen(left_side->norm_filename)+
								strlen(left_side->temp_filename_to)+
								strlen(revision_name)+10);

		strcpy(command_patch, config.patch);
		/* Work silently, unless an error occurs. */
		strcat(command_patch, " -s -o ");
		strcat(command_patch, revision_name);
		strcat(command_patch, " ");
		strcat(command_patch, left_side->norm_filename);
		strcat(command_patch, " ");
		strcat(command_patch, left_side->temp_filename_to);

		program_log(3, "Execute patch on normalized file and maked patch file: '%s'.\n", command_patch);

		/* Execut patch on origin normalized file and modife patch file. */
		system(command_patch);

		free(command_patch);

		program_log(3, "Final modification of revision file.", "");

		/* Last modification of final revision. */
		if(last_modification(revision_name))
		{
			program_log(1, "Error in last modification of revision file.", "");

			clean_programm();

			return 1;
		}

		program_log(3, "End of final modification of revision file.\n", "");
	}
	/* No diferences in input file. */
	else
	{
		program_log(0, "NO DIFERENCES IN INPUT FILES.", "");

		/* Make revision file with information that file are same. Open temp revision file. */
		if((revision = fopen(revision_name, "w+")) == NULL){
			program_log(2, "Cannot open revision file '%s'.", revision_name);
		}
		else
		{
			fprintf(revision, "NO DIFERENCES IN INPUT FILES.");
		}
	}

	free(revision_name);

	if(!config.debug)
	{
		program_log(4, "Deleting origin normalized files '%s'.", left_side->norm_filename);

		/* Deleting of normalized origin file. */
		if(remove(left_side->norm_filename) != 0){
			program_log(2, "Cannot delete file '%s'.", left_side->norm_filename);
		}

		program_log(4, "Deleting modife normalized files '%s'.", right_side->norm_filename);

		/* Deleting of normalized modife file. */
		if(remove(right_side->norm_filename) != 0){
			program_log(2, "Cannot delete file '%s'.", right_side->norm_filename);
		}

		program_log(4, "Deleting patch file (maked for revision) '%s'.", patch_file_name);

		/* Delting modife patchfile. */
		if(remove(left_side->temp_filename_to) != 0){
			program_log(2, "Cannot delete file '%s'.", patch_file_name);
		}
	}

	program_log(0, "\nEND OF PROGRAMM.", "");

	/* Before end of programm have to clean it up. */
	clean_programm();

	/* Everithing was ok. */
	return 0;
}
