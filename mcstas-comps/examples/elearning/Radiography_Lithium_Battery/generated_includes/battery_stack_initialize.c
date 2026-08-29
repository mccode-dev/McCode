// Start of initialize for generated battery_stack
positive_foil_offset_um = seperator_thickness_um + positive_electrode_thickness_um + 0.5*positive_foil_thickness_um;
negative_foil_offset_um = 2.0*seperator_thickness_um + 2.0*positive_electrode_thickness_um + positive_foil_thickness_um + negative_electrode_thickness_um + 0.5*negative_foil_thickness_um;
positive_foil_offset_m = positive_foil_offset_um * 1E-6;
negative_foil_offset_m = negative_foil_offset_um * 1E-6;
for (counter=0;counter<number_of_layers;counter++) {
  positive_foil_center[counter] = positive_foil_offset_m + (double)counter*thickness_one_layer_m;
  negative_foil_center[counter] = negative_foil_offset_m + (double)counter*thickness_one_layer_m;
}
