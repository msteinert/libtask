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

/*
 * sleep and wakeup
 */
void
tasksleep(Rendez *r)
{
	addtask(&r->waiting, taskrunning);
	if (r->l) {
		qunlock(r->l);
	}
	taskstate("sleep");
	taskswitch();
	if (r->l) {
		qlock(r->l);
	}
}

static int
_taskwakeup(Rendez *r, int all)
{
	int i;
	Task *t;
	for (i = 0;; ++i) {
		if (i == 1 && !all) {
			break;
		}
		if ((t = r->waiting.head) == nil) {
			break;
		}
		deltask(&r->waiting, t);
		taskready(t);
	}
	return i;
}

int
taskwakeup(Rendez *r)
{
	return _taskwakeup(r, 0);
}

int
taskwakeupall(Rendez *r)
{
	return _taskwakeup(r, 1);
}
