# Test_Monitor_nD_noacc

Small instrument exercising `Monitor_nD_noacc` (the host-side variant used in FUNNEL mode) next to `Monitor_nD`. Drafted from `Tests_monitors/Test_Monitor_nD`.

Run with explicit parameters and a fixed seed, then compare `nD_ref` and `nD_noacc` output:

    mcrun Test_Monitor_nD_noacc.instr lambda=2 L1=5 bins=20 xw=0.1 yh=0.1 -n 1e5 -s 1234 -d out

Checked on CPU (McStas built from `main` at f1066d85d): the numeric data of the 2D `x_y` histogram and the `list.p.x.y` list file are identical between `nD_ref` and `nD_noacc` (headers differ only by component name). Not run with OpenACC.
