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
 * @file filter_append_src.c
 * @brief Filter for changing append operation in patch file to new append for highligh in revision on source level comparation.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Replace operation append in patch file: \n
 * - on begening every line is checking if start operation
 * - append block is changed to new append block with border of sign append block
 *
 * @todo Support for special element PRE and CDATA.
 * @todo Now only reading normalization file which not contain sequence of white spaces - support for common files.
 */
#include "filter_append_src.h"

/**
 * Highligh operation append in patch file on shown text level.
 * @param side Struct SIDE with patch file where is highliting append block.
 * @return int Sign if postprocesing was ok.
 */
int filter_append_src(SIDE* side)
{
	/* Sign of new line. */
	int new_line = 1;

	/* Sign of finded operation append. */
	int op_append = 0;

	/* Struct for parsing operation for compare what kind operation is. */
	OPERATION op;

	/* Sign of if must write begening tag of append block. */
	int open_tag = 0;

	/* Read file by word to end. */
	while(side->character != EOF)
	{
		/* Read word and white spaces. */
		copy_word(side);
		copy_whitespace(side);

		/* If new line and new line begening with digit => is operation. */
		if(new_line && is_digit(side->word[0]))
		{
			new_line = 0;

			/* Load operation to struct. */
			op = take_op(side->word);

			/* Compare if is append operation. */
			if(op.operation == 'a')
			{
				/* Set signs. */
				op_append = 1;
				open_tag = 1;

				/* Copy operation to output. */
				fprintf(side->temp_file_to, "%s", side->word);
				fprintf(side->temp_file_to, "%s", side->white_space);
	
				/* Read from input and copy to output while new operation. */
				while(side->character != EOF && (new_line && side->character == '>' || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n'){new_line = 1;}
					else{new_line = 0;}

					/* Write word to output. */
					fprintf(side->temp_file_to, "%s", side->word);

					/* On new line is new operation => must write closing tag for append block. */
					if(new_line && side->character != '>'){
						fprintf(side->temp_file_to, "%s", config.hl.append_end);
					}
					/* On new line continues append block => wrap text for showing HTML source in HTML. */
					else if(new_line){
						fprintf(side->temp_file_to, "<br />");
					}

					/* Write white spaces to output. */
					fprintf(side->temp_file_to, "%s", side->white_space);
					
					/* On begening of append block input open tag for append block. */
					if(open_tag && side->word[0] == '>'){
						fprintf(side->temp_file_to, "%s", config.hl.append_start);
						open_tag = 0;
					}
					/* On begening new line in append block input middle character (for example white space for indent). */
					else if(side->word[0] == '>'){
						fprintf(side->temp_file_to, "%s", config.hl.append_middle);
					}
				}
			}
		}

		/* Write word and white spaces to output if wasn't operation append (else write it twice). */
		if(!op_append)
		{
			if(side->word[0]!=0){
				fprintf(side->temp_file_to, "%s", side->word);
			}
			if(side->white_space[0]!=0){
				fprintf(side->temp_file_to, "%s", side->white_space);
			}
		}
		else{op_append = 0;}

		/* If is new line set sign new line for checking new operation. */
		if(side->white_space[0] == '\n'){
			new_line = 1;
		} else {new_line = 0;}
	}

	return 0;
}
