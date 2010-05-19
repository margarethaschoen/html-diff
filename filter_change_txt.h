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
 * @file filter_change_txt.h
 * @brief Header for filter_change_txt.c (filter for changing change operation in patch file for shown text level comparation).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/* Make avalible configuration. For recongizing of type comparation and highlights tags. */
extern CONFIGURATION config;

/* Highligh operation change in patch file on shown text level. */
int filter_change_txt(SIDE*);

/* Highligh operation append on word level comparation. */
int word_by_word_txt(SIDE*);

/* Highligh operation append on line level comparation. */
int line_by_line_txt(SIDE*);

/* Create new second part of change operation from patch file of differences first and second part changed block. */
int change_block_compare_txt(SIDE*, SIDE*, SIDE*);
