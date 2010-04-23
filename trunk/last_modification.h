/** 
 * @file last_modification.h
 * @brief Header for last_modification.c (last modification of revision).
 *
 * \image html logo3.jpg
 * @author Ladislav Novak
 * @date 2008-2010
 * @version 1.0
 */

#include "fce.h"

/* Make avalible configuration. For recongizing of type comparation. */
extern CONFIGURATION config;

/* Process last modification of revision file. */
int last_modification(char *);
