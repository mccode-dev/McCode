//
// Created by gst on 07/07/23.
//

#ifndef MCCODE_metadata_R_H
#define MCCODE_metadata_R_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct metadata_table_struct { /* stores literal strings from components */
  char * source;  // component name which provided the literal string
  char * name;  // the name of the literal
  char * type;  // the type of the literal (free form, valid identifier)
  char * value;  // the literal string contents
};
typedef struct metadata_table_struct metadata_table_t;

/** \brief Extract the `source` part of a "source:metadata_name" key,
 *
 * @param key Ideally "source:metadata_name" but "" or "source" handled as well
 * @return the "source" part of the key, or "" if there is none (never `NULL`)
 * @note A non-null returned `char *` was allocated in the function, please handle `free`ing it when finished.
*/
char * metadata_table_key_component(char* key);

/** \brief Extract the `metadata_name` part of a "source:metadata_name" key,
 *
 * @param key Ideally "source:metadata_name" but "" or "source" handled as well
 * @return `NULL` if there is no "metadata_name" part of the key
 * @note A non-null returned `char *` was allocated in the function, please handle `free`ing it when finished.
*/
char * metadata_table_key_literal(char * key);

/** \brief Search through the provided metadata table for the provided key
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be searched
 * @param key one of "source:metadata_name", "source", or ""
 * @return the total number of matching metadata table entries (see note)
 * @note If the `key` is empty it 'matches' no entries so 0 is returned
 *       If the key is only a source name, the number of entries defined by that source is returned
 *       If the key is "source:metadata_name" the return value is 0 or 1.
 */
int metadata_table_defined(int metadata_count, metadata_table_t * metadata_table, char * key);

/** \brief Return the first matching metadata entry key name for the provided source
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be searched
 * @param key one of "source" or "source:metadata_name"
 * @return the first "metadata_name" matching for `key`, or a null pointer if this is no match
 * @note if both "source" and "metadata_name" are provided in `key` the returned value will be "metadata_name" or NULL
 */
char * metadata_table_name(int metadata_count, metadata_table_t * metadata_table, char * key);

/** \brief Return the "type" stored for the specified key in the provided metadata table
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be searched
 * @param key "source:metadata_name"
 * @return A pointer to the tabulated "type" entry -- DO NOT FREE.
 */
char * metadata_table_type(int metadata_count, metadata_table_t * metadata_table, char * key);

/** \brief Return the metadata value stored for the specified key in the provided metadata table
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be searched
 * @param key "source:metadata_name"
 * @return A pointer to the tabulated metadata value entry -- DO NOT FREE.
 */
char * metadata_table_literal(int metadata_count, metadata_table_t * metadata_table, char * key);

/** \brief Print all stored metadata table keys to standard output
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be printed
 */
void metadata_table_print_all_keys(int metadata_count, metadata_table_t * metadata_table);

/** \brief Print the names of all metadata-defining components to standard output
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be printed
 * @return the number of metadata-defining components
 */
int metadata_table_print_all_components(int metadata_count, metadata_table_t * metadata_table);

/** \brief Print the (partial) matching key(s) for one component to standard output
 *
 * @param metadata_count the number of metadata table entries
 * @param metadata_table the table to be searched
 * @param key one of "source:metadata_name", "source"
 * @return The number of matching keys
 * @note If the key has no metadata name part, all component "source" defined keys are printed
 *       and their count is the number returned. Otherwise the return value is 0 or 1.
 */
int metadata_table_print_component_keys(int metadata_count, metadata_table_t * metadata_table, char * key);

#endif //MCCODE_metadata_R_H
