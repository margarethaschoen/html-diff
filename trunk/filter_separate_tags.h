/** 
 * @file filter_separate_tags.h
 * @brief Header for filter_separate_tags.c (filter for separate HTML tags on lines).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "config.h"
#include "fce.h"

/* Make avalible configuration. For recongizing of type comparation. */
extern CONFIGURATION config;

/* Separate all HTML tags on lines. */
int filter_separate_tags(SIDE*);
