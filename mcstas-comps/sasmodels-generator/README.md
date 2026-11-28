# Description

The `sasmodels2mcstas.py` contains three main methods that create:

- c files from `models` and `other_models` folder.
- component files from the generated c files.
- test folders from the generated component files.

You may add new models to the `other_models` folder. the kernel_header.c
file is needed as a prefix for all .c files, and all folders in this
directory should keep their names.

For the moment, 93 components are created from 72 SAS models, 21 of
which also have an anisotropic (`_aniso`) description.

# Run 
In mcstas-environment run 

`python sasmodels2mcstas.py .`

By default, it will create a `generated_mcstas_models` folder in which all
c files will be stored. An `indiv_comps` folder with all the individual 
model components and a test folder with one test per model will also be created.
The C code of the components is formatted with `devel/bin/mccode-clangformat`,
which needs `clang-format`.

The generated components replace those in `../sasmodels`, except
`SasView_rectangular_prism`: the sasmodels `rectangular_prism` kernel no
longer has a 1D `Iq`, so only the `_aniso` component is generated and the
1D one is maintained by hand.

## TODO
The vector-input models `onion`, `core_multi_shell` and `rpa` are created
with `vector` parameters and compile, but their results have not been
validated against SasView. Vector parameters have no polydispersity.
`spherical_sld` is not created, as no kernel function is detected for it.
