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
 * @file filter_separate_tags.h
 * @brief Header for filter_separate_tags.c (filter for separate HTML tags on lines).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/* Make avalible configuration. For recongizing of type comparation. */
extern CONFIGURATION config;

/* Separate all HTML tags on lines. */
int filter_separate_tags(SIDE*);
