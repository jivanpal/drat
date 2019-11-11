### Compiler/linker definitions ###
FLAGS=-std=c99 -Werror -Wall -Wextra -Wno-incompatible-pointer-types -Wno-multichar \
-Wno-unused-variable
CC=gcc
CFLAGS=$(FLAGS)
LD=gcc
LDFLAGS=$(FLAGS)

### Directory definitions ###
SRCDIR=src
OBJDIR=obj
BINDIR=bin

### Target paths ###
TARGETS		:= apfs-read apfs-inspect apfs-explore-omap-tree apfs-explore-fs-tree
SOURCES		:= $(wildcard $(SRCDIR)/*.c)
HEADERS		:= $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/*/*.h) $(wildcard $(SRCDIR)/*/*/*.h)
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
	find . -name '*.gch' -delete

# Makes the target `binary-name` an alias of `bin/binary-name`
.PHONY: $(TARGETS)
$(TARGETS):		%:				$(BINDIR)/%

$(BINARIES):	$(BINDIR)/%:	$(OBJDIR)/%.o
	@[ -d $(BINDIR) ] || (mkdir -p $(BINDIR) && echo "Created directory \`$(BINDIR)/\`.")
	@$(LD) $^ $(LDFLAGS) -o $@
	@echo "$^\t==> $@"

$(OBJECTS):		$(OBJDIR)/%.o:	$(SRCDIR)/%.c $(HEADERS)
	@[ -d $(OBJDIR) ] || (mkdir -p $(OBJDIR) && echo "Created directory \`$(OBJDIR)\`.")
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$<\t==> $@ "

# This target exists to allow test compilation of
# headers, e.g. `make src/apfs/struct/object.gch`
$(HEADERS:%.h=%.gch):	%.gch:		%.h
	@$(CC) $(CFLAGS) -c $< -o $@
