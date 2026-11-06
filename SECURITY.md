# Security Policy

McCode is the shared code base of **McStas** (neutrons) and **McXtrace** (X-rays).
This policy covers this repository: the code generator, the runtime, the component
libraries, the Python tools (`mcrun`, `mcgui`, `mcplot`, `mcdisplay`, `mcdoc`, …)
and our installers and packages (conda-forge, macOS app bundles, Windows installers).

## Supported Versions

Fixes, security fixes included, go into the **current release only**. We do
not backport to older releases. If a problem is reported against an older 3.x
release, we first check whether it still exists in the current release.

| Version                                                                         | Supported                                    |
| ------------------------------------------------------------------------------- | -------------------------------------------- |
| Current 3.x release ([latest](https://github.com/mccode-dev/McCode/releases/latest)) | :white_check_mark: fixes and support         |
| Older 3.x releases                                                              | :warning: limited support, fixed by upgrading |
| McStas 1.x, 2.x / McXtrace 1.x                                                  | :x: deprecated                               |

## What is (and is not) a vulnerability

McCode compiles instrument (`.instr`) and component (`.comp`) files to C and
runs the result. **Instrument and component files are programs.** Running one
from an untrusted source is like running an untrusted script. That is by
design, so it is not a vulnerability in itself.

Examples of what we **do** want to hear about:

- **Malformed data files:** memory corruption or code execution caused by a
  malformed *data* file (e.g. reflection lists, S(q,ω) tables, PDB, OFF/mesh or
  MCPL/NeXus files read by the runtime or components). Such files are shared
  between users and are not expected to be code.
- **Command or code injection in the tools:** e.g. through `mcrun` parameters,
  file names or scan definitions, or script injection in the HTML/D3 plotting and
  display frontends.
- **Installers, packages and app bundles:** insecure downloads (missing
  integrity checks, plain HTTP), unsafe file permissions, or privilege issues.
- **CI and release workflows:** weaknesses in the GitHub Actions workflows that
  could leak secrets or let someone tamper with release artifacts.

Usually **out of scope**, please use a normal issue instead:

- Physics or numerical errors, crashes caused by invalid instrument parameters,
  and similar bugs.
- Problems in third-party dependencies (e.g. NCrystal, MCPL, NeXus, GSL,
  Python packages). Please report these upstream. Do tell us if McCode uses
  them in an unsafe way.

## Reporting a Vulnerability

**Please do not report security problems in public issues, pull requests or on
the mailing lists.**

- **Preferred:** use GitHub's private reporting. Go to the
  [Security tab](https://github.com/mccode-dev/McCode/security) and choose
  *Report a vulnerability*.
- **Alternatively:** email the maintainer, Peter Willendrup
  (<pkwi@dtu.dk>), with "McCode security" in the subject.

Please include:

- the affected version, operating system and flavour (McStas / McXtrace);
- the affected component, tool or file;
- steps or a minimal instrument / data file to reproduce;
- what an attacker could achieve.

### What to expect

McCode is maintained by a small academic team, so these are best-effort targets:

- **Acknowledgement:** within 5 working days.
- **Assessment:** an initial assessment, accepted or declined with reasons,
  within 3 weeks.
- **Progress:** updates at least every 3 weeks while we work on an accepted
  report.
- **If accepted:** the fix lands in the next release, or in a patch release if
  the problem is serious. We publish a GitHub Security Advisory when
  appropriate, and credit you unless you prefer otherwise. Please give us a
  chance to release the fix before disclosing details publicly; we aim for
  90 days at most.
- **If declined:** we explain why. Where it is still a bug, we may ask you to
  file it as a normal issue.
