#  ============================================================================
#  ============================================================================
TARGET = autograder

# ============================================================================
#  A list of files to link into the library.
# ============================================================================
SOURCES := $(wildcard *.c)
OBJECTS = $(addprefix $(OBJECT_DIR)/, $(notdir $(SOURCES:.c=.o)))

# =============================================================================
#  Build variables and settings.
# =============================================================================
OBJECT_DIR=objects

# ============================================================================
#  Build rules and flags.
# ============================================================================
BLUE=$(shell tput setaf 4)
PURPLE=$(shell tput setaf 5)
TEXTRESET=$(shell tput sgr0)
YELLOW=$(shell tput setaf 3)

ECHO=/usr/bin/printf "%s\n"
MKDIR = /bin/mkdir -p

CFLAGS := -Wall -Wextra -pedantic -ansi -D_POSIX_C_SOURCE
LDFLAGS := -Wl,-O1

# ============================================================================
#  Build rules.
# ============================================================================
.PHONY: clean
all: $(TARGET)
all: CFLAGS := -DNDEBUG

debug: $(TARGET)
debug: CFLAGS := -ggdb3 -g3

# ============================================================================
#  Build targets.
# ============================================================================
$(TARGET): $(OBJECTS)
	@$(ECHO) "$(BLUE)Linking$(YELLOW): $(PURPLE)$@$(TEXTRESET)"
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(OBJECT_DIR)/%.o: %.c %.h common.h
	@$(MKDIR) $(OBJECT_DIR)
	@$(ECHO) "$(BLUE)Compiling$(YELLOW): $(PURPLE)$<$(TEXTRESET)"
	@$(CC) $(CFLAGS) $< -c -o $@

clean:
	@$(ECHO) "$(BLUE)Cleaning...$(TEXTRESET)"
	@$(RM) $(OBJECTS) $(TARGET)

