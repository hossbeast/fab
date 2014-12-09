/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "xlinux.h"
#include "xlinux/mempolicy.h"

#include "global.h"

#include "macros.h"
#include "memblk.h"

// signal handling
static void signal_handler(int signum, siginfo_t * info, void * ctx)
{
	printf("fab[%u] %d { pid : %ld", getpid(), signum, (long)info->si_pid);
	if(signum == SIGCHLD)
		printf(", exit : %d, signal : %d", WEXITSTATUS(info->si_status), WIFSIGNALED(info->si_status) ? WSTOPSIG(info->si_status) : 0);
	printf(" }\n");
}

static int fabdcred_throw(uid_t actual_uid, gid_t actual_gid)
{
	char space[256];
	char space2[256];
	char space3[256];

	struct passwd pwd;
	struct passwd * ppwd;
	fatal(uxgetpwuid_r, actual_uid, &pwd, space, sizeof(space), &ppwd);

	struct group grp;
	struct group * pgrp;
	fatal(uxgetgrgid_r, actual_gid, &grp, space2, sizeof(space2), &pgrp);

	fail(FAB_FABDCRED);

finally:
	if(XAPI_ERRCODE == FAB_FABDCRED)
	{
		snprintf(space3, sizeof(space3), "%.*s/%d:%.*s/%d"
			, g_params.ruid_namel, g_params.ruid_name, g_params.ruid
			, g_params.rgid_namel, g_params.rgid_name, g_params.rgid
		);
		XAPI_INFOS("expected", space3);

		if(ppwd && pgrp)
			snprintf(space3, sizeof(space3), "%s/%d:%s/%d", ppwd->pw_name, actual_uid, pgrp->gr_name, actual_gid);
		else if(ppwd)
			snprintf(space3, sizeof(space3), "%s/%d:%d", ppwd->pw_name, actual_uid, actual_gid);
		else if(pgrp)
			snprintf(space3, sizeof(space3), "%d:%s/%d", actual_uid, pgrp->gr_name, actual_gid);

		XAPI_INFOS("actual", space3);
	}
coda;
}

int main(int argc, char** argv)
{
	char space[2048];
	char space2[2048];

	int mode_backtrace = DEFAULT_MODE_BACKTRACE;
	int fd = -1;
	int lockfd = -1;
	uint32_t canhash;
	int r = -1;

	memblk * mb = 0;

printf("fab[%ld]\n", (long)getpid());

	int x;
	size_t tracesz = 0;

	// initialize error tables
	error_setup();

	// process parameter gathering
	fatal(params_setup);

	// get user identity of this process, assert user:group and permissions are set appropriately
	fatal(identity_init);
	
	// assume identity of the executing user
	fatal(identity_assume_user);

	// unblock all signals
	sigset_t all;
	sigfillset(&all);
	sigprocmask(SIG_UNBLOCK, &all, 0);

	// handle all signals
	struct sigaction action = {
		  .sa_sigaction = signal_handler
		, .sa_flags = SA_SIGINFO
	};
	for(x = 1; x < SIGUNUSED; x++)
	{
		if(x != SIGKILL && x != SIGSTOP && x != SIGSEGV)
			fatal(xsigaction, x, &action, 0);
	}
	for(x = SIGRTMIN; x <= SIGRTMAX; x++)
		fatal(xsigaction, x, &action, 0);

	// initalize logger
	fatal(log_init);

	// allocate memblock to store the args
	fatal(memblk_mk, &mb, ARGS_MAX_SIZE);

	// redirect allocations to the memblock
	mempolicy_engage(memblk_getpolicy(mb));

	// parse cmdline arguments
	fatal(args_parse, mb);
	mempolicy_release(memblk_getpolicy(mb));

	mode_backtrace = g_args->mode_backtrace;
	canhash = g_args->init_fabfile_path->can_hash;

/*
/FABIPCDIR/<hash>/fabfile				<-- init fabfile path, symlink
/FABIPCDIR/<hash>/args					<-- args, binary
/FABIPCDIR/<hash>/logs					<-- logs, ascii
/FABIPCDIR/<hash>/fab/pid				<-- fab pid, ascii
/FABIPCDIR/<hash>/fab/lock			<-- fab lockfile
/FABIPCDIR/<hash>/fabd/pgid			<-- fabd pgid, ascii
/FABIPCDIR/<hash>/fabd/lock			<-- fabd lockfile
/FABIPCDIR/<hash>/fabd/lock			<-- fabd lockfile
/FABIPCDIR/<hash>/fabd/cred			<-- fabd ruid/rgid
/FABIPCDIR/<hash>/fabd/exit			<-- fabd exit status
*/

	// fabsys identity
	fatal(identity_assume_fabsys);

	// ipc-dir stem
	size_t z = snprintf(space, sizeof(space), "/%s/%u", XQUOTE(FABIPCDIR), canhash);

	// fab directory
	snprintf(space + z, sizeof(space) - z, "/fab");
	fatal(mkdirp, space, S_IRWXU | S_IRWXG);

	// canonical fabfile symlink
	snprintf(space + z, sizeof(space) - z, "/fabfile");
	fatal(uxsymlink, g_args->init_fabfile_path->can, space);

	// fab-lock file
	snprintf(space + z, sizeof(space) - z, "/fab/lock");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &lockfd);
	fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

	// fab-pid file
	snprintf(space + z, sizeof(space) - z, "/fab/pid");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
	fatal(axwrite, fd, &g_params.pid, sizeof(g_params.pid));
	fatal(ixclose, &fd);

	// fabd directory
	snprintf(space + z, sizeof(space) - z, "/fabd");
	fatal(mkdirp, space, S_IRWXU | S_IRWXG);

	// open args file for writing
	snprintf(space + z, sizeof(space) - z, "/args");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);

	// set the filesize
	fatal(xftruncate, fd, mb->l);

	// write the args : g_args unusable beyond this point
	args_freeze(mb->s);
	fatal(axwrite, fd, mb->s, mb->l);
	fatal(ixclose, &fd);

	// open logs file for writing
	snprintf(space + z, sizeof(space) - z, "/logs");
	fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);

	// set the filesize
	fatal(xftruncate, fd, g_logvsl);

	// write the logs : g_logs unusable beyond this point
	fatal(axwrite, fd, g_logvs, g_logvsl);
	fatal(ixclose, &fd);

	// fabd-pgid file
	pid_t fabd_pgid;
	snprintf(space + z, sizeof(space) - z, "/fabd/pgid");

	// terminate extant fabd process, if any
	if(g_args->invalidationsz)
	{
		// fabd-pgid file
		fatal(gxopen, space, O_RDONLY, &fd);

		if(fd != -1)
		{
			// read and validate the pgid
			fatal(axread, fd, &fabd_pgid, sizeof(fabd_pgid));

			if(fabd_pgid <= 0)
				failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)fabd_pgid);

			// kill signal
			fatal(xkill, -fabd_pgid, 15);

			// wait for it to terminate ; signal from fabw will interrupt sleep
			r = 0;
			while(r == 0)
			{
				sleep(1);
				fatal(uxkill, -fabd_pgid, 0, &r);
			}
		}
	}

	fatal(ixclose, &fd);
	fatal(gxopen, space, O_RDONLY, &fd);

	r = -1;
	if(fd != -1)
	{
		// read and validate the pgid
		fatal(axread, fd, &fabd_pgid, sizeof(fabd_pgid));

		if(fabd_pgid <= 0)
			failf(FAB_BADIPC, "expected pgid > 0, actual : %ld", (long)fabd_pgid);

		// existence check
		fatal(uxkill, -fabd_pgid, 0, &r);

		if(r == 0)
		{
			// open and read fabd-cred file
			snprintf(space + z, sizeof(space) - z, "/fabd/cred");
			fatal(ixclose, &fd);
			fatal(xopen, space, O_RDONLY, &fd);
			typeof(g_params.ruid) fabd_ruid;
			typeof(g_params.rgid) fabd_rgid;
			fatal(axreadv, fd, (struct iovec[]) {{ .iov_base = &fabd_ruid, .iov_len = sizeof(fabd_ruid) }, { .iov_base = &fabd_rgid, .iov_len = sizeof(fabd_rgid) }}, 2);

			// assert credentials equality
			if(fabd_ruid != g_params.ruid || fabd_rgid != g_params.rgid)
				fatal(fabdcred_throw, fabd_ruid, fabd_rgid);
		}
	}

	if(r)
	{
		// fabd is not running
		snprintf(space + z, sizeof(space) - z, "/fabd/exit");
		fatal(uxunlink, space, 0);

		fatal(xfork, &fabd_pgid);
		if(fabd_pgid == 0)
		{
			// fabd-pgid file
			snprintf(space + z, sizeof(space) - z, "/fabd/pgid");
			fatal(ixclose, &fd);
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
			fatal(axwrite, fd, (pid_t[]) { getpid() }, sizeof(pid_t));
			fatal(ixclose, &fd);

			// fabd-lock file
			snprintf(space + z, sizeof(space) - z, "/fabd/lock");
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &lockfd);
			fatal(xflock, lockfd, LOCK_EX | LOCK_NB);

			// fabd-cred file
			snprintf(space + z, sizeof(space) - z, "/fabd/cred");
			fatal(xopen_mode, space, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP, &fd);
			fatal(axwritev, fd, (struct iovec[]) {{ .iov_base = &g_params.ruid, .iov_len = sizeof(g_params.ruid) }, { .iov_base = &g_params.rgid, .iov_len = sizeof(g_params.rgid) }}, 2);
			fatal(ixclose, &fd);

			// files done : reassume user identity
			fatal(identity_assume_user);

			// new process group
			fatal(xsetpgid, 0, 0);

			// fabd args
			z = snprintf(space, sizeof(space), "%u", canhash);

#if DEVEL
			snprintf(space2, sizeof(space2), "%s/../fabw/fabw.devel", g_params.exedir);
			execl(space2, space2, space, (void*)0);
#else
			execlp("fabw", "fabw", space, (void*)0);
#endif

			tfail(perrtab_SYS, errno);
		}
	}
	else
	{
		// files done : reassume user identity
		fatal(identity_assume_user);

		// awaken
		fatal(xkill, -fabd_pgid, 1);
	}
	
	// wait to hear back from fabd
	pause();

	fatal(uxkill, -fabd_pgid, 0, &r);
	if(r == 0)
	{
		printf("fabd[%ld] still running : %d\n", (long)fabd_pgid, r);
	}
	else
	{
		// fabd-exit file
		snprintf(space + z, sizeof(space) - z, "/fabd/exit");
		fatal(ixclose, &fd);
		fatal(gxopen, space, O_RDONLY, &fd);
		int fabd_exit;
		fatal(axread, fd, &fabd_exit, sizeof(fabd_exit));

		printf("fabd status : %d\n", WEXITSTATUS(fabd_exit));
		printf("fabd signal : %d\n", WTERMSIG(fabd_exit));
	}

#if 0
	// summarize arguments as received
	fatal(args_summarize);

	// register object types with liblistwise
	fatal(listwise_register_object, LISTWISE_TYPE_GNLW, &gnlw);
	fatal(listwise_register_object, LISTWISE_TYPE_LIST, &listlw);

	// load additional fab-specific listwise operators
	fatal(listwise_register_opdir, XQUOTE(FABLWOPDIR));

#if DEBUG || DEVEL
	// configure liblistwise logging
	lw_configure_logging();
#endif

	// other initializations
	fatal(tmp_setup);
	fatal(gn_init);
	fatal(traverse_init);
	fatal(ff_mkparser, &ffp);

	// create the rootmap
	fatal(var_root, &rmap);

	// parse variable expression text from cmdline (the -v option)
	for(x = 0; x < g_args->rootvarsl; x++)
	{
		ff_file * vff = 0;
		fatal(ff_var_parse, ffp, g_args->rootvars[x], strlen(g_args->rootvars[x]), x, &vff);

		// process variable expressions
		int k;
		for(k = 0; k < vff->ffn->statementsl; k++)
		{
			ff_node * stmt = vff->ffn->statements[k];

			if(stmt->type == FFN_VARASSIGN || stmt->type == FFN_VARXFM_ADD || stmt->type == FFN_VARXFM_SUB)
			{
				int pn = staxp;
				fatal(list_resolve, stmt->definition, rmap, ffp->gp, &stax, &staxa, &staxp, 0, 0);
				staxp++;

				for(y = 0; y < stmt->varsl; y++)
				{
					if(stmt->type == FFN_VARASSIGN)
					{
						fatal(var_set, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, 0, stmt);
					}
					else if(stmt->type == FFN_VARXFM_ADD)
					{
						fatal(var_xfm_add, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, stmt);
					}
					else if(stmt->type == FFN_VARXFM_SUB)
					{
						fatal(var_xfm_sub, rmap, stmt->vars[y]->name->text->s, stax[pn], 1, stmt);
					}
				}
			}
			else if(stmt->type == FFN_VARXFM_LW)
			{
				for(y = 0; y < stmt->varsl; y++)
				{
					if(stmt->transform_node)
					{
						fatal(var_xfm_lw, rmap, stmt->vars[y]->name->text->s, stmt->transform_node->transform, stmt->transform_node->text->s, 1, stmt);
					}
					else if(stmt->transform_list_node)
					{

					}
				}
			}
		}
	}

	// use up one list and populate the # variable (relative directory path to the initial fabfile)
	fatal(lw_reset, &stax, &staxa, staxp);
	fatal(lstack_addw, stax[staxp], g_args->init_fabfile_path->abs_dir, g_args->init_fabfile_path->abs_dirl);
	fatal(var_set, rmap, "#", stax[staxp++], 1, 0, 0);

	// vmap for the initial fabfile is the first (and only) direct descendant of rootmap
	fatal(var_clone, rmap, &vmap);

	// parse, starting with the initial fabfile, construct the graph
	fatal(ffproc, ffp, g_args->init_fabfile_path, vmap, &stax, &staxa, &staxp, &first);

///
/// daemonize
///

	// process hashblocks for regular fabfiles that have changed
	fatal(ff_regular_reconcile);

	// compute flags and designations, reload files and dscv cache
	fatal(gn_finalize, 0);

	// comprehensive upfront dependency discovery on the entire graph
	fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
	fatal(gn_reconcile);
	fatal(gn_finalize, 1);

	// process selectors
	if(g_args->selectorsl)
	{
		fatal(selector_init);

		int pn = staxp;

		// create $!
		fatal(lw_reset, &stax, &staxa, pn);
		for(x = 0; x < gn_nodes.l; x++)
			fatal(lstack_obj_add, stax[pn], gn_nodes.e[x], LISTWISE_TYPE_GNLW);

		// map for processing selectors
		fatal(map_create, &smap, 0);
		fatal(map_set, smap, MMS("!"), MM(stax[pn]), 0);
		pn++;

		if(g_args->selectors_arequery)
			fatal(log_parse_and_describe, "+SELECT", 0, 0, L_INFO);

		// process selectors
		for(x = 0; x < g_args->selectorsl; x++)
			fatal(selector_process, &g_args->selectors[x], x, ffp, smap, &stax, &staxa, pn);

		if(g_args->selectors_arequery == 0)
		{
			fatal(selector_finalize
				, &fabrications, &fabricationsl
				, &fabricationxs, &fabricationxsl
				, &fabricationns, &fabricationnsl
				, &invalidations, &invalidationsl
				, &discoveries, &discoveriesl
				, &inspections, &inspectionsl
			);

			if(log_would(L_LISTS))
			{
				if(fabricationsl + fabricationxsl + fabricationnsl + invalidationsl + discoveriesl + inspectionsl == 0)
				{
					logf(L_LISTS, "empty");
				}
				
				for(x = 0; x < fabricationsl; x++)
					logf(L_LISTS, "fabrication(s)     =%s", (*fabrications[x])->idstring);

				for(x = 0; x < fabricationxsl; x++)
					logf(L_LISTS, "fabricationx(s)    =%s", (*fabricationxs[x])->idstring);

				for(x = 0; x < fabricationnsl; x++)
					logf(L_LISTS, "fabricationn(s)    =%s", (*fabricationns[x])->idstring);

				for(x = 0; x < invalidationsl; x++)
					logf(L_LISTS, "invalidation(s)    =%s", (*invalidations[x])->idstring);

				for(x = 0; x < discoveriesl; x++)
					logf(L_LISTS, "discover(y)(ies)   =%s", (*discoveries[x])->idstring);

				for(x = 0; x < inspectionsl; x++)
					logf(L_LISTS, "inspection(s)      =%s", (*inspections[x])->idstring);
			}
		}
	}

	if(g_args->selectors_arequery == 0)
	{
		// dependency discovery list
		if(discoveriesl)
		{
			fatal(log_parse_and_describe, "+DSC", 0, 0, L_INFO);
			fatal(dsc_exec_specific, discoveries, discoveriesl, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
		}
		else
		{
			// process invalidations
			fatal(gn_process_invalidations, invalidations, invalidationsl);
			fatal(gn_finalize, 1);

			// dependency discovery on invalidated primary nodes
			fatal(dsc_exec_entire, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);
			fatal(gn_finalize, 1);

			// process inspections
			if(inspectionsl)
			{
				// enable DGRAPH
				fatal(log_parse_and_describe, "+DGRAPH", 0, 0, L_INFO);

				for(x = 0; x < inspectionsl; x++)
					gn_dump((*inspections[x]));
			}
			else
			{
				//
				// construct a buildplan - use the first node if none was specified
				//
				if(fabricationsl + fabricationxsl + fabricationnsl == 0 && first)
				{
					fatal(xrealloc, &fabrications, sizeof(*fabrications), 1, 0);
					fabrications[fabricationsl++] = &first;
				}

				// mixing task and non-task as buildplan targets does not make sense
				for(x = 1; x < fabricationsl + fabricationxsl + fabricationnsl; x++)
				{
					int a = x - 1;
					int b = x;
					gn * A = 0;
					gn * B = 0;

					if(a < fabricationsl)
						A = *fabrications[a];
					else if(a < (fabricationsl + fabricationxsl))
						A = *fabricationxs[a - fabricationsl];
					else
						A = *fabricationns[a - fabricationsl - fabricationxsl];

					if(b < fabricationsl)
						B = *fabrications[b];
					else if(b < (fabricationsl + fabricationxsl))
						B = *fabricationxs[b - fabricationsl];
					else
						B = *fabricationns[b - fabricationsl - fabricationxsl];

					if((A->designate == GN_DESIGNATION_TASK) ^ (B->designate == GN_DESIGNATION_TASK))
						fails(FAB_BADPLAN, "cannot mix task and non-task targets");
				}

				fatal(bp_create, fabrications, fabricationsl, fabricationxs, fabricationxsl, fabricationns, fabricationnsl, &bp);

				// selector lists not referenced again past this point
				map_xfree(&smap);

				if(bp)
				{
					if(g_args->mode_bplan == MODE_BPLAN_BAKE)
					{
						// prepare bakevars map
						fatal(map_create, &bakemap, 0);

						for(x = 0; x < g_args->bakevarsl; x++)
							fatal(map_set, bakemap, MMS(g_args->bakevars[x]), 0, 0, 0);

						// dump buildplan, pending logging
						if(bp)
							bp_dump(bp);

						// create bakescript
						fatal(bake_bp, bp, vmap, ffp->gp, &stax, &staxa, staxp, bakemap, &ts, &tsa, &tsw, g_args->bakescript_path);
					}
					else
					{
						// incremental build - prune the buildplan
						fatal(bp_eval, bp);

						if(g_args->mode_bplan == MODE_BPLAN_GENERATE)
							fatal(log_parse_and_describe, "+BPDUMP", 0, 0, L_INFO);

						// dump buildplan, pending logging
						if(bp)
							bp_dump(bp);

						if(g_args->mode_bplan == MODE_BPLAN_EXEC)
						{
							if(bp && bp->stages_l)
							{
								// execute the build plan, one stage at a time
								fatal(bp_exec, bp, vmap, ffp->gp, &stax, &staxa, staxp, &ts, &tsa, &tsw);

								// commit regular fabfile hashblocks
								for(x = 0; x < ff_files.l; x++)
								{
									if(ff_files.e[x]->type == FFT_REGULAR)
									{
										fatal(hashblock_write, ff_files.e[x]->hb);
									}
								}
							}
						}
					}
				}
			}
		}
	}

#endif

finally:
	fatal(ixclose, &fd);
	fatal(ixclose, &lockfd);

	if(XAPI_UNWINDING)
	{
#if DEBUG || DEVEL
		if(mode_backtrace == MODE_BACKTRACE_PITHY)
		{
#endif
			tracesz = xapi_trace_pithy(space, sizeof(space));
#if DEBUG || DEVEL
		}
		else
		{
			tracesz = xapi_trace_full(space, sizeof(space));
		}
#endif

		logw(L_RED, space, tracesz);
	}
	else
	{
		logs(L_INFO, "exiting with status : 0");
	}

	log_teardown();
	memblk_free(mb);
coda;
}
