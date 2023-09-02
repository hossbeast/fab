/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/socket.h>

void xbind(int sfd, const struct sockaddr *addr, socklen_t addr_len);
int uxbind(int socket, const struct sockaddr *address, socklen_t address_len);

int xsocket(int domain, int type, int protocol);

void xconnect(int sfd, const struct sockaddr *addr, socklen_t addrlen);
int uxconnect(int socket, const struct sockaddr *address, socklen_t address_len);

void xsetsockopt(int sfd, int level, int optname, const void *optval, socklen_t optlen);
int uxsetsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);

void xlisten(int sockfd, int backlog);
int uxlisten(int sockfd, int backlog);

void xgetsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
void xgetpeername(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
