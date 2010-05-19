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
 * @file filter_append_txt.c
 * @brief Filter for changing append operation in patch file to new append for highligh in revision on shown text level comparation.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Replace operation append in patch file: \n
 * - on begening every line is checking if start operation
 * - append block (only plain text) is changed to new append block with border of sign append block
 * - tags in append block arn't added to output - only watched tags (table)
 *
 * @todo Support for special element PRE and CDATA.
 * @todo Now only reading normalization file which not contain sequence of white spaces - support for common files.
 */
#include "filter_append_txt.h"

/**
 * Highligh operation append in patch file.
 * @param side Struct SIDE with patch file where is highliting appned block.
 * @return int Sign if postprocesing was ok.
 */
int filter_append_txt(SIDE* side)
{
	/* Sign of new line. */
	int new_line = 1;
	
	/* Sigh if was new line; */
	int was_new_line = 1;

	/* Sign if in text block. */
	int in_txt = 0;
	
	/* Test if tag is wathed for changed. */
	int test_tag = 0;

	/* Sign if was printed space on begening of line before highlight tags. */
	int printed_space = 0;

	/* Sign if line must skip - tags that aren't watched. */
	int no_skip = 1; 

	/* Sign of finded operation append. */
	int op_append = 0;

	/* Struct for parsing operation for compare what kind operation is. */
	OPERATION op;

	/* Read file by word to end. */
	while(side->character != EOF){
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
				in_txt = 0;
				was_new_line = 1;
				no_skip = 1;
				test_tag = 0;

				/* Copy operation to output. */
				fprintf(side->temp_file_to, "%s", side->word);
				fprintf(side->temp_file_to, "%s", side->white_space);
	
				/* Read from input and copy to output while new operation. */
				while(side->character != EOF && (new_line && side->character == '>' || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* If skipped is in tag, compare for watched tags. */
					if(test_tag)
					{
						/* Test tag if is watched it's changing. */
						if(watched_tag(side->word)){no_skip = 1;}
						test_tag = 0;
					}

					/* Write word to output. */
					if(no_skip){
						fprintf(side->temp_file_to, "%s", side->word);
					}

					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n')
					{
						new_line = 1; 
						no_skip = 1;
					}
					else{new_line = 0;}

					/* On new line continue append operation. */
					if(was_new_line)
					{
						/* Begening of tag. Don't highlighting and controle if was previouse highlited text - must ended highlight tag. */
						if(side->character != '<' && !in_txt)
						{
							fprintf(side->temp_file_to, " %s", config.hl.change_append_start);
							in_txt = 1;
							printed_space = 1;
						}
						else if(side->character == '<')
						{
							if(in_txt)
							{
								fprintf(side->temp_file_to, " %s", config.hl.change_append_end);
								in_txt = 0;
								printed_space = 1;
							}
							no_skip = 0;
							test_tag = 1;
						} 
					}

					/* On new line is new operation => must write closing tag for delete block. */
					if(new_line && side->character != '>' && in_txt){
						fprintf(side->temp_file_to, " %s", config.hl.change_append_end);
					}

					/* Write white spaces to output if didn't printed on begening of line before highliht tags. */
					/* Don't skip because on skiped line must be something for patch - white spaces are ignored by HTML. */
					if(!printed_space){
						fprintf(side->temp_file_to, "%s", side->white_space);
					}else{
						printed_space = 0;
					}

					was_new_line = 0;
					if(new_line){was_new_line = 1;}
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
		}
	}

	return 0;
}
