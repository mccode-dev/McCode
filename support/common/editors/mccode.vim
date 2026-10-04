"" Vim syntax file for writing McStas and McXtrace instruments
"" Author: Erik B Knudsen erkn@fysik.dtu.dk
"" version 0.2 (sep 2014) 

"" insert something like:
"" augroup mccode_instr
""  autocmd BufRead,BufNewFile *.instr,*.comp setfiletype mccode 
"" augroup END
""  in your .vimrc file (_vimrc for windows) and put this file in the 
""  .vim/syntax/ directory (or equivalent) and it should be loaded
""  when opening a *.instr (or *.comp) -file

" McStas/McXtrace instruments are mostly c syntax so start with that
:runtime! syntax/c.vim
:setlocal commentstring=//\ %s
"keywords for mcstas statements
:syntax keyword instrStatement DECLARE DEPENDENCY DEFINE END FINALLY INITIALIZE MCDISPLAY SAVE SHARE USERVARS
:syntax keyword instrStatement TRACE DEFINITION PARAMETERS POLARISATION SETTING STATE
:syntax keyword instrStatement OUTPUT INSTRUMENT include
:highlight link instrStatement Statement

"mcstas runtime keywords
:syntax keyword instrRuntime
	\ ABSORB 
	\ SCATTERED 
	\ COMP_GETPAR3 
	\ COMP_GETPAR 
	\ DETECTOR_OUT 
	\ DETECTOR_OUT_0D 
	\ DETECTOR_OUT_1D 
	\ DETECTOR_OUT_2D 
	\ DETECTOR_OUT_3D 
	\ NAME_CURRENT_COMP 
	\ INDEX_CURRENT_COMP 
	\ POS_A_CURRENT_COMP 
	\ POS_R_CURRENT_COMP 
	\ ROT_A_CURRENT_COMP 
	\ ROT_R_CURRENT_COMP 
	\ RAD2MIN 
	\ MIN2RAD 
	\ DEG2RAD 
	\ RAD2DEG 
	\ K2V 
	\ V2K 
	\ Q2V 
	\ V2Q 
	\ SE2V 
	\ VS2E 
	\ FWHM2RMS 
	\ RMS2FWHM 
	\ HBAR 
	\ MNEUTRON 
	\ PI 
	\ POS_A_COMP_INDEX 
	\ POS_R_COMP_INDEX 
	\ NAME_COMP 
	\ POS_A_COMP 
	\ POS_R_COMP 
	\ ROT_A_COMP 
	\ ROT_R_COMP 
	\ SCATTER 
	\ STORE_XRAY 
	\ RESTORE_XRAY 
	\ STORE_NEUTRON
	\ RESTORE_NEUTRON 
	\ PROP_GRAV_DT 
	\ PROP_DT 
	\ PROP_NO_G_DT
        \ PROP_DL
	\ PROP_Z0 
	\ PROP_X0 
	\ PROP_Y0 
	\ PROP_NO_G_Z0
	\ PROP_NO_G_X0
	\ PROP_NO_G_Y0
	\ vec_prod 
	\ scalar_prod 
	\ NORM 
	\ rotate 
	\ rand01 
	\ randpm1 
	\ rand0max 
	\ randminmax 
	\ normal_vec
        \ solve_2nd_order
	\ box_intersect 
	\ cylinder_intersect 
	\ ellipsoid_intersect 
	\ sphere_intersect
        \ off_intersect
        \ off_intersect_x
	\ randvec_target_circle 
	\ randvec_target_rect_angular 
	\ randvec_target_rect 
:highlight link instrRuntime Constant

"mcstas trace keywords -these should ideally only be highlighted when within
"the TRACE region or something
:syntax keyword instrTrace ABSOLUTE AT COMPONENT EXTEND GROUP PREVIOUS NEXT MYSELF RELATIVE ROTATED WHEN JUMP ITERATE SPLIT COPY
highlight link instrTrace Statement

"mcstas type keywords
:syntax keyword instrType auto char const double float int long register restrict short signed static unsigned void volatile _Imaginary _Complex _Bool
highlight link instrType Type

"mcstas attention keywords 
:syntax keyword instrATT FIXME TODO ### Identification Description Parameters Link
:highlight link instrATT Todo
"here the insertion menus could be put
