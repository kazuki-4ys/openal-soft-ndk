#ifndef CONFIG_SIMD_H
#define CONFIG_SIMD_H

/* SIMD support */
#if defined(__i386__) || defined(__x86_64__)
#define HAVE_SSE_INTRINSICS 1
#else
#define HAVE_SSE_INTRINSICS 0
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#define HAVE_NEON_INTRINSICS 1
#else
#define HAVE_NEON_INTRINSICS 0
#endif

#endif /* CONFIG_SIMD_H */
