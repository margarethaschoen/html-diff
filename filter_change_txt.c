/** 
 * @file filter_change_txt.c
 * @brief Filter for changing append operation in patch file to new append for highligh in revision on shown text level comparation.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * For line level comparation:
 *		Replace operation change in patch file: \n
 *		- on begening every line is checking if start operation
 *		- first part of change block is copy to output and second part is modife:
 *			- first and second part is save to files - one line one word, new line in patch file is saved as "#L#" on separated line
 *			- then bouth files are compared - by the help with output patch and first file is completed the second part of changed blcok
 *		- tags are skipped beside watched tags
 *
 * For word level comparation:
 *
 *
 * @todo Support for special element PRE and CDATA.
 * @todo Now only reading normalization file which not contain sequence of white spaces - support for common files.
 */
#include "filter_change_txt.h"

/**
 * Highlighting change blocks from patch file on text level comparation.
 * @param side Struct SIDE with patch file and orgiin normalized file.
 * @return int Sign if postprocesing was ok.
 */
int filter_change_txt(SIDE* side)
{
	/* Comparation on words level. */
	if(config.compare_lvl == words){
		return word_by_word_txt(side);
	}
	/* Comparation on line level. */
	else if(config.compare_lvl == lines){
		return line_by_line_txt(side);
	}

	return 1;
}

/**
 * Highlighting change blocks from patch file on line by line comparation level.
 * @param side Struct SIDE with patch file and origin normalized file.
 * @return int Sign if postprocesing was ok.
 */
int word_by_word_txt(SIDE* side)
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

	/* Test if tag is wathed for changed. */
	int test_tag = 0;

	/* Sign if line must skip - tags that aren't watched. */
	int no_skip = 1; 

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
			if(op.operation == 'c')
			{
				/* Set signs. */
				op_delete = 1;
				in_txt = 0;
				was_new_line = 1;
				no_skip = 1;
				test_tag = 0;

				/* Copy operation to output. Checking if is it range or one line. */
				if(op.x2){
					if(op.y2){
						fprintf(side->temp_file_to, "%i,%ic%i,%i", op.x1, op.x2, op.y1, (op.y2+(op.x2-op.x1))+1);
					}
					else{
						fprintf(side->temp_file_to, "%i,%ic%i,%i", op.x1, op.x2, op.y1, (op.y1+(op.x2-op.x1))+1);
					}
				}
				else{
					if(op.y2){
						fprintf(side->temp_file_to, "%ic%i,%i", op.x1, op.y1, op.y2+1);
					}
					else{
						fprintf(side->temp_file_to, "%ic%i,%i", op.x1, op.y1, op.y1+1);
					}
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
						fprintf(side->temp_file_to, ">");

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

				/* Skip separator of change block "---". */
				copy_word(side);
				copy_whitespace(side);

				/* Set signs. */
				in_txt = 0;
				was_new_line = 1;

				/* Read the second part of changed block and copy to temporary file to start of new operation. */
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

/**
 * Highlighting change blocks from patch file on line by line comparation level.
 * @param side Struct SIDE with patch file and orgiin normalized file.
 * @return int Sign if postprocesing was ok.
 */
int line_by_line_txt(SIDE* side)
{
	/* Sign of new line. */
	int new_line = 1;

	/* Sign if is reading tag. */
	int tag = 1;

	/* Sign of finded operation append. */
	int op_change = 0;

	/* Struct for parsing operation for compare what kind operation is. */
	OPERATION op;

	/* For separate changed block to tmp files for comparation by words. */
	char *tmp_1_name;
	char *tmp_2_name;
	char *tmp_diff_name;
	FILE *tmp_1;
	FILE *tmp_2;

	/* Text command for comparation. */
	char *command_tmp_diff;

	/* Struct for tmp_1 file and for patch from compare both part of changed block. */
	SIDE ff, fp;
	SIDE* from_file = &ff;
	SIDE* from_patch = &fp;

	/* Sign if temp files was created. */
	int created_file = 0;

	/* Inicialization of struct - allocate strings. */
	if(new_side(from_file)){
		return 1;
	}

	/* Inicialization of struct - allocate strings. */
	if(new_side(from_patch)){
		return 1;
	}

	/* Generate name of tmp files. */
	tmp_1_name = my_tempnam(NULL);
	tmp_2_name = my_tempnam(NULL);

	/* Generate tmp file name for output diff from comapre of tmp files - parts of changed block. */
	tmp_diff_name = my_tempnam(NULL);

	/* Make diff comand for compare changed blocks. */
	command_tmp_diff = (char*) malloc(
							strlen(DIFF)+
							strlen(tmp_1_name)+
							strlen(tmp_2_name)+
							strlen(tmp_diff_name)+6);
	strcpy(command_tmp_diff, DIFF);
	strcat(command_tmp_diff, " ");
	strcat(command_tmp_diff, tmp_1_name);
	strcat(command_tmp_diff, " ");
	strcat(command_tmp_diff, tmp_2_name);
	strcat(command_tmp_diff, " > ");
	strcat(command_tmp_diff, tmp_diff_name);

	/* Read file by word to end. */
	while(side->character != EOF)
	{
		copy_word(side);
		copy_whitespace(side);

		/* If new line and new line begening with digit => is operation. */
		if(new_line && is_digit(side->word[0]))
		{
			/* Load operation to struct. */
			op = take_op(side->word);

			/* Compare if is change operation. */
			if(op.operation == 'c')
			{
				/* Set signs. */
				op_change = 1;
				new_line = 1;
				created_file = 1;
				tag = 0;

				/* Copy operation to output. */
				fprintf(side->temp_file_to, "%s", side->word);
				fprintf(side->temp_file_to, "%s", side->white_space);
	
				/* Open temp file for copmare change block. */
				if((tmp_1 = fopen(tmp_1_name, "w+")) == NULL){
					program_log(1, "Cannot open output file for preprocessing '%s'.", tmp_1_name);
					return 1;
				}

				/* Read from input and copy to temporary file and to output while continue first block of operation change. */
				while(side->character != EOF && (new_line && side->character == '<' || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* Write word and white spaces to output - output patchfile. */
					fprintf(side->temp_file_to, "%s", side->word);
					fprintf(side->temp_file_to, "%s", side->white_space);

					/* Write word to temp file without first line character "<". */
					if(!new_line)
					{
						/* Write word and white spaces to temporary file. */
						fprintf(tmp_1, "%s", side->word);
						if(!tag || side->white_space[0] == '\n'){
							fprintf(tmp_1, "%c", '\n');
						}else{
							fprintf(tmp_1, "%s", side->white_space);
						}

						/* Write sign of new line in patch file. Not in end of block. */
						if(side->white_space[0] == '\n' && side->character == '<'){
							fprintf(tmp_1, "%s", "#L#\n");
							tag = 0;
						}
					}
					/* Is start of tag - tag don't pars on word level. */
					else if(side->character == '<'){
						tag = 1;
					}
					
					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n'){new_line = 1;}
					else{new_line = 0;}
				}

				/* Copy to output separator of change block "---". */
				copy_word(side);
				copy_whitespace(side);
				fprintf(side->temp_file_to, "%s", side->word);
				fprintf(side->temp_file_to, "%s", side->white_space);
				
				/* Open temp file for copmare change block. */
				if((tmp_2 = fopen(tmp_2_name, "w+")) == NULL){
					program_log(1, "Cannot open output file for preprocessing '%s'.", tmp_2_name);
					return 1;
				}

				new_line = 1;
				tag = 0;

				/* Read the second part of changed block and copy to temporary file to start of new operation. */
				while(side->character != EOF && (new_line && side->character == '>' || !new_line))
				{
					/* Read new word and white spaces. */
					copy_word(side);
					copy_whitespace(side);

					/* Write word to temp file without first line character ">". */
					if(!new_line)
					{
						/* Write word and white spaces to temporary file. */
						fprintf(tmp_2,"%s",side->word);
						if(!tag || side->white_space[0] == '\n'){
							fprintf(tmp_2, "%c", '\n');
						}else{
							fprintf(tmp_2, "%s", side->white_space);
						}

						/* Write sign of new line in patch file. Not in end of block. */
						if(side->white_space[0] == '\n' && side->character == '>'){
							fprintf(tmp_2, "%s", "#L#\n");
							tag = 0;
						}
					}
					/* Is start of tag - tag don't pars on word level. */
					else if(side->character == '<'){
						tag = 1;
					}

					/* Is new line => set sign new line for checking if not begining new operation. */
					if(side->white_space[0] == '\n'){new_line = 1;}
					else{new_line = 0;}
				}

				fflush(tmp_1);
				fflush(tmp_2);

				/* Process commandline operation - diff. */
				system(command_tmp_diff);

				/* First part of changed block in file. */
				from_file->temp_file_from = tmp_1;
				fseek(from_file->temp_file_from, 0, SEEK_SET);

				/* Patch file from compare of both block in change operation. */
				from_patch->temp_file_from = fopen(tmp_diff_name, "r");

				/* Read first character from input file bacause checking end of file. */
				from_file->character = getc(from_file->temp_file_from);
				from_patch->character = getc(from_patch->temp_file_from);

				/* Make the second part of changed block with highlight deferencies. */
				change_block_compare_txt(from_file, from_patch, side);

				fclose(tmp_1);
				fclose(tmp_2);
				fclose(from_patch->temp_file_from);
			}

			new_line = 0;
		}

		//vypise na vstup, jen pokud nebyla operace append (vypsal by dvakrat to same)
		if(!op_change)
		{
			//pokud slovo neni prazdne, zkopiruje ho do vystup. souboru
			if(side->word[0]!=0){
				fprintf(side->temp_file_to,"%s",side->word);
			}
			//pokud bile znaky nejsou prazdne, zkopiruje je do vystup. souboru
			if(side->white_space[0]!=0){
				fprintf(side->temp_file_to,"%s",side->white_space);
			}
		}
		else{op_change = 0;}

		//novy radek
		if(side->white_space[0] == '\n'){
			new_line = 1;
		}
		fflush(side->temp_file_to);
	}

	/* Delete temporary file for comapration changed block on word level. */
	if(created_file){
		remove(tmp_1_name);
		remove(tmp_2_name);
		remove(tmp_diff_name);
	}

	/* Deallocate strings. */
	free(tmp_1_name);
	free(tmp_2_name);
	free(tmp_diff_name);
	free(command_tmp_diff);

	return 0;
}

/**
 * Create new second part of change operation from patch file of differences first and second part changed block.
 * Created from first file and patch file. Output print to side - new patch file from this filter.
 * @param from_file First part of change block - each line one word.
 * @param from_patch Patch file - output from diff on first and second part of change block.
 * @param side Struct SIDE with old patch file and new which is output for new second changed block.
 */
int change_block_compare_txt(SIDE* from_file, SIDE* from_patch, SIDE* side)
{
	/* For counting lines in from_file. */
	int counter_line = 0;

	/* Sing of new line if must write first line character '>'. */
	int new_line = 0;

	/* Sign if starts highlighting tag for delete and append parts. For skipping tags from highlighting. */
	int start_highlight = 0;

	/* Sign if tag is watched - then printed. */
	int is_watched = 0;

	/* Struct for parsing operation for compare what kind operation is. */
	OPERATION op;

	/* Print to output begening border of change blog. */
	fprintf(side->temp_file_to, "> ");

	/* Read file by word to end. */
	while(from_patch->character != EOF)
	{
		new_line = 0;
		start_highlight = 0;

		/* Read word and white spaces. */
		copy_word(from_patch);
		copy_whitespace(from_patch);

		/* Load operation to struct. */
		op = take_op(from_patch->word);

		/* Copy lines (from from_file) to op.x1. line. Non changed lines form from_file. */
		while(counter_line < op.x1)
		{
			counter_line++;

			/* For operation append copy lines to op.x1 (block is appended after this line).
			 * For another operation copy lines without line op.x1 becaouse before line op.x1 must added border. */
			if(op.operation == 'a' || counter_line < op.x1)
			{
				/* Read new word and white spaces. */
				copy_word(from_file);
				copy_whitespace(from_file);

				if(from_file->word[0] == '<')
				{
					fprintf(side->temp_file_to, "%s ", from_file->word);
					while(from_file->white_space[0] != '\n')
					{
						copy_word(from_file); copy_whitespace(from_file);
						fprintf(side->temp_file_to, "%s ", from_file->word);
					}
				}
				else
				{
					/* If find sign of new line set var new_line and skip this sign. */
					if(!strcmp(from_file->word, "#L#")){new_line=1;}
					/* Write word and white spaces to output - output patchfile. */
					else{
						fprintf(side->temp_file_to, "%s ", from_file->word);
					}

					/* Print first line character to patch file. */
 					if(new_line)
					{
						fprintf(side->temp_file_to, "\n> ");
						new_line=0;
					}
				}
			}
		}

		/* Decrese counter_line for corespond with printed line. In case d,c operation doesn't print last line op.x1. */
		if(op.operation != 'a' && counter_line == op.x1){counter_line--;}

		/* Operation change or delete. The first part has same meaning. */
		if(op.operation == 'c' || op.operation == 'd')
		{
			/* Copy delete block or first part of change block as deleted. */
			while(counter_line < op.x1 || counter_line < op.x2)
			{
				counter_line++;
				
				/* Read new word and white spaces. */
				copy_word(from_file);
				copy_whitespace(from_file);

				/* Skip tag on line. */
				if(from_file->word[0] == '<')
				{
					if(start_highlight)
					{
						fprintf(side->temp_file_to, "%s", config.hl.change_delete_end);
						start_highlight = 0;
					}

					fprintf(side->temp_file_to, "%s ", from_file->word);
					while(from_file->white_space[0] != '\n')
					{
						copy_word(from_file); copy_whitespace(from_file);
						fprintf(side->temp_file_to, "%s ", from_file->word);
					}
				}
				else
				{
					if(!start_highlight && strcmp(from_file->word, "#L#"))
					{
						fprintf(side->temp_file_to, "%s", config.hl.change_delete_start);
						start_highlight = 1;
					}

					/* Print sign new line to output if find sign new line in patch file. */
					if(strcmp(from_file->word, "#L#")){
						fprintf(side->temp_file_to, "%s ", from_file->word);
					}
				}

			}
			if(start_highlight){
				fprintf(side->temp_file_to, "%s", config.hl.change_delete_end);
			}

			/* Skip first part of change block or delete operation. It was printed from from_file. */
			while(from_patch->character == '<')
			{
				/* Read word and white spaces. */
				copy_word(from_patch); copy_whitespace(from_patch);

				/* Skip line (on the line would be a tag0). */
				while(from_patch->white_space[0] != '\n'){
					copy_word(from_patch); copy_whitespace(from_patch);
				}
			}
		}

		/* Skip change block separator "---". */
		if(op.operation == 'c'){
			copy_word(from_patch); copy_whitespace(from_patch);
		}

		if(op.operation == 'c' || op.operation == 'a'){
			start_highlight = 0;

			/* Copy append block or second part of change block as inserted. */
			while(from_patch->character != EOF && from_patch->character == '>')
			{
				/* Read word and white spaces. */
				copy_word(from_patch);
				copy_whitespace(from_patch);

				/* Read again = skip first character on line '>'. */
				copy_word(from_patch);
				copy_whitespace(from_patch);

				/* Skip tag on line. */
				if(from_patch->word[0] == '<')
				{
					if(start_highlight)
					{
						fprintf(side->temp_file_to, "%s", config.hl.change_append_end);
						start_highlight = 0;
					}

					/* Test tag if is watched it's changing - printed or skipped. */
					is_watched = watched_tag(from_patch->word);
					if(is_watched){fprintf(side->temp_file_to, "%s ", from_patch->word);}
					while(from_patch->white_space[0] != '\n')
					{
						copy_word(from_patch); copy_whitespace(from_patch);
						if(is_watched){fprintf(side->temp_file_to, "%s ", from_patch->word);}
					}
					is_watched = 0;
				}
				else
				{
					if(!start_highlight && strcmp(from_patch->word, "#L#"))
					{
						fprintf(side->temp_file_to, "%s", config.hl.change_append_start);
						start_highlight = 1;
					}

					/* Print sign new line to output if find sign new line in patch file. */
					if(!strcmp(from_patch->word, "#L#")){
						fprintf(side->temp_file_to, "\n> ");
					}
					/*Copy to outpu and change new line on space. */
					else{
						fprintf(side->temp_file_to, "%s ", from_patch->word);
					}
				}
			}
			if(start_highlight){
				fprintf(side->temp_file_to, "%s", config.hl.change_append_end);
			}
		}
	}

	/* Write the rest of from_file which lines aren't included in patch file. */
	while(from_file->character != EOF)
	{
		/* Read new word and white spaces. */
		copy_word(from_file);
		copy_whitespace(from_file);

		/* If find sign of new line set var new_line and skip this sign. */
		if(!strcmp(from_file->word, "#L#")){new_line=1;}
		/* Write word and white spaces to output - output patchfile. */
		else{
			fprintf(side->temp_file_to, "%s ", from_file->word);
		}

		/* Print first line character to patch file. */
		if(new_line)
		{
			fprintf(side->temp_file_to, "\n> ");
			new_line=0;
		}
	}

	fprintf(side->temp_file_to, "\n");

	return 0;
}
