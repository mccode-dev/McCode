#!/usr/bin/env python3
"""SAS constructor from sasmodels for McXtrace

Requires one input argument, where necessary files are located.
To work, it needs:
    - models: folder from sasmodels/sasmodels/models ,
    - other_models: any other models added by the user,
    - kernel_header.c: generic header file for all models.
If there foldes and file are in the same standing folder together
with sasmodels2mcxtrace.py, then run:
    python sasmodels2mcxtrace.py .

This parser performs 3 main tasks, each in an independent method:
    - generate_mcxtrace_models() : creates .c files for each model.
    - generate_indiv_files(OUTPUT_DIR) creates .comp files for each model.
    - generate_tests() creates test folder for each model.

For the moment, vector input models are created but are not working.
These include:
    - core_multi_shell
    - rpa
    - onion
    - spherical_sld
"""

import os
import re
import importlib
import sys
import shutil

# Define where to find everything needed.
DATA_PATH = sys.argv[1]
MODELS_PATH = os.path.join(DATA_PATH, "models")
OTHER_MODELS_PATH = os.path.join(DATA_PATH, "other_models")

# Define where to output sasmodels and tests
OUTPUT_DIR = "generated_mcxtrace_models"
append_to_save = "tests/test_"


# Get model names to be used globaly in this script
MODELS = [fname.split(".c")[0] for fname in os.listdir(MODELS_PATH) if ".c" in fname]
print(len(MODELS), "models.")

_template_cache = {}  # type: Dict[str, Tuple[int, str, str]]


def load_template(filename):
    """Loader function with cache."""
    global DATA_PATH
    # type: (str) -> str
    """
    Load template file from sasmodels resource directory.
    """
    path = os.path.join(DATA_PATH, filename)
    mtime = os.path.getmtime(path)
    if filename not in _template_cache or mtime > _template_cache[filename][0]:
        with open(path) as fid:
            _template_cache[filename] = (mtime, fid.read(), path)
    return _template_cache[filename][1], path


def _clean_source_filename(path):
    # type: (str) -> str
    """
    Make the source filename into a canonical, relative form if possible

    Remove the common start of the file path (if there is one), yielding
    the path relative to this file, such as:

     - ./models/sphere.c
     - ./models/lib/sas_J0.c

    This is a format that the compiler/debugger understand for indicating
    included files with relative paths. Omitting the common parent to the
    paths means that the irrelevant detail of the temporary directory where
    the source was unpacked for compilation is not included in pre-compiled
    models.
    """
    base = os.path.basename(path)
    (base, ext) = os.path.splitext(base)
    return base


def _add_source(source, code, path, lineno=1):
    """
    Add a file to the list of source code chunks, tagged with path and line.
    """
    path = _clean_source_filename(path)
    source.append('\n#ifndef SAS_HAVE_%s' % path)
    source.append('#define SAS_HAVE_%s\n' % path)
    source.append('#line %d "%s"' % (lineno, path))
    source.append(code)
    source.append('\n#endif // SAS_HAVE_%s\n' % path)


def get_source_files(module_name):
    """Find source files from python script in sasmodels models folder."""
    source = {}
    module = importlib.import_module("models." + module_name)
    if hasattr(module, "source"):
        source[module_name] = module.source
    else:
        source[module_name] = [module_name + ".c"]
    del module
    return source


def get_params(module_name):
    """Find parameters from python script in sasmodels models folder.

    Returns:
    - param_vals: default values of parameters in SasView.
    - angle_vals: default values of orientational parameters in Sasview.
    - metadata: extra data from python file: range, description, name"""
    module = importlib.import_module("models." + module_name)
    if hasattr(module, "parameters"):
        params = module.parameters
    del module
    param_vals = []
    angle_vals = []
    metadata = []
    for param_list in params:
        if param_list[0] in ["theta", "phi", "Psi", "psi"]:
            # Save angle if anisotropic
            angle_vals.append(param_list[2])
        else:
            # Save general parameters
            param_vals.append((param_list[2], param_list[0]))
        # Save parameter range and description
        metadata.append([param_list[3], param_list[5], param_list[1]])
    return param_vals, angle_vals, metadata


def search_functions(total_content):
    """Check what type of function is contained in the C file."""
    funcs = [0 for i in range(0, 4)]
    names = ["" for i in range(0, 4)]
    if "#define HAS_FQ\n" in total_content:
        funcs[0] = 1
        names[0] = "Fq"
    if "#define HAS_Iq\n" in total_content:
        funcs[1] = 1
        names[1] = "Iq"
    if "#define HAS_Iqac\n" in total_content:
        funcs[2] = 1
        names[2] = "Iqac"
    if "#define HAS_Iqabc\n" in total_content:
        funcs[3] = 1
        names[3] = "Iqabc"
    return funcs, names


def __getFormVolumeSign(text):
    """Retrieve the FormVolume function from the C file."""
    # get all cases covered
    define_str_ver1 = r"#define\s+VOLUME_PARAMETER_DECLARATIONS\s+([\w\s,]*\n)"
    sign_str = r"\w+\s+form_volume\(([\w\s,]*)\)"
    sign_str_2 = r"\w+\s+form_volume\(([\w\s,\[\]]*)\)"

    # NOTE: sometimes VOLUME_PARAMETER_DECLARATIONS turns out "void"

    # logics to extract signature
    m = re.search(define_str_ver1, text)
    if m:
        # entire non-q sign is contained in the define
        sign = re.sub("\\s+", " ", m.group(1))
        sign = sign.strip(" ")
        return sign
    else:
        # entire sign is contained in the function declaration
        m = re.search(sign_str, text)
        if not m:
            m = re.search(sign_str_2, text)
        if m:
            sign = re.sub("\\s+", " ", m.group(1))
            sign = sign.strip(" ")
            sign = re.sub("float", "", sign)
            sign = re.sub("double", "", sign)
            sign = re.sub("\\s+", " ", sign)
            Hint = sign.replace(" ", "").split(",")
            return Hint
        else:
            return []


def generate_mcxtrace_models():
    """Main function, generates the .c files for each model."""
    model_status = {}
    for model_name in MODELS:
        if model_name[0] != "_":
            source_files = get_source_files(model_name)
            user_code = [
                load_template(os.path.join(MODELS_PATH, model_code))
                for model_code in source_files[model_name]
            ]

            source = []

            for ucode, loc_ucode in user_code:
                _add_source(source, ucode, loc_ucode)
                code = "\n".join(source)
                fname = loc_ucode.split("/")[-1]
            # print(fname)
            status = 0
            if "form_volume(" in code:
                code = "\n".join(["#define FORM_VOL", code])
                code = code.replace('form_volume(', 'form_volume_' + model_name + '(')
            if ("Fq(" in code) and ("Iq(" in code):
                code = code.replace('Fq(', 'Fq_' + model_name + '(')
                code = code.replace('Iq(', 'Iq_' + model_name + '(')
                pass
            elif ("Fq(" in code) and ("Iq(" not in code):
                code = "\n".join(["#define HAS_FQ", code])
                code = code.replace('Fq(', 'Fq_' + model_name + '(')
                status = "Fq"
            if "Iq(" in code:
                code = "\n".join(["#define HAS_Iq", code])
                code = code.replace('Iq(', 'Iq_' + model_name + '(')
                status = "Iq"
            if "Iqac(" in code:
                code = "\n".join(["#define HAS_Iqac", code])
                code = code.replace('Iqac(', 'Iqac_' + model_name + '(')
                status = "Iqac"
            if "Iqabc(" in code:
                code = "\n".join(["#define HAS_Iqabc", code])
                code = code.replace('Iqabc(', 'Iqabc_' + model_name + '(')
                status = "Iqabc"

            if "radius_effective(" in code:
                code = code.replace('radius_effective(', 'radius_effective_' + model_name + '(')

            if "radius_from_crosssection(" in code:
                code = code.replace('radius_from_crosssection(', 'radius_from_crosssection_' + model_name + '(')

            if "radius_from_curvature(" in code:
                code = code.replace('radius_from_curvature(', 'radius_from_curvature_' + model_name + '(')

            if "radius_from_diagonal(" in code:
                code = code.replace('radius_from_diagonal(', 'radius_from_diagonal_' + model_name + '(')

            if "radius_from_excluded_volume(" in code:
                code = code.replace('radius_from_excluded_volume(', 'radius_from_excluded_volume_' + model_name + '(')

            if "radius_from_max_dimension(" in code:
                code = code.replace('radius_from_max_dimension(', 'radius_from_max_dimension_' + model_name + '(')

            if "radius_from_min_dimension(" in code:
                code = code.replace('radius_from_min_dimension(', 'radius_from_min_dimension_' + model_name + '(')

            if "radius_from_totallength(" in code:
                code = code.replace('radius_from_totallength(', 'radius_from_totallength_' + model_name + '(')

            if "radius_from_volume(" in code:
                code = code.replace('radius_from_volume(', 'radius_from_volume_' + model_name + '(')

            if not os.path.isdir("generated_mcxtrace_models"):
                os.mkdir("generated_mcxtrace_models")
            with open("_".join(["generated_mcxtrace_models/sas", fname]), "w") as f:
                f.writelines(code)
                f.writelines("\n\n")
            model_status[model_name] = status
    return model_status


def generate_indiv_files():
    """Main function. Creates the components for McXtrace using the .c
    files generated with generate_mcxtrace_models()."""
    # Common content to be added at the end
    final_content = """
    SCATTER;
  }
%}

MCDISPLAY
%{

  if (shape == 0) {	/* cylinder */
    cylinder(0, 0, 0, R, yheight, 0, 0, 1, 0);
  }
  else if (shape == 1) { 	/* box */
    box(0, 0, 0, xwidth, yheight, zdepth, 0, 0, 1, 0);
  }
  else if (shape == 2) {	/* sphere */
    sphere(0, 0, 0, R);
  }
%}
END\n\n"""

    for i in range(len(MODELS)):
        with open("generated_mcxtrace_models/sas_" + MODELS[i] + ".c", "r") as fcode:
            lines = fcode.readlines()
            total_content = "".join(lines)
            vol_pars = __getFormVolumeSign(total_content)
            _funcs, names = search_functions(total_content)
            params, angles, metadata = get_params(MODELS[i])

            # Create function specific variables.
            content_func = {}
            found_funcs = [namesi for namesi in names if namesi != ""]
            for ftype in found_funcs:
                if ftype in ["Fq", "Iq"]:
                    suffix = ""
                elif ftype in ["Iqac", "Iqabc"]:
                    suffix = "_aniso"

                func_param_str = ""
                content2 = ""
                content = ""
                content_pd = ""
                content_pd0 = ""
                pd_pars = []
                pd_def = ""
                pd_init = "\n"
                pd_condition = "    if ("
                n_pd_params = 0
                for (val, name), meta in zip(params, metadata):
                    if MODELS[i] == "elliptical_cylinder" and name == "axis_ratio":
                        name = "r_ratio"
                    content += f"* {name}: [{meta[2]}] ({meta[0]}) {meta[1]}.\n"
                    if "[" in name:
                        content2 += f"      vector {name}=" + "{" + f"{val}" + "},\n"
                    else:
                        content2 += f"      {name}={val},\n"
                    func_param_str += f", {name}"

                    # include polidispersity
                    for pd_par in ["rg", "length", "thick", "radius"]:
                        if pd_par in name:
                            pd_pars.append(name)
                            n_pd_params += 1
                            content_pd += f"    trace_{name} = (randnorm()*pd_{name}+1.0)*{name};\n"
                            content_pd0 += f"    double trace_{name}={name};\n"
                            pd_condition += f" pd_{name}!=0.0 ||"
                            pd_def += f"* pd_{name}: [] (0,inf) defined as (dx/x), where x is de mean value and dx the standard devition of the variable.\n"
                            pd_init += f"        pd_{name}=0.0,\n"
                            break

                pd_condition = pd_condition[:-2] + "){\n"

                content2 += "\n"
                pd_angles = []
                content_pd2 = ""
                content_pd20 = ""
                pd_condition_ang = "    if ("
                n_pd_angles = 0
                if ftype in ["Iqac", "Iqabc"]:
                    for angle, name in zip(angles, ["theta", "Phi", "Psi"]):
                        content2 += f"      {name}={angle},\n"
                        func_param_str += f", {name}"
                        pd_angles.append(name)
                        content_pd2 += f"    trace_{name} = ((rand01()-0.5)*pd_{name} + 1.0)*{name};\n    d{name} = trace_{name}-{name};\n"
                        content_pd20 += (
                            f"    double trace_{name}={name}, d{name}=0.0;\n"
                        )
                        pd_def += f"* pd_{name}: [] (0,360) defined as (dx/x), where x is de mean value and dx the standard devition of the variable.\n"
                        pd_init += f"        pd_{name}=0.0,\n"
                        pd_condition_ang += f" pd_{name}!=0.0 ||"
                        n_pd_angles += 1

                pd_def = pd_def[:-2]
                pd_init = pd_init[:-2]
                pd_condition_ang = pd_condition_ang[:-2] + "){\n"

                pd_oneliner = ''.join(pd_init)
                pd_oneliner = pd_oneliner.replace("\n", "")
                pd_oneliner = pd_oneliner.replace("\t", " ")
                pd_oneliner = pd_oneliner.replace(" ", "")
                pd_oneliner = pd_oneliner.replace(",", ", ")
                # Add model header.
                content_func[
                    ftype
                ] = f"""/*******************************************************************************
*
* McXtrace, X-ray tracing package
*         Copyright, All rights reserved
*         DTU Physics, Kgs. Lyngby, Denmark
*         Synchrotron SOLEIL, Saint-Aubin, France
*
* Component: SasView_{MODELS[i]}
*
* %Identification
* Written by: Jose Robledo
* Based on sasmodels from SasView
* Origin: FZJ / DTU / ESS DMSC
*
*
* SasView {MODELS[i]} model component as sample description.
*
* %Description
*
* SasView_{MODELS[i]} component, generated from {MODELS[i]}.c in sasmodels.
*
* Example: 
*  SasView_{MODELS[i]}{suffix}({func_param_str[2:]}, 
*     model_scale=1.0, model_abs=0.0, xwidth=0.01, yheight=0.01, zdepth=0.005, R=0, 
*     int target_index=1, target_x=0, target_y=0, target_z=1,
*     focus_xw=0.5, focus_yh=0.5, focus_aw=0, focus_ah=0, focus_r=0, 
*     {pd_oneliner})
*
* %Parameters
* INPUT PARAMETERS:
"""
                # Add description of variables.
                content_func[ftype] += content

                # Add common variable descriptions.
                content_func[
                    ftype
                ] += f"""* Optional parameters:
* model_abs: [ ] Absorption cross section density at 2200 m/s.
* model_scale: [ ] Global scale factor for scattering kernel. For systems without inter-particle interference, the form factors can be related to the scattering intensity by the particle volume fraction.
* xwidth: [m] ([-inf, inf]) Horiz. dimension of sample, as a width.
* yheight: [m] ([-inf, inf]) vert . dimension of sample, as a height for cylinder/box
* zdepth: [m] ([-inf, inf]) depth of sample
* R: [m] Outer radius of sample in (x,z) plane for cylinder/sphere.
* target_x: [m] relative focus target position.
* target_y: [m] relative focus target position.
* target_z: [m] relative focus target position.
* target_index: [ ] Relative index of component to focus at, e.g. next is +1.
* focus_xw: [m] horiz. dimension of a rectangular area.
* focus_yh: [m], vert. dimension of a rectangular area.
* focus_aw: [deg], horiz. angular dimension of a rectangular area.
* focus_ah: [deg], vert. angular dimension of a rectangular area.
* focus_r: [m] case of circular focusing, focusing radius.
{pd_def}
*
* %Link
* %End
*******************************************************************************/
"""
                func_param_str = ""
                content_func[
                    ftype
                ] += f"""DEFINE COMPONENT SasView_{MODELS[i]}{suffix}
DEFINITION PARAMETERS ()
SETTING PARAMETERS (\n"""
                for val, name in params:
                    if "[" in name:
                        content_func[ftype] += (
                            f"        vector {name}=" + "{" + f"{val}" + "},\n"
                        )
                    else:
                        if MODELS[i] == "elliptical_cylinder" and name == "axis_ratio":
                            name = "r_ratio"
                        content_func[ftype] += f"        {name}={val},\n"
                    func_param_str += f", {name}"

                if ftype in ["Iqac", "Iqabc"]:
                    for angle, name in zip(angles, ["theta", "Phi", "Psi"]):
                        content_func[ftype] += f"        {name}={angle},\n"
                        func_param_str += f", {name}"

                vol_par_str = []

                for vol_par in vol_pars:
                    # Solve specific cases
                    if "[]" in vol_par:
                        vol_par = vol_par.replace("[]", "")
                    if "fp_" == vol_par[:3]:
                        vol_par = vol_par.replace("fp_", "")
                    if vol_par == "r_minor":
                        vol_par = "R"

                    if vol_par != "void":
                        vol_par_str.append(f"{vol_par}")

                if len(vol_par_str) > 0:
                    vol_par_str = ", ".join(vol_par_str)
                    content_vol = f"    vol = form_volume_{MODELS[i]}({vol_par_str});\n"
                else:
                    content_vol = "    vol = 1;\n"

                if (len(pd_pars) > 0) or (len(pd_angles) > 0):
                    pd_init = "," + pd_init

                content_func[
                    ftype
                ] += f"""        model_scale=1.0,
        model_abs=0.0,
        xwidth=0.01,
        yheight=0.01,
        zdepth=0.005,
        R=0,
        target_x=0,
        target_y=0,
        target_z=1,
        int target_index=1,
        focus_xw=0.5,
        focus_yh=0.5,
        focus_aw=0,
        focus_ah=0,
        focus_r=0{pd_init})\n\n"""

                # include coresponding c module and header.
                content_func[
                    ftype
                ] += f"""\nSHARE %{{
%include "sas_kernel_header.c"

/* BEGIN Required header for SASmodel {MODELS[i]} */
{total_content}
/* END Required header for SASmodel {MODELS[i]} */
%}}
    """
                content_func[
                    ftype
                ] += """DECLARE
%{
  double shape;
  double my_a_k;
%}

INITIALIZE
%{
shape=-1;  /* -1:no shape, 0:cyl, 1:box, 2:sphere  */
if (xwidth && yheight && zdepth)
    shape=1;
  else if (R > 0 && yheight)
    shape=0;
  else if (R > 0 && !yheight)
    shape=2;
  if (shape < 0)
    exit(fprintf(stderr, "SasView_model: %s: sample has invalid dimensions.\\n"
                         "ERROR     Please check parameter values.\\n", NAME_CURRENT_COMP));

  /* now compute target coords if a component index is supplied */
  if (!target_index && !target_x && !target_y && !target_z) target_index=1;
  if (target_index)
  {
    Coords ToTarget;
    ToTarget = coords_sub(POS_A_COMP_INDEX(INDEX_CURRENT_COMP+target_index),POS_A_CURRENT_COMP);
    ToTarget = rot_apply(ROT_A_CURRENT_COMP, ToTarget);
    coords_get(ToTarget, &target_x, &target_y, &target_z);
  }

  if (!(target_x || target_y || target_z)) {
    printf("SasView_model: %s: The target is not defined. Using direct beam (Z-axis).\\n",
      NAME_CURRENT_COMP);
    target_z=1;
  }

  /*TODO fix absorption*/
  my_a_k = model_abs; /* assume absorption is given in 1/m */

%}


TRACE
%{
  double l0, l1, k, l_full, l, dl, d_Phi;
  double aim_x=0, aim_y=0, aim_z=1, axis_x, axis_y, axis_z;
  double f, solid_angle, kx_i, ky_i, kz_i, q, qx, qy, qz;
  char intersect=0;

  /* Intersection photon trajectory / sample (sample surface) */
  if (shape == 0){
    intersect = cylinder_intersect(&l0, &l1, x, y, z, kx, ky, kz, R, yheight);}
  else if (shape == 1){
    intersect = box_intersect(&l0, &l1, x, y, z, kx, ky, kz, xwidth, yheight, zdepth);}
  else if (shape == 2){
    intersect = sphere_intersect(&l0, &l1, x, y, z, kx, ky, kz, R);}
  if(intersect)
  {
    if(l0 < 0)
      ABSORB;

    /* Photon enters at l0. */
    k = sqrt(kx*kx + ky*ky + kz*kz);
    l_full = (l1 - l0);          /* Length of full path through sample */
    dl = rand01()*(l1 - l0) + l0;    /* Point of scattering */
    PROP_DL(dl);                     /* Point of scattering */
    l = (dl-l0);                   /* Penetration in sample */

    kx_i=kx;
    ky_i=ky;
    kz_i=kz;
    if ((target_x || target_y || target_z)) {
      aim_x = target_x-x;            /* Vector pointing at target (anal./det.) */
      aim_y = target_y-y;
      aim_z = target_z-z;
    }
    if(focus_aw && focus_ah) {
      randvec_target_rect_angular(&kx, &ky, &kz, &solid_angle,
        aim_x, aim_y, aim_z, focus_aw, focus_ah, ROT_A_CURRENT_COMP);
    } else if(focus_xw && focus_yh) {
      randvec_target_rect(&kx, &ky, &kz, &solid_angle,
        aim_x, aim_y, aim_z, focus_xw, focus_yh, ROT_A_CURRENT_COMP);
    } else {
      randvec_target_circle(&kx, &ky, &kz, &solid_angle, aim_x, aim_y, aim_z, focus_r);
    }
    NORM(kx, ky, kz);
    kx *= k;
    ky *= k;
    kz *= k;
    qx = (kx_i-kx);
    qy = (ky_i-ky);
    qz = (kz_i-kz);
    q = sqrt(qx*qx+qy*qy+qz*qz);
    \n"""

                # include polidispersity
                if len(pd_pars) >= 1:
                    content_func[ftype] += content_pd0
                    content_func[ftype] += pd_condition
                    content_func[ftype] += content_pd
                    # end polidispersity

                    content_func[
                        ftype
                    ] += """    }

        \n"""

                # include angular distribution
                if len(pd_angles) >= 1:
                    content_func[ftype] += content_pd20
                    content_func[ftype] += pd_condition_ang
                    content_func[ftype] += content_pd2
                    content_func[ftype] += "    }\n"

                content_func[
                    ftype
                ] += """

    // Sample dependent. Retrieved from SasView./////////////////////
    float Iq_out;
    Iq_out = 1;\n\n"""

                if len(pd_pars) >= 1:
                    for par in pd_pars:
                        func_param_str = func_param_str.replace(
                            f"{par}", f"trace_{par}"
                        )
                        func_param_str = func_param_str.replace(
                            f"trace_trace_{par}", f"trace_{par}"
                        )
                        if par == "kuhn_length":
                            func_param_str = func_param_str.replace(
                                "kuhn_trace_length", "trace_kuhn_length"
                            )

                        content_vol = content_vol.replace(f"{par}", f"trace_{par}")
                        content_vol = content_vol.replace(
                            f"trace_trace_{par}", f"trace_{par}"
                        )
                        if par == "kuhn_length":
                            content_vol = content_vol.replace(
                                "kuhn_trace_length", "trace_kuhn_length"
                            )

                # Check function type and add corresponding rotation.
                if ftype == "Fq":  # TODO
                    content_func[ftype] += "    double F1=0.0, F2=0.0;\n"
                    content_func[ftype] += f"    Fq_{MODELS[i]}(q, &F1, &F2{func_param_str});\n"
                    content_func[ftype] += "    Iq_out = F2;\n"
                elif ftype == "Iq":
                    content_func[ftype] += f"    Iq_out = Iq_{MODELS[i]}(q{func_param_str});\n"
                elif ftype == "Iqac":
                    content_func[
                        ftype
                    ] += """    double qab=0.0, qc=0.0;
    QACRotation rotation;

    qac_rotation(&rotation, trace_theta, trace_Phi, dtheta, dPhi);
    qac_apply(&rotation, qx, qy, &qab, &qc);
"""
                    func_param_str = func_param_str.replace(", theta,", "")
                    func_param_str = func_param_str = func_param_str.replace("Phi", "")
                    content_func[
                        ftype
                    ] += f"    Iq_out = Iqac_{MODELS[i]}(qab, qc{func_param_str});\n"
                elif ftype == "Iqabc":
                    content_func[
                        ftype
                    ] += """    double qa=0.0, qb=0.0, qc=0.0;
    QABCRotation rotation;

    qabc_rotation(&rotation, trace_theta, trace_Phi, trace_Psi, dtheta, dPhi, dPsi);
    qabc_apply(&rotation, qx, qy, &qa, &qb, &qc);
"""
                    func_param_str = func_param_str.replace(", theta,", "")
                    func_param_str = func_param_str.replace("Phi,", "")
                    func_param_str = func_param_str.replace("Psi", "")
                    content_func[
                        ftype
                    ] += f"    Iq_out = Iqabc_{MODELS[i]}(qa, qb, qc{func_param_str});\n"

                # Add volume function and multiply by scaling factor and volume.
                content_func[
                    ftype
                ] += f"""

    float vol;
{content_vol}
    // Scale by 1.0E2 [SasView: 1/cm  ->   McXtrace: 1/m]
    Iq_out = model_scale*Iq_out / vol * 1.0E2;

    
    p *= l_full*solid_angle/(4*PI)*Iq_out*exp(-my_a_k*(l+l1));

"""             # Save functions
                content_func[ftype] += final_content
                # print(content) ## Verbose
                if not os.path.isdir("indiv_comps"):
                    os.mkdir("indiv_comps")
                with open(
                    f"indiv_comps/SasView_{MODELS[i]}{suffix}.comp", "w"
                ) as outfile:
                    outfile.writelines(content_func[ftype])


def generate_tests():
    if not os.path.isdir("tests"):
        os.mkdir("tests")
    for i in range(len(MODELS)):
        content = f"""/*******************************************************************************
*
* McXtrace, X ray-tracing package
*         Copyright (C) 1997-2008, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Instrument: test_SasView_{MODELS[i]}
*
* %Identification
* Written by: Jose Robledo
* Origin: FZJ / DTU / ESS DMSC
*
*
* %INSTRUMENT_SITE: Templates
*
* Test instrument for the SasView {MODELS[i]} model component as sample description.
*
* %Description
*
* Very simple test instrument for the SasView_{MODELS[i]} component
*
* %Parameters\n"""
        with open("generated_mcxtrace_models/sas_" + MODELS[i] + ".c", "r") as fcode:
            lines = fcode.readlines()
            total_content = "\n".join(lines)
            _funcs, names = search_functions(total_content)
            params, angles, metadata = get_params(MODELS[i])

            found_funcs = [namesi for namesi in names if namesi != ""]

            content_func = {}
            for ftype in found_funcs:
                content_func[ftype] = content
                func_param_str = ""
                content2 = ""
                pd_pars = []
                pd_def = ""
                pd_init = ",\n"
                pd_args = ", "
                for (val, name), meta in zip(params, metadata):
                    if MODELS[i] == "elliptical_cylinder" and name == "axis_ratio":
                        name = "r_ratio"
                    content_func[ftype] += f"* {name}: ({meta[0]}) {meta[1]}.\n"
                    if "[" in name:
                        content2 += f"      vector {name}=" + "{" + f"{val}" + "},\n"
                    else:
                        content2 += f"      {name}={val},\n"
                    func_param_str += f"{name}={name}, "

                    # include polidispersity
                    for pd_par in ["rg", "length", "thick", "radius"]:
                        if pd_par in name:
                            pd_pars.append(name)
                            pd_def += f"* pd_{name}: [] (0,inf) defined as (dx/x), where x is de mean value and dx the standard devition of the variable.\n"
                            pd_init += f"      pd_{name}=0.0,\n"
                            pd_args += f"pd_{name}=pd_{name}, "

                content2 += "\n"
                pd_angles = []
                if ftype in ["Iqac", "Iqabc"]:
                    for angle, name in zip(angles, ["theta", "Phi", "Psi"]):
                        content2 += f"      {name}={angle},\n"
                        func_param_str += f"{name}={name}, "
                        pd_angles.append(name)
                        pd_def += f"* pd_{name}: [] (0,360) defined as (dx/x), where x is de mean value and dx the standard devition of the variable.\n"
                        pd_init += f"      pd_{name}=0.0,\n"
                        pd_args += f"pd_{name}=pd_{name}, "

                pd_def = pd_def[:-2]
                pd_init = pd_init[:-2]
                pd_args = pd_args[:-2]
                content_func[
                    ftype
                ] += f"""* lambda: [AA]   Mean wavelength of X-rays.
* dlambda: [AA]  Wavelength spread of X-rays.
* model_scale: [ ] Global scale factor for scattering kernel. For systems without inter-particle interference, the form factors can be related to the scattering intensity by the particle volume fraction.
* xwidth: [m] ([-inf, inf]) Horiz. dimension of sample, as a width.
* yheight: [m] ([-inf, inf]) vert . dimension of sample, as a height for cylinder/box
* zdepth: [m] ([-inf, inf]) depth of sample
* R: [m] Outer radius of sample in (x,z) plane for cylinder/sphere.
* target_x: [m] relative focus target position.
* target_y: [m] relative focus target position.
* target_z: [m] relative focus target position.
* target_index: [ ] Relative index of component to focus at, e.g. next is +1.
* focus_xw: [m] horiz. dimension of a rectangular area.
* focus_yh: [m], vert. dimension of a rectangular area.
* focus_aw: [deg], horiz. angular dimension of a rectangular area.
* focus_ah: [deg], vert. angular dimension of a rectangular area.
* focus_r: []
{pd_def}
*
* %Link
* %End
*******************************************************************************/
DEFINE INSTRUMENT test_SasView_{MODELS[i]}(
      lambda=6,
      dlambda=0.05,

{content2}
      model_scale=1.0,
      xwidth=0.01,
      yheight=0.01,
      zdepth=0.005,
      R=0,
      int target_index=1,
      target_x=0,
      target_y=0,
      target_z=1,
      focus_xw=0.5,
      focus_yh=0.5,
      focus_aw=0,
      focus_ah=0,
      focus_r=0{pd_init})

DECLARE %{{
%}}

USERVARS %{{
    double _qx;
    double _qy;
}}

INITIALIZE %{{
%}}

TRACE

COMPONENT a1 = Progress_bar()
  AT (0,0,0) ABSOLUTE

COMPONENT arm = Arm(
    )
  AT (0, 0, 0) ABSOLUTE
COMPONENT source = Source_flat(
	xwidth = 0.0005,
	yheight = 0.0005,
	dist = 6, 
	focus_yh = 0.0003, 
	focus_xw = 0.0003,
	lambda0 = lambda, 
	dlambda = dlambda,
    flux = 1e10)
  AT (0, 0, 0) RELATIVE arm

COMPONENT coll1 = Slit(
    radius = 0.0005)
  AT (0, 0, 3) RELATIVE arm

COMPONENT coll2 = Slit(
    radius = 0.0003)
  AT (0, 0, 6) RELATIVE arm

COMPONENT sample_arm = Arm()
AT (0,0,target_z) RELATIVE coll2

"""
                if MODELS[i] == "binary_hard_sphere":
                    print(func_param_str)

                if ftype in ["Fq", "Iq"]:
                    suffix = ""
                elif ftype in ["Iqac", "Iqabc"]:
                    suffix = "_aniso"

                content_func[
                    ftype
                ] += f"""
SPLIT COMPONENT sample = SasView_{MODELS[i]}{suffix}(
        {func_param_str}
        model_scale=model_scale, xwidth=xwidth,
        yheight=yheight, zdepth=zdepth, R=R, target_x=target_x,
        target_y=target_y, target_z=target_z, target_index=target_index,
        focus_xw=focus_xw, focus_yh=focus_yh, focus_aw=focus_aw, focus_ah=focus_ah,focus_r=focus_r{pd_args})
AT (0,0,0) RELATIVE sample_arm
"""
                content_func[
                    ftype
                ] += """EXTEND %{
       if (!SCATTERED) ABSORB;
        _qx=qx;
        _qy=qy;
%}

COMPONENT detector = PSD_monitor(
    nx = 128, ny = 128, filename = "PSD.dat", xwidth=focus_xw, yheight=focus_yh)
  AT (target_x, target_y, target_z) RELATIVE sample_arm

COMPONENT Flex=Flex_monitor_2D(ustring1="_qx", ustring2="_qy",filename="Flex_PSD", Umin1=-0.5,Umax1=0.5,Umin2=-0.5, Umax2=0.5, nU1=128, nU2=128)
  AT (target_x, target_y, target_z) RELATIVE sample_arm

COMPONENT Ldetector = L_monitor(
    nL = 200, filename = "Edet.dat", xwidth=focus_xw, yheight=focus_yh,
    Lmin = lambda-dlambda*lambda,
    Lmax = lambda+dlambda*lambda)
  AT (target_x, target_y, target_z) RELATIVE sample_arm

COMPONENT PSDrad = PSD_monitor_rad(
    filename = "psd2.dat", filename_av = "psd2_av.dat", rmax = focus_xw)
  AT (target_x, target_y, target_z) RELATIVE sample_arm

END\n"""

                if not os.path.isdir(f"{append_to_save}{MODELS[i]}{suffix}"):
                    os.mkdir(f"{append_to_save}{MODELS[i]}{suffix}")
                with open(
                    f"{append_to_save}{MODELS[i]}{suffix}/test_SasView_{MODELS[i]}{suffix}.instr",
                    "w",
                ) as outfile:
                    outfile.writelines(content_func[ftype])

                shutil.copy2(
                    f"indiv_comps/SasView_{MODELS[i]}{suffix}.comp",
                    f"{append_to_save}{MODELS[i]}{suffix}/",
                )


def main():
    _model_status = generate_mcxtrace_models()
    print(_model_status)
    generate_indiv_files()
    generate_tests()


if __name__ == "__main__":
    main()
