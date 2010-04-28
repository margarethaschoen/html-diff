/** 
 * @file filter_append_src.h
 * @brief Header for filter_append_src.c (filter for changing append operation in patch file for source level comparation).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/* Make avalible configuration. For highlihgts tags. */
extern CONFIGURATION config;

/* Highligh operation append in patch file. */
int filter_append_src(SIDE*);