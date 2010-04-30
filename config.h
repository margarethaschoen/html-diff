/** 
 * @file config.h
 * @brief Default configuration and main stuct for programm
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Order of akcepting configuration:
 *	- first place where programm search is command line
 *	- if isn't on command line then searching in config file (CONFIG_FILE_NAME)
 *	- else in (config.h) the default configuration constants
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef CONFIG
#define CONFIG /**< For only one include to programm. */

/* Properties which user can change. */

	/** Name of configuration file where is initialization. */
	#define CONFIG_FILE_NAME "ladandiff.ini"

	/** For erase all tepmorary file which are maded during process programm. */
	#define DEBUG 0

	/** Comapration level - base on source level (0) or shown text level (1). */
	#define COMPARE text

	/** Comparation level - line by line (0) or word by word comparation (1). */
	#define COMPARE_LVL words

	/** Logging level - mean what is logged (high = more log information). Range: 0-4. */
	#define LOG_LEVEL 3

	/** Sign if log write to file. */
	#define LOG_TO_FILE 0

	/** Name file for logging - is ignored if LOG_TO_FILE is desabled. */
	#define LOG_FILE_NAME "ladandiff.log"

	/** Name of diff program - (GNU Diff). */
	#define DIFF "diff.exe"

	/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
	#define DIFF_MINIMAL 0
	
	/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
	#define DIFF_SPEED_LARGE_FILES 0

	/** Name of patch program - (GNU Patch). */
	#define PATCH "patch.exe"

	/** Highlights mode - css style is added to tags directly = 1 or eternal css = 2 or embedded = 3. */
	#define CSS 3

	/** Name of external css style sheet. */
	#define CSS_NAME "ladandiff.css"	

/* Properties which user can't change. */

	/** Base size for allocation all strings. */
	#define BASE_SIZE_STRINGS 8

/** 
 * Struct of information about input files.
 *	- include variables for reading from input file
 *	- temporary files for preprocessing and postprocessing
 */
typedef struct side
{
	char *temp_filename_from;	/**< Temporary file name for reading - this file is normaliziting. */
	FILE *temp_file_from;		/**< Temporary file for reading - this file is normaliziting. */

	char *temp_filename_to;		/**< Temporary file name for writing - into this file is saving normalize file. */
	FILE *temp_file_to;			/**< Temporary file for writing - into this file is saving normalize file. */

	char *input_file_name; 		/**< Name of input file. */
	char *norm_filename;		/**< Name of final normalized file. */

	int character;				/**< For reading character from file. */
	char *word;					/**< For reading word from file. Word = sequence non-white-space characters. */
	char *white_space;			/**< For reading white-spaces from file. White-spaces = sequence white-space characters. */

	int size_word;				/**< Size of maximal reading word. This size is doubling if is reading bigger word.*/
	int size_white_space;		/**< Same case as variable size_word, but instead word is white-space. @see size_word.*/
} SIDE;

/** 
 * Struct of information about one operation from patch file.
 */
typedef struct operation
{
	int x1;			/**< The first number of line (range) before operation. */
	int x2;			/**< The second number of range before operation. */

	/** The operation.
	 * - a: apend
	 * - d: delete
	 * - c: change
	 */
	char operation;

	int y1;			/**< The first number of line (range) after operation. */
	int y2;			/**< The second number of range after operation. */
} OPERATION;

/** Types of level of comparation - source level or shown text level. */
typedef	enum {source, text, both} compare_typ;

/** Types of level of comparation - line level comparation or word level comparation. */
typedef	enum {lines, words} compare_level;

/** Struct of highlighting border tags. */
typedef struct hgh
{
	char *append_start;			/**< Hihlight tag for append. Source level comparation. */
	char *append_middle;		/**< Hihlight tag for append. Source level comparation. */
	char *append_end;			/**< Hihlight tag for append. Source level comparation. */

	char *delete_start;			/**< Hihlight tag for delete. Source level comparation. */
	char *delete_middle;		/**< Hihlight tag for delete. Source level comparation. */
	char *delete_end;			/**< Hihlight tag for delete. Source level comparation. */

	char *change_start;			/**< Hihlight tag for change. Source level comparation. */
	char *change_middle;		/**< Hihlight tag for change. Source level comparation. */
	char *change_end;			/**< Hihlight tag for change. Source level comparation. */

	char *blank_start;			/**< Hihlight tag for blank. Source level comparation. */
	char *blank_middle;			/**< Hihlight tag for blank. Source level comparation.  */
	char *blank_end;			/**< Hihlight tag for blank. Source level comparation.  */

	char *change_append_start;	/**< Hihlight tag for word change. Source and text level comparation. */
	char *change_append_end;	/**< Hihlight tag for word change. Source and text level comparation. */
	char *change_delete_start;	/**< Hihlight tag for word change. Source and text level comparation. */
	char *change_delete_end;	/**< Hihlight tag for word change. Source and text level comparation. */
} HGHLGH;

/** 
 * Struct of configure information.\n
 * (reading from command line then from config file then default value from constants).
 */
typedef struct cfg
{
	/** For erase all tepmorary file which are maded during process programm. */
	int debug;

	/** Logging level - mean what is logged (high = more log information). */
	int log_level;

	/** Sign if log write to file . */
	int log_to_file;

	/** Name file for logging - is ignored if log_to_file is desabled. */
	char *log_file_name;

	/** Log file. */
	FILE *log_file;

	/** Name of diff program - (GNU Diff). */
	char *diff;

	/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
	int diff_minimal;

	/** GNU Diff Performance Tradeoffs. http://www.gnu.org/software/diffutils/manual/html_mono/diff.html.gz#diff%20Performance. */
	int diff_speed_large_files;

	/** Name of patch program - (GNU Patch). */
	char *patch;

	/** Name of revision. */
	char *revision_name;

	/** Level of comparation - source level or shown text level. */
	compare_typ compare;

	/** Level of comparation - lines level or words level. */
	compare_level compare_lvl;

	/** Highlights mode - css is input to tags or embbaded or external style-sheet. */
	int css;

	/** Name of external css style sheet. */
	char *css_name;

	/** Highlights borders. */
	HGHLGH hl;

} CONFIGURATION;

#define APPEND_START_DIRRECT "<div style='padding-left: 5px; background-color: #99FF66; border-left: solid 7px #33CC33; width: 800px; font-size: 11pt;'>" /**< Default highlite for append block - start line. Added to begining of line. */
#define APPEND_START "<div class=\"LadanDiffAppend\">" /**< Default highlite for append block - start line. Added to begining of line. */
#define APPEND_MIDDLE "#A#" /**< Sign for recognize middle line of append block. Added to begining of line. */
#define APPEND_END "</div>" /**< Default highlite for append block - ended line. Added to end of line. */

#define DELETE_START_DIRRECT "<div style='padding-left: 5px; background-color: #FA8072; border-left: solid 7px #DC143C; width: 800px; font-size: 11pt;'>" /**< Default highlite for delete block - start line. Added to begining of line. */
#define DELETE_START "<div class=\"LadanDiffDelete\">" /**< Default highlite for delete block - start line. Added to begining of line. */
#define DELETE_MIDDLE "#D#" /**< Sign for recognize middle line of delete block. Added to begining of line. */
#define DELETE_END "</div>" /**< Default highlite for delete block - ended line. Added to end of line. */

#define CHANGE_START_DIRRECT "<div style='padding-left: 5px; background-color: #B691D6; border-left: solid 7px #8A2BE2; width: 800px; font-size: 11pt;'>" /**< Default highlite for change block - start line. Added to begining of line. */
#define CHANGE_START "<div class=\"LadanDiffChange\">" /**< Default highlite for change block - start line. Added to begining of line. */
#define CHANGE_MIDDLE "#C#" /**< Default highlite for change block - line middle of block. Added to begining of line. */
#define CHANGE_END "</div>" /**< Default highlite for change block - ended line. Added to end of line. */

#define CHANGE_APPEND_START_DIRRECT "<span style='background-color: #99FF66;'>" /**< Default highlite for append words in changed blocks. Added to begining of appended word blocks. */
#define CHANGE_APPEND_START "<span class=\"LadanDiffChangeAppend\">" /**< Default highlite for append words in changed blocks. Added to begining of appended word blocks. */
#define CHANGE_APPEND_END "</span>" /**< Default highlite for append words in changed block - ended line. Added to end of appended word blocks. */
#define CHANGE_DELETE_START_DIRRECT "<span style='background-color: #FA8072;'>" /**< Default highlite for delete words in changed blocks. Added to begining of deleted word blocks. */
#define CHANGE_DELETE_START "<span class=\"LadanDiffChangeDelete\">" /**< Default highlite for delete words in changed blocks. Added to begining of deleted word blocks. */
#define CHANGE_DELETE_END "</span>" /**< Default highlite for delete words in changed block - ended line. Added to end of deleted word blocks. */

#define BLANK_START_DIRRECT "<div style='padding-left: 5px; background-color: #B0C4DE; border-left: solid 7px #0000FF; width: 800px; font-size: 11pt;'>" /**< Default highlite for append block - start line. Added to begining of line. */
#define BLANK_START "<div class=\"LadanDiffBlank\">" /**< Default highlite for append block - start line. Added to begining of line. */
#define BLANK_MIDDLE "" /**< Default highlite for append block - line middle of block. Added to begining of line. */
#define BLANK_END "</div>" /**< Default highlite for append block - ended line. Added to end of line. */


#define CSS_STYLE "\
<style type=\"text/css\">\n\
<!--\n\
	.LadanDiffAppend,.LadanDiffDelete,.LadanDiffChange,.LadanDiffBlank		{padding-left: 5px; width: 800px; font-size: 11pt;}\n\
	.LadanDiffAppend		{background-color: #99FF66; border-left: solid 7px #33CC33;}\n\
	.LadanDiffDelete		{background-color: #FA8072; border-left: solid 7px #DC143C;}\n\
	.LadanDiffChange		{background-color: #B691D6; border-left: solid 7px #8A2BE2;}\n\
	.LadanDiffChangeAppend	{background-color: #99FF66;}\n\
	.LadanDiffChangeDelete	{background-color: #FA8072;}\n\
	.LadanDiffBlank			{background-color: #B0C4DE; border-left: solid 7px #0000FF;}\n\
	-->\n\
</style>\n" /**< Css style for classes. */

#define CSS_EXTERN_STYLE_START "\
<link href=\"" /**< Prefix of extern css style for classes. */

#define CSS_EXTERN_STYLE_END "\
\" rel=\"stylesheet\" type=\"text/css\"/>\n" /**< Postfix css style for classes. */


#endif
