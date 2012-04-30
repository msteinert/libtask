/*
 * Copyright 2005-2007 Russ Cox, Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#if defined(__sun__)
#define __EXTENSIONS__ 1 /* SunOS */
#if defined(__SunOS5_6__) || defined(__SunOS5_7__) || defined(__SunOS5_8__)
/* do nothing */
#else
#define __MAKECONTEXT_V2_SOURCE 1
#endif
#endif

#define USE_UCONTEXT 1

#if defined(__OpenBSD__) || defined(__mips__)
#undef USE_UCONTEXT
#define USE_UCONTEXT 0
#endif

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#if defined(MAC_OS_X_VERSION_10_5)
#undef USE_UCONTEXT
#define USE_UCONTEXT 0
#endif
#endif

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#if USE_UCONTEXT
#include <ucontext.h>
#endif
#include <sys/utsname.h>
#include <inttypes.h>
#include "task.h"

#define nil ((void *)0)
#define nelem(x) (sizeof(x) / sizeof((x)[0]))

#define ulong task_ulong
#define uint task_uint
#define uchar task_uchar
#define ushort task_ushort
#define uvlong task_uvlong
#define vlong task_vlong

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long long uvlong;
typedef long long vlong;

#define print task_print
#define fprint task_fprint
#define snprint task_snprint
#define seprint task_seprint
#define vprint task_vprint
#define vfprint task_vfprint
#define vsnprint task_vsnprint
#define vseprint task_vseprint
#define strecpy task_strecpy

int print(char *, ...);
int fprint(int, char *, ...);
char *snprint(char *, uint, char *, ...);
char *seprint(char *, char *, char *, ...);
int vprint(char *, va_list);
int vfprint(int, char *, va_list);
char *vsnprint(char *, uint, char *, va_list);
char *vseprint(char *, char *, char *, va_list);
char *strecpy(char *, char *, char *);

#if defined(__FreeBSD__) && __FreeBSD__ < 5
extern int getmcontext(mcontext_t*);
extern void setmcontext(const mcontext_t*);
#define setcontext(u) setmcontext(&(u)->uc_mcontext)
#define getcontext(u) getmcontext(&(u)->uc_mcontext)
extern int swapcontext(ucontext_t*, const ucontext_t*);
extern void makecontext(ucontext_t*, void(*)(), int, ...);
#endif

#if defined(__APPLE__)
#define mcontext libthread_mcontext
#define mcontext_t libthread_mcontext_t
#define ucontext libthread_ucontext
#define ucontext_t libthread_ucontext_t
#if defined(__i386__)
#include "386-ucontext.h"
#elif defined(__x86_64__)
#include "amd64-ucontext.h"
#else
#include "power-ucontext.h"
#endif
#endif

#if defined(__OpenBSD__)
#define mcontext libthread_mcontext
#define mcontext_t libthread_mcontext_t
#define ucontext libthread_ucontext
#define ucontext_t libthread_ucontext_t
#if defined __i386__
#include "386-ucontext.h"
#else
#include "power-ucontext.h"
#endif
extern pid_t rfork_thread(int, void*, int(*)(void*), void*);
#endif

#if defined(__arm__)
int getmcontext(mcontext_t*);
void setmcontext(const mcontext_t*);
#define setcontext(u) setmcontext(&(u)->uc_mcontext)
#define getcontext(u) getmcontext(&(u)->uc_mcontext)
#endif

#if defined(__mips__)
#include "mips-ucontext.h"
int getmcontext(mcontext_t*);
void setmcontext(const mcontext_t*);
#define setcontext(u) setmcontext(&(u)->uc_mcontext)
#define getcontext(u) getmcontext(&(u)->uc_mcontext)
#endif

typedef struct Context Context;

enum {
	STACK = 8192
};

struct Context {
	ucontext_t uc;
};

struct Task {
	char name[256]; // offset known to acid
	char state[256];
	Task *next;
	Task *prev;
	Task *allnext;
	Task *allprev;
	Context context;
	uvlong alarmtime;
	uint id;
	uchar *stk;
	uint stksize;
	int exiting;
	int alltaskslot;
	int system;
	int ready;
	void (*startfn)(void*);
	void *startarg;
	void *udata;
};

void taskready(Task*);
void taskswitch(void);

void addtask(Tasklist*, Task*);
void deltask(Tasklist*, Task*);

extern Task *taskrunning;
extern int taskcount;

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define TASK_PRINTF(format, argument) \
	__attribute__ ((__format__ (__printf__, format, argument)))
#define TASK_UNUSED \
	__attribute__ ((__unused__))
#else
#define TASK_PRINTF(format, argument)
#define TASK_UNUSED
#endif
