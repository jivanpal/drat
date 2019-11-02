### Compiler/linker definitions ###
FLAGS=-Werror -Wall -Wextra -Wno-incompatible-pointer-types -Wno-multichar
CC=gcc
CFLAGS=$(FLAGS)
LD=gcc
LDFLAGS=$(FLAGS)

### Directory definitions ###
SRCDIR=src
OBJDIR=obj
BINDIR=bin

### Target paths ###
TARGETS		:= apfs-read apfs-dump
SOURCES		:= $(wildcard $(SRCDIR)/*.c)
HEADERS		:= $(wildcard $(SRCDIR)/**/*.h)
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
BINARIES	:= $(TARGETS:%=$(BINDIR)/%)

### Targets ###

# Makes all targets (binaries)
.PHONY: all
all:	$(TARGETS)
	@echo "All done. The binaries are in the \`$(BINDIR)\` directory."

# Removes all binaries and object files
.PHONY: clean
clean:
	rm -rf $(BINDIR) $(OBJDIR)

# Makes the target `binary-name` an alias of `bin/binary-name`
.PHONY: $(TARGETS)
$(TARGETS):		%:				$(BINDIR)/%

$(BINARIES):	$(BINDIR)/%:	$(OBJDIR)/%.o
	@[ -d $(BINDIR) ] || (mkdir -p $(BINDIR) && echo "Created directory \`$(BINDIR)/\`.")
	@$(LD) $^ $(LDFLAGS) -o $@
	@echo "$^\t==> $@"

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@[ -d $(OBJDIR) ] || (mkdir -p $(OBJDIR) && echo "Created directory \`$(OBJDIR)\`.")
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$<\t==> $@ "
