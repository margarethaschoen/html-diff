/** 
 * @file fce.c
 * @brief Library of useful function.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/**
 * Log message to standard input (or to file) depend on loging level.
 * @param level Loging level of message.
 * @param message_str Massage with max one specifier (as in printf - typ identifier of next string).
 * @param message_var String which is placed to massage instead specifier.
 * @return (None).
 */
void program_log(int level, char *message_str, char *message_var)
{
	char prefix_level[13];

	switch(level)
	{
		/* Log without prefix. */
		case 0:
			strcpy(prefix_level, "");
			break;
		/* Errors which ended program. */
		case 1:
			strcpy(prefix_level, "Error:      ");
			break;
		/* Errors when the programm still runnig. */
		case 2:
			strcpy(prefix_level, " Warning:   ");
			break;
		/* When program do operation. For example start filter, ended filter, compare files... */
		case 3:
			strcpy(prefix_level, "  Notice:   ");
			break;
		/* When program do smaller operation. For example open file, close file, reading from file... */
		case 4:
			strcpy(prefix_level, "   Message: ");
			break;
	}

	/* If wanted this message log level. */
	if(level <= config.log_level)
	{
		/* Write to file. */
		if(config.log_to_file)
		{
			fprintf(config.log_file, "%s", prefix_level);
			fprintf(config.log_file, message_str, message_var);
			fprintf(config.log_file, "\n");
		}
		/* Write to error output. */
		else
		{
			fprintf(stderr, "%s", prefix_level);
			fprintf(stderr, message_str, message_var);
			fprintf(stderr, "\n");
		}
	}
}

/**
 * Set default value to struct SIDE
 * @param side Side which is seted.
 * @return (None).
 */
int new_side(SIDE *side)
{
	/* Set default size word and spaces. */
	side->size_word = BASE_SIZE_STRINGS;		/* Prime size for reading word which is doubled when is necessary. */
	side->size_white_space = BASE_SIZE_STRINGS;	/* Prime size for reading white spaces which is doubled when is necessary. */

	/* Alocation of strings for reading from file. */
	side->word = (char*) malloc(side->size_word);
	if(side->word == NULL){
		return 1;
	}

	side->white_space = (char*) malloc(side->size_white_space);
	if(side->word == NULL)
	{
		free(side->word);
		return 2;
	}

	return 0;
}

/**
 * Read sequence of white-space from file.
 * @param side Struct of side.
 * @return 1 - error in realloc, 0 - no error.
 */
int copy_whitespace(SIDE *side)
{
	/* For move in string side->word. */
	int i = 0;

	/* While is reading white-space. */
	while(isspace(side->character))
	{
		/* Save character to side->white_space. The character is saved from inicialization in main.c. */
		side->white_space[i] = (char) side->character;

		/* Read character from file. */
		side->character = getc(side->temp_file_from);

		i++;

		/* Dynamic resize of size reading white-space if bigger then actual size white-space. */
		if(i == side->size_white_space)
		{
			/* Size white-space is doubled. */
			side->size_white_space *= 2;

			/* Realloc of side->white_space. */
			if((side->white_space = realloc(side->white_space,side->size_white_space)) == NULL){
				printf("Error: Chyba reallocu retezce bilych znaku...");
				return 1;
			}
		}
	}

	/* Ended the white-space. */
	side->white_space[i] = 0;

	return 0;
}

/**
 * Read sequence of non-white-space (word) from file.
 * @param side Struct of side.
 * @return 1 - error in realloc, 0 - no error.
 */
int copy_word(SIDE *side)
{
	/* For move in string side->word. */
	int i = 0;

	/* If isn't end of file or isnt white space, read character from input file and save to side->word. */
	while(side->character != EOF && !isspace(side->character))
	{
		/* Save character to side->word. The character is saved from inicialization in main.c. */
		side->word[i] = (char) side->character;
		
		/* Read character from file. */
		side->character = getc(side->temp_file_from);

		i++;

		/* Dynamic resize of size reading word if bigger then actual size word. */
		if(i==side->size_word)
		{
			/* Size word is doubled. */
			side->size_word *= 2;

			/* Realloc of side->word. */
			if((side->word = realloc(side->word,side->size_word)) == NULL){
				printf("Error: realloc of reading word...");
				return 1;
			}
		}
	}

	/* Ended the word. */
	side->word[i] = 0;

	return 0;
}

/**
 * Chang string to lower-case.
 * @param string String which is lower-cased.
 * @return Lower-cased string.
 */
char* strtolower(char *string)
{
	while(*string)
    {
        *string = (char) tolower(*string);
        string++;
    }
    return string;

}

/**
 * Test tag if is watched it's changing.
 * @param tag Name of tag - "<name-of-tag", "</name-of-tag", "<name-of-tag/>", <name-of-tag>
 * @return True if is watched tag.
 */
int watched_tag(char *tag)
{
	if(	(strstr(tag, "table") != NULL) || 
		(strstr(tag, "tr") != NULL) || 
		(strstr(tag, "td") != NULL) ||
		(strstr(tag, "th") != NULL) ||
		(strstr(tag, "caption") != NULL) ||
		(strstr(tag, "col") != NULL) ||
		(strstr(tag, "colgroup") != NULL) ||
		(strstr(tag, "tbody") != NULL) ||
		(strstr(tag, "thead") != NULL) || 
		(strstr(tag, "tfoot") != NULL) ||
		(strstr(tag, "li") != NULL) ||
		(strstr(tag, "ol") != NULL) ||
		(strstr(tag, "ul") != NULL) ||
		(strstr(tag, "dl") != NULL) ||
		(strstr(tag, "dt") != NULL) ||
		(strstr(tag, "dd") != NULL) ||
		(strstr(tag, "h1") != NULL) || 
		(strstr(tag, "h2") != NULL) || 
		(strstr(tag, "h3") != NULL) || 
		(strstr(tag, "h4") != NULL) || 
		(strstr(tag, "h5") != NULL) || 
		(strstr(tag, "h6") != NULL) || 
		(strstr(tag, "a") != NULL) ||
		(strstr(tag, "br") != NULL)
	){
		return 1;
	}
	else{
		return 0;
	}
}

/**
 * Parse input string to operation sturct.
 * @param oper String of patchfile operation - ([1-9]+[0-9]*,)?[0-9]*[acd][0-9]*(,[1-9]+[0-9]*)?.
 * @return Struct of information about parsed operation.
 */
OPERATION take_op(char *oper){
	OPERATION op;

	/* Save first number of line from oper berfor operation. */
	op.x1 =	take_int(&oper);

	/* If next character is comma then follow next number of first range. */
	if(oper[0]==',')
	{
		/* Skip comma from input string. */
		oper = &oper[1];

		/* Save second number of line of range. */
		op.x2 =	take_int(&oper);
	}else{
		op.x2 = 0;
	}

	/* Save name of operation. */
	op.operation = oper[0];

	/* Skip the operation name from input string. */
	oper = &oper[1];

	/* Save first nuber of line from oper after operation. */
	op.y1 =	take_int(&oper);

	/* If next character is comma then follow next number of second range. */
	if(oper[0]==',')
	{
		/* Skip comma from input string. */
		oper = &oper[1];

		/* Save second number of line of range. */
		op.y2 =	take_int(&oper);
	}else{
		op.y2 = 0;
	}

	/* Return struct of operation. */
	return op;
}

/**
 * Cut first sequence of number from input string.
 * @param string String that begin with numbers.
 * @return First number from string.
 */
int take_int(char** string)
{
	/* For move in input string and saving number. */
	int i = 0;

	/* Temporary variable for save cut nuber from input. */
	char tmp_char[10];

	/* Read from begining while is number: 48 = "0", 57 = "9". */
	while((*string)[i] > 47 && (*string)[i] < 58)
	{
		/* Save to temporary variable. */
		tmp_char[i] = (*string)[i];

		/* Read next char and check if is nuber. */
		i++;
	}

	/* Ended nuber's string. */
	tmp_char[i] = 0;
	
	/* Cut input string by readed numver. */
	*string = &(*string)[i];

	/* Return converted string to nubmer. */
	return atoi(tmp_char);
}

/**
 * Test if input character is number.
 * @param number Testing character.
 * @return True if character is number.
 */
int is_digit(char number)
{
	if(number > 47 && number < 58){
		return 1;
	}
	else{
		return 0;
	}
}

/**
 * Generate path and temporary file name.
 * @param prefix Prefix for name of temporary file.
 * @return True if character is number.
 */
char *my_tempnam(char *prefix)
{
	if(prefix != NULL)
	{
		return _tempnam("c:\\tmp", prefix);
	}
	else
	{
		return _tempnam("c:\\tmp", "ladandiff_");
	}
}
