/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"

#include "xsocket.h"

void xbind(int sfd, const struct sockaddr *addr, socklen_t addr_len)
{
  int __attribute__((unused)) r;

  r = bind(sfd, addr, addr_len);
  RUNTIME_ASSERT(r == 0);
}

int uxbind(int socket, const struct sockaddr *address, socklen_t address_len)
{
  int r;

  if((r = bind(socket, address, address_len)) != 0) {
    RUNTIME_ASSERT(errno == EADDRINUSE || errno == EADDRNOTAVAIL);
  }

  return r;
}

int xsocket(int domain, int type, int protocol)
{
  int r;

  r = socket(domain, type, protocol);
  RUNTIME_ASSERT(r != -1);

  return r;
}

void xsetsockopt(int sfd, int level, int optname, const void *optval, socklen_t optlen)
{
  int __attribute__((unused)) r;

  r = setsockopt(sfd, level, optname, optval, optlen);
  RUNTIME_ASSERT(r == 0);
}

int uxsetsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
  int r;

  if((r = setsockopt(socket, level, option_name, option_value, option_len)) != 0) {
    RUNTIME_ASSERT(errno == EINVAL);
  }

  return r;
}

void xconnect(int sfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int __attribute__((unused)) r;

  r = connect(sfd, addr, addrlen);
  RUNTIME_ASSERT(r == 0);
}

int uxconnect(int socket, const struct sockaddr *address, socklen_t address_len)
{
  int r;

  if((r = connect(socket, address, address_len)) != 0) {
    RUNTIME_ASSERT(errno == EADDRINUSE || errno == EADDRNOTAVAIL || errno == ENETUNREACH || errno == EINVAL || errno == ECONNREFUSED);
  }

  return r;
}

void xlisten(int sockfd, int backlog)
{
  int __attribute__((unused)) r;

  r = listen(sockfd, backlog);
  RUNTIME_ASSERT(r == 0);
}

int uxlisten(int sockfd, int backlog)
{
  int r;

  if((r = listen(sockfd, backlog)) == 0) {
    return 0;
  }
  RUNTIME_ASSERT(errno == EADDRINUSE);

  return r;
}

void xgetsockname(int sockfd, struct sockaddr * restrict addr, socklen_t * restrict addrlen)
{
  int r;
  r = getsockname(sockfd, addr, addrlen);
  RUNTIME_ASSERT(r == 0);
}

void xgetpeername(int sockfd, struct sockaddr *restrict addr, socklen_t * restrict addrlen)
{
  int r;
  r = getpeername(sockfd, addr, addrlen);
  RUNTIME_ASSERT(r == 0);
}
