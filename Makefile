### Compiler/linker definitions ###
FLAGS=-std=c99 -Werror -Wall -Wextra -Wno-incompatible-pointer-types -Wno-multichar \
-Wno-unused-variable -Wno-unused-parameter
CC=gcc
CFLAGS=$(FLAGS)
LD=gcc
LDFLAGS=$(FLAGS)

### Directory definitions ###
SRCDIR=src
OBJDIR=obj

### Target paths ###
HEADERS		:= $(wildcard $(SRCDIR)/apfs/*.h) $(wildcard $(SRCDIR)/apfs/*/*.h)
COMMANDS	:= $(SRCDIR)/commands.h $(wildcard $(SRCDIR)/commands/*.h)
SOURCES		:= $(SRCDIR)/drat.c
OBJECTS		:= $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
BINARIES	:= drat

### Targets ###

# Makes all targets (binaries)
.PHONY: all
all:	$(BINARIES)
	@echo "All done. The binary is \`drat\`, in the top-level directory (the same directory as the Makefile)."

# Removes all binaries and object files
.PHONY: clean
clean:
	rm -rf drat $(OBJDIR)
	find . -name '*.gch' -delete

$(BINARIES):	%:				$(OBJDIR)/%.o
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

$(COMMANDS:%.h=%.gch):	%.gch:		%.h
	@$(CC) $(CFLAGS) -c $< -o $@
