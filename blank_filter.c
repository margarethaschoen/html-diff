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
