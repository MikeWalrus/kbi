#include <gio/gio.h>

#if defined (__ELF__) && ( __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6))
# define SECTION __attribute__ ((section (".gresource.kbi"), aligned (8)))
#else
# define SECTION
#endif

#ifdef _MSC_VER
static const SECTION union { const guint8 data[629]; const double alignment; void * const ptr;}  kbi_resource_data = { {
  0107, 0126, 0141, 0162, 0151, 0141, 0156, 0164, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 
  0030, 0000, 0000, 0000, 0164, 0000, 0000, 0000, 0000, 0000, 0000, 0050, 0003, 0000, 0000, 0000, 
  0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0002, 0000, 0000, 0000, 0371, 0332, 0215, 0013, 
  0002, 0000, 0000, 0000, 0164, 0000, 0000, 0000, 0004, 0000, 0114, 0000, 0170, 0000, 0000, 0000, 
  0174, 0000, 0000, 0000, 0335, 0310, 0307, 0131, 0000, 0000, 0000, 0000, 0174, 0000, 0000, 0000, 
  0011, 0000, 0166, 0000, 0210, 0000, 0000, 0000, 0154, 0002, 0000, 0000, 0324, 0265, 0002, 0000, 
  0377, 0377, 0377, 0377, 0154, 0002, 0000, 0000, 0001, 0000, 0114, 0000, 0160, 0002, 0000, 0000, 
  0164, 0002, 0000, 0000, 0153, 0142, 0151, 0057, 0001, 0000, 0000, 0000, 0167, 0151, 0156, 0144, 
  0157, 0167, 0056, 0165, 0151, 0000, 0000, 0000, 0324, 0001, 0000, 0000, 0000, 0000, 0000, 0000, 
  0074, 0077, 0170, 0155, 0154, 0040, 0166, 0145, 0162, 0163, 0151, 0157, 0156, 0075, 0042, 0061, 
  0056, 0060, 0042, 0040, 0145, 0156, 0143, 0157, 0144, 0151, 0156, 0147, 0075, 0042, 0125, 0124, 
  0106, 0055, 0070, 0042, 0077, 0076, 0012, 0074, 0151, 0156, 0164, 0145, 0162, 0146, 0141, 0143, 
  0145, 0076, 0074, 0157, 0142, 0152, 0145, 0143, 0164, 0040, 0143, 0154, 0141, 0163, 0163, 0075, 
  0042, 0107, 0164, 0153, 0101, 0160, 0160, 0154, 0151, 0143, 0141, 0164, 0151, 0157, 0156, 0127, 
  0151, 0156, 0144, 0157, 0167, 0042, 0040, 0151, 0144, 0075, 0042, 0141, 0160, 0160, 0137, 0167, 
  0151, 0156, 0144, 0157, 0167, 0042, 0076, 0074, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 
  0040, 0156, 0141, 0155, 0145, 0075, 0042, 0164, 0151, 0164, 0154, 0145, 0042, 0040, 0164, 0162, 
  0141, 0156, 0163, 0154, 0141, 0164, 0141, 0142, 0154, 0145, 0075, 0042, 0171, 0145, 0163, 0042, 
  0076, 0113, 0145, 0171, 0142, 0157, 0141, 0162, 0144, 0111, 0156, 0163, 0164, 0162, 0165, 0155, 
  0145, 0156, 0164, 0163, 0074, 0057, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0076, 0074, 
  0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0040, 0156, 0141, 0155, 0145, 0075, 0042, 0144, 
  0145, 0146, 0141, 0165, 0154, 0164, 0055, 0153, 0142, 0151, 0137, 0167, 0151, 0156, 0144, 0157, 
  0167, 0137, 0167, 0151, 0144, 0164, 0150, 0042, 0076, 0066, 0060, 0060, 0074, 0057, 0160, 0162, 
  0157, 0160, 0145, 0162, 0164, 0171, 0076, 0074, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 
  0040, 0156, 0141, 0155, 0145, 0075, 0042, 0144, 0145, 0146, 0141, 0165, 0154, 0164, 0055, 0153, 
  0142, 0151, 0137, 0167, 0151, 0156, 0144, 0157, 0167, 0137, 0150, 0145, 0151, 0147, 0150, 0164, 
  0042, 0076, 0064, 0060, 0060, 0074, 0057, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0076, 
  0074, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0040, 0156, 0141, 0155, 0145, 0075, 0042, 
  0150, 0151, 0144, 0145, 0055, 0157, 0156, 0055, 0143, 0154, 0157, 0163, 0145, 0042, 0076, 0124, 
  0162, 0165, 0145, 0074, 0057, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0076, 0074, 0143, 
  0150, 0151, 0154, 0144, 0076, 0074, 0157, 0142, 0152, 0145, 0143, 0164, 0040, 0143, 0154, 0141, 
  0163, 0163, 0075, 0042, 0107, 0164, 0153, 0102, 0157, 0170, 0042, 0040, 0151, 0144, 0075, 0042, 
  0143, 0157, 0156, 0164, 0145, 0156, 0164, 0137, 0142, 0157, 0170, 0042, 0076, 0074, 0160, 0162, 
  0157, 0160, 0145, 0162, 0164, 0171, 0040, 0156, 0141, 0155, 0145, 0075, 0042, 0157, 0162, 0151, 
  0145, 0156, 0164, 0141, 0164, 0151, 0157, 0156, 0042, 0076, 0166, 0145, 0162, 0164, 0151, 0143, 
  0141, 0154, 0074, 0057, 0160, 0162, 0157, 0160, 0145, 0162, 0164, 0171, 0076, 0074, 0057, 0157, 
  0142, 0152, 0145, 0143, 0164, 0076, 0074, 0057, 0143, 0150, 0151, 0154, 0144, 0076, 0074, 0057, 
  0157, 0142, 0152, 0145, 0143, 0164, 0076, 0074, 0057, 0151, 0156, 0164, 0145, 0162, 0146, 0141, 
  0143, 0145, 0076, 0012, 0000, 0000, 0050, 0165, 0165, 0141, 0171, 0051, 0057, 0000, 0000, 0000, 
  0000, 0000, 0000, 0000
} };
#else /* _MSC_VER */
static const SECTION union { const guint8 data[629]; const double alignment; void * const ptr;}  kbi_resource_data = {
  "\107\126\141\162\151\141\156\164\000\000\000\000\000\000\000\000"
  "\030\000\000\000\164\000\000\000\000\000\000\050\003\000\000\000"
  "\000\000\000\000\000\000\000\000\002\000\000\000\371\332\215\013"
  "\002\000\000\000\164\000\000\000\004\000\114\000\170\000\000\000"
  "\174\000\000\000\335\310\307\131\000\000\000\000\174\000\000\000"
  "\011\000\166\000\210\000\000\000\154\002\000\000\324\265\002\000"
  "\377\377\377\377\154\002\000\000\001\000\114\000\160\002\000\000"
  "\164\002\000\000\153\142\151\057\001\000\000\000\167\151\156\144"
  "\157\167\056\165\151\000\000\000\324\001\000\000\000\000\000\000"
  "\074\077\170\155\154\040\166\145\162\163\151\157\156\075\042\061"
  "\056\060\042\040\145\156\143\157\144\151\156\147\075\042\125\124"
  "\106\055\070\042\077\076\012\074\151\156\164\145\162\146\141\143"
  "\145\076\074\157\142\152\145\143\164\040\143\154\141\163\163\075"
  "\042\107\164\153\101\160\160\154\151\143\141\164\151\157\156\127"
  "\151\156\144\157\167\042\040\151\144\075\042\141\160\160\137\167"
  "\151\156\144\157\167\042\076\074\160\162\157\160\145\162\164\171"
  "\040\156\141\155\145\075\042\164\151\164\154\145\042\040\164\162"
  "\141\156\163\154\141\164\141\142\154\145\075\042\171\145\163\042"
  "\076\113\145\171\142\157\141\162\144\111\156\163\164\162\165\155"
  "\145\156\164\163\074\057\160\162\157\160\145\162\164\171\076\074"
  "\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042\144"
  "\145\146\141\165\154\164\055\153\142\151\137\167\151\156\144\157"
  "\167\137\167\151\144\164\150\042\076\066\060\060\074\057\160\162"
  "\157\160\145\162\164\171\076\074\160\162\157\160\145\162\164\171"
  "\040\156\141\155\145\075\042\144\145\146\141\165\154\164\055\153"
  "\142\151\137\167\151\156\144\157\167\137\150\145\151\147\150\164"
  "\042\076\064\060\060\074\057\160\162\157\160\145\162\164\171\076"
  "\074\160\162\157\160\145\162\164\171\040\156\141\155\145\075\042"
  "\150\151\144\145\055\157\156\055\143\154\157\163\145\042\076\124"
  "\162\165\145\074\057\160\162\157\160\145\162\164\171\076\074\143"
  "\150\151\154\144\076\074\157\142\152\145\143\164\040\143\154\141"
  "\163\163\075\042\107\164\153\102\157\170\042\040\151\144\075\042"
  "\143\157\156\164\145\156\164\137\142\157\170\042\076\074\160\162"
  "\157\160\145\162\164\171\040\156\141\155\145\075\042\157\162\151"
  "\145\156\164\141\164\151\157\156\042\076\166\145\162\164\151\143"
  "\141\154\074\057\160\162\157\160\145\162\164\171\076\074\057\157"
  "\142\152\145\143\164\076\074\057\143\150\151\154\144\076\074\057"
  "\157\142\152\145\143\164\076\074\057\151\156\164\145\162\146\141"
  "\143\145\076\012\000\000\050\165\165\141\171\051\057\000\000\000"
  "\000\000\000\000" };
#endif /* !_MSC_VER */

static GStaticResource static_resource = { kbi_resource_data.data, sizeof (kbi_resource_data.data) - 1 /* nul terminator */, NULL, NULL, NULL };

G_MODULE_EXPORT
GResource *kbi_get_resource (void);
GResource *kbi_get_resource (void)
{
  return g_static_resource_get_resource (&static_resource);
}
/*
  If G_HAS_CONSTRUCTORS is true then the compiler support *both* constructors and
  destructors, in a usable way, including e.g. on library unload. If not you're on
  your own.

  Some compilers need #pragma to handle this, which does not work with macros,
  so the way you need to use this is (for constructors):

  #ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(my_constructor)
  #endif
  G_DEFINE_CONSTRUCTOR(my_constructor)
  static void my_constructor(void) {
   ...
  }

*/

#ifndef __GTK_DOC_IGNORE__

#if  __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR(_func) static void __attribute__((constructor)) _func (void);
#define G_DEFINE_DESTRUCTOR(_func) static void __attribute__((destructor)) _func (void);

#elif defined (_MSC_VER) && (_MSC_VER >= 1500)
/* Visual studio 2008 and later has _Pragma */

#include <stdlib.h>

#define G_HAS_CONSTRUCTORS 1

/* We do some weird things to avoid the constructors being optimized
 * away on VS2015 if WholeProgramOptimization is enabled. First we
 * make a reference to the array from the wrapper to make sure its
 * references. Then we use a pragma to make sure the wrapper function
 * symbol is always included at the link stage. Also, the symbols
 * need to be extern (but not dllexport), even though they are not
 * really used from another object file.
 */

/* We need to account for differences between the mangling of symbols
 * for x86 and x64/ARM/ARM64 programs, as symbols on x86 are prefixed
 * with an underscore but symbols on x64/ARM/ARM64 are not.
 */
#ifdef _M_IX86
#define G_MSVC_SYMBOL_PREFIX "_"
#else
#define G_MSVC_SYMBOL_PREFIX ""
#endif

#define G_DEFINE_CONSTRUCTOR(_func) G_MSVC_CTOR (_func, G_MSVC_SYMBOL_PREFIX)
#define G_DEFINE_DESTRUCTOR(_func) G_MSVC_DTOR (_func, G_MSVC_SYMBOL_PREFIX)

#define G_MSVC_CTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _wrapper(void) { _func(); g_slist_find (NULL,  _array ## _func); return 0; } \
  __pragma(comment(linker,"/include:" _sym_prefix # _func "_wrapper")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _wrapper;

#define G_MSVC_DTOR(_func,_sym_prefix) \
  static void _func(void); \
  extern int (* _array ## _func)(void);              \
  int _func ## _constructor(void) { atexit (_func); g_slist_find (NULL,  _array ## _func); return 0; } \
   __pragma(comment(linker,"/include:" _sym_prefix # _func "_constructor")) \
  __pragma(section(".CRT$XCU",read)) \
  __declspec(allocate(".CRT$XCU")) int (* _array ## _func)(void) = _func ## _constructor;

#elif defined (_MSC_VER)

#define G_HAS_CONSTRUCTORS 1

/* Pre Visual studio 2008 must use #pragma section */
#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _wrapper(void) { _func(); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (*p)(void) = _func ## _wrapper;

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  section(".CRT$XCU",read)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void); \
  static int _func ## _constructor(void) { atexit (_func); return 0; } \
  __declspec(allocate(".CRT$XCU")) static int (* _array ## _func)(void) = _func ## _constructor;

#elif defined(__SUNPRO_C)

/* This is not tested, but i believe it should work, based on:
 * http://opensource.apple.com/source/OpenSSL098/OpenSSL098-35/src/fips/fips_premain.c
 */

#define G_HAS_CONSTRUCTORS 1

#define G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA 1
#define G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA 1

#define G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(_func) \
  init(_func)
#define G_DEFINE_CONSTRUCTOR(_func) \
  static void _func(void);

#define G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(_func) \
  fini(_func)
#define G_DEFINE_DESTRUCTOR(_func) \
  static void _func(void);

#else

/* constructors not supported for this compiler */

#endif

#endif /* __GTK_DOC_IGNORE__ */

#ifdef G_HAS_CONSTRUCTORS

#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS(resource_constructor)
#endif
G_DEFINE_CONSTRUCTOR(resource_constructor)
#ifdef G_DEFINE_DESTRUCTOR_NEEDS_PRAGMA
#pragma G_DEFINE_DESTRUCTOR_PRAGMA_ARGS(resource_destructor)
#endif
G_DEFINE_DESTRUCTOR(resource_destructor)

#else
#warning "Constructor not supported on this compiler, linking in resources will not work"
#endif

static void resource_constructor (void)
{
  g_static_resource_init (&static_resource);
}

static void resource_destructor (void)
{
  g_static_resource_fini (&static_resource);
}
