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
#include <sys/socket.h>
#include <task.h>
#include <unistd.h>

const int STACK = 32768;
char *server;
int port;

int *
mkfd2(int fd1, int fd2)
{
	int *a;
	a = malloc(2 * sizeof a[0]);
	if (a == 0) {
		fprintf(stderr, "out of memory\n");
		abort();
	}
	a[0] = fd1;
	a[1] = fd2;
	return a;
}

void
rwtask(void *v)
{
	int *a, rfd, wfd, n;
	char buf[2048];
	a = v;
	rfd = a[0];
	wfd = a[1];
	free(a);
	while ((n = fdread(rfd, buf, sizeof buf)) > 0) {
		fdwrite(wfd, buf, n);
	}
	shutdown(wfd, SHUT_WR);
	close(rfd);
}

void
proxytask(void *v)
{
	int fd, remotefd;
	fd = (int)v;
	if((remotefd = netdial(TCP, server, port)) < 0) {
		close(fd);
		return;
	}
	fprintf(stderr, "connected to %s:%d\n", server, port);
	taskcreate(rwtask, mkfd2(fd, remotefd), STACK);
	taskcreate(rwtask, mkfd2(remotefd, fd), STACK);
}

void
taskmain(int argc, char **argv)
{
	int cfd, fd;
	int rport;
	char remote[16];
	if (argc != 4){
		fprintf(stderr,
			"usage: tcpproxy localport server remoteport\n");
		taskexitall(EXIT_FAILURE);
	}
	server = argv[2];
	port = atoi(argv[3]);
	if ((fd = netannounce(TCP, 0, atoi(argv[1]))) < 0) {
		fprintf(stderr, "cannot announce on tcp port %d: %s\n",
			atoi(argv[1]), strerror(errno));
		taskexitall(EXIT_FAILURE);
	}
	fdnoblock(fd);
	while ((cfd = netaccept(fd, remote, &rport)) >= 0) {
		fprintf(stderr, "connection from %s:%d\n", remote, rport);
		taskcreate(proxytask, (void*)cfd, STACK);
	}
}
