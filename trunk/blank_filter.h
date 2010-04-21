/** 
 * @file blank_filter.h
 * @brief Header for blank_filter.c (blank pattern for new filter).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "config.h"
#include "fce.h"

/* Make avalible configuration. For highlihgts tags and type of comparation. */
extern CONFIGURATION config;

/* Blank filtr for preprocessing. */
int blank_filter(SIDE*);

/* Blank filtr for postprocessing. */
int blank_filter_re(SIDE*);
