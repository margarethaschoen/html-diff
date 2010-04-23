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
