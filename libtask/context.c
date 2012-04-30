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

#include "taskimpl.h"

#if defined(__APPLE__)
#if defined(__i386__)
#define NEEDX86MAKECONTEXT
#define NEEDSWAPCONTEXT
#elif defined(__x86_64__)
#define NEEDAMD64MAKECONTEXT
#define NEEDSWAPCONTEXT
#else
#define NEEDPOWERMAKECONTEXT
#define NEEDSWAPCONTEXT
#endif
#endif

#if defined(__FreeBSD__) && defined(__i386__) && __FreeBSD__ < 5
#define NEEDX86MAKECONTEXT
#define NEEDSWAPCONTEXT
#endif

#if defined(__OpenBSD__) && defined(__i386__)
#define NEEDX86MAKECONTEXT
#define NEEDSWAPCONTEXT
#endif

#if defined(__linux__) && defined(__arm__)
#define NEEDSWAPCONTEXT
#define NEEDARMMAKECONTEXT
#endif

#if defined(__linux__) && defined(__mips__)
#define	NEEDSWAPCONTEXT
#define	NEEDMIPSMAKECONTEXT
#endif

#ifdef NEEDPOWERMAKECONTEXT
void
makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	ulong *sp, *tos;
	va_list arg;
	tos = (ulong *)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size /
		sizeof(ulong);
	sp = tos - 16;
	ucp->mc.pc = (long)func;
	ucp->mc.sp = (long)sp;
	va_start(arg, argc);
	ucp->mc.r3 = va_arg(arg, long);
	va_end(arg);
}
#endif

#ifdef NEEDX86MAKECONTEXT
void
makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	int *sp;
	sp = (int *)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size / 4;
	sp -= argc;
	sp = (void *)((uintptr_t)sp - (uintptr_t)sp % 16); /* 16-align for OSX */
	memmove(sp, &argc+1, argc*sizeof(int));
	*--sp = 0; /* return address */
	ucp->uc_mcontext.mc_eip = (long)func;
	ucp->uc_mcontext.mc_esp = (int)sp;
}
#endif

#ifdef NEEDAMD64MAKECONTEXT
void
makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	long *sp;
	va_list va;
	memset(&ucp->uc_mcontext, 0, sizeof ucp->uc_mcontext);
	if (argc != 2) {
		*(int *)0 = 0;
	}
	va_start(va, argc);
	ucp->uc_mcontext.mc_rdi = va_arg(va, int);
	ucp->uc_mcontext.mc_rsi = va_arg(va, int);
	va_end(va);
	sp = (long *)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size / sizeof(long);
	sp -= argc;
	sp = (void *)((uintptr_t)sp - (uintptr_t)sp % 16); /* 16-align for OS X */
	*--sp = 0; /* return address */
	ucp->uc_mcontext.mc_rip = (long)func;
	ucp->uc_mcontext.mc_rsp = (long)sp;
}
#endif

#ifdef NEEDARMMAKECONTEXT
void
makecontext(ucontext_t *uc, void (*fn)(void), int argc, ...)
{
	int i, *sp;
	va_list arg;
	sp = (int *)uc->uc_stack.ss_sp + uc->uc_stack.ss_size / 4;
	va_start(arg, argc);
	for (i = 0; i < 4 && i < argc; ++i) {
		uc->uc_mcontext.gregs[i] = va_arg(arg, uint);
	}
	va_end(arg);
	uc->uc_mcontext.gregs[13] = (uint)sp;
	uc->uc_mcontext.gregs[14] = (uint)fn;
}
#endif

#ifdef NEEDMIPSMAKECONTEXT
void
makecontext(ucontext_t *uc, void (*fn)(void), int argc, ...)
{
	int i, *sp;
	va_list arg;
	va_start(arg, argc);
	sp = (int *)uc->uc_stack.ss_sp + uc->uc_stack.ss_size / 4;
	for (i = 0; i<4 && i < argc; ++i) {
		uc->uc_mcontext.mc_regs[i + 4] = va_arg(arg, int);
	}
	va_end(arg);
	uc->uc_mcontext.mc_regs[29] = (int)sp;
	uc->uc_mcontext.mc_regs[31] = (int)fn;
}
#endif

#ifdef NEEDSWAPCONTEXT
int
swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
	if (getcontext(oucp) == 0) {
		setcontext(ucp);
	}
	return 0;
}
#endif
