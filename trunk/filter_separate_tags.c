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
 * @file filter_separate_tags.c
 * @brief Filter for separating HTML tags on lines.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Separate every (start/end) tag on one separated line. 
 * Beside seaprated tags, changes character '<' which isn't main part of tag (begening of tag) with entity '&lt;'
 * And lowercase all tags.
 * And option to prepare for comparation on word by word level = escape all non tag words on new separated line. Deped on configuration (option -W must be seted).
 *
 * Definition of elements and attributes  (http://www.w3.org/TR/html4/intro/sgmltut.html#h-3.2.1):\n
 * The element's name appears in the start tag (written <element-name>) and the end tag 
 * (written </element-name>); note the slash before the element name in the end tag.
 * Elements may have associated properties, called attributes, which may have values 
 * (by default, or set by authors or scripts). Attribute/value pairs appear before the final ">" 
 * of an element's start tag. Any number of (legal) attribute value pairs, separated by spaces, 
 * may appear in an element's start tag. They may appear in any order.
 *
 * For us is tag: 
 * Searching opened character "<" with name of tag "<name-of-tag"
 *		- name-of-tags are sequence of alphabet and numeric character
 *		- if name-of-tag isn't sequence above defined is character "<" isn't taken as opening character for tag.
 * Then skip all words and searching for closing character ">".
 *
 * @todo Smarter matches for tags attributes. Now tag musn't include character ">".
 */

#include "filter_separate_tags.h"

/**
 * Separate all tags on separated lines and 
 * replace character '<' it's entity '&lt;' which isn't main part of tag (begening of tag).
 * @param side Struct SIDE with input file where do separate tags.
 * @return Void.
 */
int filter_separate_tags(SIDE* side)
{
	/* Pointer to word which contains tag. */
	char *finded = NULL;
	
	/* For replacing character '<' for entity. */
	char *finded2, *start = NULL;

	/* Temporary pointer for moving in tag. */
	char *p_tmp = NULL;

	/* Sign for what part of comment is searching. */
	int phase = 0;

	/* Saved last character in printed text for cuted rest of text. */
	char tmp_char = 0;

	/* Sign if tag is first in file - dont escaped on new line. */
	int first_tag = 1;

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
				/* Searching for begining of tag "<name-of-tag". */
				case 0:
					/* Finded begining of tag and have a name. */
					finded = strstr(side->word, "<");
					if(finded && strlen(finded) > 1)
					{
						/* Immediatly must follow name of tag. */
						p_tmp = finded;
						p_tmp++;
						while(*p_tmp != 0)
						{
							/* Alphabetic character or number. */
							if((*p_tmp > 64 && *p_tmp < 91) || (*p_tmp > 96 && *p_tmp < 123) || (*p_tmp > 47 && *p_tmp < 58))
							{
								p_tmp++;
							}
							else{
								break;
							}
						}

						/* Separate everithing before "<". */
						finded[0] = 0;

						/* Print it to file. */
						if(side->word[0] != 0){
							fprintf(side->temp_file_to, "%s\n", side->word);
						}

						if(side->word[0] != 0){first_tag = 0;}

						/* Move to next character. */
						finded++;

						/* Is begining of tag (or last character which wasn't matches as tag name is closing character of tag ">" (and isn't "<>") or "/"). */
						if(p_tmp[0] == 0 || (finded != p_tmp && p_tmp[0] == '>') || p_tmp[0] == '/')
						{
							/* Save last character. */
							tmp_char = p_tmp[0];
							p_tmp[0] = 0;

							/* Print lowercase tag on new line. */
							strtolower(finded);
							if(!first_tag){
								fprintf(side->temp_file_to, "\n<%s", finded);
							}
							else{
								fprintf(side->temp_file_to, "<%s", finded);	
							}

							p_tmp[0] = tmp_char;

							/* Move to next phase - searching end of tag ">". */
							phase++;

							/* Cut printed string and begining of tag. */
							strcpy(side->word, p_tmp);
						}
						else{
							/* Print cuted character "<" and continue searching for tag. 
							   Print entity character because latest searching for tags - tags starst's wtih character '<'. 
							   If next character is "!" is comment => dont print entity. */
							if(finded[0] != 0 && finded[0] == '!'){
								fprintf(side->temp_file_to, "<");
							}
							else{
								fprintf(side->temp_file_to, "&lt;");
							}

							first_tag = 0;

							/* Cut printed string and begining of tag. */
							strcpy(side->word, finded);
						}
					}

					/* Not finded begining of tag. Write to file word and white spaces. */
					else
					{
						first_tag = 0;

						if(side->word[0] != 0){
							fprintf(side->temp_file_to, "%s", side->word);

							/* Only on source level comparation. For deleted added new line after comparation. */
							//if(config.compare == source){fprintf(side->temp_file_to, "#DL#\n");}
						}
						if(side->white_space[0] != 0){
							fprintf(side->temp_file_to, "%s", side->white_space);
						}

						if(config.compare_lvl == words){
							fprintf(side->temp_file_to, "\n");
						}

						/* Erase word and white spaces for next reading. */
						side->word[0] = 0;
						side->white_space[0] = 0;
					}
					break;

				/* Searching end of tag ">". */
				case 1:

					/* End of tag (">"). */
					finded = strstr(side->word, ">");
					if(finded)
					{
						/* Separate everithing before ">". */
						finded[0] = 0;

						/* Replace "<" for entity. */
						finded2 = strchr(side->word, '<');
						start = side->word;
						while(finded2 != NULL)
						{
							finded2[0] = 0;
							strtolower(start);
							fprintf(side->temp_file_to, "%s", start);
						
							start = finded2+1;

							if(start[0] != 0 && start[0] == '!'){
								fprintf(side->temp_file_to, "<");
							}
							else{
								fprintf(side->temp_file_to, "&lt;");
							}


							finded2 = strchr(finded2 + 1, '<');
						}

						/* Print it to file. */
						strtolower(start);
						fprintf(side->temp_file_to, "%s>\n", start);

						/* Move after ">". */
						finded++;

						/* Cut printed string. */
						strcpy(side->word, finded);

						/* Move to first phase - searching for tag (character "<"). */
						phase--;
					}

					/* It can be a "/" for non pair tags or word is empty or list of attributes. */
					else
					{
						/* Replace "<" for entity. */
						if(strcmp(side->word, "")){
							finded2 = strchr(side->word, '<');
							start = side->word;
							while(finded2 != NULL)
							{
								finded2[0] = 0;
								strtolower(start);
								fprintf(side->temp_file_to, "%s", start);

								start = finded2+1;

								if(start[0] != 0 && start[0] == '!'){
									fprintf(side->temp_file_to, "<");
								}
								else{
									fprintf(side->temp_file_to, "&lt;");
								}

								finded2 = strchr(finded2 + 1, '<');
							}

							/* Print the rest of word to file. */
							strtolower(start);
							fprintf(side->temp_file_to, "%s", start);
						}

						/* Copy only white space because tag must be only on one line. */
						if(strcmp(side->white_space, "")){
							fprintf(side->temp_file_to, "%s", " ");
						}

						/* Erase word and white spaces for next reading. */
						side->word[0] = 0;
						side->white_space[0] = 0;
					}
					break;
			}
		}
	}

	return 0;
}
