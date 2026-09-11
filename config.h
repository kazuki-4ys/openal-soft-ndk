#ifndef CONFIG_H
#define CONFIG_H

#define ALSOFT_EAX 1
#define ALSOFT_EAX_PROTOTYPES 1

#define HAVE_STAT 1
#define HAVE_DIRENT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_STDINT_H 1

#if defined(__i386__) || defined(__x86_64__)
#define HAVE_GCC_GET_CPUID 1
#define HAVE_CPUID_H 1
#endif

#if defined(__i386__) || defined(__x86_64__)
#define HAVE_SSE 1
#define HAVE_SSE2 1
#define HAVE_SSE3 1
#define HAVE_SSE4_1 1
#define HAVE_NEON 0
#elif defined(__arm__) || defined(__aarch64__)
#define HAVE_SSE 0
#define HAVE_SSE2 0
#define HAVE_SSE3 0
#define HAVE_SSE4_1 0
#define HAVE_NEON 1
#else
#define HAVE_SSE 0
#define HAVE_SSE2 0
#define HAVE_SSE3 0
#define HAVE_SSE4_1 0
#define HAVE_NEON 0
#endif

#if defined(__i386__) && defined(__GNUC__)
#define FORCE_ALIGN __attribute__((force_align_arg_pointer))
#else
#define FORCE_ALIGN
#endif

#define HAVE_PFFFT 1
#define HAVE_LIBRT 0
#define HAVE_DBUS 0

#define ALSOFT_BACKEND_WAVE 1
#define ALSOFT_BACKEND_OPENSLES 1

#endif
