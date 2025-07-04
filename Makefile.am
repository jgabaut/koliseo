# Makefile.am

# Set the automake options
AUTOMAKE_OPTIONS = foreign

# Variables
CFLAGS = @CFLAGS@
# Specify the shell to use in the makefile
SHELL := /bin/bash
# Set the .ONESHELL special target to use a specific shell for all recipes
.ONESHELL:
# Check system info
MACHINE := $$(uname -m)
TARGET_INSTALL_DIR = /usr/local
SOURCE_INSTALL_DIR = "."
INSTALLER_LOG_FILE = "./installer.log"
# Check local version
PACK_NAME = $(SHARED_LIB)-$(VERSION)-$(OS)-$(MACHINE)

TEMPLATES_DIR = "./templates"

# Source files
demo_SOURCES = static/demo.c static/kls_banner.c
lib_SOURCES = src/koliseo.c

# Linking rule
LDADD = $(KOLISEO_LDFLAGS)

# Linker flag to strip symbols
AM_LDFLAGS = -O2

# Compiler flags
AM_CFLAGS = $(KOLISEO_CFLAGS) -O2 -Werror -Wpedantic -Wall --std=c11

if LOCATE_BUILD
AM_CFLAGS += -DKOLISEO_HAS_LOCATE
endif

if DEBUG_BUILD
AM_LDFLAGS += -ggdb -O0
AM_CFLAGS += -DKLS_DEBUG_CORE -DKLS_SETCONF_DEBUG
demo_SOURCES += src/kls_region.c
else
# Linker flag to strip symbols
AM_LDFLAGS += -s
demo_SOURCES += src/koliseo.c
endif

if EXPER_BUILD
AM_CFLAGS += -DKOLISEO_HAS_EXPER
endif

./anvil:
	@echo -e "\033[1;35m[Makefile]\e[0m    Bootstrapping \"./anvil\":"
	./scripts/bootstrap_anvil.sh repo_invil
	@echo -e "\033[1;33mDone.\e[0m"

libkoliseo.o: $(lib_SOURCES)
	@echo -e "\033[1;35m[Makefile]\e[0m    Building \"$@\":"
	$(CCOMP) -c $(CFLAGS) $(AM_CFLAGS) -fPIC $< -o $@
	@echo -e "\033[1;33mDone.\e[0m"

$(SHARED_LIB): libkoliseo.o
	@echo -e "\033[1;35m[Makefile]\e[0m    Building \"$@\":"
	@echo -e "    \033[1;35mAM_CFLAGS\e[0m: [ \"\033[1;34m$(AM_CFLAGS)\e[0m\" ]"
	@echo -e "    \033[1;35mCFLAGS\e[0m: [ \"\033[1;34m$(CFLAGS)\e[0m\" ]"
	$(CCOMP) $(CFLAGS) $(AM_CFLAGS) -shared $< -o $@
	@echo -e "\033[1;33mDone.\e[0m"

libkoliseo.a: libkoliseo.o
	@echo -e "\033[1;35m[Makefile]\e[0m    Building \"$@\":"
	ar rcs $@ $<
	@echo -e "\033[1;33mDone.\e[0m"

# Build rule for object files
%.o: %.c
	$(CCOMP) -c $(CFLAGS) $(AM_CFLAGS) $< -o $@

install: $(SHARED_LIB)
	@echo -en "Installing koliseo lib $(VERSION):  "
	./installer-sh "-i" $(VERSION) $(TARGET_INSTALL_DIR) $(SOURCE_INSTALL_DIR) $(INSTALLER_LOG_FILE)
	ldconfig
	@echo -e "\n\033[1;33mDone.\e[0m"

uninstall:
	@echo -en "Uninstalling koliseo lib:  "
	./installer-sh "-u" $(VERSION) $(TARGET_INSTALL_DIR) $(SOURCE_INSTALL_DIR) $(INSTALLER_LOG_FILE)
	@echo -e "\033[1;33mDone.\e[0m"

# Build rule for the demo binary
$(TARGET): $(demo_SOURCES:.c=.o)
	@echo -e "    \033[1;35mAM_CFLAGS\e[0m: [ \"\033[1;34m$(AM_CFLAGS)\e[0m\" ]"
	@echo -e "    \033[1;35mLDADD\e[0m: [ \"\033[1;34m$(LDADD)\e[0m\" ]"
	$(CCOMP) $(CFLAGS) $(AM_CFLAGS) $(demo_SOURCES:.c=.o) -o $@ $(LDADD) $(AM_LDFLAGS)

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
	-rm $(TARGET)
	-rm ./*.o
	-rm ./*.so
	-rm ./*.dll
	-rm src/*.o
	-rm src/*.so
	-rm src/*.dll
	-rm static/*.o
	-rm static/*.so
	-rm static/*.dll
	@echo -e "\033[1;33mDone.\e[0m"

cleanob:
	@echo -en "Cleaning object build artifacts:  "
	-rm ./*.o
	-rm ./*.so
	-rm ./*.dll
	-rm src/*.o
	-rm src/*.so
	-rm src/*.dll
	-rm static/*.o
	-rm static/*.so
	-rm static/*.dll
	@echo -e "\033[1;33mDone.\e[0m"

bad_new_size.k:
	@echo -en "Building bad_new_size.k test"
	$(CCOMP) tests/error/bad_new_size.c src/koliseo.c -o tests/error/bad_new_size.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

bad_count.k:
	@echo -en "Building bad_count.k test"
	$(CCOMP) tests/error/bad_count.c src/koliseo.c -o tests/error/bad_count.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

bad_size.k:
	@echo -en "Building bad_size.k test"
	$(CCOMP) tests/error/bad_size.c src/koliseo.c -o tests/error/bad_size.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

many_regions.k:
	@echo -en "Building many_regions.k test"
	$(CCOMP) tests/error/many_regions.c src/kls_region.c -o tests/error/many_regions.k
	@echo -e "\n\033[1;32mDone.\e[0m"

many_temp_regions.k:
	@echo -en "Building many_temp_regions.k test"
	$(CCOMP) tests/error/many_temp_regions.c src/kls_region.c -o tests/error/many_temp_regions.k
	@echo -e "\n\033[1;32mDone.\e[0m"

many_regions_named.k:
	@echo -en "Building many_regions_named.k test"
	$(CCOMP) tests/error/many_regions_named.c src/kls_region.c -o tests/error/many_regions_named.k
	@echo -e "\n\033[1;32mDone.\e[0m"

many_temp_regions_named.k:
	@echo -en "Building many_temp_regions_named.k test"
	$(CCOMP) tests/error/many_temp_regions_named.c src/kls_region.c -o tests/error/many_temp_regions_named.k
	@echo -e "\n\033[1;32mDone.\e[0m"

many_regions_typed.k:
	@echo -en "Building many_regions_typed.k test"
	$(CCOMP) tests/error/many_regions_typed.c src/kls_region.c -o tests/error/many_regions_typed.k
	@echo -e "\n\033[1;32mDone.\e[0m"

many_temp_regions_typed.k:
	@echo -en "Building many_temp_regions_typed.k test"
	$(CCOMP) tests/error/many_temp_regions_typed.c src/kls_region.c -o tests/error/many_temp_regions_typed.k
	@echo -e "\n\033[1;32mDone.\e[0m"

zero_count_err.k:
	@echo -en "Building zero_count_err.k test"
	$(CCOMP) tests/error/zero_count_err.c src/koliseo.c -o tests/error/zero_count_err.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

zero_count.k:
	@echo -en "Building zero_count.k test"
	$(CCOMP) tests/ok/zero_count.c src/koliseo.c -o tests/ok/zero_count.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

big_size.k:
	@echo -en "Building big_size.k test"
	$(CCOMP) tests/ok/big_size.c src/koliseo.c -o tests/ok/big_size.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

basic_run.k:
	@echo -en "Building basic_run.k test"
	$(CCOMP) tests/ok/basic_run.c src/koliseo.c -o tests/ok/basic_run.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

basic_gulp.k:
	@echo -en "Building basic_gulp.k test"
	$(CCOMP) tests/ok/basic_gulp.c src/koliseo.c -o tests/ok/basic_gulp.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

kstr_gulp.k:
	@echo -en "Building kstr_gulp.k test"
	$(CCOMP) tests/ok/kstr_gulp.c src/koliseo.c -o tests/ok/kstr_gulp.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

kstr_test.k:
	@echo -en "Building kstr_test.k test"
	$(CCOMP) tests/ok/kstr_test.c src/koliseo.c -o tests/ok/kstr_test.k -DKLS_DEBUG_CORE
	@echo -e "\n\033[1;32mDone.\e[0m"

anviltest: bad_new_size.k bad_count.k bad_size.k zero_count.k zero_count_err.k basic_run.k basic_gulp.k kstr_gulp.k kstr_test.k big_size.k many_regions.k many_temp_regions.k many_regions_named.k many_temp_regions_named.k many_regions_typed.k many_temp_regions_typed.k ./anvil
	@echo -en "Running anvil tests.\n"
	./anvil -tX
	@echo -e "\n\033[1;32mDone.\e[0m"

pack: rebuild
	@echo -e "Packing koliseo:  make pack for $(VERSION)"
	@echo -e "\033[1;32m[TREE] Prepping tree, pack for $(VERSION):\e[0m"
	[[ -d ~/koliseo-local/ ]] || mkdir ~/koliseo-local
	[[ -d ~/koliseo-local/share ]] || mkdir ~/koliseo-local/share
	[[ -d ~/koliseo-local/share/$(PACK_NAME) ]] || mkdir ~/koliseo-local/share/$(PACK_NAME)
	[[ -d ~/koliseo-local/share/$(PACK_NAME)/src ]] || mkdir ~/koliseo-local/share/$(PACK_NAME)/src

	@echo -e "\033[1;32m[TREE] Removing '~/koliseo-local/share/$(PACK_NAME).zip':\e[0m"
	touch ~/koliseo-local/share/$(PACK_NAME).zip
	rm ~/koliseo-local/share/$(PACK_NAME).zip
	@echo -e "\033[1;32m[TREE] Done.\e[0m"
	@echo -e "\033[1;35m[PACK] Installing:\e[0m"
	install $(TARGET) ~/koliseo-local/share/$(PACK_NAME)
	install $(SHARED_LIB) ~/koliseo-local/share/$(PACK_NAME)
	install README.md ~/koliseo-local/share/$(PACK_NAME)
	install LICENSE ~/koliseo-local/share/$(PACK_NAME)
	install ./src/koliseo.c ~/koliseo-local/share/$(PACK_NAME)/src
	install ./src/koliseo.h ~/koliseo-local/share/$(PACK_NAME)/src

	@echo -e "\033[1;35m[PACK] Done.\e[0m"
	@echo -e "\033[1;32m[ZIP] Compressing $(PACK_NAME):\e[0m"
	cd ~/koliseo-local/share && zip -r $(PACK_NAME).zip ./$(PACK_NAME)
	@echo -e "\033[1;33m[ZIP] Testing zip:\e[0m"
	echo -e "\033[1;36m[UNZIP] Result:\n"
	unzip -t ~/koliseo-local/share/$(PACK_NAME).zip | tail -n1
	@echo -e "\033[1;32m[ZIP] Done.\e[0m"
	@echo -e "\033[1;33mDone.\e[0m"

install_syntax:
	@echo -en "Uninstalling koliseo lib:  "
	./scripts/install_vim_syntax.sh
	@echo -e "\033[1;33mDone.\e[0m"

uninstall_syntax:
	@echo -en "Uninstalling koliseo lib:  "
	./scripts/uninstall_vim_syntax.sh
	@echo -e "\033[1;33mDone.\e[0m"

# Default target (builds everything)
all: $(TARGET) $(SHARED_LIB) libkoliseo.a

# Target to clean and rebuild
rebuild: clean all

# By default, build the binary
.DEFAULT_GOAL := all
