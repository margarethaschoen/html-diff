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
 * @file filter_delete_comment.c
 * @brief Filter for deleting all comments.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Definition of comment (http://www.w3.org/TR/html4/intro/sgmltut.html#h-3.2.4):\n
 * White space is not permitted between the markup declaration open delimiter("<!") 
 * and the comment open delimiter ("--"), but is permitted between the comment close delimiter ("--") 
 * and the markup declaration close delimiter (">"). A common error is to include a string of hyphens ("---") 
 * within a comment. Authors should avoid putting two or more adjacent hyphens inside comments.
 */

#include "filter_delete_comment.h"

/**
 * Delete all comments from input file.
 * @param side Struct SIDE with input file where is comment deleting.
 * @return int Sign if postprocesing was ok.
 */
int filter_delete_comment(SIDE* side)
{
	/* Pointer to word which contains comment. */
	char *finded = NULL;

	/* Sign for what part of comment is searching. */
	int phase = 0;

	/* Reading while isn't end of file. */
	while(side->character != EOF)
	{
		copy_word(side);		/* Read word. */
		copy_whitespace(side);	/* Read following white spaces. */
		
		/* Something was loaded. */
		while(side->word[0] != 0 || side->white_space[0] != 0)
		{
			switch(phase)
			{
				/* Searching for begining of comment "<!--". */
				case 0:

					/* Finded begining of comment. */
					finded = strstr(side->word, "<!--");
					if(finded)
					{
						/* Separate everithing before comment. */
						finded[0] = 0;

						/* Print it to file. */
						fprintf(side->temp_file_to, "%s", side->word);

						/* Moved after beginnig of comment. */
						finded = &finded[4];

						/* Cut printed string and begining of comment. */
						strcpy(side->word, finded);

						/* Erase white spaces only if word is empty because they are in comment. */
						if(side->word[0]==0){
							side->white_space[0] = 0;
						}

						/* Move on next phase - searching for "--". */
						phase++;
					}

					/* Not finded begining of comment. Write to file word and white spaces. */
					else
					{
						if(strcmp(side->word, "")){
							fprintf(side->temp_file_to, "%s", side->word);
						}
						if(strcmp(side->white_space, "")){
							fprintf(side->temp_file_to, "%s", side->white_space);
						}

						/* Erase word and white spaces for next reading. */
						side->word[0] = 0;
						side->white_space[0] = 0;
					}
					break;

				/* Searching for closing comma of comment "--". */
				case 1:

					/* Finded "--". */
					finded = strstr(side->word, "--");
					if(finded)
					{
						/* Separate "--". */
						finded = &finded[2];

						/* Copy to word everithing after "--". */
						strcpy(side->word, finded);

						/* Erase white spaces only if word is empty because they are in comment. */
						if(side->word[0]==0){
							side->white_space[0] = 0;
						}

						/* Move to next phase - searching for end of comment ">". */
						phase++;
					}

					/* Not finded "--". Erase word and white spaces because they are in comment. */
					else
					{
						side->word[0] = 0;
						side->white_space[0] = 0;
					}
					break;

				/* Searching for end of comment ">". */
				case 2:

					/* Finded end of comment ">". */
					finded = strstr(side->word, ">");
					if(finded)
					{
						/* Cut everithing to end of comment plus closing char ">". */
						finded = &finded[1];

						/* Copy to word everting after comment. */
						strcpy(side->word, finded);

						/* Move to first phase of searching - searchnig for begining of comment. */
						phase = 0;
					}

					/* Not finded end of comment ">". Erase word and white spaces because they are in comment. */
					else
					{
						side->word[0] = 0;
						side->white_space[0] = 0;
					}
					break;
			}
		}
	}

	return 0;
}
