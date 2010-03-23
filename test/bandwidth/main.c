/*============================================================================
  bandwidth 0.15a, a benchmark to estimate memory transfer bandwidth.
  Copyright (C) 2005,2007,2008,2010 by Zack T Smith.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  The author may be reached at fbui@comcast.net.
 *===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#if !defined(__CYGWIN__) 
#include <sys/mman.h>
#endif

#if !defined(__CYGWIN__) && !defined(ARM)
#include <linux/fb.h>
#endif

#define VERSION "0.15a"

enum {true=1,false=0};

struct cpu_features {
	unsigned long	ebx;
	unsigned long	edx;
	unsigned long	ecx;
};

extern int L1_reader (unsigned long *ptr, unsigned long count);
extern int L1_writer (unsigned long *ptr, unsigned long count);
extern int L2_reader (unsigned long *ptr, unsigned long count);
extern int L2_writer (unsigned long *ptr, unsigned long count);
extern int Main_reader (unsigned long *ptr, unsigned long count);
extern int Main_writer (unsigned long *ptr, unsigned long count);

#ifndef ARM
extern unsigned long Get_CPUID (void*, unsigned long request);
extern int Main_random_writer (unsigned long *ptr, unsigned long count);
extern int Framebuffer_reader (unsigned long *ptr, unsigned long count, unsigned long length);
extern int Framebuffer_writer (unsigned long *ptr, unsigned long count, unsigned long length);
extern void my_bzero (unsigned char *ptr, unsigned long length);
extern void my_memcpy (unsigned char *, unsigned char *, unsigned long length);
extern unsigned long Has_MMX(void);
#endif

#define MAINSIZE (16*1024*1024)
#define MAINSIZEMASK 0xffffff
#define MAINLOOPS_R 200
#define MAINLOOPS_W 200

#define L1SIZE 148
#define L1LOOPS_R (0x4000000)	// This number is so large, it limits 
#define L1LOOPS_W (0x4000000)	// the seconds per test to 63.

#define L2SIZE 131072
#define L2LOOPS_R 30000
#define L2LOOPS_W 30000

#define FBLOOPS_R 100
#define FBLOOPS_W 200

static long usec_per_test = 15000000;

unsigned long mytime()
{
	struct timeval tv;
	struct timezone tz;
	
	memset (&tz, 0, sizeof(struct timezone));

	gettimeofday (&tv, &tz);
	return 1000000 * tv.tv_sec + tv.tv_usec;
}

void
main_random_write ()
{
	unsigned long *chunk = (unsigned long*) malloc (MAINSIZE);
	unsigned long counter;
	unsigned long total_count=0;
	double rate;
	unsigned long diff=0, t0;

	//-------------------------------------------------
	printf ("Main memory random write ");
	fflush (stdout);

	memset (chunk, 0, MAINSIZE);

	t0 = mytime();

	while (diff < usec_per_test) {
		counter = MAINLOOPS_W;
		total_count += counter;
	
//		Main_random_writer (chunk, counter);
		diff = mytime() - t0;
	}

	rate = MAINSIZE;
	rate *= total_count;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	free((void*)chunk);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}

void
main_seq_write ()
{
	unsigned long *chunk = (unsigned long*) malloc (MAINSIZE);
	unsigned long counter;
	unsigned long total_count=0;
	double rate;
	unsigned long diff=0, t0;

	//-------------------------------------------------
	printf ("Main memory sequential write ");
	fflush (stdout);

	memset (chunk, 0, MAINSIZE);

	t0 = mytime();

	while (diff < usec_per_test) {
		counter = MAINLOOPS_W;
		total_count += counter;
	
		Main_writer (chunk, counter);
		diff = mytime() - t0;
	}

	rate = MAINSIZE;
	rate *= total_count;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	free((void*)chunk);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}

void
main_seq_read ()
{
	unsigned long *chunk = (unsigned long*) malloc (MAINSIZE);
	unsigned long counter;
	unsigned long total_count=0;
	double rate;
	unsigned long diff=0, t0;

	//-------------------------------------------------
	printf ("Main memory sequential read ");
	fflush (stdout);

	memset (chunk, 0, MAINSIZE);

	t0 = mytime();

	while (diff < usec_per_test) {
		counter = MAINLOOPS_R;
		total_count += counter;
	
		Main_reader (chunk, counter);
		diff = mytime() - t0;
	}

	rate = MAINSIZE;
	rate /= diff;
	rate *= 1000000;
	rate *= total_count;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);

	free((void*)chunk);
}


void
l1_seq_read ()
{
	unsigned long counter;
	unsigned long total_count=0;
	unsigned long *ptr;
	double rate;
	unsigned long diff=0, t0;
	unsigned long chunk3[L1SIZE/4];
	int i;

	//-------------------------------------------------

	ptr = chunk3;

	printf ("L1 cache sequential read ");
	fflush (stdout);

	int result = 6;

	t0 = mytime();

	ptr = chunk3;
	for (i=0; i<16; i++)
		ptr[i] = 0;
	ptr[0] = 6167;

	while (diff < usec_per_test) {
		counter = L1LOOPS_R;
		total_count += counter;

		L1_reader (ptr, counter);

		diff = mytime() - t0;
	}

	if (ptr[0] != 6167)
		puts ("corrupted\n");

	rate = total_count;	// iterations
	rate *= L1SIZE;	
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);	// divided by 1 million

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}


void
l1_seq_write ()
{
	unsigned long counter;
	unsigned long total_count=0;
	unsigned long *ptr;
	double rate;
	unsigned long diff=0, t0;
	unsigned long chunk3[L1SIZE/4];
	int i;

	//-------------------------------------------------

	ptr = chunk3;
	memset (chunk3, 0, 64);
	ptr[0] = 6167;

	printf ("L1 cache sequential write ");
	fflush (stdout);

	int result = 6;

	t0 = mytime();

	ptr = chunk3;

	while (diff < usec_per_test) {
		counter = L1LOOPS_W;
		total_count += counter;
	
		L1_writer (ptr, counter);

		diff = mytime() - t0;
	}

	if (ptr[0] == 6167)
		puts ("corrupted\n");

	rate = total_count;	// iterations
	rate *= L1SIZE;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);	// divided by 1 million

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}



void
l2_seq_read ()
{
	unsigned long counter;
	unsigned long total_count=0;
	unsigned long *ptr;
	double rate;
	unsigned long t, t0, diff=0;
	unsigned long chunk3[L2SIZE/4];

	//-------------------------------------------------

	ptr = chunk3;

	printf ("L2 cache sequential read ");
	fflush (stdout);

	t0 = mytime();

	while (diff < usec_per_test) {
		counter = L2LOOPS_R;
		total_count += counter;

		L2_reader (chunk3, counter);

		diff = mytime() - t0;
	}

	rate = total_count;
	rate *= L2SIZE;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}


void
l2_seq_write ()
{
	unsigned long counter;
	unsigned long total_count=0;
	unsigned long *ptr;
	double rate;
	unsigned long diff=0, t0;
	unsigned long chunk3[L2SIZE/4];

	//-------------------------------------------------

	ptr = chunk3;

	printf ("L2 cache sequential write ");
	fflush (stdout);

	t0 = mytime();

	while (diff < usec_per_test) {
		counter = L2LOOPS_W;
		total_count += counter;

		L2_writer (chunk3, counter);

		diff = mytime() - t0;
	}

	rate = total_count;
	rate *= L2SIZE;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}

#if !defined(__CYGWIN__) && !defined(ARM)
void
fb_readwrite ()
{

	unsigned long counter;
	unsigned long total_count;
	unsigned long length;
	double rate;
	unsigned long diff, t0;
	static struct fb_fix_screeninfo fi;
	static struct fb_var_screeninfo vi;
	unsigned long *fb = NULL;
	unsigned long datum;
	int fd;
	register unsigned long foo;

	//-------------------------------------------------

	fd = open ("/dev/fb0", O_RDWR);
	if (fd < 0) 
		fd = open ("/dev/fb/0", O_RDWR);
	if (fd < 0) {
		perror ("open");
		printf ("Can't open framebuffer.\n");
		exit(0);
	}

	if (ioctl (fd, FBIOGET_FSCREENINFO, &fi))
	{
		perror("ioctl");
		exit(1);
	}
	else
	if (ioctl (fd, FBIOGET_VSCREENINFO, &vi))
	{
		perror("ioctl");
		exit(1);
	}
	else
	{
		if (fi.visual != FB_VISUAL_TRUECOLOR &&
		    fi.visual != FB_VISUAL_DIRECTCOLOR )
		{
			fprintf (stderr, "Error: need direct/truecolor framebuffer device.\n");
			return;
		}
		else
		{
			unsigned long fblen;

			printf ("Framebuffer resolution: %dx%d, %dbpp\n",
				vi.xres, vi.yres, vi.bits_per_pixel);

			fb = (unsigned long*) fi.smem_start;
			fblen = fi.smem_len;

			fb = mmap (fb, fblen,
				PROT_WRITE | PROT_READ,
				MAP_SHARED, fd, 0);
			if (fb == MAP_FAILED)
			{
				perror ("mmap");
				return;
			}
		}
	}

	//-------------------
	// Use only the upper half of the display.
	//
	length = (vi.xres * vi.yres * vi.bits_per_pixel / 8) / 2;

	//-------------------
	// READ

	printf ("Framebuffer memory sequential read ");
	fflush (stdout);

	t0 = mytime();
	diff = 0;
	total_count = 0;

	while (diff < usec_per_test) {
		total_count += FBLOOPS_R;

		Framebuffer_reader (fb, FBLOOPS_R, length);

		diff = mytime() - t0;
	}

	rate = total_count;
	rate *= length;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);

	//-------------------
	// WRITE

	printf ("Framebuffer memory sequential write ");
	fflush (stdout);

	t0 = mytime();
	diff = 0;
	total_count = 0;

	while (diff < usec_per_test) {
		total_count += FBLOOPS_W;

		Framebuffer_writer (fb, FBLOOPS_W, length);

		diff = mytime() - t0;
	}

	rate = total_count;
	rate *= length;
	rate /= diff;
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);
}


int
library_test() 
{
	double rate;
	char *a1, *a2;
	unsigned long t, t0;
	int i;
#define LIB_CHUNK_SIZE (6*1024*1024)
#define LIB_LOOPS (200)
	unsigned long chunk_size = LIB_CHUNK_SIZE;
	
	a1 = malloc(chunk_size);
	if (!a1) {
		perror("malloc1");
		return -1;
	}
	a2 = malloc(chunk_size);
	if (!a2) {
		perror("malloc2");
		return -1;
	}

	//--------------------------------------
	printf ("Library function memset: ");
	fflush (stdout);

	t0 = mytime();
	for (i=0; i<LIB_LOOPS; i++) {
		memset (a1, i, chunk_size);
	}
	t = mytime();
	rate = chunk_size;
	rate *= LIB_LOOPS;
	rate /= (t-t0);
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);

	//--------------------------------------
	printf ("Function my_memcpy: ");
	fflush (stdout);

	t0 = mytime();
	for (i=0; i<LIB_LOOPS; i++) {
		my_memcpy (a2, a1, chunk_size);
	}
	t = mytime();
	rate = chunk_size;
	rate *= LIB_LOOPS;
	rate /= (t-t0);
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);

	//--------------------------------------
	printf ("Function my_bzero: ");
	fflush (stdout);

	t0 = mytime();
	for (i=0; i<LIB_LOOPS; i++) {
		my_bzero (a2, chunk_size);
	}
	t = mytime();
	rate = chunk_size;
	rate *= LIB_LOOPS;
	rate /= (t-t0);
	rate *= 1000000;
	rate /= (1<<20);

	printf ("%g MB/sec \n", rate);
	fflush (stdout);

	free(a1);
	free(a2);
}
#endif

#ifndef ARM
int
has_longmode ()
{
	struct cpu_features feat;
	char longmode;
	unsigned long eax;

	eax = Get_CPUID (&feat, 0x80000001);

	longmode = feat.edx & (1<<29) ? true : false;
	
	return longmode;
}
#endif

int
main(int argc, char **argv)
{
	FILE *f; 
	char longmode = false;

	if (argc == 2) {
		char *s = argv[1];
		int seconds = atoi(s);
		if (seconds > 63) {
			seconds = 63;
			fprintf (stderr, "Limited to 63 seconds.\n");
		}
		if (isdigit(*s))
			usec_per_test = 1000000 * atoi(s);
	} else if (argc != 1) {
		fprintf (stderr, "Usage: bandwidth [seconds_per_test]\n");
		return 1;
	}
	
	printf ("This is bandwidth version %s\n", VERSION);
	printf ("Copyright (C) 2005,2007,2008,2010 by Zack T Smith\n\n");

#ifndef ARM
	char str[16];
	Get_CPUID (str, 0);
	str[12] = 0;
	printf ("CPU vendor id = '%s'\n", str);

	if (has_longmode())
		puts ("CPU is 64-bit capable.");
	if (Has_MMX())
		puts ("CPU has MMX.");

	printf ("CPU MHz = ");
	fflush (stdout);
	system ("grep MHz /proc/cpuinfo | uniq | sed \"s/[\\t\\n: a-zA-Z]//g\"");
	fflush (stdout);
#endif

	printf ("\nAll tests are 32-bit:\n");

	l1_seq_read ();
	l1_seq_write ();

	l2_seq_read ();
	l2_seq_write ();

	main_seq_read ();
	main_seq_write ();

#if !defined(__CYGWIN__) && !defined(ARM)
	fb_readwrite();
#endif

#ifndef ARM
	library_test();
#endif

	return 0;
}
