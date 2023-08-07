# Makefile.am

# Set the automake options
AUTOMAKE_OPTIONS = foreign

# Variables
CC = @CC@
CFLAGS = @CFLAGS@
TARGET = demo
# Specify the shell to use in the makefile
SHELL := /bin/bash
# Set the .ONESHELL special target to use a specific shell for all recipes
.ONESHELL:
# Check system info
OS := $$(uname -s)
MACHINE := $$(uname -m)
PACK_NAME = $(TARGET)-$(VERSION)-$(OS)-$(MACHINE)

# Source files
koliseo_SOURCES = src/koliseo.c static/amboso.c static/demo.c

# Linking rule
LDADD = $(KOLISEO_LDFLAGS)

# Linker flag to strip symbols
AM_LDFLAGS = -s

# Compiler flags
AM_CFLAGS = $(KOLISEO_CFLAGS) -O2 -Werror -Wpedantic -Wall

# Build rule for object files
%.o: %.c
	$(CC) -c $(CFLAGS) $(AM_CFLAGS) $< -o $@

# Build rule for the final binary
$(TARGET): $(koliseo_SOURCES:.c=.o)
	$(CC) $(CFLAGS) $(AM_CFLAGS) $(koliseo_SOURCES:.c=.o) -o $@ $(LDADD) $(AM_LDFLAGS)

doc:
	@echo -e "Using doxygen to create tex + html from $(VERSION) src/:    "
	doxygen ./docs/koliseo.doxyfile
	@echo -e "\n\033[1;33mDone.\e[0m\n"
	sleep 1
	clear
	@echo -e "Listing ./doxygen directory contents:\n"
	ls -R ./doxygen
	@echo -e "\n\033[1;32mDone.\e[0m"
	@echo -e "\ncd to ./doxygen/latex and make tex into pdf:\n"
	cd ./doxygen/latex ; make | grep "Output written"
	@echo -e "\n\033[1;32mDone.\e[0m"
	@echo -e "\ncd back to ../..:\n"
	cd ../..
	@echo -e "\nmv ./doxygen/latex/refman.pdf to (../../docs/docs.pdf) :\n"
	mv ./doxygen/latex/refman.pdf ./docs/docs.pdf
	@echo -e "\n\033[1;32mDone.\e[0m"
	@echo -en "\nRemove ./doxygen/:    "
	yes | rm -rd ./doxygen/
	@echo -e "\n\033[1;32mDone.\e[0m"

run: demo
	@echo -en "Running ./demo.  "
	./demo
	@echo -e "\033[1;33mDone.\e[0m"

clean:
	@echo -en "Cleaning build artifacts:  "
	rm $(TARGET)
	rm src/*.o
	@echo -e "\033[1;33mDone.\e[0m"

cleanob:
	@echo -en "Cleaning object build artifacts:  "
	rm src/*.o
	@echo -e "\033[1;33mDone.\e[0m"

# Default target (builds everything)
all: $(TARGET)

# Target to clean and rebuild
rebuild: clean all

# By default, build the binary
.DEFAULT_GOAL := all
