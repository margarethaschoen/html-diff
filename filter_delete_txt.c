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
 * @file filter_delete_txt.c
 * @brief Filter for changing delete operation in patch file to new change operation for highligh in revision on shown text level comparation.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Replace operation delete in patch file: \n
 * - on begening every line is checking if start operation
 * - delete block is changed to new block - operation change
 * - the first part of new changed operation is same delete block and the second part is delete block bordered with sign of delete
 * - skip all delete tags
 *
 * @todo Support for special element PRE and CDATA.
 * @todo Now only reading normalization file which not contain sequence of white spaces - support for common files.
 */
#include "filter_delete_txt.h"

/**
 * Highligh operation delete in patch file on shown text level comparation.
 * @param side Struct SIDE with origin file and patch file where is highliting delete block.
 * @return int Sign if postprocesing was ok.
 */
int filter_delete_txt(SIDE* side)
{
	/* Sign of new line. */
	int new_line = 1;

	/* Sigh if was new line; */
	int was_new_line = 1;

	/* Sign if in text block. */
	int in_txt = 0;

	/* Sign if was printed space on begening of line before highlight tags. */
	int printed_space = 0;

	/* Sign of finded operation delete. */
	int op_delete = 0;

	/* Struct for parsing operation for compare what kind operation is. */
	OPERATION op;

	/* For holding start of delete block for repeatly write to output. */
	long start_del_block = 0;
	
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

			/* Compare if is delete operation. */
			if(op.operation == 'd')
			{
				/* Set signs. */
				op_delete = 1;
				in_txt = 0;
				was_new_line = 1;

				/* Copy operation to output. Change character 'd' to 'c'. Checking if is it range or one line. */
				if(op.x2){
					fprintf(side->temp_file_to, "%i,%ic%i,%i", op.x1, op.x2, op.x1, op.x2);
				}else{
					fprintf(side->temp_file_to, "%ic%i", op.x1, op.x1);
				}
				fprintf(side->temp_file_to, "%s", side->white_space);

				/* Remember pointer on begening of this block for write second part of new changed block. 
				Point after the first character '<' because this character is in the variable side->character. */
				start_del_block = ftell(side->temp_file_from);

				/* Copy non-changed delete block to output as a first part of new changed block. */
				while(side->character != EOF && ((new_line && side->character == '<') || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* Write word and white spaces to output. */
					fprintf(side->temp_file_to, "%s", side->word);
					fprintf(side->temp_file_to, "%s", side->white_space);

					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n'){new_line = 1;}
					else{new_line = 0;}
				}

				/* Write transition "---" to second part of new changed block to output. */
				fprintf(side->temp_file_to, "%s", "---\n");

				/* Go back to start of delete block for writing again to new change operation. */
				fseek(side->temp_file_from, start_del_block, SEEK_SET);
				side->character = '<';

				/* Read from input and copy to output while new operation. 
				Bordered delete block and print as a second part of new changed block. */
				while(side->character != EOF && ((new_line && side->character == '<') || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n'){new_line = 1;}
					else{new_line = 0;}

					/* On new line continue delete operation. */
					if(was_new_line)
					{
						fprintf(side->temp_file_to, "%c", '>');

						/* Begening of tag. Don't highlighting and controle if was previouse highlited text - must ended highlight tag. */
						if(side->character != '<' && !in_txt){
							fprintf(side->temp_file_to, " %s", config.hl.change_delete_start);
							in_txt = 1;
							printed_space = 1;
						}else if(side->character == '<' && in_txt){
							fprintf(side->temp_file_to, " %s", config.hl.change_delete_end);
							in_txt = 0;
							printed_space = 1;
						} 
					}
					else
					{
						/* Write word to output. */
						fprintf(side->temp_file_to, "%s", side->word);
					}

					/* On new line is new operation => must write closing tag for delete block. */
					if(new_line && side->character != '<' && in_txt){
						fprintf(side->temp_file_to, " %s", config.hl.change_delete_end);
					}

					/* Write white spaces to output if didn't printed on begening of line before highliht tags. */
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

		/* Write word and white spaces to output if wasn't operation delete (else write it twice). */
		if(!op_delete)
		{
			if(side->word[0]!=0){
				fprintf(side->temp_file_to, "%s", side->word);
			}
			if(side->white_space[0]!=0){
				fprintf(side->temp_file_to, "%s", side->white_space);
			}
		}
		else{op_delete = 0;}

		/* If is new line set sign new line for checking new operation. */
		if(side->white_space[0] == '\n'){
			new_line = 1;
		}
	}

	return 0;
}
