/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/attrs.h"
#include "command.h"
#include "bam/ipc.h"
#include "bam/client.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "special.h"
#include "info.h"
#include "params.h"

/* describe */
void special_describe_select(command * restrict cmd, bacon_writer * restrict builder_writer)
{
	{ MAPPING
		STRING("select")
		{ LIST
			if(cmd->id) {
				MAPPING
				STRING("lookup")
				{ SET
					{ MAPPING
						STRING("path")
						if(strlen(cmd->prefix)) {
							LOADF("%s/%016"PRIx64, cmd->prefix, cmd->id)
						} else {
							LOADF("//by-id/%016"PRIx64, cmd->id)
						}
					}
				}
			} else if(strlen(cmd->path)) {
				MAPPING
				STRING("lookup")
				{ SET
					{ MAPPING
						STRING("path")
						STRING(cmd->path)
					}
				}
			} else {
				RUNTIME_ASSERT(cmd->special_target.len);
				RUNTIME_ASSERT(cmd->visit);

				{ MAPPING
					STRING("lookup")
					{ SET
						{ MAPPING
							STRING("path")
							BYTES(cmd->special_target.s, cmd->special_target.len)
						}
					}
				}
			}
			/* limit to vertices of the matching type ; not really needed */
			if(cmd->visit) {
				MAPPING
				STRING("traverse")
				{ SET
					{ MAPPING
						STRING("direction")
						STRING("up")
					}
					{ MAPPING
						STRING("graph")
						STRING("special")
					}
					{ MAPPING
						STRING("visit")
						STRING(cmd->visit)
					}
					{ MAPPING
						STRING("min-distance")
						UINT(0)
					}
					{ MAPPING
						STRING("max-distance")
						UINT(0)
					}
				}
			}
		}
	}
}

void special_describe_connected(command * restrict cmd, bam_client * restrict client)
{
	bamipc_message * msg;
	bacon_writer writer, *builder_writer;
	narrator *N;
	narrator_fixed fixed;

	msg = bam_client_produce(client);
	msg->type = BAMIPC_MSG_REQUEST;
	cmd->request_id = msg->id = ++client->msgid;

	N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
	bacon_writer_init(&writer);
	builder_writer = bacon_writer_open(&writer, N);

	{ LIST
		command_context(cmd, builder_writer);
		special_describe_select(cmd, builder_writer);
		STRING("describe")
	}

	bacon_writer_close(&writer);
	bacon_writer_destroy(&writer);
	narrator_sayc(N, 0);
	narrator_sayc(N, 0);

	msg->size = fixed.l;
	client_post(client, msg);
}

void special_describe_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
	if(msg->id != cmd->request_id) {
		return;
	}
	if(msg->code != 0) {
		g_params.shutdown = true;
		return;
	}
	if(msg->type == BAMIPC_MSG_RESPONSE) {
		g_params.shutdown = true;
		return;
	}

	RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
	(void)!write(1, msg->text, msg->size);
	cmd->any = true;
}

void special_describe_disconnected(command * restrict cmd)
{
	if(cmd->any) {
		printf("\n");
	}
}

/* list */
void special_list_build(command * restrict cmd, bacon_writer * restrict builder_writer)
{
	{ LIST
		{ MAPPING
			STRING("select")
			{ LIST
				{ MAPPING
					STRING("lookup")
					{ SET
						{ MAPPING
							STRING("path")
							STRING(cmd->path)
						}
					}
				}
				{ MAPPING
					STRING("traverse")
					{ SET
						{ MAPPING
							STRING("nofollow")
							BOOL(false) // follow links
						}
						{ MAPPING
							STRING("direction")
							STRING("down")
						}
						{ MAPPING
							STRING("graph")
							STRING("fs")
						}
						{ MAPPING
							STRING("min-distance")
							UINT(1)
						}
						{ MAPPING
							STRING("max-distance")
							UINT(1)
						}
					}
				}
			}
		}
		STRING("list")
	}
}

void special_list_connected(command * restrict cmd, bam_client * restrict client)
{
	cmd->list_id = command_post(cmd, client, special_list_build);
}

void special_list_process(command * restrict cmd, bam_client * restrict client, bamipc_message * restrict msg)
{
	if(msg->id != cmd->list_id) {
		return;
	}

	if(msg->type == BAMIPC_MSG_RESPONSE) {
		g_params.shutdown = true;
		return;
	}

	RUNTIME_ASSERT(msg->type == BAMIPC_MSG_RESULT);
	(void)!write(1, msg->text, msg->size);
	cmd->any = true;
}

void special_list_disconnected(command * restrict cmd)
{
	if(cmd->any) {
		printf("\n");
	}
}
