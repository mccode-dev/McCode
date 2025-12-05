### Free-form text area
_Please describe what your PR is adding in terms of features or bugfixes:_



--------------
### Development OS / boundary conditions
_Please describe what OS you developed and tested your additions on, and if any special dependencies are required:_


--------------
# PR Checklist for contributing to McStas/McXtrace
## For a coherent and useful contribution to McStas/McXtrace, please fill in _relevant parts_ of the checklist:
* ### My contribution includes patches to an **existing component** file
  * [ ] I have used the `mcdoc` utility and **rendered** a reasonable documentation page for the component (please attach as screenshot in comments!)
  * [ ] I have ensured that basic use of the component is OK (e.g. an instrument using it compiles?)
  * [ ] I have used the `mctest` utility to **test** one or more instruments making use of the component (please attach `mcviewtest` report as screenshot in comments)
  * [ ] I have used the `mcrun --c-lint` "linter" and followed advice to remove most / all warnings that are raised
* ### My contribution includes patches to an **existing** instrument file
  * [ ] I have used the `mcdoc` utility and **rendered** a reasonable documentation page for the instrument (please attach as screenshot in comments!)
  * [ ] I have used the `mctest` utility to **test** the instrument (please attach `mcviewtest` report as screenshot in comments)
  * [ ] I have used the `mcrun --c-lint` "linter" and followed advice to remove most / all warnings that are raised
* ### My contribution includes a **new component** file
  * [ ] I have ensured that naming of parameters are in the style of existing components. (Please check the [McStas](https://github.com/mccode-dev/McCode/blob/main/mcstas-comps/NOMENCLATURE.md) or [McXtrace](https://github.com/mccode-dev/McCode/blob/main/mcxtrace-comps/NOMENCLATURE) NOMENCLATURE docs.)
  * [ ] I have ensured that component parameters are in the usually units of McStas or McXtrace (SI + neutron/x-ray 'usual' units)
  * [ ] I have used the `mcdoc` utility and **rendered** a reasonable documentation page for the component (please attach as screenshot in comments!)
  * [ ] I have ensured that basic use of the component is OK (e.g. an instrument using it compiles?)
  * [ ] I have included a corresponding **example** instrument and will fill in the **new instrument** section below
  * [ ] My new component is added within the `contrib` component category
* ### My contribution includes a **new instrument** file
  * [ ] I have used the `mcdoc` utility and **rendered** a reasonable documentation page for the instrument (please attach as screenshot in comments!)
  * [ ] I have ensured that basic use of the instrument is OK (e.g. it compiles?)
  * [ ] ... and provided reasonable default parameters in that instrument that produce reasonable output
  * [ ] ... and maybe even added a `%Example:` line to describe expected behaviour
  * [ ] I have used the `mcrun --c-lint` "linter" and followed advice to remove most / all warnings that are raised
  * [ ] My new instrument is added within the `examples` hierarchy in a folder in the style of `examples/ESS/New_stuff/New_stuff.instr`
  * [ ] My new instrument has a new, unique filename, not clashing with existing example instruments
  * [ ] My new instrument requires a data/input file. If the datafile is _specific_ for my instrument I have left it in the same `example` folder, but if general use I have placed it in the global `data` folder.
* ### My work touches the code-generator in mccode/src
  * [ ] I have added reasoning and documentation for the change through an ADR record in our [GRAMMAR](https://github.com/mccode-dev/McCode/tree/main/docs/GRAMMAR) section
  * [ ] I am attaching test output in the comments
* ### My work touches / adds to the runtime lib code (.c,.h etc in multiple locations
  * [ ] I am have added reasoning and documentation for the change below
  * [ ] I am attaching test output in the comments
* ### My PR is meant to fix a specific, existing issue
  * [ ] I have indicated the issue number here:
  * [ ] I have added documentation for the fix and possible side effects
* ### My contribution contains something else
  * [ ] Explanation is added in free form text above or below the checklist

--------------



