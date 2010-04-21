/** 
 * @file filter_change_specchar.c
 * @brief Filter for changing special character.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * @todo Replace (ignore) sepcial character in element PRE and CDATA - in this element is diferent interpretation in HTML.
 *
 * Read input file by character and replacing special character for entits.
 * For showing file as a programm source in HTML no as a HTML.
 *  - character "<" (begening of HTML tag) for "&lt"
 *  - character ">" (ending of HTML tag) for "&gt"
 *  - character "&" (begening of entits) for "&amp"
 */

#include "filter_change_specchar.h"

/**
 * Replace all special character for entits.
 * @param side Struct SIDE with input file where is replacing white spaces.
 * @return int Sign if postprocesing was ok.
 */
int filter_change_specchar(SIDE* side)
{
	/* Read file by character to end of it. */
	while(side->character != EOF)
	{
		/* Copy all white-spaces to output file without changes. */
		if(isspace(side->character)){
			copy_whitespace(side);
			fprintf(side->temp_file_to,"%s",side->white_space);
		}

		/* After write white-spaces is loaded new character => must control if is end of file. */
		if(side->character != EOF){
			/* Replace if is special character. */
			switch(side->character)
			{
				/* Is character: "<". */
				case 60:
					fprintf(side->temp_file_to, "%s", "&lt;");
					break;

				/* Is character: ">". */
				case 62:
					fprintf(side->temp_file_to, "%s", "&gt;");
					break;

				/* Is character: "&". */
				case 38:
					fprintf(side->temp_file_to, "%s", "&amp;");
					break;
				
				/* Isn't special character. */
				default:
					fprintf(side->temp_file_to, "%c", side->character);
			}

			/* Read character from file. */
			side->character = getc(side->temp_file_from);
		}
	}

	return 0;
}
