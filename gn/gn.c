#include <unistd.h>
#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <dirent.h>

#include "gn.h"
#include "fml.h"
#include "args.h"
#include "identity.h"
#include "traverse.h"

#include "log.h"
#include "control.h"
#include "xmem.h"
#include "unitstring.h"
#include "canon.h"
#include "xstring.h"
#include "dirutil.h"
#include "map.h"
#include "cksum.h"
#include "parseint.h"

#define restrict __restrict

// avoid circular dependency on ff
char * ff_idstring(struct ff_file * const);

//
// data
//

union gn_nodes_t gn_nodes = { { .size = sizeof(gn) } };

//
// static
//

static void freenode(gn * const gn)
{
	if(gn)
	{
		path_free(gn->path);
		free(gn->idstring);

		hashblock_free(gn->primary_hb);
		depblock_free(gn->dscv_block);

		free(gn->dscvs);

		int x;
		for(x = 0; x < gn->needs.l; x++)
			free(gn->needs.e[x]);

		free(gn->needs.e);
		map_free(gn->needs.by_B);

		free(gn->feeds.e);
		map_free(gn->feeds.by_A);

		free(gn->closure_ffs);

		free(gn->noforce_dir);
		free(gn->noforce_ff_path);
		free(gn->noforce_gn_path);
	}

	free(gn);
}

/// lookup
//
// SUMMARY
//  lookup a gn by absolute path, relative path, or NOFILE-name
//
// PARAMETERS
//  [base] - directory path for resolving relative paths
//  [sstk] - stringstack for resolving nofile paths
//  iscan  - skip canonicalization (path is already known to be canonicalized)
//  s      - string
//  l      - string length, or 0 for strlen
//  r      - node returned here, if found
//
// RETURNS
//  0 on failure (ENOMEN) and 1 otherwise
//
static int lookup(const char * const restrict base, strstack * const restrict sstk, const char * const restrict s, int l, gn ** r)
	__attribute__((nonnull(3)));

static int lookup(const char * const base, strstack * const sstk, const char * const s, int l, gn ** r)
{
	char space[512];									// space for canonicalization
	const char * canp	= s;						// canonical path for lookup
	int canpl		= l ?: strlen(s);			// canp length

	if(canpl > 4 && memcmp(canp, "/../", 4) == 0 && sstk)
	{
		char * sstr = 0;
		fatal(strstack_string, sstk, "/", "/", &sstr);
		fatal(canon, canp + 4, canpl - 4, space, sizeof(space), sstr, CAN_REALPATH);

		canp = space;
		canpl = strlen(canp);
	}
	else if(base)
	{
		fatal(canon, canp, canpl, space, sizeof(space), base, CAN_REALPATH);

		canp = space;
		canpl = strlen(canp);
	}

	gn ** R = 0;
	if((R = map_get(gn_nodes.by_path, canp, canpl)))
		(*r) = *R;

	finally : coda;
}

///
/// public
///

int gn_match(const char * const base, const char * const s, gn *** const r, int * const rl, int * const ra)
{
	int add(gn * e)
	{
		if((*rl) == (*ra))
		{
			int ns = (*ra) ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, r, sizeof(**r), ns, *ra);
			*ra = ns;
		}
		(*r)[(*rl)++] = e;

		finally : coda;
	};

	if(!gn_nodes.by_path)
	{
		fatal(map_create, &gn_nodes.by_path, 0);
		fatal(map_create, &gn_nodes.by_pathhash, 0);
	}

	char can[512];

	int sl = strlen(s);
	if(s[0] == '/' && s[sl - 1] == '/')
	{
		// regex match (substring for now)
		int x;
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(xstrstr(gn_nodes.e[x]->path->can, 0, s + 1, sl - 2, 0) || xstrstr(gn_nodes.e[x]->path->abs, 0, s + 1, sl - 2, 0) || xstrstr(gn_nodes.e[x]->path->rel, 0, s + 1, sl - 2, 0))
				fatal(add, gn_nodes.e[x]);
		}
	}
	else if(s[0] == '@')
	{
		// nofile
		int d = 0;
		d += snprintf(can + d, sizeof(can) - d, "/..");

		const char * p[2] = { [0] = s + 1 };
		while((p[1] = strstr(p[0], ".")))
		{
			d += snprintf(can + d, sizeof(can) - d, "/%.*s", (int)(p[1] - p[0]), p[0]);
			p[0] = p[1] + 1;
		}

		d += snprintf(can + d, sizeof(can) - d, "/%s", p[0]);

		gn ** R = 0;
		if((R = map_get(gn_nodes.by_path, can, strlen(can))))
			fatal(add, *R);
	}
	else
	{
		// relative to base, or canonical path
		if(canon(s, 0, can, sizeof(can), base, CAN_REALPATH) == 0)
			return 0;

		gn ** R = 0;
		if((R = map_get(gn_nodes.by_path, can, strlen(can))))
			fatal(add, *R);
	}

	finally : coda;
}

int gn_add(const char * const restrict base, strstack * const restrict sstk, char * const restrict A, int Al, gn ** gna, int * const restrict new)
{
	if(gn_nodes.by_path)
		fatal(lookup, base, sstk, A, Al, gna);
	else
	{
		fatal(map_create, &gn_nodes.by_path, 0);
		fatal(map_create, &gn_nodes.by_pathhash, 0);
	}

	if((*gna) == 0)
	{
		fatal(coll_doubly_add, &gn_nodes.c, 0, gna);

		Al = Al ?: strlen(A);

		// populate gna
		if(Al > 4 && memcmp(A, "/../", 4) == 0 && sstk)
		{
			char * sstr;
			fatal(strstack_string, sstk, "/", "/", &sstr);
			fatal(path_create_canon, &(*gna)->path, "%s/%.*s", sstr, Al - 4, A + 4);
		}
		else if(base)
		{
			fatal(path_create, &(*gna)->path, base, "%.*s", Al, A);
		}
		else
		{
			fatal(path_create_canon, &(*gna)->path, "%.*s", Al, A);
		}

		(*gna)->needs.z		= sizeof((*gna)->needs.e[0]);
		(*gna)->feeds.z		= sizeof((*gna)->feeds.e[0]);

		// idstring
		if(g_args.mode_gnid == MODE_GNID_RELATIVE)
		{
			(*gna)->idstring = strdup((*gna)->path->rel);
		}
		else if(g_args.mode_gnid == MODE_GNID_CANON)
		{
			(*gna)->idstring = strdup((*gna)->path->can);
		}
		(*gna)->idstringl = strlen((*gna)->idstring);

		map_set(gn_nodes.by_path, (*gna)->path->can, (*gna)->path->canl, gna, sizeof(*gna));
		map_set(gn_nodes.by_pathhash, MM((*gna)->path->can_hash), gna, sizeof(*gna));

		if(new)
			(*new)++;
	}

	finally : coda;
}

int gn_edge_add(
	  char * const base
	, strstack * const sstk
	, void ** const A, int Al, int At
	, void ** const B, int Bl, int Bt
	, struct ff_node * const reg_ffn
	, gn * const dscv_gn
	, int isweak
	, int * const newa
	, int * const newb
	, int * const newr
)
{
	gn *	gna = 0;
	gn *	gnb = 0;

	int _newa = 0;
	int _newb = 0;

	if(At)
		gna = *(gn**)A;
	else
		fatal(gn_add, base, sstk, *(char**)A, Al, &gna, &_newa);

	if(Bt)
		gnb = *(gn**)B;
	else
		fatal(gn_add, base, sstk, *(char**)B, Bl, &gnb, &_newb);

	if(gna == gnb)
	{
		// as syntactic sugar, silently ignore dependencies of a node on itself
	}
	else
	{
		if(newa)
			(*newa) += _newa;
		if(newb)
			(*newb) += _newb;

		// error check
		if(gnb->path->is_nofile && !gna->path->is_nofile)
			fail("file-backed node %s may not depend on non-file-backed node %s", gna->path->can, gnb->path->can);

		relation * rel = 0;

		if(!gna->needs.by_B)
			fatal(map_create, &gna->needs.by_B, 0);
		
		if(!gnb->feeds.by_A)
			fatal(map_create, &gnb->feeds.by_A, 0);

		if((rel = map_get(gna->needs.by_B, &gnb, sizeof(gnb))) == 0)
		{
			fatal(xmalloc, &rel, sizeof(*rel));

			fatal(coll_doubly_add, &gna->needs.c, &rel, 0);
			fatal(coll_doubly_add, &gnb->feeds.c, &rel, 0);

			fatal(map_set, gna->needs.by_B, &gnb, sizeof(gnb), &rel, sizeof(rel));
			fatal(map_set, gnb->feeds.by_A, &gna, sizeof(gna), &rel, sizeof(rel));

			rel->A = gna;
			rel->B = gnb;
			rel->weak = isweak;

			if(reg_ffn)
			{
				rel->type = GN_RELATION_REGULAR;
				rel->ffn = reg_ffn;
			}
			else if(dscv_gn)
			{
				rel->type = GN_RELATION_CACHED;
				rel->dscv_gn = dscv_gn;
			}

			if(newr)
				(*newr)++;
		}
	}

	*A = gna;
	*B = gnb;

	finally : coda;
}

char* gn_designate(gn * gn)
{
	gn->flags = 0;

	if(gn->path->is_nofile)
		gn->flags |= GN_FLAGS_NOFILE;

	if(gn->needs.l)
		gn->flags |= GN_FLAGS_HASNEED;

	if(gn->fabv)
		gn->flags |= GN_FLAGS_CANFAB;
	
	if(gn->flags & GN_FLAGS_CANFAB)
	{
		if(gn->flags & GN_FLAGS_NOFILE)
			gn->designation = GN_DESIGNATION_TASK;
		else if(gn->flags & GN_FLAGS_HASNEED)
			gn->designation = GN_DESIGNATION_SECONDARY;
		else
			gn->designation = GN_DESIGNATION_GENERATED;
	}
	else if(gn->flags & GN_FLAGS_NOFILE)
		gn->designation = GN_DESIGNATION_NOFILE;
	else if(gn->flags & GN_FLAGS_HASNEED)
	{
		// but there's no fmlv, which is a warning
		gn->designation = GN_DESIGNATION_SECONDARY;
	}
	else
		gn->designation = GN_DESIGNATION_PRIMARY;

	return GN_DESIGNATION_STR(gn->designation);
}

int gn_secondary_reload(gn * const gn)
{
	if(gn->noforce_dir == 0)
	{
		fatal(xsprintf, &gn->noforce_dir
			, CACHEDIR_BASE "/INIT/%u/gn/%u/SECONDARY/fab"
			, g_args.init_fabfile_path->can_hash
			, gn->path->can_hash
		);
		fatal(xsprintf, &gn->noforce_ff_path, "%s/noforce_ff", gn->noforce_dir);
		fatal(xsprintf, &gn->noforce_gn_path, "%s/noforce_gn", gn->noforce_dir);

		fatal(identity_assume_fabsys);

		if(euidaccess(gn->path->can, F_OK) == 0)
		{
			gn->fab_exists = 1;
		}
		else if(errno != ENOENT)
		{
			fail("access(%s)=[%d][%s]", gn->path->can, errno, strerror(errno));
		}

		gn->fab_force_ff = 1;
		if(euidaccess(gn->noforce_ff_path, F_OK) == 0)
		{
			gn->fab_force_ff = 0;
		}
		else if(errno != ENOENT)
		{
			fail("access(%s)=[%d][%s]", gn->noforce_ff_path, errno, strerror(errno));
		}

		gn->fab_force_gn = 1;
		if(euidaccess(gn->noforce_gn_path, F_OK) == 0)
		{
			gn->fab_force_gn = 0;
		}
		else if(errno != ENOENT)
		{
			fail("access(%s)=[%d][%s]", gn->noforce_gn_path, errno, strerror(errno));
		}

		fatal(identity_assume_user);
	}

	finally : coda;
}

int gn_secondary_rewrite_fab(gn * const gn, map * const ws)
{
	char tmp[3][512];
	uint32_t ** needs = 0;
	int needsl = 0;

	int fd = 0;
	struct timespec times[2] = { { .tv_nsec = UTIME_NOW } , { .tv_nsec = UTIME_NOW } };

	DIR * dh = 0;

	// create noforce dir
	fatal(identity_assume_fabsys);
	fatal(mkdirp, gn->noforce_dir, S_IRWXU | S_IRWXG | S_IRWXO);

	if((fd = open(gn->noforce_ff_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		fail("open(%s)=[%d][%s]", gn->noforce_ff_path, errno, strerror(errno));
	fatal_os(futimens, fd, times);
	fatal_os(close, fd);
	fd = 0;

	if((fd = open(gn->noforce_gn_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		fail("open(%s)=[%d][%s]", gn->noforce_gn_path, errno, strerror(errno));
	fatal_os(futimens, fd, times);
	fatal_os(close, fd);
	fd = 0;

	// construct aneed_primary_skipweak dir for this node
	snprintf(tmp[2], sizeof(tmp[2])
		, CACHEDIR_BASE "/INIT/%u/gn/%u/SECONDARY/aneed_primary_skipweak"
		, g_args.init_fabfile_path->can_hash
		, gn->path->can_hash
	);

	// ensure the directory exists
	fatal(mkdirp, tmp[2], S_IRWXU | S_IRWXG | S_IRWXO);

	// construct a map of PRIMARY needs for this node
	int logic(struct gn * n, int d)
	{
		if(n->designation == GN_DESIGNATION_PRIMARY)
			fatal(map_set, ws, MM(n->path->can_hash), MM(n));

		finally : coda;
	};

	map_clear(ws);
	fatal(traverse_depth_bynodes_needsward_skipweak, gn, logic);

	// delete existing links which no longer apply
	if((dh = opendir(tmp[2])) == 0)
		fail("opendir(%s)=[%d][%s]", tmp[2], errno, strerror(errno));

	struct dirent ent;
	struct dirent * entp = 0;
	while(1)
	{
		fatal_os(readdir_r, dh, &ent, &entp);

		if(!entp)
			break;

		if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
		{
			// get the canhash for this gn
			uint32_t canhash = 0;
			if(parseuint(entp->d_name, SCNu32, 1, 0xFFFFFFFF, 1, UINT8_MAX, &canhash, 0) == 0)
				fail("unexpected file %s/%s", tmp[2], entp->d_name);

			if(map_get(ws, MM(canhash)) == 0)
			{
				// remove link
				snprintf(tmp[0], sizeof(tmp[0]), "%s/%s/PRIMARY/afeed_secondary_skipweak/%u", tmp[2], entp->d_name, gn->path->can_hash);
				if(unlink(tmp[0]) != 0 && errno != ENOENT)
					fail("unlink(%s)=[%d][%s]", tmp[0], errno, strerror(errno));
			}
		}
	}

	// finally, create bidirectional symlinks for PRIMARY needs of this node
	fatal(map_keys, ws, &needs, &needsl);

	int x;
	for(x = 0; x < needsl; x++)
	{
		// primary node
		uint32_t need = needs[x][0];

		snprintf(tmp[0], sizeof(tmp[0]), CACHEDIR_BASE "/INIT/%u/gn/%u", g_args.init_fabfile_path->can_hash, need);
		snprintf(tmp[1], sizeof(tmp[1]), CACHEDIR_BASE "/INIT/%u/gn/%u/SECONDARY/aneed_primary_skipweak/%u", g_args.init_fabfile_path->can_hash, gn->path->can_hash, need);

		if(symlink(tmp[0], tmp[1]) != 0 && errno != EEXIST)
			fail("symlink=[%d][%s]", errno, strerror(errno));

		snprintf(tmp[0], sizeof(tmp[0]), CACHEDIR_BASE "/INIT/%u/gn/%u", g_args.init_fabfile_path->can_hash, gn->path->can_hash);
		snprintf(tmp[1], sizeof(tmp[1]), CACHEDIR_BASE "/INIT/%u/gn/%u/PRIMARY/afeed_secondary_skipweak/%u", g_args.init_fabfile_path->can_hash, need, gn->path->can_hash);

		if(symlink(tmp[0], tmp[1]) != 0 && errno != EEXIST)
			fail("symlink=[%d][%s]", errno, strerror(errno));
	}

	fatal(identity_assume_user);

finally:
	if(fd)
		close(fd);
	if(dh)
		closedir(dh);
	free(needs);
coda;
}

int gn_primary_reload_dscv(gn * const gn)
{
	if(gn->dscv_block == 0)
	{
		// reload if not already reloaded
		fatal(gn_primary_reload, gn);

		// create dscv block
		fatal(depblock_create, &gn->dscv_block, CACHEDIR_BASE "/INIT/%u/gn/%u/PRIMARY/dscv", g_args.init_fabfile_path->can_hash, gn->path->can_hash);

		// node has not been invalidated
		if(gn->invalid == 0)
		{
			// actually load the depblock from cache
			fatal(depblock_read, gn->dscv_block);
		}
	}

	finally : coda;
}

int gn_primary_reload(gn * const gn)
{
	DIR * dh = 0;
	char tmp[3][512];

	if(gn->primary_hb == 0)
	{
		// create hashblock
		fatal(hashblock_create, &gn->primary_hb, CACHEDIR_BASE "/INIT/%u/gn/%u/PRIMARY", g_args.init_fabfile_path->can_hash, gn->path->can_hash);

		// load the previous hashblocks
		fatal(hashblock_read, gn->primary_hb);

		// stat the file, compute new stathash
		fatal(hashblock_stat, gn->path->can, gn->primary_hb, gn->primary_hb, 0);

		// construct directory path for aneed_primary_skipweak for this node
		snprintf(tmp[2], sizeof(tmp[2])
			, CACHEDIR_BASE "/INIT/%u/gn/%u/PRIMARY/afeed_secondary_skipweak"
			, g_args.init_fabfile_path->can_hash
			, gn->path->can_hash
		);

		fatal(identity_assume_fabsys);

		// ensure that the directory exists
		fatal(mkdirp, tmp[2], S_IRWXU | S_IRWXG | S_IRWXO);

		// process a change to the source file
		int r;
		if((r = hashblock_cmp(gn->primary_hb)))
		{
			log(L_CHANGE, "PRIMARY change (%7s) : %s"
				,   r == HB_VERSION ? "version"
					: r == HB_STAT ? "stat"
					: r == HB_CONTENT ? "content"
					: 0
				, gn->idstring
			);

			// mark as changed for THIS execution
			gn->changed = 1;

			// delete discovery results for this node, if any
			snprintf(tmp[0], sizeof(tmp[0]), CACHEDIR_BASE "/INIT/%u/gn/%u/PRIMARY/dscv", g_args.init_fabfile_path->can_hash, gn->path->can_hash);

			if(unlink(tmp[0]) != 0 && errno != ENOENT)
				fail("unlink(%s)=[%d][%s]", tmp[0], errno, strerror(errno));

			// process existing links
			if((dh = opendir(tmp[2])) == 0)
			{
				if(errno != ENOENT)
					fail("opendir(%s)=[%d][%s]", tmp[2], errno, strerror(errno));
			}
			else
			{
				struct dirent ent;
				struct dirent * entp = 0;
				while(1)
				{
					fatal_os(readdir_r, dh, &ent, &entp);

					if(!entp)
						break;

					if(strcmp(entp->d_name, ".") && strcmp(entp->d_name, ".."))
					{
						// force fabrication of secondary node
						snprintf(tmp[0], sizeof(tmp[0]), "%s/%s/SECONDARY/fab/noforce_gn", tmp[2], entp->d_name);
						if(unlink(tmp[0]) != 0 && errno != ENOENT)
						{
							if(errno == ENOENT)
							{
								// delete dangling links
								snprintf(tmp[0], sizeof(tmp[0]), "%s/%s", tmp[2], entp->d_name);
								unlink(tmp[0]);
							}
							else
							{
								fail("unlink(%s)=[%d][%s]", tmp[0], errno, strerror(errno));
							}
						}

						if(log_would(L_CHANGEL))
						{
							uint32_t canhash = 0;
							if(parseuint(entp->d_name, SCNu32, 1, UINT32_MAX, 1, UINT8_MAX, &canhash, 0) == 0)
								fail("unexpected : %s/%s", tmp[2], entp->d_name);

							struct gn ** g = 0;
							if((g = map_get(gn_nodes.by_pathhash, MM(canhash))))
							{
								log(L_CHANGE | L_CHANGEL, " -> %s", (*g)->idstring);
							}
						}
					}
				}
			}

			// commmit
			fatal(hashblock_write, gn->primary_hb);
		}

		fatal(identity_assume_user);
	}

finally:
	if(dh)
		closedir(dh);
coda;
}

int gn_invalidations()
{
	int x;

	// update all node designations
	for(x = 0; x < gn_nodes.l; x++)
		gn_designate(gn_nodes.e[x]);

	if(g_args.invalidationsz)
	{
		for(x = 0; x < gn_nodes.l; x++)
		{
			if(gn_nodes.e[x]->designation == GN_DESIGNATION_PRIMARY)
				gn_nodes.e[x]->invalid = 1;

			if(gn_nodes.e[x]->designation == GN_DESIGNATION_SECONDARY)
				gn_nodes.e[x]->invalid = 1;
		}
	}
	else
	{
		for(x = 0; x < g_args.invalidationsl; x++)
		{
			gn * gn = 0;
			fatal(lookup, g_args.init_fabfile_path->abs_dir, 0, g_args.invalidations[x], 0, &gn);

			if(gn)
			{
				if(gn->designation == GN_DESIGNATION_PRIMARY)
					gn->invalid = 1;

				if(gn->designation == GN_DESIGNATION_SECONDARY)
					gn->invalid = 1;
			}
			else
			{
				log(L_WARN, "invalidation : %s not found", g_args.invalidations[x]);
			}
		}
	}

	finally : coda;
}

void gn_teardown()
{
	int x;
	for(x = 0; x < gn_nodes.l; x++)
		freenode(gn_nodes.e[x]);

	free(gn_nodes.e);
	map_free(gn_nodes.by_path);
	map_free(gn_nodes.by_pathhash);
}

// necessary for a module to call which cannot include the struct gn definition
char* gn_idstring(gn * const gn)
{
	return gn->idstring;
}

int gn_enclose_ff(gn * const gn, struct ff_file * const ff, int * const newa)
{
	int x;
	for(x = 0; x < gn->closure_ffsl; x++)
	{
		if(gn->closure_ffs[x] == ff)
			break;
	}

	if(x >= gn->closure_ffsl)
	{
		if(gn->closure_ffsl == gn->closure_ffsa)
		{
			int ns = gn->closure_ffsa ?: 10;
			ns = ns * 2 + ns / 2;
			fatal(xrealloc, &gn->closure_ffs, sizeof(*gn->closure_ffs), ns, gn->closure_ffsa);
			gn->closure_ffsa = ns;
		}

		gn->closure_ffs[gn->closure_ffsl++] = ff;
		(*newa)++;
	}

	finally : coda;
}

void gn_dump(gn * gn)
{
	int x;
	char space[128];

	if(log_would(L_DG | L_DGRAPH))
	{
		// path properties
		log(L_DG | L_DGRAPH, "%12s : %s", "can-path"	, gn->path->can);
		log(L_DG | L_DGRAPH, "%12s : %s", "abs-path"	, gn->path->abs);
		log(L_DG | L_DGRAPH, "%12s : %s", "rel-path"	, gn->path->rel);
		log(L_DG | L_DGRAPH, "%12s : %s", "path-in"	, gn->path->in);
		log(L_DG | L_DGRAPH, "%12s : %s", "path-base", gn->path->base);
		log(L_DG | L_DGRAPH, "%12s : %u", "canhash"	, gn->path->can_hash);
		log(L_DG | L_DGRAPH, "%12s : %s", "name"			, gn->path->name);
		log(L_DG | L_DGRAPH, "%12s : %s", "ext"			, gn->path->ext);
		log(L_DG | L_DGRAPH, "%12s : %s", "ext_last"	, gn->path->ext_last);

		log(L_DG | L_DGRAPH, "%12s : %s", "designation", gn_designate(gn));

		if(gn->designation == GN_DESIGNATION_PRIMARY)
		{
			if(gn->dscvsl)
			{
				for(x = 0; x < gn->dscvsl; x++)
				{
					log(L_DG | L_DGRAPH, "%12s : (%s)[%3d,%3d - %3d,%3d]", "dsc formula"
						, ff_idstring(gn->dscvs[x]->fml->ffn->loc.ff)
						, gn->dscvs[x]->fml->ffn->loc.f_lin + 1
						, gn->dscvs[x]->fml->ffn->loc.f_col + 1
						, gn->dscvs[x]->fml->ffn->loc.l_lin + 1
						, gn->dscvs[x]->fml->ffn->loc.l_col + 1
					);
				}
			}
			else
			{
				log(L_DG | L_DGRAPH, "%12s : none", "dsc formula");
			}

			log(L_DG | L_DGRAPH, "%12s : %d", "size", (int)gn->primary_hb->size);
			if(gn->primary_hb->mtime)
			{
				struct tm ltm;
				localtime_r(&gn->primary_hb->mtime, &ltm);
				strftime(space, sizeof(space), "%a %b %d %Y %H:%M:%S", &ltm);

				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-abs", space);
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime-del", durationstring(time(0) - gn->primary_hb->mtime));
			}
			else
			{
				log(L_DG | L_DGRAPH, "%12s : %s", "mtime", "");
			}
		}

		if(gn->flags & GN_FLAGS_CANFAB)
		{
			if(gn->fabv)
			{
				log(L_DG | L_DGRAPH, "%12s : (%s)[%3d,%3d - %3d,%3d]", "fab formula"
					, ff_idstring(gn->fabv->fml->ffn->loc.ff)
					, gn->fabv->fml->ffn->loc.f_lin + 1
					, gn->fabv->fml->ffn->loc.f_col + 1
					, gn->fabv->fml->ffn->loc.l_lin + 1
					, gn->fabv->fml->ffn->loc.l_col + 1
				);

				if(gn->fabv->productsl > 1)
				{
					int x;
					for(x = 0; x < gn->fabv->productsl; x++)
						log(L_DG | L_DGRAPH, "%12s --> %s", "", gn->fabv->products[x]->idstring);
				}
			}
			else
			{
				log(L_WARN | L_DG | L_DGRAPH, "%12s : %s", "fab formula", "(no formula)");
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "height", gn->height);
		log(L_DG | L_DGRAPH, "%12s : %d", "stage", gn->stage);

		log(L_DG | L_DGRAPH, "%12s : %d", "needs", gn->needs.l);
		for(x = 0; x < gn->needs.l; x++)
		{
			if(gn->needs.e[x]->type == GN_RELATION_REGULAR)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
					, ""
					, gn->needs.e[x]->weak ? "*" : " "
					, gn->needs.e[x]->B->idstring
					, ff_idstring(gn->needs.e[x]->ffn->loc.ff)
					, gn->needs.e[x]->ffn->loc.f_lin + 1
					, gn->needs.e[x]->ffn->loc.f_col + 1
					, gn->needs.e[x]->ffn->loc.l_lin + 1
					, gn->needs.e[x]->ffn->loc.l_col + 1
				);
			}
			else if(gn->needs.e[x]->type == GN_RELATION_CACHED)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (DSC:%s)[%6s%s%6s]"
					, ""
					, gn->needs.e[x]->weak ? "*" : " "
					, gn->needs.e[x]->B->idstring
					, gn->needs.e[x]->dscv_gn->idstring
					, "", "cache", ""
				);
			}
		}

		log(L_DG | L_DGRAPH, "%12s : %d", "feeds", gn->feeds.l);
		for(x = 0; x < gn->feeds.l; x++)
		{
			if(gn->feeds.e[x]->type == GN_RELATION_REGULAR)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (%s)[%3d,%3d - %3d,%3d]"
					, ""
					, gn->feeds.e[x]->weak ? "*" : " "
					, gn->feeds.e[x]->A->idstring
					, ff_idstring(gn->feeds.e[x]->ffn->loc.ff)
					, gn->feeds.e[x]->ffn->loc.f_lin + 1
					, gn->feeds.e[x]->ffn->loc.f_col + 1
					, gn->feeds.e[x]->ffn->loc.l_lin + 1
					, gn->feeds.e[x]->ffn->loc.l_col + 1
				);
			}
			else if(gn->feeds.e[x]->type == GN_RELATION_CACHED)
			{
				log(L_DG | L_DGRAPH, "%10s %s --> %-40s @ (DSC:%s)[%6s%s%6s]"
					, ""
					, gn->feeds.e[x]->weak ? "*" : " "
					, gn->feeds.e[x]->A->idstring
					, gn->feeds.e[x]->dscv_gn->idstring
					, "", "cache", ""
				);
			}
		}

		log(L_DG | L_DGRAPH, "");
	}
}
