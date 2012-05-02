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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <task.h>
#include "taskimpl.h"

const int HTTP_STACK = 32768;
char *server;
char *url;

void
fetchtask(TASK_UNUSED void *v)
{
	char buf[512];
	int fd, n, i = 0;
	snprintf(buf, sizeof buf,
		 "GET %s HTTP/1.0\r\n"
		 "Host: %s\r\n\r\n",
		 url, server);
	fprintf(stdout, "%s %s\n", server, url);
	for (;;) {
		if ((fd = netdial(TCP, server, 80)) < 0) {
			fprintf(stderr, "dial %s: %s (%s)\n", server,
				strerror(errno), taskgetstate());
			continue;
		}
		fdwrite(fd, buf, strlen(buf));
		while ((n = fdread(fd, buf, sizeof buf)) > 0) {
			i += n;
			if (1024 <= i) {
				if (0 > write(STDOUT_FILENO, ".", 1)) {
					taskexitall(EXIT_FAILURE);
				}
				i -= 1024;
			}
		}
		close(fd);
	}
}

void
taskmain(int argc, char **argv)
{
	int i, n;
	if (argc != 4){
		fprintf(stderr, "usage: httpload n server url\n");
		taskexitall(EXIT_FAILURE);
	}
	n = atoi(argv[1]);
	server = argv[2];
	url = argv[3];
	for (i = 0; i < n; i++){
		taskcreate(fetchtask, 0, HTTP_STACK);
		while (taskyield() > 1) {
			/* do nothing */
		}
	}
}
