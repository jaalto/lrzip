/*
   Copyright (C) 2006-2011 Con Kolivas
   Copyright (C) 2011 Peter Hyman
   Copyright (C) 1998-2003 Andrew Tridgell

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LRZIP_PRIV_H
#define LRZIP_PRIV_H

#define NUM_STREAMS 2
#define STREAM_BUFSIZE (1024 * 1024 * 10)

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef int32
#if (SIZEOF_INT == 4)
#define int32 int
#elif (SIZEOF_LONG == 4)
#define int32 long
#elif (SIZEOF_SHORT == 4)
#define int32 short
#endif
#endif

#ifndef int16
#if (SIZEOF_INT == 2)
#define int16 int
#elif (SIZEOF_SHORT == 2)
#define int16 short
#endif
#endif

#ifndef uint32
#define uint32 unsigned int32
#endif

#ifndef uint16
#define uint16 unsigned int16
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b)? (a): (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b)? (a): (b))
#endif

#if !HAVE_STRERROR
extern char *sys_errlist[];
#define strerror(i) sys_errlist[i]
#endif

#ifndef HAVE_ERRNO_H
extern int errno;
#endif

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

typedef long long int i64;
typedef uint32_t u32;

typedef struct rzip_control rzip_control;
typedef struct md5_ctx md5_ctx;

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
	defined(__APPLE__) || defined(__CYGWIN__)
 #define mremap fake_mremap
#endif

#if defined(__APPLE__)
# define MD5_RELIABLE (0)
#else
# define MD5_RELIABLE (1)
#endif

#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

# define bswap_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)				      \
      | (((x) & 0x00ff000000000000ull) >> 40)				      \
      | (((x) & 0x0000ff0000000000ull) >> 24)				      \
      | (((x) & 0x000000ff00000000ull) >> 8)				      \
      | (((x) & 0x00000000ff000000ull) << 8)				      \
      | (((x) & 0x0000000000ff0000ull) << 24)				      \
      | (((x) & 0x000000000000ff00ull) << 40)				      \
      | (((x) & 0x00000000000000ffull) << 56))

#ifdef leto32h
# define le32toh(x) leto32h(x)
# define le64toh(x) leto64h(x)
#endif

#ifndef le32toh
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htole32(x) (x)
#  define le32toh(x) (x)
#  define htole64(x) (x)
#  define le64toh(x) (x)
# elif __BYTE_ORDER == __BIG_ENDIAN
#  define htole32(x) bswap_32 (x)
#  define le32toh(x) bswap_32 (x)
#  define htole64(x) bswap_64 (x)
#  define le64toh(x) bswap_64 (x)
#else
#error UNKNOWN BYTE ORDER
#endif
#endif

#define FLAG_SHOW_PROGRESS	(1 << 0)
#define FLAG_KEEP_FILES		(1 << 1)
#define FLAG_TEST_ONLY		(1 << 2)
#define FLAG_FORCE_REPLACE	(1 << 3)
#define FLAG_DECOMPRESS		(1 << 4)
#define FLAG_NO_COMPRESS	(1 << 5)
#define FLAG_LZO_COMPRESS	(1 << 6)
#define FLAG_BZIP2_COMPRESS	(1 << 7)
#define FLAG_ZLIB_COMPRESS	(1 << 8)
#define FLAG_ZPAQ_COMPRESS	(1 << 9)
#define FLAG_VERBOSITY		(1 << 10)
#define FLAG_VERBOSITY_MAX	(1 << 11)
#define FLAG_STDIN		(1 << 12)
#define FLAG_STDOUT		(1 << 13)
#define FLAG_INFO		(1 << 14)
#define FLAG_UNLIMITED		(1 << 15)
#define FLAG_HASH		(1 << 16)
#define FLAG_MD5		(1 << 17)
#define FLAG_CHECK		(1 << 18)
#define FLAG_KEEP_BROKEN	(1 << 19)
#define FLAG_THRESHOLD		(1 << 20)
#define FLAG_TMP_OUTBUF		(1 << 21)
#define FLAG_TMP_INBUF		(1 << 22)
#define FLAG_ENCRYPT		(1 << 23)

#define NO_MD5		(!(HASH_CHECK) && !(HAS_MD5))

#define BITS32		(sizeof(long) == 4)

#define CTYPE_NONE 3
#define CTYPE_BZIP2 4
#define CTYPE_LZO 5
#define CTYPE_LZMA 6
#define CTYPE_GZIP 7
#define CTYPE_ZPAQ 8

#define PASS_LEN 512
#define HASH_LEN 64
#define SALT_LEN 8
#define CBC_LEN 16

#define print_err(format, args...)	do {\
	fprintf(stderr, format, ##args);	\
} while (0)


/* Structure to save state of computation between the single steps.  */
struct md5_ctx
{
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;

	uint32_t total[2];
	uint32_t buflen;
	uint32_t buffer[32];
};

struct rzip_control {
	char *infile;
	char *outname;
	char *outfile;
	char *outdir;
	char *tmpdir; // when stdin, stdout, or test used
	uchar *tmp_outbuf; // Temporary file storage for stdout
	i64 out_ofs; // Output offset when tmp_outbuf in use
	i64 hist_ofs; // History offset
	i64 out_len; // Total length of tmp_outbuf
	i64 out_maxlen; // The largest the tmp_outbuf can be used
	i64 out_relofs; // Relative tmp_outbuf offset when stdout has been flushed
	uchar *tmp_inbuf;
	i64 in_ofs;
	i64 in_len;
	i64 in_maxlen;
	FILE *msgout; //stream for output messages
	const char *suffix;
	uchar compression_level;
	i64 overhead; // compressor overhead
	i64 usable_ram; // the most ram we'll try to use on one activity
	i64 maxram; // the largest chunk of ram to allocate
	unsigned char lzma_properties[5]; // lzma properties, encoded
	i64 window;
	unsigned long flags;
	i64 ramsize;
	i64 max_chunk;
	i64 max_mmap;
	int threads;
	char nice_val;		// added for consistency
	char major_version;
	char minor_version;
	i64 st_size;
	long page_size;
	int fd_in;
	int fd_out;
	int fd_hist;
	i64 encloops;
	i64 secs;
	uchar salt[SALT_LEN];
	uchar *salt_pass;
	int salt_pass_len;
	uchar *hash;
	unsigned char eof;
	unsigned char magic_written;
	md5_ctx ctx;
	i64 md5_read; // How far into the file the md5 has done so far
};

struct stream {
	i64 last_head;
	uchar *buf;
	i64 buflen;
	i64 bufp;
	uchar eos;
	long uthread_no;
	long unext_thread;
	long base_thread;
	int total_threads;
	i64 last_headofs;
};

struct stream_info {
	struct stream *s;
	uchar num_streams;
	int fd;
	i64 bufsize;
	i64 cur_pos;
	i64 initial_pos;
	i64 ram_alloced;
	i64 size;
	long thread_no;
	long next_thread;
	int chunks;
	char chunk_bytes;
};
#endif
