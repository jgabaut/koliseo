# Changelog

## [0.6.0] - Unreleased

### Changed

- Drop deprecated symbols
  - `kls__check_available()` macro -> now a function (previously named `kls__check_available_failable()`)
  - `KLS_DEFAULT_EXTENSIONS_LEN`, `KLS_MAX_EXTENSIONS`
  - `KLS_AUTOREGION_EXT_SLOT` (from `src/kls_region.[h,c]`)
- Drop `kls_pop_AR()`, `kls_temp_pop_AR()`
- Refactor `kls_clear()`:
  - Frees the chained `Koliseo`s
  - Calls `on_new_handler()` to reinit the extension
- Since extension data is shared in the `Koliseo` chain:
  - Made `on_free_handler()` use the extension data properly
  - Made `kls__try_grow()` pass `NULL` `on_new_handler` to the next `Koliseo`
  - Made `kls_free()` clear up a pending `Koliseo_Temp` before calling `on_free()`
- Drop `kls_` prefix from `KLS_Conf` fields

## [0.5.10] - 2026-01-10

### Added

- Add `KLS_Push_Error`, used internally to handle failures on `kls_push`, `kls_temp_push` calls
- Add support for `ASan` (build TUs with `-fsanitize=address`)
- Add `templates/hashmap.h`, `static/hashmap_example.c`
- Add `DARRAY_STARTING_CAPACITY` macro to `templates/darray.h`

### Changed

- Refactor `kls_push`, `kls_temp_push` functions to use `kls__advance`, `kls__temp_advance`
- Deprecate `kls__check_available()` macro
- Add `examples`, `tests` targets to `Makefile.am`

## [0.5.9] - 2026-01-02

### Added

- Add `.github/workflows/codeql.yml`

### Changed

- Fix `static/region_example.c`
- Deprecate multiple extensions support
- Deprecate `KLS_DEFAULT_EXTENSIONS_LEN`, `KLS_MAX_EXTENSIONS`

## [0.5.8] - 2025-12-27

### Added

- Add `kls_sprintf()`, `kls_temp_sprintf()`

### Changed

- Bump `invil` to `0.2.28`
- Bump `amboso` to `2.1.0`

## [0.5.7] - 2025-10-13

### Added

- Add `KLS_REPUSH_T()` to expand realloc-like semantics to `Koliseo_Temp`
- Update `templates/darray.h` to support `Koliseo_Temp` allocation

### Changed

- Drop unused var in darray template
- Add docs for `repush()`
- Drop `darray_X_free()` from `templates/darray.h`

## [0.5.6] - 2025-10-07

### Changed

- Fix `kls_repush_dbg()`

## [0.5.5] - 2025-10-07

### Added

- Add `KLS_REPUSH()` to have realloc-like semantics
- Add `templates/dllist.h`
- Add `templates/darray.h`
- Add `static/dllist_example.c`
- Add `KLS_ALIGNOF()` for C++ interop
- Add `extern C` header declaration for C++ interop

## [0.5.4] - 2025-08-04

### Changed

- Fix `kls_new_alloc_ext_dbg()`

## [0.5.3] - 2025-08-04

### Added

- Add `KLS_MAX_EXTENSIONS` and permit more than one extension
- Add `KLS_DEFAULT_EXTENSIONS_LEN` to permit silent upgrade of `kls_new()` variants not ending in `_ext`
- Add `oom`, `oom_temp` tests

### Changed

- Fix: check for non-NULL `OOM_handler `when `OOM_happened`
- Fix experimental `kls_list_pop()` in `kls_region.c`
- Made all `kls_new()` variants into proper functions
- Fix demo in region mode
- Bump `invil` to `0.2.26`
- Bump `amboso` to `2.0.12`

## [0.5.2] - 2025-08-01

### Added

- Add `kls_growable` to `KLS_Conf`
- Add `next` field to `Koliseo`
- New tests for `kls_growable`
- Add new backend for `kls_region` using the growable feature

### Changed

- Update all core calls to support growable `Koliseo`

## [0.5.1] - 2025-07-05

### Added

- Add `KLS_DEFAULT_FREEF` to pair up with `KLS_DEFAULT_ALLOCF` in order to allow custom malloc/free pairs

### Changed

- Dropped unused parameters from `kls_conf_init()`
- Demo uses `kls_region.c` in debug builds

## [0.5.0] - 2025-06-08

### Changed

- Dropped `KOLISEO_HAS_REGION` guards and code
  - Still available as an extension in `src/kls_region.c`
- Dropped `KOLISEO_HAS_GULP` guards and code
  - Still available as an additional header in `src/kls_gulp.h`
- Dropped `kls_new_traced_AR_KLS()` and relatives
- Renamed `kls_push_zero_AR` to `kls_push_zero_ext()`
- Renamed `kls_temp_push_zero_AR` to `kls_temp_push_zero_ext()`
- Avoid using `KLS_DEFAULT_ERR_HANDLERS` in `kls_region.c`
- Include `time.h` in `kls_region.h`
- Fixed typo in `kls_region.c`
- Update CHANGELOG

## [0.4.9] - 2025-06-06

## Added
- Add `src/kls_region.h`
  - Related tests don't need `KOLISEO_HAS_REGION` anymore
- Add `KLS_Hooks` to support user callbacks
- Add `kls_new_alloc_ext()`, `kls_new_conf_ext()`
- `static/region_example.c` shows how to use the `kls_region` extension
  - Build with `gcc -I./src static/region_example.c src/kls_region.c -o static/region_example`

## [0.4.8] - 2025-04-01

## Added

- Add `src/kls_gulp.h`
  - Related tests don't need `KOLISEO_HAS_GULP` anymore

## Changed

- Change default install dir to `/usr/local`
- Fix uninstall target in `Makefile.am`
  - Closes #83
- Fix decorated string push macros not reserving space for NULL pointer
  - Closes #82
- Update `templates/list.h`
  - New `LIST_CMP_FN` to specify comparators
  - New `_p` functions for explicit pointer comparison
- Bump `amboso` to `2.0.10`
- Bump `invil` to `0.2.21`

## [0.4.7] - 2025-01-30

### Added

- Add KLS_ZEROCOUNT_Handler
  - Only useful to have a custom cleanup.
  - Library expects a custom handler not to return.
- Add kls_allow_zerocount_push to KLS_Conf
  - Handle invalid size, count, align arguments for push calls
    - size: >0
    - count: >0 by default, >=0 if conf.kls_allow_zerocount_push == 1
    - align: >0 && power of 2
- Add kls_total_padding() to get total space used by padding
- Add --enable-all configure option to turn on most extensions (no debug and exper)
- Add templates/kls_pit.h
  - Provides KLS_Pit and related APIs
- Added bad_count, zero_count, zero_count_err tests
- Added scripts/libkoliseo.vim
  - Install with make install_syntax
  - Uninstall with make uninstall_syntax

### Changed

- Refactor kls__check_available() to be an internal macro
  - Enables returning NULL from push call on some check errors
  - Improved error reports by getting caller name
- Update templates/list.h to 0.1.1
  - LIST_free_gl() calls free(list->value)
- Rename bad_size test to bad_new_size since it tests kls_new()
- Log PTRDIFF_MAX, OOM faults
- Bump amboso to 2.0.9
- Bump invil to 0.2.20


## [0.4.6] - 2024-09-02

### Added

- Add support for custom error handlers in push calls
  - Out-Of-Memory
  - PTRDIFF_MAX constraint violation
- Add KLS_Err_Handlers to hold the handlers together
- Add new err_handlers field to KLS_Conf
- Add default handlers for all existing APIs:

```c
    #ifndef KOLISEO_HAS_LOCATE
    void KLS_OOM_default_handler__(
        Koliseo* kls,
        ptrdiff_t available,
        ptrdiff_t padding,
        ptrdiff_t size,
        ptrdiff_t count
    );
    void KLS_PTRDIFF_MAX_default_handler__(
        Koliseo* kls,
        ptrdiff_t size,
        ptrdiff_t count
    );
    #else
    void KLS_OOM_default_handler_dbg__(
        Koliseo* kls,
        ptrdiff_t available,
        ptrdiff_t padding,
        ptrdiff_t size,
        ptrdiff_t count,
        Koliseo_Loc loc
    );
    void KLS_PTRDIFF_MAX_default_handler_dbg__(
        Koliseo* kls,
        ptrdiff_t size,
        ptrdiff_t count,
        Koliseo_Loc loc
    );
    #endif // KOLISEO_HAS_LOCATE
```

- Add new constructors with custom OOM handler arguments
  - kls_new_traced_handled() // Macro
  - kls_new_dbg_handled() // Macro
  - kls_new_traced_AR_KLS_handled() // Function
  - kls_new_traced_alloc_handled() // Function
  - kls_new_dbg_alloc_handled() // Function
  - kls_new_traced_AR_KLS_alloc_handled() // Function
- Add new kls_conf_init_handled() to pass handlers with a custom KLS_Conf
- Add KLS_DEFAULT_ERR_HANDLERS macro

### Changed
  - Drop ncurses, title features and all related symbols
  - The previous banner can be found in static/kls_banner.c

## [0.4.5] - 2024-08-09

### Changed

- Turns on the config flag introduced by previous patch for both internal default configs
- Refactor kls_dbg_features()

## [0.4.4] - 2024-08-08

### Added

- Add field to KLS_Conf to selectively block usage of a Koliseo while it has a Koliseo_Temp session open on it
  - Off by default
  - For now, any push call may return NULL in this case and print a diagnostic
  - Future versions may change this to avoid the return and either quit the program immediately or call a passed handler for this specific case
- Add --enable-locate to enable building with KOLISEO_HAS_LOCATE
  - Causes many core calls to be redefined as macros to automatically pass the caller's location.
  - May cause build issues if your object and your header don't agree on wether this feature is on
    - A mismatch may show up as undefined symbols being referenced (since the actual implementation will be named differently and has a different signature)

### Changed

- Move list template to new file
  - Available under ./templates/list.h. The template should be included only after koliseo.h inclusion.
- Rename __KLS_STRCPY() to avoid using __[A-Z]* name, which may be reserved for the compiler
- Collect common parts of push ops in internal functions
- Include profileapi.h instead of whole windows.h for the needed timing calls
- Corrected README about the current Region list implementation
- Bump invil to 0.2.15
- Bump anvil version in stego.lock to 2.0.6

## [0.4.3] - 2024-04-27

### Added

- Add static/list_example.c

### Changed

- Fix: clean some warnings
  - Use %td format specifier for ptrdiff_t
  - Use const for some signature for unmodified pointers
  - PR by @khushal-banks
- Bump amboso to 2.0.6
- Bump invil to 0.2.13

## [0.4.2] - 2024-04-09

### Added

- Add linked list template to generate code, including Koliseo-based allocation
  - Work in progress. May change or be removed soon.

### Changed

- Rename internal KLS_Region_List functions
  - Adds rl_ to each identifier, after kls_
- Bump amboso to 2.0.5
- Bump invil to 0.2.9
- Bump scripts/bootstrap_anvil.sh to v0.2
- Refactor WINDOWS_BUILD preproc macros to use \_WIN32
- Fix basic_example.c using <0.4.x macro signature
- Fix type arg in KLS_PUSH_T_ARR_NAMED() definition


## [0.4.1] - 2024-03-01

### Added

- Add kls_conf_init() as a convenience (since a designated initialiser would have to be coded as feature sensitive over the Region-only fields)
- Add KLS_PUSH_STR() macros to push C strings to a Koliseo
- Support arbitrary allocation function for backing buffer
  - kls_new_alloc() takes a malloc-like function pointer to use
  - Defining KLS_DEFAULT_ALLOCF to be the name of your function will ensure your custom function will be called even on plain kls_new() calls
- Add KLS_PUSH_EX() macros to automate passing a description field to pushes (defaults to passed type stringification)


### Changed

- All KLS_Region functionalities are now an optional feature
  - Building without defining KOLISEO_HAS_REGION provides a slimmer API.
- Move pop() functions under experimental feature
  - Building without defining KOLISEO_HAS_EXPER doesn't include them.
- KLS_Stats's worst_pushcall_time requires KLS_DEBUG_CORE
- Pack name starts with $(SHARED_LIB)
- Move title banner shenanigans under an optional feature
- Bump amboso to 2.0.4
- Bump invil to 0.2.8

## [0.4.0] - 2024-02-05

### Added

- Add const default for KLS_BASIC reglist Koliseo

### Changed

- Refactor core macro functions
- Update KLS_Conf_Fmt to print a string for the reglist backend
- Bump amboso to 2.0.3

## [0.3.20] - 2023-12-28

### Added

- Add SHARED_LIB define, which is now also in pack chain (build .dll for Windows build)

### Changed

- Refactor MINGW32_BUILD defines to WINDOWS_BUILD
- Bump amboso to 2.0.0
- Bump invil to 0.2.1

## [0.3.19] - 2023-12-22

### Added

- Add libkoliseo.a target

### Changed

- Bump invil to 0.1.7
- Now running make pack includes README
- Run ldconfig on make install
- Fix format

## [0.3.17] - 2023-12-16
## [0.3.18] - 2023-12-17

### Added

- Add kls_dbg_features()

### Changed

- Bump invil to 0.1.6

## [0.3.17] - 2023-12-16

### Changed

- Bump invil to 0.1.5

## [0.3.16] - 2023-12-16

### Added

- Add --enable-gulp to configure.ac

## [0.3.15] - 2023-12-15

### Added

- Add --enable-curses to configure.ac to conditionally enable ncurses dependent functions

## [0.3.14] - 2023-12-15

### Added

- Add installer-sh to aid in installing the library to /usr/lib

### Changed

- Bump amboso to 1.9.8
- Bump invil to 0.1.3
- Drop -DKOLISEO_HAS_CURSES as on by default from configure.ac
  - Can be enabled with --enable-debug=yes like other optional header parts

## [0.3.13] - 2023-12-10

### Added

- Add invil and scripts/bootstrap_anvil.sh

### Changed

- Ignore ./anvil
- Bump amboso to 1.9.7
- Rename check_format to check_format.k

## [0.3.12] - 2023-12-03

### Changed

- Bump amboso to 1.9.6
- Update stego.lock for new format

## [0.3.11] - 2023-11-16

### Added

- Add KLS_GULP_FILE_KSTR()

### Changed

- Bump amboso to 1.9.0

## [0.3.10] - 2023-11-12

### Added

- Add basic_gulp test

### Changed

- Quieter kls_set_conf()
- Bump amboso to 1.7.4

## [0.3.9] - 2023-11-08

### Added

- Add KOLISEO_HAS_GULP to enable inclusion of new functions
- Add KLS_GULP_FILE() to map a file from its path
- Add kls_new_dbg() for debugging with no filepath
- Add Gulp_Res, Gulp_Res_Fmt, Gulp_Res_Arg(), string_from_Gulp_Res()
- Add kls_gulp_file_sized(), try_kls_gulp_file()

## [0.3.8] - 2023-11-06

### Changed

- Bump amboso to 1.6.11

## [0.3.7] - 2023-11-06

### Changed

- Switch to astyle format

## [0.3.6] - 2023-11-06

### Changed

- Indent all source code

## [0.3.5] - 2023-11-03

### Changed

- Made kls_temp_start() return NULL on failures instead of failing assertion

## [0.3.4] - 2023-10-23

### Changed

- Drop KLS_REGIONS_MAX_IMPL()
- Use kls_get_maxRegions_KLS_BASIC() for init of reglist_kls
- Handle KLS_BASIC in kls_insord_p()

## [0.3.3] - 2023-10-20

### Changed

- Fixes the supporting reglist_kls for Koliseo_Temp having static size, by using the same kls_reglist_kls_size value as parent Koliseo

## [0.3.2] - 2023-10-20

### Added

- Add kls_new_traced_AR_KLS()
- Add kls_t_cons() as base for Koliseo_Temp region handling

### Changed

- Fix: kls_set_conf() sets stderr by default
- Refactor KLS_Region_List functions
  - Many of them now take a Koliseo arg, used to get memory as needed when kls_reglist_alloc_backend is KLS_BASIC
  - As a result, they are not suitable to work with a Koliseo_Temp regions since they will get memory from the passed Koliseo' reglist_kls.
- Drop bad usage of kls_copy() in iterative list functions

## [0.3.1] - 2023-10-15

### Added

- Add KLS_Stats to collect info about performed API calls

### Changed

- Drop KOLISEO_DEBUG, KOLISEO_DEBUG_FP globals
- Extended KLS_Conf
- Remove kls_set_conf() from header
- Fix kls logfile not being closed on kls_free()

## [0.3.0] - 2023-10-12

### Added

- Add KLS_Conf, KLS_Temp_Conf
- Introduce KLS_DEFAULT_CONF, to always initialise the conf structure on kls_new()
- Add KLS_Conf_Fmt and KLS_Conf_Arg() for formatted output
- Add kls_set_conf(), kls_temp_set_conf()
  - By default, kls_temp_start() will setup KLS_Temp_Conf according to the parent Koliseo conf fields
- Add kls_new_conf()

### Changed

- Drop global vars KLS_AUTOSET_REGIONS and KLS_AUTOSET_TEMP_REGIONS
- Cleaner namespace
  - Prepend KLS_ to Region, Region_Type, Region_List, element, list_region, list_element
- Change all abort() calls to exit(EXIT_FAILURE)

## [0.2.5] - 2023-10-10

### Changed

- Fix push size in logs

## [0.2.4] - 2023-10-09

### Added

- Adds KLSFmt, KLS_Arg() to be used with formatted output functions

### Changed

- Change KOLISEO_AUTOSET_REGION default to 0
- Fix mising argument for some kls_log() calls which led to crashes

## [0.2.3] - 2023-10-07

### Added

- Add some missing NULL checks in various functions
- Adds ./static/basic_example.c

### Changed

- Call kls_free() before abort() on failures

## [0.2.2] - 2023-09-18

### Added

- Adds a Koliseo_Temp pointer inside Koliseo

### Changed

- Calls to kls_temp_start() pushes the Koliseo_Temp itself to the passed Koliseo, properly saving the offset beforehand
- Updates all kls_temp functions to take a Koliseo_Temp pointer as argument

## [0.2.1] - 2023-09-11

### Changed

- Fix build for mingw32

## [0.2.0] - 2023-09-11

### Added

- Add a Region_List to Koliseo_Temp
- Add KOLISEO_AUTOSET_TEMP_REGIONS global var to handle Koliseo_Temp list
- Add -a parameter to the demo to have a non-interactive run (used in makefile.yml workflow)

### Changed

- KLS_PUSH_T() macros have been refactored to use specific kls_temp_push() expecting a Koliseo_Temp, instead of basic ones expecting a Koliseo. Same for KLS_POP_T
- Turns on AUTOSET_REGION and the new temp version of the macro for a mingw32 demo

## [0.1.22] - 2023-09-11

### Added

- Add KLS_DEBUG_CORE macro to enable logging of function calls

## [0.1.21] - 2023-09-08

### Added

- Add MINGW32_BUILD

## [0.1.20] - 2023-09-07

### Added

- Add kls->has_temp

## [0.1.19] - 2023-09-04

### Added

- Add kls_print_logo()

## [0.1.18] - 2023-09-02

### Changed

- Take int for Region type

## [0.1.17] - 2023-09-02

### Added

- Add kls_type_usage()

## [0.1.16] - 2023-09-02

### Added

- Add KLS_PUSH_T_TYPED()

## [0.1.15] - 2023-09-02

### Changed

- Fixed KLS_PUSH_TYPED()

## [0.1.14] - 2023-09-01

### Added

- Add KLS_PUSH_TYPED()

## [0.1.13] - 2023-09-01

### Changed

- Log API level in some operations

## [0.1.12] - 2023-09-01

### Added

- Defined KOLISEO_API_VERSION_INT

### Changed

- Made kls_log() variadic
  - This introduced a bug which would only be fixed in 0.2.4

## [0.1.11] - 2023-08-28

### Changed

- Update debug print functions to also show human readable sizes
- Renamed kls_delet() to kls_delete()

## [0.1.10] - 2023-08-25

### Added

- Add debug functions with ncurses integration

## [0.1.9] - 2023-08-25

### Added

- Add KOLISEO_AUTOSET_REGIONS global var to enable automatic Region allocations upon doing kls_new() or a KLS_PUSH() op
- Add kls_usageReport() to print relative memory usage for each Region in a Koliseo's Region_List

## [0.1.8] - 2023-08-24

### Changed

- Improved Region API

## [0.1.7] - 2023-08-24

### Added

- Add Region_List

## [0.1.6] - 2023-08-21

### Changed

- Fix Koliseo_Temp, which should now work as intended

## [0.1.5] - 2023-08-21

### Added

- Log state in debug mode

## [0.1.4] - 2023-08-21

### Added

- Add kls_log() for internal logs

## [0.1.3] - 2023-08-20

### Changed

- Now push ops update kls->prev_offset

## [0.1.2] - 2023-08-19

### Added

- Add print_kls_2file()

## [0.1.1] - 2023-08-08

### Added

- Documentatio for most functions

### Changed

- Call to abort() on failures

## [0.1.0] - 2023-08-07
