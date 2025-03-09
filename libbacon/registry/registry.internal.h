/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "registry.h"

struct bacon_object;

void registry_setup(void);
void registry_cleanup(void);

const struct bacon_object *registry_lookup(uint32_t type);
const struct bacon_object *registry_get(uint32_t type);
