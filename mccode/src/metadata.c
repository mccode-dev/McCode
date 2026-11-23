//
// Created by gst on 07/07/23.
//
#include <string.h>
#include "mccode.h"


/* post-parsing, combine the list(s) of metadata strings ********************* */
/** @brief Assign component metadata to the instrument metadata list
 *
 * @param inst the instrument_definition
 * @param instr_metadata instrument-level METADATA, keyed by the instrument name
 * @return 0 if successful, non-zero if not
 */

int metadata_construct_table(instr_ptr_t inst, List instr_metadata){
  Symtab_handle lit_iter;
  struct Symtab_entry * lit_entry;
  struct comp_inst * comp_ptr;
  struct metadata_struct * l_ptr;
  char sep[3] = "::\0";
  char * key = malloc(2048 * sizeof(char));
  char * start = malloc(1024 * sizeof(char));
  Symtab lit_map = symtab_create();
  int added = 0;

  if (inst->compmap == NULL) return 1;
  if (inst->metadata == NULL || list_undef(inst->metadata)) {
    fprintf(stderr, "No metadata to check because its null!\n");
    return 1;
  }

  // iterate through all components
  for (int c=0; c<list_len(inst->complist); ++c) {
    comp_ptr = (struct comp_inst *) list_access(inst->complist, c);
    if (list_undef(comp_ptr->metadata)){
      printf("How did component %s get produced without metadata?\n", comp_ptr->name);
      continue;
    }

    strcpy(start, comp_ptr->name);
    strcat(start, sep);

    for (int i=0; i<list_len(comp_ptr->metadata); ++i){
      // get a pointer to the next metadata structure
      l_ptr = (struct metadata_struct*) list_access(comp_ptr->metadata, i);
      // Assign the component instance name, which has not been set yet
      if (l_ptr->source == NULL) l_ptr->source = comp_ptr->name;
      // construct the key for insertion into the symbol table
      strcpy(key, start);
      strcat(key, l_ptr->name); // using the McCode str_cat caused a segmentation fault?!
      // check if the same-keyed metadata was already provided
      if ((lit_entry = symtab_lookup(lit_map, key))){
        // This key already exists! What do we do? Trust users to know what they're doing!
        // Component-definition metadata came before Component-declaration metadata -- so overwrite!
        lit_entry->val = (void *) l_ptr;
      } else {
        symtab_add(lit_map, key, (void *) l_ptr);
      }
    }
  }
  // instrument-level metadata, added last as in mccode-antlr; a later duplicate key wins
  for (int i=0; i<list_len(instr_metadata); ++i){
    l_ptr = (struct metadata_struct*) list_access(instr_metadata, i);
    l_ptr->source = inst->name;
    strcpy(key, inst->name);
    strcat(key, sep);
    strcat(key, l_ptr->name);
    if ((lit_entry = symtab_lookup(lit_map, key))) lit_entry->val = (void *) l_ptr;
    else symtab_add(lit_map, key, (void *) l_ptr);
  }
  // str_cat allocated new memory, and symtab_add only copied the key; so free it now
  if (key) free(key);
  if (start) free(start);

  // metadata table construction complete, now copy it to the instrument metadata list:
  inst->metadata = list_create();
  lit_iter = symtab_iterate(lit_map);
  while ((lit_entry = symtab_next(lit_iter))){
    list_add(inst->metadata, lit_entry->val);
  }
  symtab_iterate_end(lit_iter);

  // Free the Symtab *but not the metadata structures pointed at!
  symtab_free(lit_map, NULL);
  return 0;
}

static void _metadata_assign_source(List metadata, char * source, int overwrite){
  struct metadata_struct * l_ptr;
  for (int i=0; i<list_len(metadata); ++i){
    l_ptr = (struct metadata_struct *) list_access(metadata, i);
    if (overwrite || l_ptr->source == NULL) {
      l_ptr->source = str_dup(source);
    } else {
      fprintf(stderr, "metadata already has source defined as %s!\n", l_ptr->source);
    }
  }
}

static void _metadata_assign_instance_field(List metadata, int value){
  struct metadata_struct * l_ptr;
  for (int i=0; i<list_len(metadata); ++i){
    l_ptr = (struct metadata_struct *) list_access(metadata, i);
    l_ptr->instance = value;
  }
}

void metadata_assign_source(List metadata, char * source){
  _metadata_assign_source(metadata, source, 0);
}
void metadata_overwrite_source(List metadata, char * source){
  _metadata_assign_source(metadata, source, 1);
}

void metadata_assign_from_definition(List metadata){
  _metadata_assign_instance_field(metadata, 0);
}
void metadata_assign_from_instance(List metadata){
  _metadata_assign_instance_field(metadata, 1);
}


Symtab metadata_separate_by_source(List metadata, int source_type){
  Symtab sources = symtab_create();
  struct Symtab_entry * s_ptr;
  struct metadata_struct * l_ptr;
  for (int i=0; i<list_len(metadata); ++i){
    l_ptr = list_access(metadata, i);
    if (source_type == l_ptr->instance) {
      s_ptr = symtab_lookup(sources, l_ptr->source == NULL ? "null" : l_ptr->source);
      if (s_ptr == NULL) {
        s_ptr = symtab_add(sources, l_ptr->source == NULL ? "null" : l_ptr->source, (void *) list_create());
      }
      list_add(s_ptr->val, l_ptr);
    }
  }
  return sources;
}

struct metadata_struct * metadata_copy(struct metadata_struct * from){
  struct metadata_struct * to;
  palloc(to);
  to->instance = from->instance;
  to->source = from->source == NULL ? NULL : strdup(from->source);
  to->name = from->name == NULL ? NULL : strdup(from->name);
  to->type = from->type == NULL ? NULL : strdup(from->type);
  // No need to *copy* the lines themselves, since they're not modified at any point.
  to->lines = list_create();
  if (list_len(from->lines)) list_cat(to->lines, from->lines);
  return to;
}

void * _void_metadata_copy_void(void * from){
  void * to = (void *) metadata_copy((struct metadata_struct *) from);
  return to;
}

List metadata_list_copy(List from){
  return list_copy(from, _void_metadata_copy_void);
}