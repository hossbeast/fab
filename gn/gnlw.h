#ifndef _GNLW_H
#define _GNLW_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include <listwise.h>
#include <listwise/object.h>

#include "gn.h"

/* listwise interface for gn objects
**
**
*/

#define LISTWISE_TYPE_GNLW		0x01			/* listwise type id */

// listwise interface to gn objects
listwise_object gnlw;

#endif
