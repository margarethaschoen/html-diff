/** 
 * @file blank_filter.c
 * @brief Blank pattern for new filter.
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 *
 */

#include "blank_filter.h"

/**
 * Blank filtr for preprocessing.
 * @param side Struct SIDE with input file.
 * @return int Sign if preprocessing was ok.
 */
int blank_filter(SIDE* side)
{
	/* Read file by word to end. */
	while(side->character != EOF)
	{
		/* Read word and white spaces. */
		copy_word(side);
		copy_whitespace(side);
	}

	return 0;
}

/**
 * Blank filtr for postprocessing.
 * @param side Struct SIDE with input patchfile.
 * @return int Sign if postprocessing was ok.
 */
int blank_filter_re(SIDE* side)
{
	/* Read file by word to end. */
	while(side->character != EOF)
	{
		/* Read word and white spaces. */
		copy_word(side);
		copy_whitespace(side);
	}

	return 0;
}
