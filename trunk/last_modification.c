/** 
 * @file last_modification.c
 * @brief Last modification of revision file.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 * Process last modification of rivision file:
 *	Wrap all line that aren't in append/delete/change blocks.
 *	Delete sign of (APPEND|DELETE|CHANGE)_MIDDLE on source level comparation.
 *	Add css style to begening of file.
 */

#include "last_modification.h"

/**
 * Process last modification of revision file.
 * @param name_of_revision Name of revision file.
 * @return Void.
 */
int last_modification(char* name_of_revision)
{
	SIDE rev;
	SIDE* revision = &rev;

	/* Sign of new line. */
	int new_line = 1;
	
	/* Sign for what part of file is. */
	int faze = 1;

	/* Revision file is reopen. And css style is appended to the begening of file. */
	if(config.compare == source)
	{
		/* Inicialization of struct - allocate strings. */
		if(new_side(revision)){
			return 1;
		}

		program_log(4, "Open file '%s' as input for last modification.", name_of_revision);

		/* Open revision file. */
		if((revision->temp_file_from = fopen(name_of_revision, "r")) == NULL)
		{
			program_log(1, "Cannot open file '%s' for last modification.", name_of_revision);
			return 1;
		}

		program_log(4, "Open file '%s' as output for last modification.", &name_of_revision[4]);

		/* Open new file for output of last midification. Name of file is without prefix "tmp_"*/
		if((revision->temp_file_to = fopen(&name_of_revision[4], "w+")) == NULL)
		{
			program_log(1, "Cannot open file '%s' for last modification.", &name_of_revision[4]);
			return 1;
		}
		
		revision->character = getc(revision->temp_file_from);

		/* Add embedded css style. */
		if(config.css == 3){
			fprintf(revision->temp_file_to, "%s", CSS_STYLE);
		}
		/* Add external css style. */
		else if(config.css == 2){
			fprintf(revision->temp_file_to, "%s", CSS_EXTERN_STYLE_START);
			fprintf(revision->temp_file_to, "%s", config.css_name);
			fprintf(revision->temp_file_to, "%s", CSS_EXTERN_STYLE_END);
		}

		/* Reading while isn't end of file. */
		while(revision->character != EOF)
		{
			copy_word(revision);		/* Read word. */
			copy_whitespace(revision);	/* Read following white spaces. */
			
			/* Something was loaded. */
			if(revision->word[0] != 0 || revision->white_space[0] != 0)
			{
				/* Begining of new line => check if bordered non-bordered blocks. */
				if(new_line)
				{
					/* Starts or continues bordered block => skip this line. 
					 * Control if begening of line is equeal of sign (APPNED|DELETE)_MIDDLE or <div. */
					if(strstr(revision->word, "<div") || 
						!memcmp(revision->word, config.hl.append_middle, strlen(config.hl.append_middle)) || 
						!memcmp(revision->word, config.hl.delete_middle, strlen(config.hl.delete_middle)) || 
						!memcmp(revision->word, config.hl.change_middle, strlen(config.hl.change_middle)))
					{
						/* New line begin sign APPEND_MIDDLE => delete the sign from line. */
						if(!memcmp(revision->word, config.hl.append_middle, strlen(config.hl.append_middle))){
							strcpy(revision->word, &revision->word[strlen(config.hl.append_middle)]);
						}
						/* New line begin sign DELETE_MIDDLE => delete the sign from line. */
						else if(!memcmp(revision->word, config.hl.delete_middle, strlen(config.hl.delete_middle))){
							strcpy(revision->word, &revision->word[strlen(config.hl.delete_middle)]);
						}
						/* New line begin sign CHANGE_MIDDLE => delete the sign from line. */
						else if(!memcmp(revision->word, config.hl.change_middle, strlen(config.hl.change_middle))){
							strcpy(revision->word, &revision->word[strlen(config.hl.change_middle)]);
						}

						if(!faze)
						{
							fprintf(revision->temp_file_to, "%s", config.hl.blank_end);
							faze++;
						}
					}
					else{
						if(faze)
						{
							fprintf(revision->temp_file_to, "%s", config.hl.blank_start);
							faze--;
						}
						else
						{
							fprintf(revision->temp_file_to, "%s", config.hl.blank_middle);
						}
					}
				}
				
				/* Write non changed word to output. */
				fprintf(revision->temp_file_to, "%s", revision->word);
				
				/* If is new line => set sign new line for checking what kind line is. */
				if(revision->white_space[0] == '\n')
				{
					if(!faze){fprintf(revision->temp_file_to, "<br />");}
					new_line = 1;
				}
				else{new_line = 0;}
				
				/* Write non changed white-space to output. After posibile <br /> */
				fprintf(revision->temp_file_to, "%s", revision->white_space);
			}
		}

		/* Close temporary revision and finaly revision file. */
		if(fclose(revision->temp_file_from)){
			program_log(2, "Cannot close temporary revision file '%s'.", name_of_revision);
		}
		if(fclose(revision->temp_file_to)){
			program_log(2, "Cannot close finaly revision file '%s'.", &name_of_revision[4]);
		}

		program_log(4, "Deleting temporary revision file '%s'.", name_of_revision);

		/* Delting temporary revision file. */
		if(remove(name_of_revision) != 0){
			program_log(2, "Cannot delete temporary revision file '%s'.", name_of_revision);
		}
	}
	/* Revision file is renamed. And css style is appneded to the end of file. */
	else{
		/* Remove old revison file if exists. Because new revision file which is renamed. */
		remove(&name_of_revision[4]);

		/* Rename revision file - earese frist characters "tmp_". */
		rename(name_of_revision, &name_of_revision[4]);

		/* Open revision file. */
		if((revision->temp_file_to = fopen(&name_of_revision[4], "a")) == NULL)
		{
			program_log(1, "Cannot open file '%s' for last modification.", name_of_revision);
			return 1;
		}

		/* Add embedded css style. */
		if(config.css == 3){
			fprintf(revision->temp_file_to, "%s", CSS_STYLE);
		}
		/* Add external css style. */
		else if(config.css == 2){
			fprintf(revision->temp_file_to, "%s", CSS_EXTERN_STYLE_START);
			fprintf(revision->temp_file_to, "%s", config.css_name);
			fprintf(revision->temp_file_to, "%s", CSS_EXTERN_STYLE_END);
		}

		if(fclose(revision->temp_file_to)){
			program_log(2, "Cannot close revision file '%s'.", name_of_revision);
		}
	}

	return 0;
}
