# External contributions (`*.ext`)

Some components, library snippets and example instruments shipped with McCode
are maintained in their own repositories. Rather than copying them into this
tree (where they drift) or hiding the fetch inside a bespoke CMake module
(where nobody finds it), each such contribution declares itself with a small
JSON manifest named `<something>.ext`, **placed in the directory the files are
populated into**.

So a developer browsing `mcstas-comps/contrib/` for a component finds either
the `.comp` itself or an `.ext` file naming the upstream repository, the
release it is pinned to, and the SHA256 of every file taken from it. The
record lives where you would look for the thing it describes.

The CMake side is `cmake/Modules/External.cmake`; the manifests are kept
honest by `buildscripts/mcext`.

## Where the manifests live

`mcstas-chopper-lib` contributes files to three destinations, so it has a
manifest in each:

| Manifest | Populates |
| --- | --- |
| `mcstas-comps/share/chopper-lib.ext` | `chopper-lib.c`, `chopper-lib.h` — `%include`-d into generated instrument C, like the other `*-lib.c` snippets beside it |
| `mcstas-comps/contrib/chopper-lib.ext` | `NXdisk_chopper.comp`, `Masked_ESS_butterfly.comp` |
| `mcstas-comps/examples/Tests_optics/<Instrument>/<Instrument>.ext` | that one example instrument |

Manifests are not installed: the `install( DIRECTORY ... )` rules that cover
those directories carry `PATTERN "*.ext" EXCLUDE`.

## Manifest format

Short form — a JSON array of fully self-describing file entries:

```json
[
  { "name": "chopper-lib.h",
    "url": "https://raw.githubusercontent.com/mcdotstar/mcstas-chopper-lib/v4.1.0/chopper-lib.h",
    "sha256": "0965f6660850acce12d665d7a57064389ccb570e218f21c1dbad9155ee7d8ef9" }
]
```

Long form — an object of contribution-wide defaults plus a `files` array. Any
key understood in a file entry may also appear at the top level, where it
applies to every entry that does not override it:

```json
{
  "name": "mcstas-chopper-lib",
  "description": "Chopper opening/closing times and (inverse velocity, time) masks.",
  "homepage": "https://github.com/mcdotstar/mcstas-chopper-lib",
  "git": "https://github.com/mcdotstar/mcstas-chopper-lib.git",
  "version": "v4.1.0",
  "base": "https://raw.githubusercontent.com/mcdotstar/mcstas-chopper-lib/v4.1.0/",
  "files": [
    { "name": "chopper-lib.h", "sha256": "0965f666..." },
    { "name": "chopper-lib.c", "sha256": "d914a307..." }
  ]
}
```

Prefer the long form: it is the one that records *what* the contribution is,
not just where some bytes live.

### File-entry keys

| Key | |
| --- | --- |
| `name` | **required** — file name upstream, and the default installed name |
| `sha256` | **required** — SHA256 of the contents |
| `as` | install path relative to the manifest's own directory; may name a subdirectory, e.g. `"Test_Foo/Test_Foo.instr"`. Default: `name` |
| `url` | explicit download URL for this one file |
| `from` | path of the file inside the release archive. Default: `name` |
| `base` | URL prefix; the file is fetched from `<base><from>` |
| `git` | upstream repository — informational, but see below |
| `version` | upstream tag/ref |

### Contribution-wide keys

| Key | |
| --- | --- |
| `files` | **required** in the long form |
| `archive` | `{ "url", "sha256", "strip" }` — a release tarball or zip, downloaded and unpacked once and shared across every manifest that names the same URL. `strip` leading path components are dropped; the default of `1` matches GitHub's generated source archives |
| `license`, `description`, `homepage` | recorded in configure output |

A single file resolves through `url`, then `base`, then `archive`; failing all
three, a `github.com` `git` plus a `version` is enough to derive a raw base
URL. Mixing is fine — a manifest may take most of its files from a release
tarball and one from a direct URL.

## Why the hashes are mandatory

The hash is the point. A pinned tag says what *should* be fetched; the SHA256
is what makes a build fail loudly when the bytes behind that tag change —
a moved tag, a regenerated release archive, a compromised host. Without it,
"pinned to v4.1.0" is a statement of intent rather than a fact.

This matters more than usual for GitHub's auto-generated source archives
(`/archive/refs/tags/*.tar.gz`), which are produced on demand and have changed
byte-for-byte across GitHub compression changes in the past. A *release asset*
that the maintainer uploads is stable and is the better thing to point
`archive.url` at where one exists.

A manifest may omit `sha256` only if the build is configured with
`-DMCCODE_EXTERNALS_ALLOW_UNVERIFIED=ON`, which exists for bisecting an
upstream problem and should not appear in a manifest's normal life.

## Adding a contribution

1. Create the directory the files belong in, if it does not exist.
2. Write the manifest there with `name`/`git`/`version`/`base` (or `archive`)
   and the list of files, leaving the hashes out.
3. Fill them in: `buildscripts/mcext update path/to/thing.ext`
4. Add a `mccode_install_externals()` call for the containing directory if one
   does not already exist — see `mcstas-comps/CMakeLists.txt`, where each call
   mirrors the `install( DIRECTORY ... )` rule for the same directory.

To move a contribution to a new upstream release:

```sh
buildscripts/mcext update mcstas-comps/contrib/chopper-lib.ext -v v4.2.1
```

which repoints every recorded URL at the new tag and recomputes every hash.

To confirm nothing upstream has shifted under a pinned reference:

```sh
buildscripts/mcext check          # whole tree; non-zero exit on any mismatch
```

That is the command to run from CI.

## Build options

| Option | Default | |
| --- | --- | --- |
| `ENABLE_EXTERNALS` | `ON` | populate external contributions at all |
| `MCCODE_EXTERNALS_CACHE` | `<build>/externals-cache` | content-addressed download cache; point it somewhere persistent to survive a wiped build directory |
| `MCCODE_EXTERNALS_LOCAL` | *(empty)* | directory of pre-fetched files, searched by name before any download and still hash-verified |
| `MCCODE_EXTERNALS_OFFLINE` | `OFF` | never download: any file not in the cache or `MCCODE_EXTERNALS_LOCAL` is a hard error rather than a silent fetch |
| `MCCODE_EXTERNALS_ALLOW_UNVERIFIED` | `OFF` | accept entries with no `sha256` |
| `MCCODE_EXTERNALS_TIMEOUT` | `60` | per-file download timeout, seconds |

Distribution packagers who may not reach the network during a build have two
routes: pre-populate `MCCODE_EXTERNALS_CACHE`, or drop the files into
`MCCODE_EXTERNALS_LOCAL`. Either way, set `MCCODE_EXTERNALS_OFFLINE=ON` so a
gap in the provided set fails the configure instead of quietly downloading.

The manifest machinery uses `string( JSON )` and `file( ARCHIVE_EXTRACT )`,
which is why McCode's `cmake_minimum_required` is 3.19. The `cmake_policy(
VERSION 3.17.0 )` pins in the tools subdirectories were deliberately left
alone: raising the floor should not quietly change policy behaviour anywhere
it was explicitly fixed.
