/* Part of LadanDiff (Show differences between two HTML files.)
   Copyright (C) 2008-2010
   Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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

	/* Empty file. */
	int is_empty = 1;

	/* Reading file to end of it. */
	while(side->character != EOF)
	{
		is_empty = 0;

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
			
			/* Write white space. */
			if(side->white_space[i]!=0){
				fprintf(side->temp_file_to, "%s", side->white_space);
			}
		}
	}

	/* Add blank line to the end of file if there wasn't one. Because GNU Diff. */
	if(side->white_space[0] != '\n' && !is_empty){
		fprintf(side->temp_file_to, "%s", "\n");
	}
	return 0;
}
