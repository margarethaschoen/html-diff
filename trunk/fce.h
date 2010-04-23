/** 
 * @file fce.h
 * @brief Header for fce.c (Library of function for program).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include <ctype.h>
#include "config.h"

/* Make avalible configuration. */
extern CONFIGURATION config;

/* Log message to standard input (or to file) depend on loging level. */
void program_log(int, char *, char *);

/* Set default value to struct SIDE. */
int new_side(SIDE *);

/* Read sequence of white-space from file. */
int copy_whitespace(SIDE *);

/* Read sequence of non-white-space (word) from file. */
int copy_word(SIDE *);

/* Chang string to lower-case. */
char* strtolower(char *);

/* Test tag if is watched it's changing. */
int watched_tag(char *);

/* Parse input string to operation sturct. */
OPERATION take_op(char *);

/* Cut first sequence of number from input string. */
int take_int(char **);

/* Test if character is number. */
int is_digit(char);

/* Generate path and temporary file name. */
char *my_tempnam(char *);
