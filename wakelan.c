/* 
 *
 * Copyright (C) 1998 by Christopher Chan-Nui
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#include "config.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <errno.h>

#ifndef DEFAULTMAC
#define DEFAULTMAC "00a0c9852a5f"
#endif
#ifndef DEFAULTTARGET
#define DEFAULTTARGET "255.255.255.255"
#endif

static char *rcsid="@(#) $Id: wakelan.c,v 1.8 1998/08/30 05:04:28 channui Exp $";
static void *use_rcsid = (void *)((char *)&use_rcsid || (void *)&rcsid);
char *versionid = "1.0";

void usage(char *name) {
    printf ("Usage: %s [options] [mac] [broadcast] [port]\n"
	    "    -b addr    broadcast address\n"
	    "    -m mac     mac address of host\n"
	    "    -p port    UDP port to broadcast to\n"
	    "    -v[v]      version\n"
	    , name);
    exit (0);
}

int parse_mac(unsigned char *mac, char *str) {
    int i;
    int count;
    char c;
    unsigned char val;
    int colon_ok = 1;
    for (i = 0; i < 6; i++) {
	mac[i] = 0;
    }
    for (i = 0; i < 6; i++) {
	count = 0;
	val   = 0;
	do {
	    c = toupper(*str++);
	    if (c >= '0' && c <= '9') {
		val = (val * 16) + (c - '0');
	    } else if (c >= 'A' && c <= 'F') {
		val = (val * 16) + (c - 'A') + 10;
	    } else if (c == ':') {
		if (colon_ok || count-- != 0)
		    break;
	    } else if (c == '\0') {
		str--;
		break;
	    } else {
		return 0;
	    }
	    colon_ok=1;
	} while (++count < 2);
	colon_ok=(count<2);
	*mac++ = val;
    }
    if (*str)
	return 0;
    return 1;
}

int main (int argc, char *argv[]) {
    int sock;
    int optval = 1;
    int version =0;
    int i, j, c, rc;
    char msg[1024];
    int  msglen = 0;
    struct sockaddr_in bcast;
    struct hostent *he;
    struct in_addr inaddr;
    unsigned char macaddr[6];
    char *mac    = DEFAULTMAC;
    char *target = DEFAULTTARGET;
    short bport = htons(32767);

    while ((c = getopt(argc, argv, "hvp:m:b:")) != EOF) {
	switch (c) {
	    case 'b': target = optarg;             break;
	    case 'm': mac    = optarg;             break;
	    case 'p': bport = htons(atoi(optarg)); break;
	    case 'v': version++;                   break;
	    case 'h':
	    case '?':
		usage(argv[0]);
	}
    }

    if (version) {
        printf ("Version: %s\n", versionid);
        if (version > 1) {
            printf ("  RCSID: %s\n", rcsid);
        }
        exit (0);
    }

    if (argv[optind] != NULL) {
	mac = argv[optind++];
    }
    if (argv[optind] != NULL) {
	target = argv[optind++];
    }
    if (argv[optind] != NULL) {
	bport = htons(atoi(argv[optind++]));
    }
    if (argv[optind] != NULL) {
	usage(argv[0]);
    }

    if (!parse_mac(macaddr, mac)) {
	printf ("Illegal MAC address '%s'\n", mac);
	exit (1);
    }

    if (!inet_aton(target, &inaddr)) {
	he = gethostbyname(target);
	inaddr = *(struct in_addr *)he->h_addr_list[0];
    }

    for (i = 0; i < 6; i++) {
	msg[msglen++] = 0xff;
    }
    for (i = 0; i < 16; i++) {
	for (j = 0; j < sizeof(macaddr); j++) {
	    msg[msglen++] = macaddr[j];
	}
    }

    memset(&bcast, 0, sizeof(bcast));
    bcast.sin_family      = AF_INET;
    bcast.sin_addr.s_addr = inaddr.s_addr;
    bcast.sin_port        = bport;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
	printf ("Can't allocate socket\n");
	exit (1);
    }
    if ((rc=setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval))) < 0) {
	printf ("Can't socket option SO_BROADCAST: rc = %d, errno=%s(%d)\n",
		rc, strerror(errno), errno);
	exit (1);
    }
    sendto(sock, &msg, msglen, 0, (struct sockaddr *)&bcast, sizeof(bcast));
    return 0;
}
