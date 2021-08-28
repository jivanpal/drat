# Drat (formerly apfs-tools)

Drat is a tool for analysing and recovering data from [APFS (Apple File System)](https://en.wikipedia.org/wiki/Apple_File_System)
partitions. Its creation was inspired by a [personal data loss incident](https://apple.stackexchange.com/questions/373718)
and [Jonathan Levin's](https://twitter.com/Morpheus______) closed-source
`fsleuth` tool, which he demonstrated in [this lecture](http://docs.macsysadmin.se/2018/video/Day4Session2.mp4).

The name "Drat" is a loose acronym for "Disaster Recovery APFS Tools", and a bad
pun on how one might say "drat!" after discovering that their data is corrupted.

This software is currently in development, and is being implemented with reference
to [Apple's official APFS specification (PDF)](https://developer.apple.com/support/downloads/Apple-File-System-Reference.pdf).
Copies of various versions of this spec are included in the `spec` directory for
archival purposes, particularly in case the online version of the document changes.

Currently, all of Drat's commands (except `modify`, which is currently disabled
as it is not fit for use) operate in a read-only fashion, as they are intended
to be used in situations involving data recovery or data forensics.

## Build instructions

If you're running macOS on an Intel Mac, precompiled binaries for versioned
releases are available on the [releases page](https://github.com/jivanpal/drat/releases).

Documentation for versioned releases and as generated from the `main` branch
[can be viewed online](https://drat.readthedocs.io/).

Compilation and execution has been tested on macOS Catalina 10.15.7 (19H524) on
an Intel x86-64 machine (MacBookPro9,2).

### Compiling the software

#### Requirements

- `gcc` — Required because we use `__attribute__((packed))`. Tested with
  GCC 11.2.0, installed via [Homebrew](https://brew.sh) (Homebrew GCC 11.2.0).

- `make` — Tested with GNU Make 3.81, as included in Xcode Command Line Tools.

- `<argp.h>` (GNU Argp library) — If compiling on macOS, you can get this by
  installing the [Homebrew](https://brew.sh) package `argp-standalone`; the
  Makefile will handle this configuration automatically. If you acquire this
  library any other way, you will need to configure `CFLAGS` and `LDFLAGS` as
  appropriate (see lines in `Makefile` after `ifeq ($(shell uname -s),Darwin)`).

#### Instructions

- Ensure that `gcc` is in your `$PATH`, or modify the `CC` and `LD` values in 
  `Makefile` to reflect the location of `gcc` on your system.
  
- Run `make` from the project root (where this `README.md` file resides). An
  `out` directory will be created in which the object files will be stored. The
  final binary `drat` will be stored in the project root.

- Run `make clean` to remove the compiled binary (`drat`) and other output files
  (`out` directory).

### Generating the documentation

[Sphinx](https://www.sphinx-doc.org/en/master/) is used to manage the
documentation. This facilitates usage of [Read the Docs](https://readthedocs.org/),
which hosts the documentation online for you to read easily, both for all
versioned releases and as generated from the `main` branch.

We use a variant of Markdown called [MyST](https://myst-parser.readthedocs.io/en/latest/)
that supports all of the features of reStructuredText.

#### Requirements

Sphinx requires Python and its `sphinx` package. We also require the
Read the Docs theme (`sphinx_rtd_theme`) and the MyST parser (`myst-parser`).
If/when you have Python installed, you can install the required packages all at
once with the following: `pip install sphinx sphinx_rtd_theme myst-parser`.

<!-- TODO: Use pip requirements file instead -->

#### Instructions

- From the project root (the directory where this `README.md` file resides):

  - Run `make docs` to generate HTML documentation in `docs/_build/html`.
    Open `docs/_build/html/index.html` in your browser to view the generated
    documentation.

  - Run `make clean-docs` to remove the generated documentation (`docs/_build`
    directory).

- From the `docs` directory:

  - Run `make <format>` to generate the documentation in a format other than
    HTML, where `<format>` is any of the formats listed in the output of
    `make help`. You may need to install other software to generate
    documentation in these other formats.

  - Run `make clean` to remove the generated documentation.
