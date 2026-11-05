# McCode release checklist and web/CI overview

State as of 2026-11-03 (after the 3.10.2 release). `TODO:` marks blanks to fill in.

## 1. Before the release

| Step | Where | Notes |
|---|---|---|
| Code and component changes merged | `mccode-dev/McCode` `main` | |
| Full set of testsuites "acceptable" | Manual run of CI on main / last branch with "code" changes | |
| Manuals: bump `\reldate` | `docs/manuals/mcstas/preamble_common.tex.in`, `docs/manuals/mcxtrace/preamble.tex.in` | Month/year are hard-coded (`\version` comes from CMake). **Consider options for automation** |
| Instrument-md's: run mcdoc --in-repo on instrument-folders | `mcdoc --in-repo --dir mcstas-comps/examples --md`, `mxdoc --in-repo --dir mcxtrace-comps/examples --md` | **Consider options for automation** |
| Manuals: rebuild and commit PDFs + html tgz | `docpkg/manuals/{mcstas,mcxtrace}/` | Prebuilt binaries, **not** rebuilt by CI: the release ships whatever is committed (3.10.1/3.10.2 still say "3.10.0, October 2026"). Recipe: `docs/manuals/BUILDING.TXT`. The websites copy these PDFs from the release .debs. **Consider options for automation** |
| Metapackage contents | `meta-pkgs/deb/control.*-suite-python*`, `meta-pkgs/windows/*-MinGW.iss` | Hand-maintained package lists. Check new tools are listed (e.g. webgl-classic, #3018). MSVC installers pull from conda-forge instead.**Consider options for automation** |
| Draft the release notes | | Text becomes the GitHub release body, which becomes `RELEASE_NOTES.md`. **Tip: use the GitHub release-drafting** |
| Put the notes in `CHANGES_McStas` / `CHANGES_McXtrace`, merge | McCode `main` | **Must** be merged **before tagging**: the McStas website copies `CHANGES_McStas` from the tag.  **Consider options for automation** |


## 2. At the release

1. **Create and publish the GitHub release** with a new tag `vX.Y.Z` on `main`:
   web UI, or `gh release create vX.Y.Z -R mccode-dev/McCode --notes-file notes.md`.
   - Publish it, don't just push a tag: with a bare tag the autobuild creates a
     **draft**, and every step below ignores drafts until you publish it.
   - A **pre-release** is skipped by all downstream automation (useful for test builds).
2. **Merge the "INSTALL docs for vX.Y.Z" PR** that `install-docs` opens
   (branch `external/install-docs-vX.Y.Z`). It bumps `INSTALL-McStas/` and
   `INSTALL-McXtrace/` and writes `RELEASE_NOTES.md` from the release text.
   The text is read once, at publish time; later edits to the release body are not picked up.
3. **Wait / check** (all automatic, see section 3):
   - about 1 h: autobuild attaches the archives to the release;
   - ≤ 3 h after that: the apt repo has the new .debs;
   - ≤ 1 h after the archives: both websites show the new version.
   To skip the waiting, once the archives are attached use Run workflow on
   `publish` (mccode-dev/debian), then `pages` (mccode-dev/mcstas, mccode-dev/mcxtrace).
4. **conda-forge**: `conda-forge/mcstas-suite-feedstock` and `mcxtrace-suite-feedstock`:
   version and sha256 in `recipe/meta.yaml` (`recipe/release_sync.sh X.Y.Z` prints them).
   The update usually happens pr. bot PR, may also be done by by hand from the feedstocks.
5. **Manual step**: 
    - Major (4.0.0) and minor (3.11.0), require announcement, increments/bugfixes (3.10.3) do not
    - Announce to:
      - [mcstas-users](https://mailman2.mcstas.org/mailman/listinfo/mcstas-users), [mcxtrace-users](https://mailman2.mcxtrace.org/mailman/listinfo/mcxtrace-users), [NOBUGS](https://lists.nobugsconference.org/mailman/listinfo/nobugs), [neutron mailing list](https://lists.neutronsources.org/mailman/listinfo/neutron) and internal science list at ESS.
      - Add newsflashes on front pages (edit `contents/main.html/`, …)
      - McStas/McXtrace FaceBook pages

## 3. What runs automatically on a release

| Workflow | Repo | Trigger | Does | Needs |
|---|---|---|---|---|
| `mcstas-autobuild`, `mcxtrace-autobuild` | McCode | push of tag `v*` | Builds all platforms; `assemble-release` uploads `<flavor>-debian-{x86_64,arm64}.tgz`, `-macOS-conda.tgz`, `-Windows-{MinGW,MSVC}-x86_64.tgz` to the release (creates a draft if none exists) | — |
| `install-docs` | McCode | release **published** (not pre-release), or dispatch with a tag | Opens the INSTALL docs PR | Ruleset: the bot may only create `external/**` branches |
| `McCode Pooch Registries Dispatch` | McCode | any tag created/deleted | Notifies `mccode-dev/mccode-pooch-registries` | secret `RECEIVER_TOKEN` |
| `publish` | debian | every 3 h (`23 */3 * * *`), dispatch | Adds the newest releases that have **all four** debian tgz; latest cif2hkl from SOLEIL GitLab; any .deb uploaded by hand to the `apt` release; re-signs; install test on amd64 + arm64; re-enables itself on scheduled runs (keepalive) | secret `APT_GPG_KEY` |
| `pages` | mcstas, mcxtrace | hourly (`41 * * * *`), push to `main`/`master`, dispatch | Finds the newest published release with `<flavor>-debian-x86_64.tgz`, **installs** its .debs (apt, using the debian repo), runs mcdoc/mxdoc, builds the site with `createweb.pl`, deploys; re-enables itself on scheduled runs (keepalive). On a new version a bot commit updates `contents/download/current`, the manual PDFs and (McStas) `CHANGES*` | the debian repo must already carry the release's dependencies (cif2hkl, mcpl, ncrystal) |

## 4. Websites on GitHub Pages

| Site | URL | Repo (branch) | Built by | Content source / notes |
|---|---|---|---|---|
| McCode landing page | https://mccode.org/ | `mccode-dev/mccode-dev.github.io` (`main`) | classic Pages (static) | Hand-written `index.html`; links mcstas.org, mcxtrace.org and root-relative `/debian/`, `/mcstas-nightly/`, … As the org site, its custom domain also moves every project site without its own domain to `mccode.org/<repo>/` (github.io URLs redirect). |
| McStas | https://mcstas.org/ | `mccode-dev/mcstas` (`main`) | `pages` workflow | `createweb.pl` over `contents/` plus mcdoc from the latest release .debs. HTTPS enforced. GitHub mark under "Code-repository (GitHub)" (Pages build only). |
| McXtrace | https://mcxtrace.org/ | `mccode-dev/mcxtrace` (`master`) | `pages` workflow | As McStas, with mxdoc and no CHANGES. HTTPS enforced. |
| apt repository | https://mccode.org/debian/ | `mccode-dev/debian` (`main`) | classic Pages (static `index.html`, key, `mccode.sources`) | The packages themselves are assets of the `apt` release (`github.com/mccode-dev/debian/releases/download/apt/`), so the apt URL doesn't depend on Pages or the domain. |
| McStas nightly | https://mccode.org/mcstas-nightly/ | `mccode-dev/mcstas-nightly` (`main`) | `nightly-site` (calls McCode `nightly-site.yml@main`) | Triggered by McCode `mcstas-conda-testsuite` (02:00 schedule) via `repository_dispatch`; `history/` is committed to the nightly repo. |
| McXtrace nightly | https://mccode.org/mcxtrace-nightly/ | `mccode-dev/mcxtrace-nightly` (`main`) | `nightly-site` | Same, from `mcxtrace-conda-testsuite`. |
| mccode-antlr | https://mccode.org/mccode-antlr/ | `mccode-dev/mccode-antlr` (`gh-pages`) | workflow | TODO: |
| ESS instrument CI tool | https://mccode.org/mcstas-ess-instr-citool/ | `mccode-dev/mcstas-ess-instr-citool` (`main`) | workflow | TODO: |
| McStasToX | https://mccode.org/McStasToX/ | `mccode-dev/McStasToX` (`gh-pages`) | classic Pages | TODO: |

Still on the old server at DTU:
- `packages.mccode.org/debian`: redirects old `[trusted=yes] … stable main` clients to the new repo; `mccode.list`/`mccode.sources` there point to the new repo.
- `download(s).mcstas.org/current`, `download(s).mcxtrace.org/current`: redirect to `INSTALL-McStas` / `INSTALL-McXtrace` on GitHub.
- Old download archive (`download.mcstas.org/mcstas-*`), workshops, … TODO: Decide what to keep and what not to keep. Migrage to 'frozen' pages on ESS infrastructure?

## 5. Interconnections and things to keep an eye on

- **Order matters at release time:** CHANGES merged → release published → autobuild
  archives → apt repo (needs all four debian tgz) → websites (install from the apt repo).
  If the apt repo is behind, the website build can't resolve the new .debs' dependencies.
- **Scheduled workflows run only from the default branch**, and GitHub disables
  them in a public repo after 60 days without repository activity. `publish`
  (debian) commits nothing, so it re-enables itself via the API on every scheduled
  run, which resets the clock; `pages` (mcstas/mcxtrace) does the same in its hourly
  `release` job. If updates still stop, check `gh workflow list -a -R mccode-dev/<repo>`
  and re-enable with `gh workflow enable <name> -R <repo>`.
- **Secrets that expire:** `APT_GPG_KEY` (debian; the signing key expires
  **2029-11-01**, extend it and re-publish `mccode-apt.gpg/.asc` before then),
  `NIGHTLY_TOKEN` (McCode + both nightly repos), `RECEIVER_TOKEN` (McCode). TODO: token expiry dates.
- **apt release asset limit:** GitHub allows 1000 assets per release. Now 351
  (346 .debs + 5 index files); each McCode release adds about 86, so room for about
  7 more releases before old 3.x versions must be pruned (or moved to a second release).
- **Bot branch ruleset in McCode:** "Limit Registrar to `external/` branches", so any
  new bot that pushes to McCode must use `external/...`.
- **Metapackage dependencies are unversioned** (`mcstas-suite-python` depends on `mcstas`,
  not `mcstas (= X.Y.Z)`), so `apt install mcstas-suite-python=<old>` doesn't roll back.
  Fixed revisions can be added with `gh release upload apt <pkg>_X.Y.Z-1_all.deb -R mccode-dev/debian`
  and a `publish` run.
- **Legacy packages** in the apt repo: McStas 2.7.2 and McXtrace 1.7.1
  (`*-suite-{python,perl}-legacy`), side by side with 3.x; Perl tools need `pgplot5` (multiverse).
- **Pages CDN cache:** up to 10 minutes. After a domain or prefix change, pages can
  show stale HTML (broken images) for that long; hard reload.
- **Custom domains:** mcstas.org / mcxtrace.org / mccode.org are set in each repo's
  Settings → Pages; the website build takes its URL prefix from that setting
  (`configure-pages`), so changing a domain needs one `pages` rebuild.
- TODO:
