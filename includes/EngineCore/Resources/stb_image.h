/* stb_image - v2.28 - public domain image loader - http://nothings.org/stb
                                  no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION
   #include "stb_image.h"

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as stock IJG lib)
      PNG 1/2/4/8/16-bit-per-channel

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels, 8/16 bit-per-channel)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


LICENSE

  See end of file for license information.

RECENT REVISION HISTORY:

      2.28  (2023-01-29) many error fixes, security errors, just tons of stuff
      2.27  (2021-07-11) document stbi_info better, 16-bit PNM support, bug fixes
      2.26  (2020-07-13) many minor fixes
      2.25  (2020-02-02) fix warnings
      2.24  (2020-02-02) fix warnings; thread-local failure_reason and flip_vertically
      2.23  (2019-08-11) fix clang static analysis warning
      2.22  (2019-03-04) gif fixes, fix warnings
      2.21  (2019-02-25) fix typo in comment
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) bugfix, 1-bit BMP, 16-bitness query, fix warnings
      2.16  (2017-07-23) all functions have 16-bit variants; optimizations; bugfixes
      2.15  (2017-03-18) fix png-1,2,4; all Imagenet JPGs; no runtime SSE detection on GCC
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-12-04) experimental 16-bit API, only for PNG so far; fixes
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) 16-bit PNGS; enable SSE2 in non-gcc x64
                         RGB-format JPEG; remove white matting in PSD;
                         allocate large structures on the stack;
                         correct channel count for PNG & BMP
      2.10  (2016-01-22) avoid warning introduced in 2.09
      2.09  (2016-01-16) 16-bit TGA; comments in PNM files; STBI_REALLOC_SIZED

   See end of file for full revision history.


 ============================    Contributors    =========================

 Image formats                          Extensions, features
    Sean Barrett (jpeg, png, bmp)          Jetro Lauha (stbi_info)
    Nicolas Schulz (hdr, psd)              Martin "SpartanJ" Golini (stbi_info)
    Jonathan Dummer (tga)                  James "moose2000" Brown (iPhone PNG)
    Jean-Marc Lienher (gif)                Ben "Disch" Wenger (io callbacks)
    Tom Seddon (pic)                       Omar Cornut (1/2/4-bit PNG)
    Thatcher Ulrich (psd)                  Nicolas Guillemot (vertical flip)
    Ken Miller (pgm, ppm)                  Richard Mitton (16-bit PSD)
    github:urraka (animated gif)           Junggon Kim (PNM comments)
    Christopher Forseth (animated gif)     Daniel Gibson (16-bit TGA)
                                           socks-the-fox (16-bit PNG)
                                           Jeremy Sawicki (handle all ImageNet JPGs)
 Optimizations & bugfixes                  Mikhail Morozov (1-bit BMP)
    Fabian "ryg" Giesen                    Anael Seghezzi (is-16-bit query)
    Arseny Kapoulkine                      Simon Breuss (16-bit PNM)
    John-Mark Allen
    Carmelo J Fdez-Aguera

 Bug & warning fixes
    Marc LeBlanc            David Woo          Guillaume George     Martins Mozeiko
    Christpher Lloyd        Jerry Jansson      Joseph Thomson       Blazej Dariusz Roszkowski
    Phil Jordan                                Dave Moore           Roy Eltham
    Hayaki Saito            Nathan Reed        Won Chun
    Luke Graham             Johan Duparc       Nick Verigakis       the Horde3D community
    Thomas Ruf              Ronny Chevalier                         github:rlyeh
    Janez Zemva             John Bartholomew   Michal Cichon        github:romigrou
    Jonathan Blow           Ken Hamada         Tero Hanninen        github:svdijk
    Eugene Golushkov        Laurent Gomila     Cort Stratton        github:snagar
    Aruelien Pocheville     Sergio Gonzalez    Thibault Reuille     github:Zelex
    Cass Everitt            Ryamond Barbiero                        github:grim210
    Paul Du Bois            Engin Manap        Aldo Culquicondor    github:sammyhw
    Philipp Wiesemann       Dale Weiler        Oriol Ferrer Mesia   github:phprus
    Josh Tobin              Neil Bickford      Matthew Gregan       github:poppolopoppo
    Julian Raschke          Gregory Mullen     Christian Floisand   github:darealshinji
    Baldur Karlsson         Kevin Schmidt      JR Smith             github:Michaelangel007
                            Brad Weinberger    Matvey Cherevko      github:mosra
    Luca Sas                Alexander Veselov  Zack Middleton       [reserved]
    Ryan C. Gordon          [reserved]                              [reserved]
                     DO NOT ADD YOUR NAME HERE

                     Jacko Dirks

  To add your name to the credits, pick a random blank space in the middle and fill it.
  80% of merge conflicts on stb PRs are due to people adding their name at the end
  of the credits.
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you said 0
//    stbi_image_free(data);
//
// Standard parameters:
//    int *x                 -- outputs image width in pixels
//    int *y                 -- outputs image height in pixels
//    int *channels_in_file  -- outputs # of image components in image file
//    int desired_channels   -- if non-zero, # of image components requested in result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'desired_channels' if desired_channels is non-zero, or
// *channels_in_file otherwise. If desired_channels is non-zero,
// *channels_in_file has the number of components that _would_ have been
// output otherwise. E.g. if you set desired_channels to 4, you will always
// get RGBA output, but you can check *channels_in_file to see if it's trivially
// opaque because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *channels_in_file will be unchanged. The function
// stbi_failure_reason() can be queried for an extremely brief, end-user
// unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
// to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
// more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// To query the width, height and component count of an image without having to
// decode the full file, you can use the stbi_info family of functions:
//
//   int x,y,n,ok;
//   ok = stbi_info(filename, &x, &y, &n);
//   // returns ok=1 and sets x, y, n if image is a supported format,
//   // 0 otherwise.
//
// Note that stb_image pervasively uses ints in its public API for sizes,
// including sizes of memory buffers. This is now part of the API and thus
// hard to change without causing breakage. As a result, the various image
// loaders all have certain limits on image size; these differ somewhat
// by format but generally boil down to either just under 2GB or just under
// 1GB. When the decoded image would be larger than this, stb_image decoding
// will fail.
//
// Additionally, stb_image will reject image files that have any of their
// dimensions set to a larger value than the configurable STBI_MAX_DIMENSIONS,
// which defaults to 2**24 = 16777216 pixels. Due to the above memory limit,
// the only way to have an image with such dimensions load correctly
// is for it to have a rather extreme aspect ratio. Either way, the
// assumption here is that such larger images are likely to be malformed
// or malicious. If you do need to load an image with individual dimensions
// larger than that, and it still fits in the overall size limit, you can
// #define STBI_MAX_DIMENSIONS on your own to be something larger.
//
// ===========================================================================
//
// UNICODE:
//
//   If compiling for Windows and you wish to use Unicode filenames, compile
//   with
//       #define STBI_WINDOWS_UTF8
//   and pass utf8-encoded filenames. Call stbi_convert_wchar_to_utf8 to convert
//   Windows wchar_t filenames to utf8.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to maintain",
// and for best performance I may provide less-easy-to-use APIs that give higher
// performance, in addition to the easy-to-use ones. Nevertheless, it's important
// to keep in mind that from the standpoint of you, a client of this library,
// all you care about is #1 and #3, and stb libraries DO NOT emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// provide more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the end).
//
// ===========================================================================
//
// SIMD support
//
// The JPEG decoder will try to automatically use SIMD kernels on x86 when
// supported by the compiler. For ARM Neon support, you must explicitly
// request it.
//
// (The old do-it-yourself SIMD API is no longer supported in the current
// code.)
//
// On x86, SSE2 will automatically be used when available based on a run-time
// test; if not, the generic C versions are used as a fall-back. On ARM targets,
// the typical path is to have separate builds for NEON and non-NEON devices
// (at least this is true for iOS and Android). Therefore, the NEON support is
// toggled by a build flag: define STBI_NEON to get NEON loops.
//
// If for some reason you do not want to use any of SIMD code, or if
// you have issues compiling it, you can disable it entirely by
// defining STBI_NO_SIMD.
//
// ===========================================================================
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image supports loading HDR images in general, and currently the Radiance
// .HDR file format specifically. You can still load any file through the existing
// interface; if you attempt to load an HDR file, it will be automatically remapped
// to LDR, assuming gamma 2.2 and an arbitrary scale factor defaulting to 1;
// both of these constants can be reconfigured through this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values, run through the inverse of
// constants corresponding to the above:
//
//     stbi_ldr_to_hdr_scale(1.0f);
//     stbi_ldr_to_hdr_gamma(2.2f);
//
// Finally, given a filename (or an open file or memory block--see header
// file for details) containing image data, you can query for the "most
// appropriate" interface to use (that is, whether the image is HDR or
// not), using:
//
//     stbi_is_hdr(char *filename);
//
// ===========================================================================
//
// iPhone PNG support:
//
// We optionally support converting iPhone-formatted PNGs (which store
// premultiplied BGRA) back to RGB, even though they're internally encoded
// differently. To enable this conversion, call
// stbi_convert_iphone_png_to_rgb(1).
//
// Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
// pixel to remove any premultiplied alpha *only* if the image file explicitly
// says there's premultiplied data (currently only happens in iPhone images,
// and only if iPhone convert-to-rgb processing is on).
//
// ===========================================================================
//
// ADDITIONAL CONFIGURATION
//
//  - You can suppress implementation of any of the decoders to reduce
//    your code footprint by #defining one or more of the following
//    symbols before creating the implementation.
//
//        STBI_NO_JPEG
//        STBI_NO_PNG
//        STBI_NO_BMP
//        STBI_NO_PSD
//        STBI_NO_TGA
//        STBI_NO_GIF
//        STBI_NO_HDR
//        STBI_NO_PIC
//        STBI_NO_PNM   (.ppm and .pgm)
//
//  - You can request *only* certain decoders and suppress all other ones
//    (this will be more forward-compatible, as addition of new decoders
//    doesn't require you to disable them explicitly):
//
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
//
//   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
//     want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB
//
//  - If you define STBI_MAX_DIMENSIONS, stb_image will reject images greater
//    than that size (in either width or height) without further processing.
//    This is to let programs in the wild set an upper bound to prevent
//    denial-of-service attacks on untrusted data, as one could generate a
//    valid image of gigantic dimensions and force stb_image to allocate a
//    huge block of memory and spend disproportionate time decoding it. By
//    default this is set to (1 << 24), which is 16777216, but that's still
//    very big.

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum
{
    STBI_default = 0, // only used for desired_channels

    STBI_grey = 1,
    STBI_grey_alpha = 2,
    STBI_rgb = 3,
    STBI_rgb_alpha = 4
};

#include <stdlib.h>
typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBIDEF
#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif
#endif

    //////////////////////////////////////////////////////////////////////////////
    //
    // PRIMARY API - works on images of any type
    //

    //
    // load image by filename, open file, or memory buffer
    //

    typedef struct
    {
        int      (*read)  (void* user, char* data, int size);   // fill 'data' with 'size' bytes.  return number of bytes actually read
        void     (*skip)  (void* user, int n);                 // skip the next 'n' bytes, or 'unget' the last -n bytes if negative
        int      (*eof)   (void* user);                       // returns nonzero if we are at end of file/data
    } stbi_io_callbacks;

    ////////////////////////////////////
    //
    // 8-bits-per-channel interface
    //

    STBIDEF stbi_uc* stbi_load_from_memory(stbi_uc           const* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF stbi_uc* stbi_load_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
    STBIDEF stbi_uc* stbi_load(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF stbi_uc* stbi_load_from_file(FILE* f, int* x, int* y, int* channels_in_file, int desired_channels);
    // for stbi_load_from_file, file pointer is left pointing immediately after image
#endif

#ifndef STBI_NO_GIF
    STBIDEF stbi_uc* stbi_load_gif_from_memory(stbi_uc const* buffer, int len, int** delays, int* x, int* y, int* z, int* comp, int req_comp);
#endif

#ifdef STBI_WINDOWS_UTF8
    STBIDEF int stbi_convert_wchar_to_utf8(char* buffer, size_t bufferlen, const wchar_t* input);
#endif

    ////////////////////////////////////
    //
    // 16-bits-per-channel interface
    //

    STBIDEF stbi_us* stbi_load_16_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF stbi_us* stbi_load_16_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
    STBIDEF stbi_us* stbi_load_16(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF stbi_us* stbi_load_from_file_16(FILE* f, int* x, int* y, int* channels_in_file, int desired_channels);
#endif

    ////////////////////////////////////
    //
    // float-per-channel interface
    //
#ifndef STBI_NO_LINEAR
    STBIDEF float* stbi_loadf_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF float* stbi_loadf_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* channels_in_file, int desired_channels);

#ifndef STBI_NO_STDIO
    STBIDEF float* stbi_loadf(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels);
    STBIDEF float* stbi_loadf_from_file(FILE* f, int* x, int* y, int* channels_in_file, int desired_channels);
#endif
#endif

#ifndef STBI_NO_HDR
    STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma);
    STBIDEF void   stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
    STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma);
    STBIDEF void   stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

    // stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
    STBIDEF int    stbi_is_hdr_from_callbacks(stbi_io_callbacks const* clbk, void* user);
    STBIDEF int    stbi_is_hdr_from_memory(stbi_uc const* buffer, int len);
#ifndef STBI_NO_STDIO
    STBIDEF int      stbi_is_hdr(char const* filename);
    STBIDEF int      stbi_is_hdr_from_file(FILE* f);
#endif // STBI_NO_STDIO


    // get a VERY brief reason for failure
    // on most compilers (and ALL modern mainstream compilers) this is threadsafe
    STBIDEF const char* stbi_failure_reason(void);

    // free the loaded image -- this is just free()
    STBIDEF void     stbi_image_free(void* retval_from_stbi_load);

    // get image dimensions & components without fully decoding
    STBIDEF int      stbi_info_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* comp);
    STBIDEF int      stbi_info_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp);
    STBIDEF int      stbi_is_16_bit_from_memory(stbi_uc const* buffer, int len);
    STBIDEF int      stbi_is_16_bit_from_callbacks(stbi_io_callbacks const* clbk, void* user);

#ifndef STBI_NO_STDIO
    STBIDEF int      stbi_info(char const* filename, int* x, int* y, int* comp);
    STBIDEF int      stbi_info_from_file(FILE* f, int* x, int* y, int* comp);
    STBIDEF int      stbi_is_16_bit(char const* filename);
    STBIDEF int      stbi_is_16_bit_from_file(FILE* f);
#endif



    // for image formats that explicitly notate that they have premultiplied alpha,
    // we just return the colors as stored in the file. set this flag to force
    // unpremultiplication. results are undefined if the unpremultiply overflow.
    STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

    // indicate whether we should process iphone images back to canonical format,
    // or just pass them through "as-is"
    STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

    // flip the image vertically, so the first pixel in the output array is the bottom left
    STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

    // as above, but only applies to images loaded on the thread that calls the function
    // this function is only available if your compiler supports thread-local variables;
    // calling it will fail to link if your compiler doesn't
    STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply);
    STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert);
    STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip);

    // ZLIB client - used by PNG, available for other purposes

    STBIDEF char* stbi_zlib_decode_malloc_guesssize(const char* buffer, int len, int initial_size, int* outlen);
    STBIDEF char* stbi_zlib_decode_malloc_guesssize_headerflag(const char* buffer, int len, int initial_size, int* outlen, int parse_header);
    STBIDEF char* stbi_zlib_decode_malloc(const char* buffer, int len, int* outlen);
    STBIDEF int   stbi_zlib_decode_buffer(char* obuffer, int olen, const char* ibuffer, int ilen);

    STBIDEF char* stbi_zlib_decode_noheader_malloc(const char* buffer, int len, int* outlen);
    STBIDEF int   stbi_zlib_decode_noheader_buffer(char* obuffer, int olen, const char* ibuffer, int ilen);


#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#if defined(STBI_ONLY_JPEG) || defined(STBI_ONLY_PNG) || defined(STBI_ONLY_BMP) \
  || defined(STBI_ONLY_TGA) || defined(STBI_ONLY_GIF) || defined(STBI_ONLY_PSD) \
  || defined(STBI_ONLY_HDR) || defined(STBI_ONLY_PIC) || defined(STBI_ONLY_PNM) \
  || defined(STBI_ONLY_ZLIB)
#ifndef STBI_ONLY_JPEG
#define STBI_NO_JPEG
#endif
#ifndef STBI_ONLY_PNG
#define STBI_NO_PNG
#endif
#ifndef STBI_ONLY_BMP
#define STBI_NO_BMP
#endif
#ifndef STBI_ONLY_PSD
#define STBI_NO_PSD
#endif
#ifndef STBI_ONLY_TGA
#define STBI_NO_TGA
#endif
#ifndef STBI_ONLY_GIF
#define STBI_NO_GIF
#endif
#ifndef STBI_ONLY_HDR
#define STBI_NO_HDR
#endif
#ifndef STBI_ONLY_PIC
#define STBI_NO_PIC
#endif
#ifndef STBI_ONLY_PNM
#define STBI_NO_PNM
#endif
#endif

#if defined(STBI_NO_PNG) && !defined(STBI_SUPPORT_ZLIB) && !defined(STBI_NO_ZLIB)
#define STBI_NO_ZLIB
#endif


#include <stdarg.h>
#include <stddef.h> // ptrdiff_t on osx
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR)
#include <math.h>  // ldexp, pow
#endif

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

#ifndef STBI_ASSERT
#include <assert.h>
#define STBI_ASSERT(x) assert(x)
#endif

#ifdef __cplusplus
#define STBI_EXTERN extern "C"
#else
#define STBI_EXTERN extern
#endif


#ifndef _MSC_VER
#ifdef __cplusplus
#define stbi_inline inline
#else
#define stbi_inline
#endif
#else
#define stbi_inline __forceinline
#endif

#ifndef STBI_NO_THREAD_LOCALS
#if defined(__cplusplus) &&  __cplusplus >= 201103L
#define STBI_THREAD_LOCAL       thread_local
#elif defined(__GNUC__) && __GNUC__ < 5
#define STBI_THREAD_LOCAL       __thread
#elif defined(_MSC_VER)
#define STBI_THREAD_LOCAL       __declspec(thread)
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#define STBI_THREAD_LOCAL       _Thread_local
#endif

#ifndef STBI_THREAD_LOCAL
#if defined(__GNUC__)
#define STBI_THREAD_LOCAL       __thread
#endif
#endif
#endif

#if defined(_MSC_VER) || defined(__SYMBIAN32__)
typedef unsigned short stbi__uint16;
typedef   signed short stbi__int16;
typedef unsigned int   stbi__uint32;
typedef   signed int   stbi__int32;
#else
#include <stdint.h>
typedef uint16_t stbi__uint16;
typedef int16_t  stbi__int16;
typedef uint32_t stbi__uint32;
typedef int32_t  stbi__int32;
#endif

// should produce compiler error if size is wrong
typedef unsigned char validate_uint32[sizeof(stbi__uint32) == 4 ? 1 : -1];

#ifdef _MSC_VER
#define STBI_NOTUSED(v)  (void)(v)
#else
#define STBI_NOTUSED(v)  (void)sizeof(v)
#endif

#ifdef _MSC_VER
#define STBI_HAS_LROTL
#endif

#ifdef STBI_HAS_LROTL
#define stbi_lrot(x,y)  _lrotl(x,y)
#else
#define stbi_lrot(x,y)  (((x) << (y)) | ((x) >> (-(y) & 31)))
#endif

#if defined(STBI_MALLOC) && defined(STBI_FREE) && (defined(STBI_REALLOC) || defined(STBI_REALLOC_SIZED))
// ok
#elif !defined(STBI_MALLOC) && !defined(STBI_FREE) && !defined(STBI_REALLOC) && !defined(STBI_REALLOC_SIZED)
// ok
#else
#error "Must define all or none of STBI_MALLOC, STBI_FREE, and STBI_REALLOC (or STBI_REALLOC_SIZED)."
#endif

#ifndef STBI_MALLOC
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)
#endif

#ifndef STBI_REALLOC_SIZED
#define STBI_REALLOC_SIZED(p,oldsz,newsz) STBI_REALLOC(p,newsz)
#endif

// x86/x64 detection
#if defined(__x86_64__) || defined(_M_X64)
#define STBI__X64_TARGET
#elif defined(__i386) || defined(_M_IX86)
#define STBI__X86_TARGET
#endif

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) && !defined(STBI_NO_SIMD)
// gcc doesn't support sse2 intrinsics unless you compile with -msse2,
// which in turn means it gets to use SSE2 everywhere. This is unfortunate,
// but previous attempts to provide the SSE2 functions with runtime
// detection caused numerous issues. The way architecture extensions are
// exposed in GCC/Clang is, sadly, not really suited for one-file libs.
// New behavior: if compiled with -msse2, we use SSE2 without any
// detection; if not, we don't use it at all.
#define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) && !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
// Note that __MINGW32__ doesn't actually mean 32-bit, so we have to avoid STBI__X64_TARGET
//
// 32-bit MinGW wants ESP to be 16-byte aligned, but this is not in the
// Windows ABI and VC++ as well as Windows DLLs don't maintain that invariant.
// As a result, enabling SSE2 on 32-bit MinGW is dangerous when not
// simultaneously enabling "-mstackrealign".
//
// See https://github.com/nothings/stb/issues/81 for more information.
//
// So default to no SSE2 on 32-bit MinGW. If you've read this far and added
// -mstackrealign to your build settings, feel free to #define STBI_MINGW_ENABLE_SSE2.
#define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) && (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400  // not VC6
#include <intrin.h> // __cpuid
static int stbi__cpuid3(void)
{
    int info[4];
    __cpuid(info, 1);
    return info[3];
}
#else
static int stbi__cpuid3(void)
{
    int res;
    __asm {
        mov  eax, 1
        cpuid
        mov  res, edx
    }
    return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
    int info3 = stbi__cpuid3();
    return ((info3 >> 26) & 1) != 0;
}
#endif

#else // assume GCC-style if not VC++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

#if !defined(STBI_NO_JPEG) && defined(STBI_SSE2)
static int stbi__sse2_available(void)
{
    // If we're even attempting to compile this on GCC/Clang, that means
    // -msse2 is on, which means the compiler is allowed to use SSE2
    // instructions at will, and so are we.
    return 1;
}
#endif

#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
#undef STBI_NEON
#endif

#ifdef STBI_NEON
#include <arm_neon.h>
#ifdef _MSC_VER
#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name
#else
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
#endif
#endif

#ifndef STBI_SIMD_ALIGN
#define STBI_SIMD_ALIGN(type, name) type name
#endif

#ifndef STBI_MAX_DIMENSIONS
#define STBI_MAX_DIMENSIONS (1 << 24)
#endif

///////////////////////////////////////////////
//
//  stbi__context struct and start_xxx functions

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct
{
    stbi__uint32 img_x, img_y;
    int img_n, img_out_n;

    stbi_io_callbacks io;
    void* io_user_data;

    int read_from_callbacks;
    int buflen;
    stbi_uc buffer_start[128];
    int callback_already_read;

    stbi_uc* img_buffer, * img_buffer_end;
    stbi_uc* img_buffer_original, * img_buffer_original_end;
} stbi__context;


static void stbi__refill_buffer(stbi__context* s);

// initialize a memory-decode context
static void stbi__start_mem(stbi__context* s, stbi_uc const* buffer, int len)
{
    s->io.read = NULL;
    s->read_from_callbacks = 0;
    s->callback_already_read = 0;
    s->img_buffer = s->img_buffer_original = (stbi_uc*)buffer;
    s->img_buffer_end = s->img_buffer_original_end = (stbi_uc*)buffer + len;
}

// initialize a callback-based context
static void stbi__start_callbacks(stbi__context* s, stbi_io_callbacks* c, void* user)
{
    s->io = *c;
    s->io_user_data = user;
    s->buflen = sizeof(s->buffer_start);
    s->read_from_callbacks = 1;
    s->callback_already_read = 0;
    s->img_buffer = s->img_buffer_original = s->buffer_start;
    stbi__refill_buffer(s);
    s->img_buffer_original_end = s->img_buffer_end;
}

#ifndef STBI_NO_STDIO

static int stbi__stdio_read(void* user, char* data, int size)
{
    return (int)fread(data, 1, size, (FILE*)user);
}

static void stbi__stdio_skip(void* user, int n)
{
    int ch;
    fseek((FILE*)user, n, SEEK_CUR);
    ch = fgetc((FILE*)user);  /* have to read a byte to reset feof()'s flag */
    if (ch != EOF) {
        ungetc(ch, (FILE*)user);  /* push byte back onto stream if valid. */
    }
}

static int stbi__stdio_eof(void* user)
{
    return feof((FILE*)user) || ferror((FILE*)user);
}

static stbi_io_callbacks stbi__stdio_callbacks =
{
   stbi__stdio_read,
   stbi__stdio_skip,
   stbi__stdio_eof,
};

static void stbi__start_file(stbi__context* s, FILE* f)
{
    stbi__start_callbacks(s, &stbi__stdio_callbacks, (void*)f);
}

//static void stop_file(stbi__context *s) { }

#endif // !STBI_NO_STDIO

static void stbi__rewind(stbi__context* s)
{
    // conceptually rewind SHOULD rewind to the beginning of the stream,
    // but we just rewind to the beginning of the initial buffer, because
    // we only use it after doing 'test', which only ever looks at at most 92 bytes
    s->img_buffer = s->img_buffer_original;
    s->img_buffer_end = s->img_buffer_original_end;
}

enum
{
    STBI_ORDER_RGB,
    STBI_ORDER_BGR
};

typedef struct
{
    int bits_per_channel;
    int num_channels;
    int channel_order;
} stbi__result_info;

#ifndef STBI_NO_JPEG
static int      stbi__jpeg_test(stbi__context* s);
static void* stbi__jpeg_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__jpeg_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_PNG
static int      stbi__png_test(stbi__context* s);
static void* stbi__png_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__png_info(stbi__context* s, int* x, int* y, int* comp);
static int      stbi__png_is16(stbi__context* s);
#endif

#ifndef STBI_NO_BMP
static int      stbi__bmp_test(stbi__context* s);
static void* stbi__bmp_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__bmp_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_TGA
static int      stbi__tga_test(stbi__context* s);
static void* stbi__tga_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__tga_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_PSD
static int      stbi__psd_test(stbi__context* s);
static void* stbi__psd_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri, int bpc);
static int      stbi__psd_info(stbi__context* s, int* x, int* y, int* comp);
static int      stbi__psd_is16(stbi__context* s);
#endif

#ifndef STBI_NO_HDR
static int      stbi__hdr_test(stbi__context* s);
static float* stbi__hdr_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__hdr_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_PIC
static int      stbi__pic_test(stbi__context* s);
static void* stbi__pic_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__pic_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_GIF
static int      stbi__gif_test(stbi__context* s);
static void* stbi__gif_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static void* stbi__load_gif_main(stbi__context* s, int** delays, int* x, int* y, int* z, int* comp, int req_comp);
static int      stbi__gif_info(stbi__context* s, int* x, int* y, int* comp);
#endif

#ifndef STBI_NO_PNM
static int      stbi__pnm_test(stbi__context* s);
static void* stbi__pnm_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri);
static int      stbi__pnm_info(stbi__context* s, int* x, int* y, int* comp);
static int      stbi__pnm_is16(stbi__context* s);
#endif

static
#ifdef STBI_THREAD_LOCAL
STBI_THREAD_LOCAL
#endif
const char* stbi__g_failure_reason;

STBIDEF const char* stbi_failure_reason(void)
{
    return stbi__g_failure_reason;
}

#ifndef STBI_NO_FAILURE_STRINGS
static int stbi__err(const char* str)
{
    stbi__g_failure_reason = str;
    return 0;
}
#endif

static void* stbi__malloc(size_t size)
{
    return STBI_MALLOC(size);
}

// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int stbi__addsizes_valid(int a, int b)
{
    if (b < 0) return 0;
    // now 0 <= b <= INT_MAX, hence also
    // 0 <= INT_MAX - b <= INTMAX.
    // And "a + b <= INT_MAX" (which might overflow) is the
    // same as a <= INT_MAX - b (no overflow)
    return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int stbi__mul2sizes_valid(int a, int b)
{
    if (a < 0 || b < 0) return 0;
    if (b == 0) return 1; // mul-by-0 is always safe
    // portable way to check for no overflows in a*b
    return a <= INT_MAX / b;
}

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// returns 1 if "a*b + add" has no negative terms/factors and doesn't overflow
static int stbi__mad2sizes_valid(int a, int b, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a * b, add);
}
#endif

// returns 1 if "a*b*c + add" has no negative terms/factors and doesn't overflow
static int stbi__mad3sizes_valid(int a, int b, int c, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
        stbi__addsizes_valid(a * b * c, add);
}

// returns 1 if "a*b*c*d + add" has no negative terms/factors and doesn't overflow
#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add)
{
    return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
        stbi__mul2sizes_valid(a * b * c, d) && stbi__addsizes_valid(a * b * c * d, add);
}
#endif

#if !defined(STBI_NO_JPEG) || !defined(STBI_NO_PNG) || !defined(STBI_NO_TGA) || !defined(STBI_NO_HDR)
// mallocs with size overflow checking
static void* stbi__malloc_mad2(int a, int b, int add)
{
    if (!stbi__mad2sizes_valid(a, b, add)) return NULL;
    return stbi__malloc(a * b + add);
}
#endif

static void* stbi__malloc_mad3(int a, int b, int c, int add)
{
    if (!stbi__mad3sizes_valid(a, b, c, add)) return NULL;
    return stbi__malloc(a * b * c + add);
}

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR) || !defined(STBI_NO_PNM)
static void* stbi__malloc_mad4(int a, int b, int c, int d, int add)
{
    if (!stbi__mad4sizes_valid(a, b, c, d, add)) return NULL;
    return stbi__malloc(a * b * c * d + add);
}
#endif

// returns 1 if the sum of two signed ints is valid (between -2^31 and 2^31-1 inclusive), 0 on overflow.
static int stbi__addints_valid(int a, int b)
{
    if ((a >= 0) != (b >= 0)) return 1; // a and b have different signs, so no overflow
    if (a < 0 && b < 0) return a >= INT_MIN - b; // same as a + b >= INT_MIN; INT_MIN - b cannot overflow since b < 0.
    return a <= INT_MAX - b;
}

// returns 1 if the product of two signed shorts is valid, 0 on overflow.
static int stbi__mul2shorts_valid(short a, short b)
{
    if (b == 0 || b == -1) return 1; // multiplication by 0 is always 0; check for -1 so SHRT_MIN/b doesn't overflow
    if ((a >= 0) == (b >= 0)) return a <= SHRT_MAX / b; // product is positive, so similar to mul2sizes_valid
    if (b < 0) return a <= SHRT_MIN / b; // same as a * b >= SHRT_MIN
    return a >= SHRT_MIN / b;
}

// stbi__err - error
// stbi__errpf - error returning pointer to float
// stbi__errpuc - error returning pointer to unsigned char

#ifdef STBI_NO_FAILURE_STRINGS
#define stbi__err(x,y)  0
#elif defined(STBI_FAILURE_USERMSG)
#define stbi__err(x,y)  stbi__err(y)
#else
#define stbi__err(x,y)  stbi__err(x)
#endif

#define stbi__errpf(x,y)   ((float *)(size_t) (stbi__err(x,y)?NULL:NULL))
#define stbi__errpuc(x,y)  ((unsigned char *)(size_t) (stbi__err(x,y)?NULL:NULL))

STBIDEF void stbi_image_free(void* retval_from_stbi_load)
{
    STBI_FREE(retval_from_stbi_load);
}

#ifndef STBI_NO_LINEAR
static float* stbi__ldr_to_hdr(stbi_uc* data, int x, int y, int comp);
#endif

#ifndef STBI_NO_HDR
static stbi_uc* stbi__hdr_to_ldr(float* data, int x, int y, int comp);
#endif

static int stbi__vertically_flip_on_load_global = 0;

STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
    stbi__vertically_flip_on_load_global = flag_true_if_should_flip;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__vertically_flip_on_load  stbi__vertically_flip_on_load_global
#else
static STBI_THREAD_LOCAL int stbi__vertically_flip_on_load_local, stbi__vertically_flip_on_load_set;

STBIDEF void stbi_set_flip_vertically_on_load_thread(int flag_true_if_should_flip)
{
    stbi__vertically_flip_on_load_local = flag_true_if_should_flip;
    stbi__vertically_flip_on_load_set = 1;
}

#define stbi__vertically_flip_on_load  (stbi__vertically_flip_on_load_set       \
                                         ? stbi__vertically_flip_on_load_local  \
                                         : stbi__vertically_flip_on_load_global)
#endif // STBI_THREAD_LOCAL

static void* stbi__load_main(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri, int bpc)
{
    memset(ri, 0, sizeof(*ri)); // make sure it's initialized if we add new fields
    ri->bits_per_channel = 8; // default is 8 so most paths don't have to be changed
    ri->channel_order = STBI_ORDER_RGB; // all current input & output are this, but this is here so we can add BGR order
    ri->num_channels = 0;

    // test the formats with a very explicit header first (at least a FOURCC
    // or distinctive magic number first)
#ifndef STBI_NO_PNG
    if (stbi__png_test(s))  return stbi__png_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_BMP
    if (stbi__bmp_test(s))  return stbi__bmp_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_GIF
    if (stbi__gif_test(s))  return stbi__gif_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PSD
    if (stbi__psd_test(s))  return stbi__psd_load(s, x, y, comp, req_comp, ri, bpc);
#else
    STBI_NOTUSED(bpc);
#endif
#ifndef STBI_NO_PIC
    if (stbi__pic_test(s))  return stbi__pic_load(s, x, y, comp, req_comp, ri);
#endif

    // then the formats that can end up attempting to load with just 1 or 2
    // bytes matching expectations; these are prone to false positives, so
    // try them later
#ifndef STBI_NO_JPEG
    if (stbi__jpeg_test(s)) return stbi__jpeg_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PNM
    if (stbi__pnm_test(s))  return stbi__pnm_load(s, x, y, comp, req_comp, ri);
#endif

#ifndef STBI_NO_HDR
    if (stbi__hdr_test(s)) {
        float* hdr = stbi__hdr_load(s, x, y, comp, req_comp, ri);
        return stbi__hdr_to_ldr(hdr, *x, *y, req_comp ? req_comp : *comp);
    }
#endif

#ifndef STBI_NO_TGA
    // test tga last because it's a crappy test!
    if (stbi__tga_test(s))
        return stbi__tga_load(s, x, y, comp, req_comp, ri);
#endif

    return stbi__errpuc("unknown image type", "Image not of any known type, or corrupt");
}

static stbi_uc* stbi__convert_16_to_8(stbi__uint16* orig, int w, int h, int channels)
{
    int i;
    int img_len = w * h * channels;
    stbi_uc* reduced;

    reduced = (stbi_uc*)stbi__malloc(img_len);
    if (reduced == NULL) return stbi__errpuc("outofmem", "Out of memory");

    for (i = 0; i < img_len; ++i)
        reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // top half of each byte is sufficient approx of 16->8 bit scaling

    STBI_FREE(orig);
    return reduced;
}

static stbi__uint16* stbi__convert_8_to_16(stbi_uc* orig, int w, int h, int channels)
{
    int i;
    int img_len = w * h * channels;
    stbi__uint16* enlarged;

    enlarged = (stbi__uint16*)stbi__malloc(img_len * 2);
    if (enlarged == NULL) return (stbi__uint16*)stbi__errpuc("outofmem", "Out of memory");

    for (i = 0; i < img_len; ++i)
        enlarged[i] = (stbi__uint16)((orig[i] << 8) + orig[i]); // replicate to high and low byte, maps 0->0, 255->0xffff

    STBI_FREE(orig);
    return enlarged;
}

static void stbi__vertical_flip(void* image, int w, int h, int bytes_per_pixel)
{
    int row;
    size_t bytes_per_row = (size_t)w * bytes_per_pixel;
    stbi_uc temp[2048];
    stbi_uc* bytes = (stbi_uc*)image;

    for (row = 0; row < (h >> 1); row++) {
        stbi_uc* row0 = bytes + row * bytes_per_row;
        stbi_uc* row1 = bytes + (h - row - 1) * bytes_per_row;
        // swap row0 with row1
        size_t bytes_left = bytes_per_row;
        while (bytes_left) {
            size_t bytes_copy = (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
            memcpy(temp, row0, bytes_copy);
            memcpy(row0, row1, bytes_copy);
            memcpy(row1, temp, bytes_copy);
            row0 += bytes_copy;
            row1 += bytes_copy;
            bytes_left -= bytes_copy;
        }
    }
}

#ifndef STBI_NO_GIF
static void stbi__vertical_flip_slices(void* image, int w, int h, int z, int bytes_per_pixel)
{
    int slice;
    int slice_size = w * h * bytes_per_pixel;

    stbi_uc* bytes = (stbi_uc*)image;
    for (slice = 0; slice < z; ++slice) {
        stbi__vertical_flip(bytes, w, h, bytes_per_pixel);
        bytes += slice_size;
    }
}
#endif

static unsigned char* stbi__load_and_postprocess_8bit(stbi__context* s, int* x, int* y, int* comp, int req_comp)
{
    stbi__result_info ri;
    void* result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

    if (result == NULL)
        return NULL;

    // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
    STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

    if (ri.bits_per_channel != 8) {
        result = stbi__convert_16_to_8((stbi__uint16*)result, *x, *y, req_comp == 0 ? *comp : req_comp);
        ri.bits_per_channel = 8;
    }

    // @TODO: move stbi__convert_format to here

    if (stbi__vertically_flip_on_load) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
    }

    return (unsigned char*)result;
}

static stbi__uint16* stbi__load_and_postprocess_16bit(stbi__context* s, int* x, int* y, int* comp, int req_comp)
{
    stbi__result_info ri;
    void* result = stbi__load_main(s, x, y, comp, req_comp, &ri, 16);

    if (result == NULL)
        return NULL;

    // it is the responsibility of the loaders to make sure we get either 8 or 16 bit.
    STBI_ASSERT(ri.bits_per_channel == 8 || ri.bits_per_channel == 16);

    if (ri.bits_per_channel != 16) {
        result = stbi__convert_8_to_16((stbi_uc*)result, *x, *y, req_comp == 0 ? *comp : req_comp);
        ri.bits_per_channel = 16;
    }

    // @TODO: move stbi__convert_format16 to here
    // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to keep more precision

    if (stbi__vertically_flip_on_load) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi__uint16));
    }

    return (stbi__uint16*)result;
}

#if !defined(STBI_NO_HDR) && !defined(STBI_NO_LINEAR)
static void stbi__float_postprocess(float* result, int* x, int* y, int* comp, int req_comp)
{
    if (stbi__vertically_flip_on_load && result != NULL) {
        int channels = req_comp ? req_comp : *comp;
        stbi__vertical_flip(result, *x, *y, channels * sizeof(float));
    }
}
#endif

#ifndef STBI_NO_STDIO

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBI_EXTERN __declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int cp, unsigned long flags, const char* str, int cbmb, wchar_t* widestr, int cchwide);
STBI_EXTERN __declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, const wchar_t* widestr, int cchwide, char* str, int cbmb, const char* defchar, int* used_default);
#endif

#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
STBIDEF int stbi_convert_wchar_to_utf8(char* buffer, size_t bufferlen, const wchar_t* input)
{
    return WideCharToMultiByte(65001 /* UTF8 */, 0, input, -1, buffer, (int)bufferlen, NULL, NULL);
}
#endif

static FILE* stbi__fopen(char const* filename, char const* mode)
{
    FILE* f;
#if defined(_WIN32) && defined(STBI_WINDOWS_UTF8)
    wchar_t wMode[64];
    wchar_t wFilename[1024];
    if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, filename, -1, wFilename, sizeof(wFilename) / sizeof(*wFilename)))
        return 0;

    if (0 == MultiByteToWideChar(65001 /* UTF8 */, 0, mode, -1, wMode, sizeof(wMode) / sizeof(*wMode)))
        return 0;

#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (0 != _wfopen_s(&f, wFilename, wMode))
        f = 0;
#else
    f = _wfopen(wFilename, wMode);
#endif

#elif defined(_MSC_VER) && _MSC_VER >= 1400
    if (0 != fopen_s(&f, filename, mode))
        f = 0;
#else
    f = fopen(filename, mode);
#endif
    return f;
}


STBIDEF stbi_uc* stbi_load(char const* filename, int* x, int* y, int* comp, int req_comp)
{
    FILE* f = stbi__fopen(filename, "rb");
    unsigned char* result;
    if (!f) return stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}

STBIDEF stbi_uc* stbi_load_from_file(FILE* f, int* x, int* y, int* comp, int req_comp)
{
    unsigned char* result;
    stbi__context s;
    stbi__start_file(&s, f);
    result = stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
    if (result) {
        // need to 'unget' all the characters in the IO buffer
        fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
    }
    return result;
}

STBIDEF stbi__uint16* stbi_load_from_file_16(FILE* f, int* x, int* y, int* comp, int req_comp)
{
    stbi__uint16* result;
    stbi__context s;
    stbi__start_file(&s, f);
    result = stbi__load_and_postprocess_16bit(&s, x, y, comp, req_comp);
    if (result) {
        // need to 'unget' all the characters in the IO buffer
        fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
    }
    return result;
}

STBIDEF stbi_us* stbi_load_16(char const* filename, int* x, int* y, int* comp, int req_comp)
{
    FILE* f = stbi__fopen(filename, "rb");
    stbi__uint16* result;
    if (!f) return (stbi_us*)stbi__errpuc("can't fopen", "Unable to open file");
    result = stbi_load_from_file_16(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}


#endif //!STBI_NO_STDIO

STBIDEF stbi_us* stbi_load_16_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file, desired_channels);
}

STBIDEF stbi_us* stbi_load_16_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* channels_in_file, int desired_channels)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)clbk, user);
    return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file, desired_channels);
}

STBIDEF stbi_uc* stbi_load_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* comp, int req_comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

STBIDEF stbi_uc* stbi_load_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp, int req_comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)clbk, user);
    return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_GIF
STBIDEF stbi_uc* stbi_load_gif_from_memory(stbi_uc const* buffer, int len, int** delays, int* x, int* y, int* z, int* comp, int req_comp)
{
    unsigned char* result;
    stbi__context s;
    stbi__start_mem(&s, buffer, len);

    result = (unsigned char*)stbi__load_gif_main(&s, delays, x, y, z, comp, req_comp);
    if (stbi__vertically_flip_on_load) {
        stbi__vertical_flip_slices(result, *x, *y, *z, *comp);
    }

    return result;
}
#endif

#ifndef STBI_NO_LINEAR
static float* stbi__loadf_main(stbi__context* s, int* x, int* y, int* comp, int req_comp)
{
    unsigned char* data;
#ifndef STBI_NO_HDR
    if (stbi__hdr_test(s)) {
        stbi__result_info ri;
        float* hdr_data = stbi__hdr_load(s, x, y, comp, req_comp, &ri);
        if (hdr_data)
            stbi__float_postprocess(hdr_data, x, y, comp, req_comp);
        return hdr_data;
    }
#endif
    data = stbi__load_and_postprocess_8bit(s, x, y, comp, req_comp);
    if (data)
        return stbi__ldr_to_hdr(data, *x, *y, req_comp ? req_comp : *comp);
    return stbi__errpf("unknown image type", "Image not of any known type, or corrupt");
}

STBIDEF float* stbi_loadf_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* comp, int req_comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}

STBIDEF float* stbi_loadf_from_callbacks(stbi_io_callbacks const* clbk, void* user, int* x, int* y, int* comp, int req_comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)clbk, user);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_STDIO
STBIDEF float* stbi_loadf(char const* filename, int* x, int* y, int* comp, int req_comp)
{
    float* result;
    FILE* f = stbi__fopen(filename, "rb");
    if (!f) return stbi__errpf("can't fopen", "Unable to open file");
    result = stbi_loadf_from_file(f, x, y, comp, req_comp);
    fclose(f);
    return result;
}

STBIDEF float* stbi_loadf_from_file(FILE* f, int* x, int* y, int* comp, int req_comp)
{
    stbi__context s;
    stbi__start_file(&s, f);
    return stbi__loadf_main(&s, x, y, comp, req_comp);
}
#endif // !STBI_NO_STDIO

#endif // !STBI_NO_LINEAR

// these is-hdr-or-not is defined independent of whether STBI_NO_LINEAR is
// defined, for API simplicity; if STBI_NO_LINEAR is defined, it always
// reports false!

STBIDEF int stbi_is_hdr_from_memory(stbi_uc const* buffer, int len)
{
#ifndef STBI_NO_HDR
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__hdr_test(&s);
#else
    STBI_NOTUSED(buffer);
    STBI_NOTUSED(len);
    return 0;
#endif
}

#ifndef STBI_NO_STDIO
STBIDEF int      stbi_is_hdr(char const* filename)
{
    FILE* f = stbi__fopen(filename, "rb");
    int result = 0;
    if (f) {
        result = stbi_is_hdr_from_file(f);
        fclose(f);
    }
    return result;
}

STBIDEF int stbi_is_hdr_from_file(FILE* f)
{
#ifndef STBI_NO_HDR
    long pos = ftell(f);
    int res;
    stbi__context s;
    stbi__start_file(&s, f);
    res = stbi__hdr_test(&s);
    fseek(f, pos, SEEK_SET);
    return res;
#else
    STBI_NOTUSED(f);
    return 0;
#endif
}
#endif // !STBI_NO_STDIO

STBIDEF int      stbi_is_hdr_from_callbacks(stbi_io_callbacks const* clbk, void* user)
{
#ifndef STBI_NO_HDR
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)clbk, user);
    return stbi__hdr_test(&s);
#else
    STBI_NOTUSED(clbk);
    STBI_NOTUSED(user);
    return 0;
#endif
}

#ifndef STBI_NO_LINEAR
static float stbi__l2h_gamma = 2.2f, stbi__l2h_scale = 1.0f;

STBIDEF void   stbi_ldr_to_hdr_gamma(float gamma) { stbi__l2h_gamma = gamma; }
STBIDEF void   stbi_ldr_to_hdr_scale(float scale) { stbi__l2h_scale = scale; }
#endif

static float stbi__h2l_gamma_i = 1.0f / 2.2f, stbi__h2l_scale_i = 1.0f;

STBIDEF void   stbi_hdr_to_ldr_gamma(float gamma) { stbi__h2l_gamma_i = 1 / gamma; }
STBIDEF void   stbi_hdr_to_ldr_scale(float scale) { stbi__h2l_scale_i = 1 / scale; }


//////////////////////////////////////////////////////////////////////////////
//
// Common code used by all image loaders
//

enum
{
    STBI__SCAN_load = 0,
    STBI__SCAN_type,
    STBI__SCAN_header
};

static void stbi__refill_buffer(stbi__context* s)
{
    int n = (s->io.read)(s->io_user_data, (char*)s->buffer_start, s->buflen);
    s->callback_already_read += (int)(s->img_buffer - s->img_buffer_original);
    if (n == 0) {
        // at end of file, treat same as if from memory, but need to handle case
        // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
        s->read_from_callbacks = 0;
        s->img_buffer = s->buffer_start;
        s->img_buffer_end = s->buffer_start + 1;
        *s->img_buffer = 0;
    }
    else {
        s->img_buffer = s->buffer_start;
        s->img_buffer_end = s->buffer_start + n;
    }
}

stbi_inline static stbi_uc stbi__get8(stbi__context* s)
{
    if (s->img_buffer < s->img_buffer_end)
        return *s->img_buffer++;
    if (s->read_from_callbacks) {
        stbi__refill_buffer(s);
        return *s->img_buffer++;
    }
    return 0;
}

#if defined(STBI_NO_JPEG) && defined(STBI_NO_HDR) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
stbi_inline static int stbi__at_eof(stbi__context* s)
{
    if (s->io.read) {
        if (!(s->io.eof)(s->io_user_data)) return 0;
        // if feof() is true, check if buffer = end
        // special case: we've only got the special 0 character at the end
        if (s->read_from_callbacks == 0) return 1;
    }

    return s->img_buffer >= s->img_buffer_end;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC)
// nothing
#else
static void stbi__skip(stbi__context* s, int n)
{
    if (n == 0) return;  // already there!
    if (n < 0) {
        s->img_buffer = s->img_buffer_end;
        return;
    }
    if (s->io.read) {
        int blen = (int)(s->img_buffer_end - s->img_buffer);
        if (blen < n) {
            s->img_buffer = s->img_buffer_end;
            (s->io.skip)(s->io_user_data, n - blen);
            return;
        }
    }
    s->img_buffer += n;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_TGA) && defined(STBI_NO_HDR) && defined(STBI_NO_PNM)
// nothing
#else
static int stbi__getn(stbi__context* s, stbi_uc* buffer, int n)
{
    if (s->io.read) {
        int blen = (int)(s->img_buffer_end - s->img_buffer);
        if (blen < n) {
            int res, count;

            memcpy(buffer, s->img_buffer, blen);

            count = (s->io.read)(s->io_user_data, (char*)buffer + blen, n - blen);
            res = (count == (n - blen));
            s->img_buffer = s->img_buffer_end;
            return res;
        }
    }

    if (s->img_buffer + n <= s->img_buffer_end) {
        memcpy(buffer, s->img_buffer, n);
        s->img_buffer += n;
        return 1;
    }
    else
        return 0;
}
#endif

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static int stbi__get16be(stbi__context* s)
{
    int z = stbi__get8(s);
    return (z << 8) + stbi__get8(s);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD) && defined(STBI_NO_PIC)
// nothing
#else
static stbi__uint32 stbi__get32be(stbi__context* s)
{
    stbi__uint32 z = stbi__get16be(s);
    return (z << 16) + stbi__get16be(s);
}
#endif

#if defined(STBI_NO_BMP) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF)
// nothing
#else
static int stbi__get16le(stbi__context* s)
{
    int z = stbi__get8(s);
    return z + (stbi__get8(s) << 8);
}
#endif

#ifndef STBI_NO_BMP
static stbi__uint32 stbi__get32le(stbi__context* s)
{
    stbi__uint32 z = stbi__get16le(s);
    z += (stbi__uint32)stbi__get16le(s) << 16;
    return z;
}
#endif

#define STBI__BYTECAST(x)  ((stbi_uc) ((x) & 255))  // truncate int to byte without warnings

#if defined(STBI_NO_JPEG) && defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases ). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static stbi_uc stbi__compute_y(int r, int g, int b)
{
    return (stbi_uc)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_BMP) && defined(STBI_NO_PSD) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF) && defined(STBI_NO_PIC) && defined(STBI_NO_PNM)
// nothing
#else
static unsigned char* stbi__convert_format(unsigned char* data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
    int i, j;
    unsigned char* good;

    if (req_comp == img_n) return data;
    STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

    good = (unsigned char*)stbi__malloc_mad3(req_comp, x, y, 0);
    if (good == NULL) {
        STBI_FREE(data);
        return stbi__errpuc("outofmem", "Out of memory");
    }

    for (j = 0; j < (int)y; ++j) {
        unsigned char* src = data + j * x * img_n;
        unsigned char* dest = good + j * x * req_comp;

#define STBI__COMBO(a,b)  ((a)*8+(b))
#define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
        // convert source image with img_n components to one with req_comp components;
        // avoid switch per pixel, so use switch per scanline and massive macros
        switch (STBI__COMBO(img_n, req_comp)) {
            STBI__CASE(1, 2) { dest[0] = src[0]; dest[1] = 255; } break;
            STBI__CASE(1, 3) { dest[0] = dest[1] = dest[2] = src[0]; } break;
            STBI__CASE(1, 4) { dest[0] = dest[1] = dest[2] = src[0]; dest[3] = 255; } break;
            STBI__CASE(2, 1) { dest[0] = src[0]; } break;
            STBI__CASE(2, 3) { dest[0] = dest[1] = dest[2] = src[0]; } break;
            STBI__CASE(2, 4) { dest[0] = dest[1] = dest[2] = src[0]; dest[3] = src[1]; } break;
            STBI__CASE(3, 4) { dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; dest[3] = 255; } break;
            STBI__CASE(3, 1) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); } break;
            STBI__CASE(3, 2) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); dest[1] = 255; } break;
            STBI__CASE(4, 1) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); } break;
            STBI__CASE(4, 2) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); dest[1] = src[3]; } break;
            STBI__CASE(4, 3) { dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; } break;
        default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return stbi__errpuc("unsupported", "Unsupported format conversion");
        }
#undef STBI__CASE
    }

    STBI_FREE(data);
    return good;
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16 stbi__compute_y_16(int r, int g, int b)
{
    return (stbi__uint16)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}
#endif

#if defined(STBI_NO_PNG) && defined(STBI_NO_PSD)
// nothing
#else
static stbi__uint16* stbi__convert_format16(stbi__uint16* data, int img_n, int req_comp, unsigned int x, unsigned int y)
{
    int i, j;
    stbi__uint16* good;

    if (req_comp == img_n) return data;
    STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

    good = (stbi__uint16*)stbi__malloc(req_comp * x * y * 2);
    if (good == NULL) {
        STBI_FREE(data);
        return (stbi__uint16*)stbi__errpuc("outofmem", "Out of memory");
    }

    for (j = 0; j < (int)y; ++j) {
        stbi__uint16* src = data + j * x * img_n;
        stbi__uint16* dest = good + j * x * req_comp;

#define STBI__COMBO(a,b)  ((a)*8+(b))
#define STBI__CASE(a,b)   case STBI__COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
        // convert source image with img_n components to one with req_comp components;
        // avoid switch per pixel, so use switch per scanline and massive macros
        switch (STBI__COMBO(img_n, req_comp)) {
            STBI__CASE(1, 2) { dest[0] = src[0]; dest[1] = 0xffff; } break;
            STBI__CASE(1, 3) { dest[0] = dest[1] = dest[2] = src[0]; } break;
            STBI__CASE(1, 4) { dest[0] = dest[1] = dest[2] = src[0]; dest[3] = 0xffff; } break;
            STBI__CASE(2, 1) { dest[0] = src[0]; } break;
            STBI__CASE(2, 3) { dest[0] = dest[1] = dest[2] = src[0]; } break;
            STBI__CASE(2, 4) { dest[0] = dest[1] = dest[2] = src[0]; dest[3] = src[1]; } break;
            STBI__CASE(3, 4) { dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; dest[3] = 0xffff; } break;
            STBI__CASE(3, 1) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); } break;
            STBI__CASE(3, 2) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); dest[1] = 0xffff; } break;
            STBI__CASE(4, 1) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); } break;
            STBI__CASE(4, 2) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); dest[1] = src[3]; } break;
            STBI__CASE(4, 3) { dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; } break;
        default: STBI_ASSERT(0); STBI_FREE(data); STBI_FREE(good); return (stbi__uint16*)stbi__errpuc("unsupported", "Unsupported format conversion");
        }
#undef STBI__CASE
    }

    STBI_FREE(data);
    return good;
}
#endif

#ifndef STBI_NO_LINEAR
static float* stbi__ldr_to_hdr(stbi_uc* data, int x, int y, int comp)
{
    int i, k, n;
    float* output;
    if (!data) return NULL;
    output = (float*)stbi__malloc_mad4(x, y, comp, sizeof(float), 0);
    if (output == NULL) { STBI_FREE(data); return stbi__errpf("outofmem", "Out of memory"); }
    // compute number of non-alpha components
    if (comp & 1) n = comp; else n = comp - 1;
    for (i = 0; i < x * y; ++i) {
        for (k = 0; k < n; ++k) {
            output[i * comp + k] = (float)(pow(data[i * comp + k] / 255.0f, stbi__l2h_gamma) * stbi__l2h_scale);
        }
    }
    if (n < comp) {
        for (i = 0; i < x * y; ++i) {
            output[i * comp + n] = data[i * comp + n] / 255.0f;
        }
    }
    STBI_FREE(data);
    return output;
}
#endif

// public domain zlib decode    v0.2  Sean Barrett 2006-11-18
//    simple implementation
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    performance
//      - fast huffman

#ifndef STBI_NO_ZLIB

// fast-way is faster to check than jpeg huffman, but slow way is slower
#define STBI__ZFAST_BITS  9 // accelerate all cases in default tables
#define STBI__ZFAST_MASK  ((1 << STBI__ZFAST_BITS) - 1)
#define STBI__ZNSYMS 288 // number of symbols in literal/length alphabet

// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
typedef struct
{
    stbi__uint16 fast[1 << STBI__ZFAST_BITS];
    stbi__uint16 firstcode[16];
    int maxcode[17];
    stbi__uint16 firstsymbol[16];
    stbi_uc  size[STBI__ZNSYMS];
    stbi__uint16 value[STBI__ZNSYMS];
} stbi__zhuffman;

stbi_inline static int stbi__bitreverse16(int n)
{
    n = ((n & 0xAAAA) >> 1) | ((n & 0x5555) << 1);
    n = ((n & 0xCCCC) >> 2) | ((n & 0x3333) << 2);
    n = ((n & 0xF0F0) >> 4) | ((n & 0x0F0F) << 4);
    n = ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
    return n;
}

stbi_inline static int stbi__bit_reverse(int v, int bits)
{
    STBI_ASSERT(bits <= 16);
    // to bit reverse n bits, reverse 16 and shift
    // e.g. 11 bits, bit reverse and shift away 5
    return stbi__bitreverse16(v) >> (16 - bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman* z, const stbi_uc* sizelist, int num)
{
    int i, k = 0;
    int code, next_code[16], sizes[17];

    // DEFLATE spec for generating codes
    memset(sizes, 0, sizeof(sizes));
    memset(z->fast, 0, sizeof(z->fast));
    for (i = 0; i < num; ++i)
        ++sizes[sizelist[i]];
    sizes[0] = 0;
    for (i = 1; i < 16; ++i)
        if (sizes[i] > (1 << i))
            return stbi__err("bad sizes", "Corrupt PNG");
    code = 0;
    for (i = 1; i < 16; ++i) {
        next_code[i] = code;
        z->firstcode[i] = (stbi__uint16)code;
        z->firstsymbol[i] = (stbi__uint16)k;
        code = (code + sizes[i]);
        if (sizes[i])
            if (code - 1 >= (1 << i)) return stbi__err("bad codelengths", "Corrupt PNG");
        z->maxcode[i] = code << (16 - i); // preshift for inner loop
        code <<= 1;
        k += sizes[i];
    }
    z->maxcode[16] = 0x10000; // sentinel
    for (i = 0; i < num; ++i) {
        int s = sizelist[i];
        if (s) {
            int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
            stbi__uint16 fastv = (stbi__uint16)((s << 9) | i);
            z->size[c] = (stbi_uc)s;
            z->value[c] = (stbi__uint16)i;
            if (s <= STBI__ZFAST_BITS) {
                int j = stbi__bit_reverse(next_code[s], s);
                while (j < (1 << STBI__ZFAST_BITS)) {
                    z->fast[j] = fastv;
                    j += (1 << s);
                }
            }
            ++next_code[s];
        }
    }
    return 1;
}

// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer

typedef struct
{
    stbi_uc* zbuffer, * zbuffer_end;
    int num_bits;
    stbi__uint32 code_buffer;

    char* zout;
    char* zout_start;
    char* zout_end;
    int   z_expandable;

    stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

stbi_inline static int stbi__zeof(stbi__zbuf* z)
{
    return (z->zbuffer >= z->zbuffer_end);
}

stbi_inline static stbi_uc stbi__zget8(stbi__zbuf* z)
{
    return stbi__zeof(z) ? 0 : *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf* z)
{
    do {
        if (z->code_buffer >= (1U << z->num_bits)) {
            z->zbuffer = z->zbuffer_end;  /* treat this as EOF so we fail. */
            return;
        }
        z->code_buffer |= (unsigned int)stbi__zget8(z) << z->num_bits;
        z->num_bits += 8;
    } while (z->num_bits <= 24);
}

stbi_inline static unsigned int stbi__zreceive(stbi__zbuf* z, int n)
{
    unsigned int k;
    if (z->num_bits < n) stbi__fill_bits(z);
    k = z->code_buffer & ((1 << n) - 1);
    z->code_buffer >>= n;
    z->num_bits -= n;
    return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf* a, stbi__zhuffman* z)
{
    int b, s, k;
    // not resolved by fast table, so compute it the slow way
    // use jpeg approach, which requires MSbits at top
    k = stbi__bit_reverse(a->code_buffer, 16);
    for (s = STBI__ZFAST_BITS + 1; ; ++s)
        if (k < z->maxcode[s])
            break;
    if (s >= 16) return -1; // invalid code!
    // code size is s, so:
    b = (k >> (16 - s)) - z->firstcode[s] + z->firstsymbol[s];
    if (b >= STBI__ZNSYMS) return -1; // some data was corrupt somewhere!
    if (z->size[b] != s) return -1;  // was originally an assert, but report failure instead.
    a->code_buffer >>= s;
    a->num_bits -= s;
    return z->value[b];
}

stbi_inline static int stbi__zhuffman_decode(stbi__zbuf* a, stbi__zhuffman* z)
{
    int b, s;
    if (a->num_bits < 16) {
        if (stbi__zeof(a)) {
            return -1;   /* report error for unexpected end of data. */
        }
        stbi__fill_bits(a);
    }
    b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
    if (b) {
        s = b >> 9;
        a->code_buffer >>= s;
        a->num_bits -= s;
        return b & 511;
    }
    return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf* z, char* zout, int n)  // need to make room for n bytes
{
    char* q;
    unsigned int cur, limit, old_limit;
    z->zout = zout;
    if (!z->z_expandable) return stbi__err("output buffer limit", "Corrupt PNG");
    cur = (unsigned int)(z->zout - z->zout_start);
    limit = old_limit = (unsigned)(z->zout_end - z->zout_start);
    if (UINT_MAX - cur < (unsigned)n) return stbi__err("outofmem", "Out of memory");
    while (cur + n > limit) {
        if (limit > UINT_MAX / 2) return stbi__err("outofmem", "Out of memory");
        limit *= 2;
    }
    q = (char*)STBI_REALLOC_SIZED(z->zout_start, old_limit, limit);
    STBI_NOTUSED(old_limit);
    if (q == NULL) return stbi__err("outofmem", "Out of memory");
    z->zout_start = q;
    z->zout = q + cur;
    z->zout_end = q + limit;
    return 1;
}

static const int stbi__zlength_base[31] = {
   3,4,5,6,7,8,9,10,11,13,
   15,17,19,23,27,31,35,43,51,59,
   67,83,99,115,131,163,195,227,258,0,0 };

static const int stbi__zlength_extra[31] =
{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

static const int stbi__zdist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0 };

static const int stbi__zdist_extra[32] =
{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };

static int stbi__parse_huffman_block(stbi__zbuf* a)
{
    char* zout = a->zout;
    for (;;) {
        int z = stbi__zhuffman_decode(a, &a->z_length);
        if (z < 256) {
            if (z < 0) return stbi__err("bad huffman code", "Corrupt PNG"); // error in huffman codes
            if (zout >= a->zout_end) {
                if (!stbi__zexpand(a, zout, 1)) return 0;
                zout = a->zout;
            }
            *zout++ = (char)z;
        }
        else {
            stbi_uc* p;
            int len, dist;
            if (z == 256) {
                a->zout = zout;
                return 1;
            }
            if (z >= 286) return stbi__err("bad huffman code", "Corrupt PNG"); // per DEFLATE, length codes 286 and 287 must not appear in compressed data
            z -= 257;
            len = stbi__zlength_base[z];
            if (stbi__zlength_extra[z]) len += stbi__zreceive(a, stbi__zlength_extra[z]);
            z = stbi__zhuffman_decode(a, &a->z_distance);
            if (z < 0 || z >= 30) return stbi__err("bad huffman code", "Corrupt PNG"); // per DEFLATE, distance codes 30 and 31 must not appear in compressed data
            dist = stbi__zdist_base[z];
            if (stbi__zdist_extra[z]) dist += stbi__zreceive(a, stbi__zdist_extra[z]);
            if (zout - a->zout_start < dist) return stbi__err("bad dist", "Corrupt PNG");
            if (zout + len > a->zout_end) {
                if (!stbi__zexpand(a, zout, len)) return 0;
                zout = a->zout;
            }
            p = (stbi_uc*)(zout - dist);
            if (dist == 1) { // run of one byte; common in images.
                stbi_uc v = *p;
                if (len) { do *zout++ = v; while (--len); }
            }
            else {
                if (len) { do *zout++ = *p++; while (--len); }
            }
        }
    }
}

static int stbi__compute_huffman_codes(stbi__zbuf* a)
{
    static const stbi_uc length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
    stbi__zhuffman z_codelength;
    stbi_uc lencodes[286 + 32 + 137];//padding for maximum single op
    stbi_uc codelength_sizes[19];
    int i, n;

    int hlit = stbi__zreceive(a, 5) + 257;
    int hdist = stbi__zreceive(a, 5) + 1;
    int hclen = stbi__zreceive(a, 4) + 4;
    int ntot = hlit + hdist;

    memset(codelength_sizes, 0, sizeof(codelength_sizes));
    for (i = 0; i < hclen; ++i) {
        int s = stbi__zreceive(a, 3);
        codelength_sizes[length_dezigzag[i]] = (stbi_uc)s;
    }
    if (!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;

    n = 0;
    while (n < ntot) {
        int c = stbi__zhuffman_decode(a, &z_codelength);
        if (c < 0 || c >= 19) return stbi__err("bad codelengths", "Corrupt PNG");
        if (c < 16)
            lencodes[n++] = (stbi_uc)c;
        else {
            stbi_uc fill = 0;
            if (c == 16) {
                c = stbi__zreceive(a, 2) + 3;
                if (n == 0) return stbi__err("bad codelengths", "Corrupt PNG");
                fill = lencodes[n - 1];
            }
            else if (c == 17) {
                c = stbi__zreceive(a, 3) + 3;
            }
            else if (c == 18) {
                c = stbi__zreceive(a, 7) + 11;
            }
            else {
                return stbi__err("bad codelengths", "Corrupt PNG");
            }
            if (ntot - n < c) return stbi__err("bad codelengths", "Corrupt PNG");
            memset(lencodes + n, fill, c);
            n += c;
        }
    }
    if (n != ntot) return stbi__err("bad codelengths", "Corrupt PNG");
    if (!stbi__zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
    if (!stbi__zbuild_huffman(&a->z_distance, lencodes + hlit, hdist)) return 0;
    return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf* a)
{
    stbi_uc header[4];
    int len, nlen, k;
    if (a->num_bits & 7)
        stbi__zreceive(a, a->num_bits & 7); // discard
    // drain the bit-packed data into header
    k = 0;
    while (a->num_bits > 0) {
        header[k++] = (stbi_uc)(a->code_buffer & 255); // suppress MSVC run-time check
        a->code_buffer >>= 8;
        a->num_bits -= 8;
    }
    if (a->num_bits < 0) return stbi__err("zlib corrupt", "Corrupt PNG");
    // now fill header the normal way
    while (k < 4)
        header[k++] = stbi__zget8(a);
    len = header[1] * 256 + header[0];
    nlen = header[3] * 256 + header[2];
    if (nlen != (len ^ 0xffff)) return stbi__err("zlib corrupt", "Corrupt PNG");
    if (a->zbuffer + len > a->zbuffer_end) return stbi__err("read past buffer", "Corrupt PNG");
    if (a->zout + len > a->zout_end)
        if (!stbi__zexpand(a, a->zout, len)) return 0;
    memcpy(a->zout, a->zbuffer, len);
    a->zbuffer += len;
    a->zout += len;
    return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf* a)
{
    int cmf = stbi__zget8(a);
    int cm = cmf & 15;
    /* int cinfo = cmf >> 4; */
    int flg = stbi__zget8(a);
    if (stbi__zeof(a)) return stbi__err("bad zlib header", "Corrupt PNG"); // zlib spec
    if ((cmf * 256 + flg) % 31 != 0) return stbi__err("bad zlib header", "Corrupt PNG"); // zlib spec
    if (flg & 32) return stbi__err("no preset dict", "Corrupt PNG"); // preset dictionary not allowed in png
    if (cm != 8) return stbi__err("bad compression", "Corrupt PNG"); // DEFLATE required for png
    // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
    return 1;
}

static const stbi_uc stbi__zdefault_length[STBI__ZNSYMS] =
{
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
   8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8
};
static const stbi_uc stbi__zdefault_distance[32] =
{
   5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5
};
/*
Init algorithm:
{
   int i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
   for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
   for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
   for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;

   for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
}
*/

static int stbi__parse_zlib(stbi__zbuf* a, int parse_header)
{
    int final, type;
    if (parse_header)
        if (!stbi__parse_zlib_header(a)) return 0;
    a->num_bits = 0;
    a->code_buffer = 0;
    do {
        final = stbi__zreceive(a, 1);
        type = stbi__zreceive(a, 2);
        if (type == 0) {
            if (!stbi__parse_uncompressed_block(a)) return 0;
        }
        else if (type == 3) {
            return 0;
        }
        else {
            if (type == 1) {
                // use fixed code lengths
                if (!stbi__zbuild_huffman(&a->z_length, stbi__zdefault_length, STBI__ZNSYMS)) return 0;
                if (!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance, 32)) return 0;
            }
            else {
                if (!stbi__compute_huffman_codes(a)) return 0;
            }
            if (!stbi__parse_huffman_block(a)) return 0;
        }
    } while (!final);
    return 1;
}

static int stbi__do_zlib(stbi__zbuf* a, char* obuf, int olen, int exp, int parse_header)
{
    a->zout_start = obuf;
    a->zout = obuf;
    a->zout_end = obuf + olen;
    a->z_expandable = exp;

    return stbi__parse_zlib(a, parse_header);
}

STBIDEF char* stbi_zlib_decode_malloc_guesssize(const char* buffer, int len, int initial_size, int* outlen)
{
    stbi__zbuf a;
    char* p = (char*)stbi__malloc(initial_size);
    if (p == NULL) return NULL;
    a.zbuffer = (stbi_uc*)buffer;
    a.zbuffer_end = (stbi_uc*)buffer + len;
    if (stbi__do_zlib(&a, p, initial_size, 1, 1)) {
        if (outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF char* stbi_zlib_decode_malloc(char const* buffer, int len, int* outlen)
{
    return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

STBIDEF char* stbi_zlib_decode_malloc_guesssize_headerflag(const char* buffer, int len, int initial_size, int* outlen, int parse_header)
{
    stbi__zbuf a;
    char* p = (char*)stbi__malloc(initial_size);
    if (p == NULL) return NULL;
    a.zbuffer = (stbi_uc*)buffer;
    a.zbuffer_end = (stbi_uc*)buffer + len;
    if (stbi__do_zlib(&a, p, initial_size, 1, parse_header)) {
        if (outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF int stbi_zlib_decode_buffer(char* obuffer, int olen, char const* ibuffer, int ilen)
{
    stbi__zbuf a;
    a.zbuffer = (stbi_uc*)ibuffer;
    a.zbuffer_end = (stbi_uc*)ibuffer + ilen;
    if (stbi__do_zlib(&a, obuffer, olen, 0, 1))
        return (int)(a.zout - a.zout_start);
    else
        return -1;
}

STBIDEF char* stbi_zlib_decode_noheader_malloc(char const* buffer, int len, int* outlen)
{
    stbi__zbuf a;
    char* p = (char*)stbi__malloc(16384);
    if (p == NULL) return NULL;
    a.zbuffer = (stbi_uc*)buffer;
    a.zbuffer_end = (stbi_uc*)buffer + len;
    if (stbi__do_zlib(&a, p, 16384, 1, 0)) {
        if (outlen) *outlen = (int)(a.zout - a.zout_start);
        return a.zout_start;
    }
    else {
        STBI_FREE(a.zout_start);
        return NULL;
    }
}

STBIDEF int stbi_zlib_decode_noheader_buffer(char* obuffer, int olen, const char* ibuffer, int ilen)
{
    stbi__zbuf a;
    a.zbuffer = (stbi_uc*)ibuffer;
    a.zbuffer_end = (stbi_uc*)ibuffer + ilen;
    if (stbi__do_zlib(&a, obuffer, olen, 0, 0))
        return (int)(a.zout - a.zout_start);
    else
        return -1;
}
#endif

// public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006-11-18
//    simple implementation
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    performance
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

#ifndef STBI_NO_PNG
typedef struct
{
    stbi__uint32 length;
    stbi__uint32 type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context* s)
{
    stbi__pngchunk c;
    c.length = stbi__get32be(s);
    c.type = stbi__get32be(s);
    return c;
}

static int stbi__check_png_header(stbi__context* s)
{
    static const stbi_uc png_sig[8] = { 137,80,78,71,13,10,26,10 };
    int i;
    for (i = 0; i < 8; ++i)
        if (stbi__get8(s) != png_sig[i]) return stbi__err("bad png sig", "Not a PNG");
    return 1;
}

typedef struct
{
    stbi__context* s;
    stbi_uc* idata, * expanded, * out;
    int depth;
} stbi__png;


enum {
    STBI__F_none = 0,
    STBI__F_sub = 1,
    STBI__F_up = 2,
    STBI__F_avg = 3,
    STBI__F_paeth = 4,
    // synthetic filters used for first scanline to avoid needing a dummy row of 0s
    STBI__F_avg_first,
    STBI__F_paeth_first
};

static stbi_uc first_row_filter[5] =
{
   STBI__F_none,
   STBI__F_sub,
   STBI__F_none,
   STBI__F_avg_first,
   STBI__F_paeth_first
};

static int stbi__paeth(int a, int b, int c)
{
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

static const stbi_uc stbi__depth_scale_table[9] = { 0, 0xff, 0x55, 0, 0x11, 0,0,0, 0x01 };

// create the png data from post-deflated data
static int stbi__create_png_image_raw(stbi__png* a, stbi_uc* raw, stbi__uint32 raw_len, int out_n, stbi__uint32 x, stbi__uint32 y, int depth, int color)
{
    int bytes = (depth == 16 ? 2 : 1);
    stbi__context* s = a->s;
    stbi__uint32 i, j, stride = x * out_n * bytes;
    stbi__uint32 img_len, img_width_bytes;
    int k;
    int img_n = s->img_n; // copy it into a local for later

    int output_bytes = out_n * bytes;
    int filter_bytes = img_n * bytes;
    int width = x;

    STBI_ASSERT(out_n == s->img_n || out_n == s->img_n + 1);
    a->out = (stbi_uc*)stbi__malloc_mad3(x, y, output_bytes, 0); // extra bytes to write off the end into
    if (!a->out) return stbi__err("outofmem", "Out of memory");

    if (!stbi__mad3sizes_valid(img_n, x, depth, 7)) return stbi__err("too large", "Corrupt PNG");
    img_width_bytes = (((img_n * x * depth) + 7) >> 3);
    img_len = (img_width_bytes + 1) * y;

    // we used to check for exact match between raw_len and img_len on non-interlaced PNGs,
    // but issue #276 reported a PNG in the wild that had extra data at the end (all zeros),
    // so just check for raw_len < img_len always.
    if (raw_len < img_len) return stbi__err("not enough pixels", "Corrupt PNG");

    for (j = 0; j < y; ++j) {
        stbi_uc* cur = a->out + stride * j;
        stbi_uc* prior;
        int filter = *raw++;

        if (filter > 4)
            return stbi__err("invalid filter", "Corrupt PNG");

        if (depth < 8) {
            if (img_width_bytes > x) return stbi__err("invalid width", "Corrupt PNG");
            cur += x * out_n - img_width_bytes; // store output to the rightmost img_len bytes, so we can decode in place
            filter_bytes = 1;
            width = img_width_bytes;
        }
        prior = cur - stride; // bugfix: need to compute this after 'cur +=' computation above

        // if first row, use special filter that doesn't sample previous row
        if (j == 0) filter = first_row_filter[filter];

        // handle first byte explicitly
        for (k = 0; k < filter_bytes; ++k) {
            switch (filter) {
            case STBI__F_none: cur[k] = raw[k]; break;
            case STBI__F_sub: cur[k] = raw[k]; break;
            case STBI__F_up: cur[k] = STBI__BYTECAST(raw[k] + prior[k]); break;
            case STBI__F_avg: cur[k] = STBI__BYTECAST(raw[k] + (prior[k] >> 1)); break;
            case STBI__F_paeth: cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(0, prior[k], 0)); break;
            case STBI__F_avg_first: cur[k] = raw[k]; break;
            case STBI__F_paeth_first: cur[k] = raw[k]; break;
            }
        }

        if (depth == 8) {
            if (img_n != out_n)
                cur[img_n] = 255; // first pixel
            raw += img_n;
            cur += out_n;
            prior += out_n;
        }
        else if (depth == 16) {
            if (img_n != out_n) {
                cur[filter_bytes] = 255; // first pixel top byte
                cur[filter_bytes + 1] = 255; // first pixel bottom byte
            }
            raw += filter_bytes;
            cur += output_bytes;
            prior += output_bytes;
        }
        else {
            raw += 1;
            cur += 1;
            prior += 1;
        }

        // this is a little gross, so that we don't switch per-pixel or per-component
        if (depth < 8 || img_n == out_n) {
            int nk = (width - 1) * filter_bytes;
#define STBI__CASE(f) \
             case f:     \
                for (k=0; k < nk; ++k)
            switch (filter) {
                // "none" filter turns into a memcpy here; make that explicit.
            case STBI__F_none:         memcpy(cur, raw, nk); break;
                STBI__CASE(STBI__F_sub) { cur[k] = STBI__BYTECAST(raw[k] + cur[k - filter_bytes]); } break;
                STBI__CASE(STBI__F_up) { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); } break;
                STBI__CASE(STBI__F_avg) { cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k - filter_bytes]) >> 1)); } break;
                STBI__CASE(STBI__F_paeth) { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - filter_bytes], prior[k], prior[k - filter_bytes])); } break;
                STBI__CASE(STBI__F_avg_first) { cur[k] = STBI__BYTECAST(raw[k] + (cur[k - filter_bytes] >> 1)); } break;
                STBI__CASE(STBI__F_paeth_first) { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - filter_bytes], 0, 0)); } break;
            }
#undef STBI__CASE
            raw += nk;
        }
        else {
            STBI_ASSERT(img_n + 1 == out_n);
#define STBI__CASE(f) \
             case f:     \
                for (i=x-1; i >= 1; --i, cur[filter_bytes]=255,raw+=filter_bytes,cur+=output_bytes,prior+=output_bytes) \
                   for (k=0; k < filter_bytes; ++k)
            switch (filter) {
                STBI__CASE(STBI__F_none) { cur[k] = raw[k]; } break;
                STBI__CASE(STBI__F_sub) { cur[k] = STBI__BYTECAST(raw[k] + cur[k - output_bytes]); } break;
                STBI__CASE(STBI__F_up) { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); } break;
                STBI__CASE(STBI__F_avg) { cur[k] = STBI__BYTECAST(raw[k] + ((prior[k] + cur[k - output_bytes]) >> 1)); } break;
                STBI__CASE(STBI__F_paeth) { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - output_bytes], prior[k], prior[k - output_bytes])); } break;
                STBI__CASE(STBI__F_avg_first) { cur[k] = STBI__BYTECAST(raw[k] + (cur[k - output_bytes] >> 1)); } break;
                STBI__CASE(STBI__F_paeth_first) { cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - output_bytes], 0, 0)); } break;
            }
#undef STBI__CASE

            // the loop above sets the high byte of the pixels' alpha, but for
            // 16 bit png files we also need the low byte set. we'll do that here.
            if (depth == 16) {
                cur = a->out + stride * j; // start at the beginning of the row again
                for (i = 0; i < x; ++i, cur += output_bytes) {
                    cur[filter_bytes + 1] = 255;
                }
            }
        }
    }

    // we make a separate pass to expand bits to pixels; for performance,
    // this could run two scanlines behind the above code, so it won't
    // intefere with filtering but will still be in the cache.
    if (depth < 8) {
        for (j = 0; j < y; ++j) {
            stbi_uc* cur = a->out + stride * j;
            stbi_uc* in = a->out + stride * j + x * out_n - img_width_bytes;
            // unpack 1/2/4-bit into a 8-bit buffer. allows us to keep the common 8-bit path optimal at minimal cost for 1/2/4-bit
            // png guarante byte alignment, if width is not multiple of 8/4/2 we'll decode dummy trailing data that will be skipped in the later loop
            stbi_uc scale = (color == 0) ? stbi__depth_scale_table[depth] : 1; // scale grayscale values to 0..255 range

            // note that the final byte might overshoot and write more data than desired.
            // we can allocate enough data that this never writes out of memory, but it
            // could also overwrite the next scanline. can it overwrite non-empty data
            // on the next scanline? yes, consider 1-pixel-wide scanlines with 1-bit-per-pixel.
            // so we need to explicitly clamp the final ones

            if (depth == 4) {
                for (k = x * img_n; k >= 2; k -= 2, ++in) {
                    *cur++ = scale * ((*in >> 4));
                    *cur++ = scale * ((*in) & 0x0f);
                }
                if (k > 0) *cur++ = scale * ((*in >> 4));
            }
            else if (depth == 2) {
                for (k = x * img_n; k >= 4; k -= 4, ++in) {
                    *cur++ = scale * ((*in >> 6));
                    *cur++ = scale * ((*in >> 4) & 0x03);
                    *cur++ = scale * ((*in >> 2) & 0x03);
                    *cur++ = scale * ((*in) & 0x03);
                }
                if (k > 0) *cur++ = scale * ((*in >> 6));
                if (k > 1) *cur++ = scale * ((*in >> 4) & 0x03);
                if (k > 2) *cur++ = scale * ((*in >> 2) & 0x03);
            }
            else if (depth == 1) {
                for (k = x * img_n; k >= 8; k -= 8, ++in) {
                    *cur++ = scale * ((*in >> 7));
                    *cur++ = scale * ((*in >> 6) & 0x01);
                    *cur++ = scale * ((*in >> 5) & 0x01);
                    *cur++ = scale * ((*in >> 4) & 0x01);
                    *cur++ = scale * ((*in >> 3) & 0x01);
                    *cur++ = scale * ((*in >> 2) & 0x01);
                    *cur++ = scale * ((*in >> 1) & 0x01);
                    *cur++ = scale * ((*in) & 0x01);
                }
                if (k > 0) *cur++ = scale * ((*in >> 7));
                if (k > 1) *cur++ = scale * ((*in >> 6) & 0x01);
                if (k > 2) *cur++ = scale * ((*in >> 5) & 0x01);
                if (k > 3) *cur++ = scale * ((*in >> 4) & 0x01);
                if (k > 4) *cur++ = scale * ((*in >> 3) & 0x01);
                if (k > 5) *cur++ = scale * ((*in >> 2) & 0x01);
                if (k > 6) *cur++ = scale * ((*in >> 1) & 0x01);
            }
            if (img_n != out_n) {
                int q;
                // insert alpha = 255
                cur = a->out + stride * j;
                if (img_n == 1) {
                    for (q = x - 1; q >= 0; --q) {
                        cur[q * 2 + 1] = 255;
                        cur[q * 2 + 0] = cur[q];
                    }
                }
                else {
                    STBI_ASSERT(img_n == 3);
                    for (q = x - 1; q >= 0; --q) {
                        cur[q * 4 + 3] = 255;
                        cur[q * 4 + 2] = cur[q * 3 + 2];
                        cur[q * 4 + 1] = cur[q * 3 + 1];
                        cur[q * 4 + 0] = cur[q * 3 + 0];
                    }
                }
            }
        }
    }
    else if (depth == 16) {
        // force the image data from big-endian to platform-native.
        // this is done in a separate pass due to the decoding relying
        // on the data being untouched, but could probably be done
        // per-line during decode if care is taken.
        stbi_uc* cur = a->out;
        stbi__uint16* cur16 = (stbi__uint16*)cur;

        for (i = 0; i < x * y * out_n; ++i, cur16++, cur += 2) {
            *cur16 = (cur[0] << 8) | cur[1];
        }
    }

    return 1;
}

static int stbi__create_png_image(stbi__png* a, stbi_uc* image_data, stbi__uint32 image_data_len, int out_n, int depth, int color, int interlaced)
{
    int bytes = (depth == 16 ? 2 : 1);
    int out_bytes = out_n * bytes;
    stbi_uc* final;
    int p;
    if (!interlaced)
        return stbi__create_png_image_raw(a, image_data, image_data_len, out_n, a->s->img_x, a->s->img_y, depth, color);

    // de-interlacing
    final = (stbi_uc*)stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
    if (!final) return stbi__err("outofmem", "Out of memory");
    for (p = 0; p < 7; ++p) {
        int xorig[] = { 0,4,0,2,0,1,0 };
        int yorig[] = { 0,0,4,0,2,0,1 };
        int xspc[] = { 8,8,4,4,2,2,1 };
        int yspc[] = { 8,8,8,4,4,2,2 };
        int i, j, x, y;
        // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
        x = (a->s->img_x - xorig[p] + xspc[p] - 1) / xspc[p];
        y = (a->s->img_y - yorig[p] + yspc[p] - 1) / yspc[p];
        if (x && y) {
            stbi__uint32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
            if (!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x, y, depth, color)) {
                STBI_FREE(final);
                return 0;
            }
            for (j = 0; j < y; ++j) {
                for (i = 0; i < x; ++i) {
                    int out_y = j * yspc[p] + yorig[p];
                    int out_x = i * xspc[p] + xorig[p];
                    memcpy(final + out_y * a->s->img_x * out_bytes + out_x * out_bytes,
                        a->out + (j * x + i) * out_bytes, out_bytes);
                }
            }
            STBI_FREE(a->out);
            image_data += img_len;
            image_data_len -= img_len;
        }
    }
    a->out = final;

    return 1;
}

static int stbi__compute_transparency(stbi__png* z, stbi_uc tc[3], int out_n)
{
    stbi__context* s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi_uc* p = z->out;

    // compute color-based transparency, assuming we've
    // already got 255 as the alpha value in the output
    STBI_ASSERT(out_n == 2 || out_n == 4);

    if (out_n == 2) {
        for (i = 0; i < pixel_count; ++i) {
            p[1] = (p[0] == tc[0] ? 0 : 255);
            p += 2;
        }
    }
    else {
        for (i = 0; i < pixel_count; ++i) {
            if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
                p[3] = 0;
            p += 4;
        }
    }
    return 1;
}

static int stbi__compute_transparency16(stbi__png* z, stbi__uint16 tc[3], int out_n)
{
    stbi__context* s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi__uint16* p = (stbi__uint16*)z->out;

    // compute color-based transparency, assuming we've
    // already got 65535 as the alpha value in the output
    STBI_ASSERT(out_n == 2 || out_n == 4);

    if (out_n == 2) {
        for (i = 0; i < pixel_count; ++i) {
            p[1] = (p[0] == tc[0] ? 0 : 65535);
            p += 2;
        }
    }
    else {
        for (i = 0; i < pixel_count; ++i) {
            if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
                p[3] = 0;
            p += 4;
        }
    }
    return 1;
}

static int stbi__expand_png_palette(stbi__png* a, stbi_uc* palette, int len, int pal_img_n)
{
    stbi__uint32 i, pixel_count = a->s->img_x * a->s->img_y;
    stbi_uc* p, * temp_out, * orig = a->out;

    p = (stbi_uc*)stbi__malloc_mad2(pixel_count, pal_img_n, 0);
    if (p == NULL) return stbi__err("outofmem", "Out of memory");

    // between here and free(out) below, exitting would leak
    temp_out = p;

    if (pal_img_n == 3) {
        for (i = 0; i < pixel_count; ++i) {
            int n = orig[i] * 4;
            p[0] = palette[n];
            p[1] = palette[n + 1];
            p[2] = palette[n + 2];
            p += 3;
        }
    }
    else {
        for (i = 0; i < pixel_count; ++i) {
            int n = orig[i] * 4;
            p[0] = palette[n];
            p[1] = palette[n + 1];
            p[2] = palette[n + 2];
            p[3] = palette[n + 3];
            p += 4;
        }
    }
    STBI_FREE(a->out);
    a->out = temp_out;

    STBI_NOTUSED(len);

    return 1;
}

static int stbi__unpremultiply_on_load_global = 0;
static int stbi__de_iphone_flag_global = 0;

STBIDEF void stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply)
{
    stbi__unpremultiply_on_load_global = flag_true_if_should_unpremultiply;
}

STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert)
{
    stbi__de_iphone_flag_global = flag_true_if_should_convert;
}

#ifndef STBI_THREAD_LOCAL
#define stbi__unpremultiply_on_load  stbi__unpremultiply_on_load_global
#define stbi__de_iphone_flag  stbi__de_iphone_flag_global
#else
static STBI_THREAD_LOCAL int stbi__unpremultiply_on_load_local, stbi__unpremultiply_on_load_set;
static STBI_THREAD_LOCAL int stbi__de_iphone_flag_local, stbi__de_iphone_flag_set;

STBIDEF void stbi_set_unpremultiply_on_load_thread(int flag_true_if_should_unpremultiply)
{
    stbi__unpremultiply_on_load_local = flag_true_if_should_unpremultiply;
    stbi__unpremultiply_on_load_set = 1;
}

STBIDEF void stbi_convert_iphone_png_to_rgb_thread(int flag_true_if_should_convert)
{
    stbi__de_iphone_flag_local = flag_true_if_should_convert;
    stbi__de_iphone_flag_set = 1;
}

#define stbi__unpremultiply_on_load  (stbi__unpremultiply_on_load_set           \
                                       ? stbi__unpremultiply_on_load_local      \
                                       : stbi__unpremultiply_on_load_global)
#define stbi__de_iphone_flag  (stbi__de_iphone_flag_set                         \
                                ? stbi__de_iphone_flag_local                    \
                                : stbi__de_iphone_flag_global)
#endif // STBI_THREAD_LOCAL

static void stbi__de_iphone(stbi__png* z)
{
    stbi__context* s = z->s;
    stbi__uint32 i, pixel_count = s->img_x * s->img_y;
    stbi_uc* p = z->out;

    if (s->img_out_n == 3) {  // convert bgr to rgb
        for (i = 0; i < pixel_count; ++i) {
            stbi_uc t = p[0];
            p[0] = p[2];
            p[2] = t;
            p += 3;
        }
    }
    else {
        STBI_ASSERT(s->img_out_n == 4);
        if (stbi__unpremultiply_on_load) {
            // convert bgr to rgb and unpremultiply
            for (i = 0; i < pixel_count; ++i) {
                stbi_uc a = p[3];
                stbi_uc t = p[0];
                if (a) {
                    stbi_uc half = a / 2;
                    p[0] = (p[2] * 255 + half) / a;
                    p[1] = (p[1] * 255 + half) / a;
                    p[2] = (t * 255 + half) / a;
                }
                else {
                    p[0] = p[2];
                    p[2] = t;
                }
                p += 4;
            }
        }
        else {
            // convert bgr to rgb
            for (i = 0; i < pixel_count; ++i) {
                stbi_uc t = p[0];
                p[0] = p[2];
                p[2] = t;
                p += 4;
            }
        }
    }
}

#define STBI__PNG_TYPE(a,b,c,d)  (((unsigned) (a) << 24) + ((unsigned) (b) << 16) + ((unsigned) (c) << 8) + (unsigned) (d))

static int stbi__parse_png_file(stbi__png* z, int scan, int req_comp)
{
    stbi_uc palette[1024], pal_img_n = 0;
    stbi_uc has_trans = 0, tc[3] = { 0 };
    stbi__uint16 tc16[3];
    stbi__uint32 ioff = 0, idata_limit = 0, i, pal_len = 0;
    int first = 1, k, interlace = 0, color = 0, is_iphone = 0;
    stbi__context* s = z->s;

    z->expanded = NULL;
    z->idata = NULL;
    z->out = NULL;

    if (!stbi__check_png_header(s)) return 0;

    if (scan == STBI__SCAN_type) return 1;

    for (;;) {
        stbi__pngchunk c = stbi__get_chunk_header(s);
        switch (c.type) {
        case STBI__PNG_TYPE('C', 'g', 'B', 'I'):
            is_iphone = 1;
            stbi__skip(s, c.length);
            break;
        case STBI__PNG_TYPE('I', 'H', 'D', 'R'): {
            int comp, filter;
            if (!first) return stbi__err("multiple IHDR", "Corrupt PNG");
            first = 0;
            if (c.length != 13) return stbi__err("bad IHDR len", "Corrupt PNG");
            s->img_x = stbi__get32be(s);
            s->img_y = stbi__get32be(s);
            if (s->img_y > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
            if (s->img_x > STBI_MAX_DIMENSIONS) return stbi__err("too large", "Very large image (corrupt?)");
            z->depth = stbi__get8(s);  if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 && z->depth != 16)  return stbi__err("1/2/4/8/16-bit only", "PNG not supported: 1/2/4/8/16-bit only");
            color = stbi__get8(s);  if (color > 6)         return stbi__err("bad ctype", "Corrupt PNG");
            if (color == 3 && z->depth == 16)                  return stbi__err("bad ctype", "Corrupt PNG");
            if (color == 3) pal_img_n = 3; else if (color & 1) return stbi__err("bad ctype", "Corrupt PNG");
            comp = stbi__get8(s);  if (comp) return stbi__err("bad comp method", "Corrupt PNG");
            filter = stbi__get8(s);  if (filter) return stbi__err("bad filter method", "Corrupt PNG");
            interlace = stbi__get8(s); if (interlace > 1) return stbi__err("bad interlace method", "Corrupt PNG");
            if (!s->img_x || !s->img_y) return stbi__err("0-pixel image", "Corrupt PNG");
            if (!pal_img_n) {
                s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
                if ((1 << 30) / s->img_x / s->img_n < s->img_y) return stbi__err("too large", "Image too large to decode");
            }
            else {
                // if paletted, then pal_n is our final components, and
                // img_n is # components to decompress/filter.
                s->img_n = 1;
                if ((1 << 30) / s->img_x / 4 < s->img_y) return stbi__err("too large", "Corrupt PNG");
            }
            // even with SCAN_header, have to scan to see if we have a tRNS
            break;
        }

        case STBI__PNG_TYPE('P', 'L', 'T', 'E'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (c.length > 256 * 3) return stbi__err("invalid PLTE", "Corrupt PNG");
            pal_len = c.length / 3;
            if (pal_len * 3 != c.length) return stbi__err("invalid PLTE", "Corrupt PNG");
            for (i = 0; i < pal_len; ++i) {
                palette[i * 4 + 0] = stbi__get8(s);
                palette[i * 4 + 1] = stbi__get8(s);
                palette[i * 4 + 2] = stbi__get8(s);
                palette[i * 4 + 3] = 255;
            }
            break;
        }

        case STBI__PNG_TYPE('t', 'R', 'N', 'S'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (z->idata) return stbi__err("tRNS after IDAT", "Corrupt PNG");
            if (pal_img_n) {
                if (scan == STBI__SCAN_header) { s->img_n = 4; return 1; }
                if (pal_len == 0) return stbi__err("tRNS before PLTE", "Corrupt PNG");
                if (c.length > pal_len) return stbi__err("bad tRNS len", "Corrupt PNG");
                pal_img_n = 4;
                for (i = 0; i < c.length; ++i)
                    palette[i * 4 + 3] = stbi__get8(s);
            }
            else {
                if (!(s->img_n & 1)) return stbi__err("tRNS with alpha", "Corrupt PNG");
                if (c.length != (stbi__uint32)s->img_n * 2) return stbi__err("bad tRNS len", "Corrupt PNG");
                has_trans = 1;
                // non-paletted with tRNS = constant alpha. if header-scanning, we can stop now.
                if (scan == STBI__SCAN_header) { ++s->img_n; return 1; }
                if (z->depth == 16) {
                    for (k = 0; k < s->img_n; ++k) tc16[k] = (stbi__uint16)stbi__get16be(s); // copy the values as-is
                }
                else {
                    for (k = 0; k < s->img_n; ++k) tc[k] = (stbi_uc)(stbi__get16be(s) & 255) * stbi__depth_scale_table[z->depth]; // non 8-bit images will be larger
                }
            }
            break;
        }

        case STBI__PNG_TYPE('I', 'D', 'A', 'T'): {
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (pal_img_n && !pal_len) return stbi__err("no PLTE", "Corrupt PNG");
            if (scan == STBI__SCAN_header) {
                // header scan definitely stops at first IDAT
                if (pal_img_n)
                    s->img_n = pal_img_n;
                return 1;
            }
            if (c.length > (1u << 30)) return stbi__err("IDAT size limit", "IDAT section larger than 2^30 bytes");
            if ((int)(ioff + c.length) < (int)ioff) return 0;
            if (ioff + c.length > idata_limit) {
                stbi__uint32 idata_limit_old = idata_limit;
                stbi_uc* p;
                if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
                while (ioff + c.length > idata_limit)
                    idata_limit *= 2;
                STBI_NOTUSED(idata_limit_old);
                p = (stbi_uc*)STBI_REALLOC_SIZED(z->idata, idata_limit_old, idata_limit); if (p == NULL) return stbi__err("outofmem", "Out of memory");
                z->idata = p;
            }
            if (!stbi__getn(s, z->idata + ioff, c.length)) return stbi__err("outofdata", "Corrupt PNG");
            ioff += c.length;
            break;
        }

        case STBI__PNG_TYPE('I', 'E', 'N', 'D'): {
            stbi__uint32 raw_len, bpl;
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if (scan != STBI__SCAN_load) return 1;
            if (z->idata == NULL) return stbi__err("no IDAT", "Corrupt PNG");
            // initial guess for decoded data size to avoid unnecessary reallocs
            bpl = (s->img_x * z->depth + 7) / 8; // bytes per line, per component
            raw_len = bpl * s->img_y * s->img_n /* pixels */ + s->img_y /* filter mode per row */;
            z->expanded = (stbi_uc*)stbi_zlib_decode_malloc_guesssize_headerflag((char*)z->idata, ioff, raw_len, (int*)&raw_len, !is_iphone);
            if (z->expanded == NULL) return 0; // zlib should set error
            STBI_FREE(z->idata); z->idata = NULL;
            if ((req_comp == s->img_n + 1 && req_comp != 3 && !pal_img_n) || has_trans)
                s->img_out_n = s->img_n + 1;
            else
                s->img_out_n = s->img_n;
            if (!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n, z->depth, color, interlace)) return 0;
            if (has_trans) {
                if (z->depth == 16) {
                    if (!stbi__compute_transparency16(z, tc16, s->img_out_n)) return 0;
                }
                else {
                    if (!stbi__compute_transparency(z, tc, s->img_out_n)) return 0;
                }
            }
            if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
                stbi__de_iphone(z);
            if (pal_img_n) {
                // pal_img_n == 3 or 4
                s->img_n = pal_img_n; // record the actual colors we had
                s->img_out_n = pal_img_n;
                if (req_comp >= 3) s->img_out_n = req_comp;
                if (!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
                    return 0;
            }
            else if (has_trans) {
                // non-paletted image with tRNS -> source image has (constant) alpha
                ++s->img_n;
            }
            STBI_FREE(z->expanded); z->expanded = NULL;
            // end of PNG chunk, read and skip CRC
            stbi__get32be(s);
            return 1;
        }

        default:
            // if critical, fail
            if (first) return stbi__err("first not IHDR", "Corrupt PNG");
            if ((c.type & (1 << 29)) == 0) {
#ifndef STBI_NO_FAILURE_STRINGS
                // not threadsafe
                static char invalid_chunk[] = "XXXX PNG chunk not known";
                invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
                invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
                invalid_chunk[2] = STBI__BYTECAST(c.type >> 8);
                invalid_chunk[3] = STBI__BYTECAST(c.type >> 0);
#endif
                return stbi__err(invalid_chunk, "PNG not supported: unknown PNG chunk type");
            }
            stbi__skip(s, c.length);
            break;
        }
        // end of PNG chunk, read and skip CRC
        stbi__get32be(s);
    }
}

static void* stbi__do_png(stbi__png* p, int* x, int* y, int* n, int req_comp, stbi__result_info* ri)
{
    void* result = NULL;
    if (req_comp < 0 || req_comp > 4) return stbi__errpuc("bad req_comp", "Internal error");
    if (stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
        if (p->depth <= 8)
            ri->bits_per_channel = 8;
        else if (p->depth == 16)
            ri->bits_per_channel = 16;
        else
            return stbi__errpuc("bad bits_per_channel", "PNG not supported: unsupported color depth");
        result = p->out;
        p->out = NULL;
        if (req_comp && req_comp != p->s->img_out_n) {
            if (ri->bits_per_channel == 8)
                result = stbi__convert_format((unsigned char*)result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
            else
                result = stbi__convert_format16((stbi__uint16*)result, p->s->img_out_n, req_comp, p->s->img_x, p->s->img_y);
            p->s->img_out_n = req_comp;
            if (result == NULL) return result;
        }
        *x = p->s->img_x;
        *y = p->s->img_y;
        if (n) *n = p->s->img_n;
    }
    STBI_FREE(p->out);      p->out = NULL;
    STBI_FREE(p->expanded); p->expanded = NULL;
    STBI_FREE(p->idata);    p->idata = NULL;

    return result;
}

static void* stbi__png_load(stbi__context* s, int* x, int* y, int* comp, int req_comp, stbi__result_info* ri)
{
    stbi__png p;
    p.s = s;
    return stbi__do_png(&p, x, y, comp, req_comp, ri);
}

static int stbi__png_test(stbi__context* s)
{
    int r;
    r = stbi__check_png_header(s);
    stbi__rewind(s);
    return r;
}

static int stbi__png_info_raw(stbi__png* p, int* x, int* y, int* comp)
{
    if (!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
        stbi__rewind(p->s);
        return 0;
    }
    if (x) *x = p->s->img_x;
    if (y) *y = p->s->img_y;
    if (comp) *comp = p->s->img_n;
    return 1;
}

static int stbi__png_info(stbi__context* s, int* x, int* y, int* comp)
{
    stbi__png p;
    p.s = s;
    return stbi__png_info_raw(&p, x, y, comp);
}

static int stbi__png_is16(stbi__context* s)
{
    stbi__png p;
    p.s = s;
    if (!stbi__png_info_raw(&p, NULL, NULL, NULL))
        return 0;
    if (p.depth != 16) {
        stbi__rewind(p.s);
        return 0;
    }
    return 1;
}
#endif

static int stbi__info_main(stbi__context* s, int* x, int* y, int* comp)
{
#ifndef STBI_NO_JPEG
    if (stbi__jpeg_info(s, x, y, comp)) return 1;
#endif

#ifndef STBI_NO_PNG
    if (stbi__png_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_GIF
    if (stbi__gif_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_BMP
    if (stbi__bmp_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PSD
    if (stbi__psd_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PIC
    if (stbi__pic_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_PNM
    if (stbi__pnm_info(s, x, y, comp))  return 1;
#endif

#ifndef STBI_NO_HDR
    if (stbi__hdr_info(s, x, y, comp))  return 1;
#endif

    // test tga last because it's a crappy test!
#ifndef STBI_NO_TGA
    if (stbi__tga_info(s, x, y, comp))
        return 1;
#endif
    return stbi__err("unknown image type", "Image not of any known type, or corrupt");
}

static int stbi__is_16_main(stbi__context* s)
{
#ifndef STBI_NO_PNG
    if (stbi__png_is16(s))  return 1;
#endif

#ifndef STBI_NO_PSD
    if (stbi__psd_is16(s))  return 1;
#endif

#ifndef STBI_NO_PNM
    if (stbi__pnm_is16(s))  return 1;
#endif
    return 0;
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const* filename, int* x, int* y, int* comp)
{
    FILE* f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_info_from_file(f, x, y, comp);
    fclose(f);
    return result;
}

STBIDEF int stbi_info_from_file(FILE* f, int* x, int* y, int* comp)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__info_main(&s, x, y, comp);
    fseek(f, pos, SEEK_SET);
    return r;
}

STBIDEF int stbi_is_16_bit(char const* filename)
{
    FILE* f = stbi__fopen(filename, "rb");
    int result;
    if (!f) return stbi__err("can't fopen", "Unable to open file");
    result = stbi_is_16_bit_from_file(f);
    fclose(f);
    return result;
}

STBIDEF int stbi_is_16_bit_from_file(FILE* f)
{
    int r;
    stbi__context s;
    long pos = ftell(f);
    stbi__start_file(&s, f);
    r = stbi__is_16_main(&s);
    fseek(f, pos, SEEK_SET);
    return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const* buffer, int len, int* x, int* y, int* comp)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const* c, void* user, int* x, int* y, int* comp)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)c, user);
    return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_is_16_bit_from_memory(stbi_uc const* buffer, int len)
{
    stbi__context s;
    stbi__start_mem(&s, buffer, len);
    return stbi__is_16_main(&s);
}

STBIDEF int stbi_is_16_bit_from_callbacks(stbi_io_callbacks const* c, void* user)
{
    stbi__context s;
    stbi__start_callbacks(&s, (stbi_io_callbacks*)c, user);
    return stbi__is_16_main(&s);
}

#endif // STB_IMAGE_IMPLEMENTATION

/*
   revision history:
      2.20  (2019-02-07) support utf8 filenames in Windows; fix warnings and platform ifdefs
      2.19  (2018-02-11) fix warning
      2.18  (2018-01-30) fix warnings
      2.17  (2018-01-29) change sbti__shiftsigned to avoid clang -O2 bug
                         1-bit BMP
                         *_is_16_bit api
                         avoid warnings
      2.16  (2017-07-23) all functions have 16-bit variants;
                         STBI_NO_STDIO works again;
                         compilation fixes;
                         fix rounding in unpremultiply;
                         optimize vertical flip;
                         disable raw_len validation;
                         documentation fixes
      2.15  (2017-03-18) fix png-1,2,4 bug; now all Imagenet JPGs decode;
                         warning fixes; disable run-time SSE detection on gcc;
                         uniform handling of optional "return" values;
                         thread-safe initialization of zlib tables
      2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet JPGs
      2.13  (2016-11-29) add 16-bit API, only supported for PNG right now
      2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
      2.11  (2016-04-02) allocate large structures on the stack
                         remove white matting for transparent PSD
                         fix reported channel count for PNG & BMP
                         re-enable SSE2 in non-gcc 64-bit
                         support RGB-formatted JPEG
                         read 16-bit PNGs (only as 8-bit)
      2.10  (2016-01-22) avoid warning introduced in 2.09 by STBI_REALLOC_SIZED
      2.09  (2016-01-16) allow comments in PNM files
                         16-bit-per-pixel TGA (not bit-per-component)
                         info() for TGA could break due to .hdr handling
                         info() for BMP to shares code instead of sloppy parse
                         can use STBI_REALLOC_SIZED if allocator doesn't support realloc
                         code cleanup
      2.08  (2015-09-13) fix to 2.07 cleanup, reading RGB PSD as RGBA
      2.07  (2015-09-13) fix compiler warnings
                         partial animated GIF support
                         limited 16-bpc PSD support
                         #ifdef unused functions
                         bug with < 92 byte PIC,PNM,HDR,TGA
      2.06  (2015-04-19) fix bug where PSD returns wrong '*comp' value
      2.05  (2015-04-19) fix bug in progressive JPEG handling, fix warning
      2.04  (2015-04-15) try to re-enable SIMD on MinGW 64-bit
      2.03  (2015-04-12) extra corruption checking (mmozeiko)
                         stbi_set_flip_vertically_on_load (nguillemot)
                         fix NEON support; fix mingw support
      2.02  (2015-01-19) fix incorrect assert, fix warning
      2.01  (2015-01-17) fix various warnings; suppress SIMD on gcc 32-bit without -msse2
      2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG
      2.00  (2014-12-25) optimize JPG, including x86 SSE2 & NEON SIMD (ryg)
                         progressive JPEG (stb)
                         PGM/PPM support (Ken Miller)
                         STBI_MALLOC,STBI_REALLOC,STBI_FREE
                         GIF bugfix -- seemingly never worked
                         STBI_NO_*, STBI_ONLY_*
      1.48  (2014-12-14) fix incorrectly-named assert()
      1.47  (2014-12-14) 1/2/4-bit PNG support, both direct and paletted (Omar Cornut & stb)
                         optimize PNG (ryg)
                         fix bug in interlaced PNG with user-specified channel count (stb)
      1.46  (2014-08-26)
              fix broken tRNS chunk (colorkey-style transparency) in non-paletted PNG
      1.45  (2014-08-16)
              fix MSVC-ARM internal compiler error by wrapping malloc
      1.44  (2014-08-07)
              various warning fixes from Ronny Chevalier
      1.43  (2014-07-15)
              fix MSVC-only compiler problem in code changed in 1.42
      1.42  (2014-07-09)
              don't define _CRT_SECURE_NO_WARNINGS (affects user code)
              fixes to stbi__cleanup_jpeg path
              added STBI_ASSERT to avoid requiring assert.h
      1.41  (2014-06-25)
              fix search&replace from 1.36 that messed up comments/error messages
      1.40  (2014-06-22)
              fix gcc struct-initialization warning
      1.39  (2014-06-15)
              fix to TGA optimization when req_comp != number of components in TGA;
              fix to GIF loading because BMP wasn't rewinding (whoops, no GIFs in my test suite)
              add support for BMP version 5 (more ignored fields)
      1.38  (2014-06-06)
              suppress MSVC warnings on integer casts truncating values
              fix accidental rename of 'skip' field of I/O
      1.37  (2014-06-04)
              remove duplicate typedef
      1.36  (2014-06-03)
              convert to header file single-file library
              if de-iphone isn't set, load iphone images color-swapped instead of returning NULL
      1.35  (2014-05-27)
              various warnings
              fix broken STBI_SIMD path
              fix bug where stbi_load_from_file no longer left file pointer in correct place
              fix broken non-easy path for 32-bit BMP (possibly never used)
              TGA optimization by Arseny Kapoulkine
      1.34  (unknown)
              use STBI_NOTUSED in stbi__resample_row_generic(), fix one more leak in tga failure case
      1.33  (2011-07-14)
              make stbi_is_hdr work in STBI_NO_HDR (as specified), minor compiler-friendly improvements
      1.32  (2011-07-13)
              support for "info" function for all supported filetypes (SpartanJ)
      1.31  (2011-06-20)
              a few more leak fixes, bug in PNG handling (SpartanJ)
      1.30  (2011-06-11)
              added ability to load files via callbacks to accomidate custom input streams (Ben Wenger)
              removed deprecated format-specific test/load functions
              removed support for installable file formats (stbi_loader) -- would have been broken for IO callbacks anyway
              error cases in bmp and tga give messages and don't leak (Raymond Barbiero, grisha)
              fix inefficiency in decoding 32-bit BMP (David Woo)
      1.29  (2010-08-16)
              various warning fixes from Aurelien Pocheville
      1.28  (2010-08-01)
              fix bug in GIF palette transparency (SpartanJ)
      1.27  (2010-08-01)
              cast-to-stbi_uc to fix warnings
      1.26  (2010-07-24)
              fix bug in file buffering for PNG reported by SpartanJ
      1.25  (2010-07-17)
              refix trans_data warning (Won Chun)
      1.24  (2010-07-12)
              perf improvements reading from files on platforms with lock-heavy fgetc()
              minor perf improvements for jpeg
              deprecated type-specific functions so we'll get feedback if they're needed
              attempt to fix trans_data warning (Won Chun)
      1.23    fixed bug in iPhone support
      1.22  (2010-07-10)
              removed image *writing* support
              stbi_info support from Jetro Lauha
              GIF support from Jean-Marc Lienher
              iPhone PNG-extensions from James Brown
              warning-fixes from Nicolas Schulz and Janez Zemva (i.stbi__err. Janez (U+017D)emva)
      1.21    fix use of 'stbi_uc' in header (reported by jon blow)
      1.20    added support for Softimage PIC, by Tom Seddon
      1.19    bug in interlaced PNG corruption check (found by ryg)
      1.18  (2008-08-02)
              fix a threading bug (local mutable static)
      1.17    support interlaced PNG
      1.16    major bugfix - stbi__convert_format converted one too many pixels
      1.15    initialize some fields for thread safety
      1.14    fix threadsafe conversion bug
              header-file-only version (#define STBI_HEADER_FILE_ONLY before including)
      1.13    threadsafe
      1.12    const qualifiers in the API
      1.11    Support installable IDCT, colorspace conversion routines
      1.10    Fixes for 64-bit (don't use "unsigned long")
              optimized upsampling by Fabian "ryg" Giesen
      1.09    Fix format-conversion for PSD code (bad global variables!)
      1.08    Thatcher Ulrich's PSD code integrated by Nicolas Schulz
      1.07    attempt to fix C++ warning/errors again
      1.06    attempt to fix C++ warning/errors again
      1.05    fix TGA loading to return correct *comp and use good luminance calc
      1.04    default float alpha is 1, not 255; use 'void *' for stbi_image_free
      1.03    bugfixes to STBI_NO_STDIO, STBI_NO_HDR
      1.02    support for (subset of) HDR files, float interface for preferred access to them
      1.01    fix bug: possible bug in handling right-side up bmps... not sure
              fix bug: the stbi__bmp_load() and stbi__tga_load() functions didn't work at all
      1.00    interface to zlib that skips zlib header
      0.99    correct handling of alpha in palette
      0.98    TGA loader by lonesock; dynamically add loaders (untested)
      0.97    jpeg errors on too large a file; also catch another malloc failure
      0.96    fix detection of invalid v value - particleman@mollyrocket forum
      0.95    during header scan, seek to markers in case of padding
      0.94    STBI_NO_STDIO to disable stdio usage; rename all #defines the same
      0.93    handle jpegtran output; verbose errors
      0.92    read 4,8,16,24,32-bit BMP files of several formats
      0.91    output 24-bit Windows 3.0 BMP files
      0.90    fix a few more warnings; bump version number to approach 1.0
      0.61    bugfixes due to Marc LeBlanc, Christopher Lloyd
      0.60    fix compiling as c++
      0.59    fix warnings: merge Dave Moore's -Wall fixes
      0.58    fix bug: zlib uncompressed mode len/nlen was wrong endian
      0.57    fix bug: jpg last huffman symbol before marker was >9 bits but less than 16 available
      0.56    fix bug: zlib uncompressed mode len vs. nlen
      0.55    fix bug: restart_interval not initialized to 0
      0.54    allow NULL for 'int *comp'
      0.53    fix bug in png 3->4; speedup png decoding
      0.52    png handles req_comp=3,4 directly; minor cleanup; jpeg comments
      0.51    obey req_comp requests, 1-component jpegs return as 1-component,
              on 'test' only check type, not whether we support this variant
      0.50  (2006-11-19)
              first released version
*/


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/