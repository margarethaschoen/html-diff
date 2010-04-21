/** 
 * @file filter_delete_space.c
 * @brief Filter for deleting extra white spaces.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * @todo Replace (ignore) sequnece white spaces in element PRE - in this element is diferent interpretation in HTML.
 *
 * Replace sequence white spaces to one character - HTML ignor sequences
 *  - if sequence include new line then replace sequence to one new line
 *  - if sequence not include new line then replace sequnece to one space
 *
 * Add blank line to the end of file if wasn't there one. Because GNU Diff.
 */

#include "filter_delete_space.h"

/**
 * Replace all extra white spaces from input file.
 * @param side Struct SIDE with input file where is replacing white spaces.
 * @return int Sign if postprocesing was ok.
 */
int filter_delete_space(SIDE* side)
{
	/* For moving in sequence white spaces. */
	int i = 0;

	/* Sign if was found new line. */
	int new_line = 0;

	/* Reading file to end of it. */
	while(side->character != EOF)
	{
		/* Read word and white spaces from file. */
		copy_word(side);
		copy_whitespace(side);

		/* If white spaces not empty. */
		if(side->white_space[i] != 0)
		{
			/* Reading white spaces by character and searching for new line. */
			while(side->white_space[i]!=0)
			{
				/* Find new line => end and set the sign. */
				if(side->white_space[i] == '\n')
				{
					new_line = 1;
					break;
				}
				i++;
			}

			/* If was founded new line replace hole sequence on one new line. */
			if(new_line){
				strcpy(side->white_space, "\n");
			}
			/* Not founded new line => replace hole sequence on one space. */
			else{
				strcpy(side->white_space, " ");
			}
		}

		/* Inicialization. */
		i = 0;
		new_line = 0;
		
		/* Write word to output. */
		if(side->word[i]!=0)
		{
			fprintf(side->temp_file_to, "%s", side->word);
		}

		/* Write white space. */
		if(side->white_space[i]!=0){
			fprintf(side->temp_file_to, "%s", side->white_space);
		}
	}

	/* Add blank line to the end of file if there wasn't one. Because GNU Diff. */
	if(side->white_space[0] != '\n'){
		fprintf(side->temp_file_to, "%s", "\n");
	}
	return 0;
}
