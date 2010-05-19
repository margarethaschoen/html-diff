/*
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
 * @file fce.h
 * @brief Header for fce.c (Library of function for program).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include <ctype.h>
#include "config.h"

/* Make avalible configuration. */
extern CONFIGURATION config;

/* Log message to standard input (or to file) depend on loging level. */
void program_log(int, const char *, const char *);

/* Set default value to struct SIDE. */
int new_side(SIDE *);

/* Read sequence of white-space from file. */
int copy_whitespace(SIDE *);

/* Read sequence of non-white-space (word) from file. */
int copy_word(SIDE *);

/* Chang string to lower-case. */
char* strtolower(char *);

/* Test tag if is watched it's changing. */
int watched_tag(char *);

/* Parse input string to operation sturct. */
OPERATION take_op(char *);

/* Cut first sequence of number from input string. */
int take_int(char **);

/* Test if character is number. */
int is_digit(char);

/* Generate path and temporary file name. */
char *my_tempnam(char *);
