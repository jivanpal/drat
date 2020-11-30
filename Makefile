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
HEADER_SOURCES	:= $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/*/*.h) $(wildcard $(SRCDIR)/*/*/*.h)
COMMAND_SOURCES	:= $(wildcard $(SRCDIR)/commands/*.c)
PROGRAM_SOURCES	:= $(wildcard $(SRCDIR)/*.c)

HEADER_OBJECTS	:= $(HEADER_SOURCES:$(SRCDIR)/%.h=$(OUTDIR)/%.gch)
COMMAND_OBJECTS	:= $(COMMAND_SOURCES:$(SRCDIR)/%.c=$(OUTDIR)/%.o)
PROGRAM_OBJECTS	:= $(PROGRAM_SOURCES:$(SRCDIR)/%.c=$(OUTDIR)/%.o)

BINARIES		:= $(PROGRAM_SOURCES:$(SRCDIR)/%.c=%)

### Targets ###

.PHONY: binaries
binaries: $(BINARIES)

.PHONY: commands
commands: $(COMMAND_OBJECTS)

.PHONY: headers
headers: $(HEADER_OBJECTS)

$(BINARIES): %: $(OUTDIR)/%.o $(COMMAND_OBJECTS)
	@$(LD) $^ $(LDFLAGS) -o $@
	@echo "$^\t==> $@"

$(PROGRAM_OBJECTS) $(COMMAND_OBJECTS): $(OUTDIR)/%.o: $(SRCDIR)/%.c $(HEADER_SOURCES)
	@[ -d $(@D) ] || (mkdir -p $(@D) && echo "Created directory \`$(@D)\`.")
	@$(CC) $(CFLAGS) -c "$<" -o "$@"
	@echo "$<\t==> $@"

$(HEADER_OBJECTS): $(OUTDIR)/%.gch: $(SRCDIR)/%.h
	@[ -d $(@D) ] || (mkdir -p $(@D) && echo "Created directory \`$(@D)\`.")
	@$(CC) $(CFLAGS) -c "$<" -o "$@"
	@echo "$<\t==> $@"

.PHONY: all
all: binaries commands headers
	@echo "All done. The binaries are in the top-level directory (the same directory as the Makefile)."

.PHONY: clean
clean:
	rm -rf $(BINARIES) $(OUTDIR)
