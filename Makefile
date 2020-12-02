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

### Source paths ###
HEADERS		:= $(shell find $(SRCDIR) -name '*.h')
SOURCES		:= $(shell find $(SRCDIR) -name '*.c')
CMD_SRCS	:= $(wildcard $(SRCDIR)/commands/*.c)
BIN_SRCS	:= $(wildcard $(SRCDIR)/*.c)

# Target paths
GCHS		:= $(HEADERS:$(SRCDIR)/%.h=$(OUTDIR)/%.gch)
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OUTDIR)/%.o)
COMMANDS	:= $(CMD_SRCS:$(SRCDIR)/commands/%.c=%)
BINARIES	:= $(BIN_SRCS:$(SRCDIR)/%.c=%)

### Targets ###

.PHONY: binaries
binaries: $(BINARIES)

.PHONY: commands
commands: $(COMMANDS)

.PHONY: headers
headers: $(GCHS)

$(BINARIES): %: $(OUTDIR)/%.o $(OBJECTS)
	@echo "BINARIES +++ $< +++ $@"
	$(LD) $^ $(LDFLAGS) -o $@
	@echo

# Make `<command_name>` an alias of `out/commands/<command_name>.o`
$(COMMANDS): %: $(OUTDIR)/commands/%.o

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
