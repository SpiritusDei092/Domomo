#if BUILDING_LIBDOMOMO
# define LIBFOO_SHLIB_EXPORTED __attribute__((__visibility__("default")))
#else
# define LIBFOO_SHLIB_EXPORTED
#endif