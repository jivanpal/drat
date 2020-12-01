### Compiler/linker definitions ###
FLAGS=-std=c99 -Werror -Wall -Wextra -Wno-incompatible-pointer-types -Wno-multichar \
-Wno-unused-variable -Wno-unused-parameter
CC=gcc
CFLAGS=$(FLAGS)
LD=gcc
LDFLAGS=$(FLAGS)

### Directory definitions ###
SRCDIR=src
OUTDIR=out

### Target paths ###
PROGRAMS	:= $(wildcard $(SRCDIR)/*.c)
BINARIES	:= $(PROGRAMS:$(SRCDIR)/%.c=%)

HEADERS		:= $(shell find src -name '*.h')
GCHS		:= $(HEADERS:$(SRCDIR)/%.h=$(OUTDIR)/%.gch)
SOURCES		:= $(shell find src -name '*.c')
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OUTDIR)/%.o)

### Targets ###

.PHONY: binaries
binaries: $(BINARIES)

.PHONY: headers
headers: $(GCHS)

$(BINARIES): %: $(OUTDIR)/%.o $(OBJECTS)
	@echo "BINARIES +++ $< +++ $@"
	$(LD) $^ $(LDFLAGS) -o $@
	@echo

$(OBJECTS): $(OUTDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@echo "OBJECTS +++ $< +++ $@"
	@[ -d $(@D) ] || (mkdir -p $(@D) && echo "Created directory \`$(@D)\`.")
	$(CC) $(CFLAGS) -c "$<" -o "$@"
	@echo

$(GCHS): $(OUTDIR)/%.gch: $(SRCDIR)/%.h
	@echo "GCHS +++ $< +++ $@"
	@[ -d $(@D) ] || (mkdir -p $(@D) && echo "Created directory \`$(@D)\`.")
	$(CC) $(CFLAGS) -c "$<" -o "$@"
	@echo

.PHONY: all
all: binaries headers
	@echo "All done. The binaries are in the top-level directory (the same directory as the Makefile)."

.PHONY: clean
clean:
	rm -rf $(BINARIES) $(OUTDIR)
