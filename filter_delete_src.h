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
 * @file filter_delete_src.h
 * @brief Header for filter_delete_src.c (filter for changing delete operation in patch file for source level comparation).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/* Make avalible configuration. For highlihgts tags. */
extern CONFIGURATION config;

/* Highligh operation delete in patch file. */
int filter_delete_src(SIDE*);
