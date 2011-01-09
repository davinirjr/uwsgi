/*

 *** uWSGI ***

 Copyright (C) 2009-2010 Unbit S.a.s. <info@unbit.it>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


#include "uwsgi.h"

struct uwsgi_server uwsgi;

extern char **environ;

static char *short_options = NULL;

static char *base_short_options = "s:p:t:x:d:l:v:b:mcaCTiMhrR:z:A:Q:Ly:";

extern struct uwsgi_plugin uwsgi_cache_plugin;

UWSGI_DECLARE_EMBEDDED_PLUGINS

static struct option long_base_options[] = {
	{"socket", required_argument, 0, 's'},
	{"processes", required_argument, 0, 'p'},
	{"harakiri", required_argument, 0, 't'},
#ifdef UWSGI_XML
	{"xmlconfig", required_argument, 0, 'x'},
#endif
	{"daemonize", required_argument, 0, 'd'},
	{"listen", required_argument, 0, 'l'},
	{"max-vars", required_argument, 0, 'v'},
	{"buffer-size", required_argument, 0, 'b'},
	{"memory-report", no_argument, 0, 'm'},
	{"cgi-mode", no_argument, 0, 'c'},
	{"abstract-socket", no_argument, 0, 'a'},
	{"chmod-socket", optional_argument, 0, 'C'},
#ifdef UWSGI_THREADING
	{"enable-threads", no_argument, 0, 'T'},
#endif
	{"single-interpreter", no_argument, 0, 'i'},
	{"master", no_argument, 0, 'M'},
	{"help", no_argument, 0, 'h'},
	{"reaper", no_argument, 0, 'r'},
	{"max-requests", required_argument, 0, 'R'},
	{"socket-timeout", required_argument, 0, 'z'},
	{"sharedarea", required_argument, 0, 'A'},
	{"cache", required_argument, 0, LONG_ARGS_CACHE},
#ifdef UWSGI_SPOOLER
	{"spooler", required_argument, 0, 'Q'},
#endif
	{"disable-logging", no_argument, 0, 'L'},

	{"pidfile", required_argument, 0, LONG_ARGS_PIDFILE},
	{"chroot", required_argument, 0, LONG_ARGS_CHROOT},
	{"gid", required_argument, 0, LONG_ARGS_GID},
	{"uid", required_argument, 0, LONG_ARGS_UID},
#ifdef UWSGI_INI
	{"ini", required_argument, 0, LONG_ARGS_INI},
#endif
#ifdef UWSGI_YAML
	{"yaml", required_argument, 0, 'y'},
	{"yml", required_argument, 0, 'y'},
#endif
#ifdef UWSGI_LDAP
	{"ldap", required_argument, 0, LONG_ARGS_LDAP},
	{"ldap-schema", no_argument, 0, LONG_ARGS_LDAP_SCHEMA},
	{"ldap-schema-ldif", no_argument, 0, LONG_ARGS_LDAP_SCHEMA_LDIF},
#endif
	{"no-server", no_argument, &uwsgi.no_server, 1},
	{"no-defer-accept", no_argument, &uwsgi.no_defer_accept, 1},
	{"limit-as", required_argument, 0, LONG_ARGS_LIMIT_AS},
	{"limit-post", required_argument, 0, LONG_ARGS_LIMIT_POST},
	{"no-orphans", no_argument, &uwsgi.no_orphans, 1},
	{"prio", required_argument, 0, LONG_ARGS_PRIO},
	{"post-buffering", required_argument, 0, LONG_ARGS_POST_BUFFERING},
	{"post-buffering-bufsize", required_argument, 0, LONG_ARGS_POST_BUFFERING_SIZE},
	{"upload-progress", required_argument, 0, LONG_ARGS_UPLOAD_PROGRESS},
	{"no-default-app", no_argument, &uwsgi.no_default_app, 1},
	{"manage-script-name", no_argument, &uwsgi.manage_script_name, 1},
#ifdef UWSGI_UDP
	{"udp", required_argument, 0, LONG_ARGS_UDP},
#endif
#ifdef UWSGI_MULTICAST
	{"multicast", required_argument, 0, LONG_ARGS_MULTICAST},
	{"cluster", required_argument, 0, LONG_ARGS_CLUSTER},
#endif
	{"cluster-reload", required_argument, 0, LONG_ARGS_CLUSTER_RELOAD},
	{"cluster-log", required_argument, 0, LONG_ARGS_CLUSTER_LOG},
#ifdef UWSGI_SNMP
	{"snmp", no_argument, 0, LONG_ARGS_SNMP},
	{"snmp-community", required_argument, 0, LONG_ARGS_SNMP_COMMUNITY},
#endif
	{"check-interval", required_argument, 0, LONG_ARGS_CHECK_INTERVAL},

	{"binary-path", required_argument, 0, LONG_ARGS_BINARY_PATH},
#ifdef UWSGI_PROXY
	{"proxy", required_argument, 0, LONG_ARGS_PROXY},
	{"proxy-node", required_argument, 0, LONG_ARGS_PROXY_NODE},
	{"proxy-max-connections", required_argument, 0, LONG_ARGS_PROXY_MAX_CONNECTIONS},
#endif
#ifdef UWSGI_ASYNC
	{"async", required_argument, 0, LONG_ARGS_ASYNC},
#endif
	{"logto", required_argument, 0, LONG_ARGS_LOGTO},
	{"log-syslog", optional_argument, 0, LONG_ARGS_LOG_SYSLOG},
	{"logdate", optional_argument, 0, LONG_ARGS_LOG_DATE},
	{"log-zero", no_argument, 0, LONG_ARGS_LOG_ZERO},
	{"log-slow", required_argument, 0, LONG_ARGS_LOG_SLOW},
	{"log-4xx", no_argument, 0, LONG_ARGS_LOG_4xx},
	{"log-5xx", no_argument, 0, LONG_ARGS_LOG_5xx},
	{"log-big", required_argument, 0, LONG_ARGS_LOG_BIG},
	{"log-sendfile", required_argument, 0, LONG_ARGS_LOG_SENDFILE},
	{"chdir", required_argument, 0, LONG_ARGS_CHDIR},
	{"chdir2", required_argument, 0, LONG_ARGS_CHDIR2},
	{"mount", required_argument, 0, LONG_ARGS_MOUNT},
	{"grunt", no_argument, &uwsgi.grunt, 1},
	{"threads", required_argument, 0, LONG_ARGS_THREADS},
	{"vhost", no_argument, &uwsgi.vhost, 1},
	{"vhost-host", no_argument, 0, LONG_ARGS_VHOSTHOST},
#ifdef UWSGI_ROUTING
	{"routing", no_argument, &uwsgi.routing, 1},
#endif

#ifdef UWSGI_HTTP
	{"http", required_argument, 0, LONG_ARGS_HTTP},
	{"http-only", no_argument, &uwsgi.http_only, 1},
	{"http-var", required_argument, 0, LONG_ARGS_HTTP_VAR},
	{"http-modifier1", required_argument, 0, LONG_ARGS_HTTP_MODIFIER1},
#endif
	{"close-on-exec", no_argument, &uwsgi.close_on_exec, 1},
	{"mode", required_argument, 0, LONG_ARGS_MODE},
	{"env", required_argument, 0, LONG_ARGS_ENV},
	{"vacuum", no_argument, &uwsgi.vacuum, 1},
#ifdef __linux__
	{"cgroup", required_argument, 0, LONG_ARGS_CGROUP},
	{"cgroup-opt", required_argument, 0, LONG_ARGS_CGROUP_OPT},
	{"namespace", required_argument, 0, LONG_ARGS_LINUX_NS},
	{"ns", required_argument, 0, LONG_ARGS_LINUX_NS},
#endif
	{"loop", required_argument, 0, LONG_ARGS_LOOP},
	{"plugins", required_argument, 0, LONG_ARGS_PLUGINS},
	{"remap-modifier", required_argument, 0, LONG_ARGS_REMAP_MODIFIER},
	{"version", no_argument, 0, LONG_ARGS_VERSION},
	{0, 0, 0, 0}
};

int find_worker_id(pid_t pid)
{
	int i;
	for (i = 1; i <= uwsgi.numproc; i++) {
		if (uwsgi.workers[i].pid == pid)
			return i;
	}

	return -1;
}


void warn_pipe()
{
	struct wsgi_request *wsgi_req = current_wsgi_req();

	if (uwsgi.async < 2 && wsgi_req->uri_len > 0) {
		uwsgi_log("SIGPIPE: writing to a closed pipe/socket/fd (probably the client disconnected) on request %.*s !!!\n", wsgi_req->uri_len, wsgi_req->uri);
	} else {
		uwsgi_log("SIGPIPE: writing to a closed pipe/socket/fd (probably the client disconnected) !!!\n");
	}
}

void gracefully_kill()
{
	uwsgi_log("Gracefully killing worker %d...\n", uwsgi.mypid);
	if (UWSGI_IS_IN_REQUEST) {
		uwsgi.workers[uwsgi.mywid].manage_next_request = 0;
	} else {
		reload_me();
	}
}

void reload_me()
{
	exit(UWSGI_RELOAD_CODE);
}

void end_me()
{
	exit(UWSGI_END_CODE);
}

void goodbye_cruel_world()
{
	uwsgi_log("...The work of process %d is done. Seeya!\n", getpid());
	exit(0);
}

void kill_them_all()
{
	int i;
	uwsgi.to_hell = 1;
	uwsgi_log("SIGINT/SIGQUIT received...killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGINT);
	}
}

void grace_them_all()
{
	int i;
	uwsgi.to_heaven = 1;
	uwsgi_log("...gracefully killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGHUP);
	}
}

void reap_them_all()
{
	int i;
	uwsgi.to_heaven = 1;
	uwsgi_log("...brutally killing workers...\n");
	for (i = 1; i <= uwsgi.numproc; i++) {
		kill(uwsgi.workers[i].pid, SIGTERM);
	}
}

void harakiri()
{

	uwsgi_log("\nF*CK !!! i must kill myself (pid: %d app_id: %d)...\n", uwsgi.mypid, uwsgi.wsgi_req->app_id);

	if (!uwsgi.master_process) {
		uwsgi_log("*** if you want your workers to be automatically respawned consider enabling the uWSGI master process ***\n");
	}
	exit(0);
}

void stats()
{
	//fix this for better logging(this cause races)
	struct uwsgi_app *ua = NULL;
	int i;

	uwsgi_log("*** pid %d stats ***\n", getpid());
	uwsgi_log("\ttotal requests: %llu\n", uwsgi.workers[0].requests);
	for (i = 0; i < uwsgi.apps_cnt; i++) {
		ua = &uwsgi.apps[i];
		if (ua) {
			uwsgi_log("\tapp %d requests: %d\n", i, ua->requests);
		}
	}
	uwsgi_log("\n");
}

void what_i_am_doing()
{

	struct wsgi_request *wsgi_req = current_wsgi_req();

	if (uwsgi.async < 2 && wsgi_req->uri_len > 0) {

		if (uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] > 0 && uwsgi.workers[uwsgi.mywid].harakiri < time(NULL)) {
			uwsgi_log("HARAKIRI: --- uWSGI worker %d (pid: %d) WAS managing request %.*s since %.*s ---\n", (int) uwsgi.mywid, (int) uwsgi.mypid, wsgi_req->uri_len, wsgi_req->uri, 24, ctime((const time_t *) & wsgi_req->start_of_request.tv_sec));
		} else {
			uwsgi_log("SIGUSR2: --- uWSGI worker %d (pid: %d) is managing request %.*s since %.*s ---\n", (int) uwsgi.mywid, (int) uwsgi.mypid, wsgi_req->uri_len, wsgi_req->uri, 24, ctime((const time_t *) & wsgi_req->start_of_request.tv_sec));
		}
	}
}


pid_t masterpid;
struct timeval last_respawn;


int unconfigured_hook(struct wsgi_request * wsgi_req)
{
	uwsgi_log("-- unavailable modifier requested: %d --\n", wsgi_req->uh.modifier1);
	return -1;
}

static void unconfigured_after_hook(struct wsgi_request * wsgi_req)
{
	return;
}

struct uwsgi_plugin unconfigured_plugin = {

	.name = "unconfigured",
	.request = unconfigured_hook,
	.after_request = unconfigured_after_hook,
};

static void vacuum(void)
{

	int i;

	if (uwsgi.vacuum) {
		if (getpid() == masterpid) {
			if (chdir(uwsgi.cwd)) {
				uwsgi_error("chdir()");
			}
			for (i = 0; i < uwsgi.sockets_cnt; i++) {
				if (uwsgi.sockets[i].family == AF_UNIX) {
					if (unlink(uwsgi.sockets[i].name)) {
						uwsgi_error("unlink()");
					} else {
						uwsgi_log("VACUUM: unix socket %s removed.\n", uwsgi.sockets[i].name);
					}
				}
			}
			if (uwsgi.pidfile && !uwsgi.uid) {
				if (unlink(uwsgi.pidfile)) {
					uwsgi_error("unlink()");
				} else {
					uwsgi_log("VACUUM: pidfile removed.\n");
				}
			}
		}
	}
}

int main(int argc, char *argv[], char *envp[])
{

	int i;
	int rlen;

	FILE *pidfile;

	char *env_reloads;
	unsigned int reloads = 0;
	char env_reload_buf[11];
	
	char *plugins_requested;



#ifdef UNBIT
	//struct uidsec_struct us;
#endif

#ifdef UWSGI_DEBUG
	struct utsname uuts;
#endif

	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	//initialize masterpid with a default value
	masterpid = getpid();

	memset(&uwsgi, 0, sizeof(struct uwsgi_server));
	uwsgi.cwd = uwsgi_get_cwd();

	atexit(vacuum);


#ifdef UWSGI_DEBUG
#ifdef __sun__
	if (uname(&uuts) < 0) {
#else
	if (uname(&uuts)) {
#endif
		uwsgi_error("uname()");
	} else {
		uwsgi_log("SYSNAME: %s\nNODENAME: %s\nRELEASE: %s\nVERSION: %s\nMACHINE: %s\n",
			  uuts.sysname,
			  uuts.nodename,
			  uuts.release,
			  uuts.version,
			  uuts.machine);
	}
#endif


	uwsgi.shared = (struct uwsgi_shared *) mmap(NULL, sizeof(struct uwsgi_shared), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (!uwsgi.shared) {
		uwsgi_error("mmap()");
		exit(1);
	}
	memset(uwsgi.shared, 0, sizeof(struct uwsgi_shared));

#ifdef UWSGI_SPOOLER
	//set the spooler frequency to 30 seconds by default
	uwsgi.shared->spooler_frequency = 30;
#endif

	for (i = 0; i < 0xFF; i++) {
		uwsgi.p[i] = &unconfigured_plugin;
	}

	uwsgi.master_queue = -1;

	uwsgi.cluster_fd = -1;
	uwsgi.cores = 1;

	uwsgi.apps_cnt = 0;
	uwsgi.default_app = -1;

	uwsgi.buffer_size = 4096;
	uwsgi.numproc = 1;

	uwsgi.async = 1;
	uwsgi.listen_queue = 64;

	uwsgi.max_vars = MAX_VARS;
	uwsgi.vec_size = 4 + 1 + (4 * MAX_VARS);

	uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT] = 4;
	uwsgi.shared->options[UWSGI_OPTION_LOGGING] = 1;


	gettimeofday(&uwsgi.start_tv, NULL);

	setlinebuf(stdout);

	uwsgi.rl.rlim_cur = 0;
	uwsgi.rl.rlim_max = 0;


	env_reloads = getenv("UWSGI_RELOADS");
	if (env_reloads) {
		//convert env value to int
		reloads = atoi(env_reloads);
		reloads++;
		//convert reloads to string
		rlen = snprintf(env_reload_buf, 10, "%u", reloads);
		if (rlen > 0) {
			env_reload_buf[rlen] = 0;
			if (setenv("UWSGI_RELOADS", env_reload_buf, 1)) {
				uwsgi_error("setenv()");
			}
		}
		uwsgi.is_a_reload = 1;
	} else {
		if (setenv("UWSGI_RELOADS", "0", 1)) {
			uwsgi_error("setenv()");
		}
	}

	uwsgi.binary_path = argv[0];

	uwsgi_register_loop("simple", simple_loop);
	uwsgi_register_loop("async", complex_loop);

	//initialize embedded plugins
	UWSGI_LOAD_EMBEDDED_PLUGINS

	// now a bit of magic, if the argv[0] contains a _ try to automatically load a plugin
	//uwsgi_log("executable name: %s\n", argv[0]);
	char *p = strtok(argv[0], "_");
	plugins_requested = NULL;
	while (p != NULL) {
		p = strtok(NULL, "_");
		if (p) plugins_requested = p;
	}
	
	if (plugins_requested) {
		uwsgi_log("plugin = %s\n", plugins_requested);
		uwsgi_load_plugin(0, plugins_requested, NULL, 0);
	}

	plugins_requested = getenv("UWSGI_PLUGINS");
	if (plugins_requested) {
		char *p = strtok(plugins_requested, ",");
		while (p != NULL) {
			uwsgi_load_plugin(-1, p, NULL, 0);
			p = strtok(NULL, ",");
		}
	}
	build_options();

	while ((i = getopt_long(argc, argv, short_options, uwsgi.long_options, &uwsgi.option_index)) != -1) {
		if (i == 0) {
			add_exported_option(0, (char *)uwsgi.long_options[uwsgi.option_index].name);
		}
		else {
			manage_opt(i, optarg);
		}
	}



	   if (optind < argc) {
	   	char *lazy = argv[optind];
		if (lazy[0] != '[') {
			if (!strcmp(lazy+strlen(lazy)-4, ".xml")) {
				uwsgi.xml_config = lazy;
			}
#ifdef UWSGI_INI
			else if (!strcmp(lazy+strlen(lazy)-4, ".ini")) {
				uwsgi.ini = lazy;
			}
#endif
#ifdef UWSGI_YAML
			else if (!strcmp(lazy+strlen(lazy)-4, ".yml")) {
				uwsgi.yaml = lazy;
			}
#endif
			// manage magic mountpoint
			else if ( (lazy[0] == '/' || strchr(lazy, '|')) && strchr(lazy,'=')) {
			}
			else {
				int magic = 0;
				for(i =0; i < uwsgi.gp_cnt; i++) {
                			if (uwsgi.gp[i]->magic) {
                        			if (uwsgi.gp[i]->magic(NULL, lazy)) {
							magic = 1;
							break;
						}
                			}
				}
				if (!magic) {
        				for (i = 0; i < 0xFF; i++) {
                				if (uwsgi.p[i]->magic) {
                        				if (uwsgi.p[i]->magic(NULL, lazy)) {
								magic = 1;
								break;
							}
                				}
        				}
				}
			}
		}
	}

#ifdef UWSGI_XML
	if (uwsgi.xml_config != NULL) {
		uwsgi_xml_config(uwsgi.wsgi_req, 0);
	}
#endif
#ifdef UWSGI_INI
	if (uwsgi.ini != NULL) {
		uwsgi_ini_config(uwsgi.ini);
	}
#endif
#ifdef UWSGI_YAML
	if (uwsgi.yaml != NULL) {
		uwsgi_yaml_config(uwsgi.yaml);
	}
#endif
#ifdef UWSGI_LDAP
	if (uwsgi.ldap != NULL) {
		uwsgi_ldap_config();
	}
#endif

	//parse environ
	parse_sys_envs(environ);

	if (gethostname(uwsgi.hostname, 255)) {
		uwsgi_error("gethostname()");
	}

#ifdef UWSGI_UDP
        // get cluster configuration
	if (uwsgi.cluster != NULL) {
		// get multicast socket

		uwsgi_log("CLUSTER: %s\n", uwsgi.cluster);
		uwsgi.cluster_fd = uwsgi_cluster_join(uwsgi.cluster);
	
		// ask for cluster options only if bot pre-existent options are set
		if (uwsgi.exported_opts_cnt == 1) {
			// now wait max 60 seconds and resend multicast request every 10 seconds
			for(;;) {
				uwsgi_log("asking \"%s\" uWSGI cluster for configuration data:\n", uwsgi.cluster);
				if (uwsgi_send_empty_pkt(uwsgi.cluster_fd, uwsgi.cluster, 99, 0) < 0) {
					uwsgi_log("unable to send multicast message to %s\n", uwsgi.cluster);	
					continue;
				}
waitfd:
				rlen = uwsgi_waitfd(uwsgi.cluster_fd, 10);
				if (rlen < 0) {
					break;	
				}
				else if (rlen > 0) {
					// receive the packet
					char clusterbuf[4096];
					if (!uwsgi_hooked_parse_dict_dgram(uwsgi.cluster_fd, clusterbuf, 4096, 99, 1, manage_string_opt, NULL)) {
						goto options_parsed;
					}
					else {
						goto waitfd;
					}
				}
			}
		}
options_parsed:

		uwsgi_cluster_add_me();	
	}
#endif

#ifdef UWSGI_PROXY
	if (uwsgi.proxy_add_me) {
		uwsgi_cluster_simple_add_node(uwsgi.sockets[0].name, 1, CLUSTER_NODE_STATIC);
	}
#endif


	//call after_opt hooks

	if (uwsgi.binary_path == argv[0]) {
		uwsgi.binary_path = uwsgi_malloc(strlen(argv[0]) + 1);
		memcpy(uwsgi.binary_path, argv[0], strlen(argv[0]) + 1);
	}

	if (!uwsgi.no_initial_output) {
		if (uwsgi.shared->options[UWSGI_OPTION_CGI_MODE] == 0) {
			uwsgi_log("*** Starting uWSGI %s (%dbit) on [%.*s] ***\n", UWSGI_VERSION, (int) (sizeof(void *)) * 8, 24, ctime((const time_t *) &uwsgi.start_tv.tv_sec));
		} else {
			uwsgi_log("*** Starting uWSGI %s (CGI mode) (%dbit) on [%.*s] ***\n", UWSGI_VERSION, (int) (sizeof(void *)) * 8, 24, ctime((const time_t *) &uwsgi.start_tv.tv_sec));
		}

#ifdef UWSGI_DEBUG
	uwsgi_log("***\n*** You are running a DEBUG version of uWSGI, plese disable DEBUG in uwsgiconfig.py and recompile it ***\n***\n");
#endif

	uwsgi_log("compiled with version: %s\n", __VERSION__);

#ifdef __BIG_ENDIAN__
	uwsgi_log("*** big endian arch detected ***\n");
#endif

	}

	if (uwsgi.pidfile && !uwsgi.is_a_reload) {
		uwsgi_log("writing pidfile to %s\n", uwsgi.pidfile);
		pidfile = fopen(uwsgi.pidfile, "w");
		if (!pidfile) {
			uwsgi_error("fopen");
			exit(1);
		}
		if (fprintf(pidfile, "%d\n", (int) getpid()) < 0) {
			uwsgi_log("could not write pidfile.\n");
		}
		fclose(pidfile);
	}

#ifdef __linux__

#ifndef CLONE_NEWUTS
#define CLONE_NEWUTS 0x04000000
#endif

#ifndef CLONE_NEWPID
#define CLONE_NEWPID 0x20000000
#endif

#ifndef CLONE_NEWIPC
#define CLONE_NEWIPC 0x08000000
#endif
	if (uwsgi.ns) {
		void *linux_clone_stack = alloca(uwsgi.page_size);
		for(;;) {
			pid_t pid = clone(uwsgi_start, linux_clone_stack+uwsgi.page_size, SIGCHLD|CLONE_NEWUTS|CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWNS, (void *)argv);
			if (pid == -1) {
				uwsgi_error("clone()");
				exit(1);
			}
			uwsgi_log("waiting for jailed master dead...\n");
			pid = waitpid(pid, NULL, 0);
			uwsgi_log("pid %d ended. Respawning...\n", (int) pid);	
		}
	}
	else {
#endif
		uwsgi_start((void *)argv);
#ifdef __linux__
	}
#endif


	// never here
	return 0;
}

int uwsgi_start(void *v_argv) {

	char **argv = v_argv;

#ifdef UWSGI_DEBUG
        int so_bufsize;
        socklen_t so_bufsize_len;
#endif


#ifdef UWSGI_HTTP
	pid_t http_pid;
#endif

	pid_t pid;
	int i, j;

	struct sockaddr_un usa;
        struct sockaddr *gsa;
        struct sockaddr_in *isa;
        socklen_t socket_type_len;

	FILE *pidfile;

	int uwsgi_will_starts = 0;

	uwsgi_log("my PID is %d\n", (int) getpid());

#ifdef __linux__
	if (uwsgi.ns && getpid() == 1) {
			
		char *ns_hostname = strchr(uwsgi.ns, ':');
		if (ns_hostname) {
			ns_hostname[0] = 0;
			ns_hostname++;
			if (sethostname(ns_hostname, strlen(ns_hostname))) {
				uwsgi_error("sethostname()");
			}
		}

		FILE *procmounts;
		char line[1024];
		int unmounted = 1;
		char *delim0, *delim1;

		while(unmounted) {

			unmounted = 0;
			procmounts = fopen("/proc/self/mounts", "r");
			while(fgets(line,1024,procmounts) != NULL) {
				delim0 = strchr(line, ' ');
				delim0++;
				delim1 = strchr(delim0, ' ');
				*delim1 = 0;
				if (!umount(delim0)) {
					unmounted++;
				}
			}
			fclose(procmounts);
		}

		uwsgi_log("unmounting /proc\n");
		if (umount("/proc")) {
			uwsgi_error("umount()");
		}

		char *ns_tmp_mountpoint = uwsgi_concat2(uwsgi.ns, "/.uwsgi_ns_tmp_mountpoint");
		mkdir(ns_tmp_mountpoint, S_IRWXU);

		char *ns_tmp_mountpoint2 = uwsgi_concat2(ns_tmp_mountpoint, "/.uwsgi_ns_tmp_mountpoint");
		mkdir(ns_tmp_mountpoint2, S_IRWXU);

		if (mount(uwsgi.ns, ns_tmp_mountpoint, "none", MS_BIND, NULL)) {
			uwsgi_error("mount()");
		}
		if (chdir(ns_tmp_mountpoint)) {
			uwsgi_error("chdir()");
		}
		if (pivot_root(".", ns_tmp_mountpoint2)) {
			uwsgi_error("pivot_root()");
		}

		if (umount("/.uwsgi_ns_tmp_mountpoint")) {
			uwsgi_error("umount tmp()");
		}

		rmdir("/.uwsgi_ns_tmp_mountpoint/.uwsgi_ns_tmp_mountpoint");
		rmdir("/.uwsgi_ns_tmp_mountpoint");

		free(ns_tmp_mountpoint2);
		free(ns_tmp_mountpoint);
		
		uwsgi_log("remounting /proc\n");
		if (mount("proc","/proc", "proc", 0, NULL)) {
			uwsgi_error("mount()");
		}

	}
#endif

	uwsgi_as_root();

	if (uwsgi.chdir) {
		if (chdir(uwsgi.chdir)) {
			uwsgi_error("chdir()");
			exit(1);
		}
	}

	if (!uwsgi.no_initial_output) {
		if (!uwsgi.master_process) {
			uwsgi_log(" *** WARNING: you are running uWSGI without its master process manager ***\n");
		}
	}
#ifndef __OpenBSD__

	if (uwsgi.rl.rlim_max > 0) {
		uwsgi_log("limiting address space of processes...\n");
		if (setrlimit(RLIMIT_AS, &uwsgi.rl)) {
			uwsgi_error("setrlimit()");
		}
	}
	if (uwsgi.prio != 0) {
#ifdef __HAIKU__
		if (set_thread_priority(find_thread(NULL), uwsgi.prio) == B_BAD_THREAD_ID) {
			uwsgi_error("set_thread_priority()");
#else
		if (setpriority(PRIO_PROCESS, 0, uwsgi.prio)) {
			uwsgi_error("setpriority()");
#endif

		} else {
			uwsgi_log("scheduler priority set to %d\n", uwsgi.prio);
		}
	}
	if (!getrlimit(RLIMIT_AS, &uwsgi.rl)) {
		//check for overflow
		if (uwsgi.rl.rlim_max != RLIM_INFINITY && !uwsgi.no_initial_output) {
			uwsgi_log("your process address space limit is %lld bytes (%lld MB)\n", (long long) uwsgi.rl.rlim_max, (long long) uwsgi.rl.rlim_max / 1024 / 1024);
		}
	}
#endif

	uwsgi.page_size = getpagesize();

	if (!uwsgi.no_initial_output) {
		uwsgi_log("your memory page size is %d bytes\n", uwsgi.page_size);
	}

	if (uwsgi.buffer_size > 65536) {
		uwsgi_log("invalid buffer size.\n");
		exit(1);
	}
	sanitize_args();

#ifdef UWSGI_HTTP
	if (uwsgi.http && !uwsgi.is_a_reload) {
		char *tcp_port = strchr(uwsgi.http, ':');
		if (tcp_port) {
			uwsgi.http_server_port = tcp_port + 1;
			uwsgi.http_fd = bind_to_tcp(uwsgi.http, uwsgi.listen_queue, tcp_port);
#ifdef UWSGI_DEBUG
			uwsgi_debug("HTTP FD: %d\n", uwsgi.http_fd);
#endif
		} else {
			uwsgi_log("invalid http address.\n");
			exit(1);
		}

		if (uwsgi.http_fd < 0) {
			uwsgi_log("unable to create http server socket.\n");
			exit(1);
		}
		if (!uwsgi.sockets[0].name) {

			uwsgi.sockets[0].name = uwsgi_malloc(64);
			uwsgi.sockets_cnt++;
			snprintf(uwsgi.sockets[0].name, 64, "%d_%d.sock", (int) time(NULL), (int) getpid());
			uwsgi_log("using %s as uwsgi protocol socket\n", uwsgi.sockets[0].name);
		}
		if (uwsgi.http_only) {
			http_loop();
			//never here
			exit(1);
		}
		http_pid = fork();

		if (http_pid > 0) {
			masterpid = http_pid;
			http_loop();
			//never here
			exit(1);
		} else if (http_pid < 0) {
			uwsgi_error("fork()");
			exit(1);
		}
		if (uwsgi.pidfile && !uwsgi.is_a_reload) {
			uwsgi_log("updating pidfile with pid %d\n", (int) getpid());
			pidfile = fopen(uwsgi.pidfile, "w");
			if (!pidfile) {
				uwsgi_error("fopen");
				exit(1);
			}
			if (fprintf(pidfile, "%d\n", (int) getpid()) < 0) {
				uwsgi_log("could not update pidfile.\n");
			}
			fclose(pidfile);
		}
		close(uwsgi.http_fd);
	}
#endif


	if (uwsgi.async > 1) {
		if (!getrlimit(RLIMIT_NOFILE, &uwsgi.rl)) {
			if ((unsigned long) uwsgi.rl.rlim_cur < (unsigned long) uwsgi.async) {
				uwsgi_log("- your current max open files limit is %lu, this is lower than requested async cores !!! -\n", (unsigned long) uwsgi.rl.rlim_cur);
				if (uwsgi.rl.rlim_cur < uwsgi.rl.rlim_max && (unsigned long) uwsgi.rl.rlim_max > (unsigned long) uwsgi.async) {
					unsigned long tmp_nofile = (unsigned long) uwsgi.rl.rlim_cur;
					uwsgi.rl.rlim_cur = uwsgi.async;
					if (!setrlimit(RLIMIT_NOFILE, &uwsgi.rl)) {
						uwsgi_log("max open files limit reset to %lu\n", (unsigned long) uwsgi.rl.rlim_cur);
						uwsgi.async = uwsgi.rl.rlim_cur;
					} else {
						uwsgi.async = (int) tmp_nofile;
					}
				} else {
					uwsgi.async = uwsgi.rl.rlim_cur;
				}

				uwsgi_log("- async cores set to %d -\n", uwsgi.async);
			}
		}
	}

	uwsgi.wsgi_requests = uwsgi_malloc(sizeof(struct wsgi_request *) * uwsgi.cores);

	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.wsgi_requests[i] = uwsgi_malloc(sizeof(struct wsgi_request));
		memset(uwsgi.wsgi_requests[i], 0, sizeof(struct wsgi_request));
	}

	uwsgi.async_buf = uwsgi_malloc(sizeof(char *) * uwsgi.cores);

	if (uwsgi.post_buffering > 0) {
		uwsgi.async_post_buf = uwsgi_malloc(sizeof(char *) * uwsgi.cores);
		if (!uwsgi.post_buffering_bufsize) {
			uwsgi.post_buffering_bufsize = 8192;
		}
	}
	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.async_buf[i] = uwsgi_malloc(uwsgi.buffer_size);
		if (uwsgi.post_buffering > 0) {
			uwsgi.async_post_buf[i] = uwsgi_malloc(uwsgi.post_buffering_bufsize);
		}
	}


	//by default set wsgi_req to the first slot
	uwsgi.wsgi_req = uwsgi.wsgi_requests[0];

	if (uwsgi.cores > 1) {
		uwsgi_log("allocated %llu bytes (%llu KB) for %d cores per worker.\n", (uint64_t) (sizeof(struct wsgi_request) * uwsgi.cores),
			  (uint64_t) ((sizeof(struct wsgi_request) * uwsgi.cores) / 1024),
			  uwsgi.cores);
	}
	if (uwsgi.vhost) {
		uwsgi_log("VirtualHosting mode enabled.\n");
		uwsgi.apps_cnt = 0;
	}

	/* plugin initialization */
	for(i =0; i < uwsgi.gp_cnt; i++) {
		if (uwsgi.gp[i]->init) {
			uwsgi.gp[i]->init();
		}
	}
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->init) {
			uwsgi.p[i]->init();
		}
	}

	// application generic lock
	uwsgi.user_lock = uwsgi_mmap_shared_lock();
	uwsgi_lock_init(uwsgi.user_lock);

	if (uwsgi.master_process) {
		// signal table lock
		uwsgi.signal_table_lock = uwsgi_mmap_shared_lock();
		uwsgi_lock_init(uwsgi.signal_table_lock);

		// fmon table lock
		uwsgi.fmon_table_lock = uwsgi_mmap_shared_lock();
		uwsgi_lock_init(uwsgi.fmon_table_lock);

		// timer table lock
		uwsgi.timer_table_lock = uwsgi_mmap_shared_lock();
		uwsgi_lock_init(uwsgi.timer_table_lock);
	}

	uwsgi.rpc_table_lock = uwsgi_mmap_shared_lock();
	uwsgi_lock_init(uwsgi.rpc_table_lock);

	if (uwsgi.sharedareasize > 0) {
		uwsgi.sharedareamutex = uwsgi_mmap_shared_lock();

		uwsgi.sharedarea = mmap(NULL, uwsgi.page_size * uwsgi.sharedareasize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (uwsgi.sharedarea) {
			uwsgi_log("shared area mapped at %p, you can access it with uwsgi.sharedarea* functions.\n", uwsgi.sharedarea);
			uwsgi_lock_init(uwsgi.sharedareamutex);
		} else {
			uwsgi_error("mmap()");
			exit(1);
		}

	}

	if (uwsgi.cache_max_items > 0) {
		uwsgi.cache_items = (struct uwsgi_cache_item *) mmap(NULL, sizeof(struct uwsgi_cache_item) * uwsgi.cache_max_items, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (!uwsgi.cache_items) {
			uwsgi_error("mmap()");
                        exit(1);
		}

		uwsgi.cache = mmap(NULL, 32768 * uwsgi.cache_max_items, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		if (!uwsgi.cache) {
			uwsgi_error("mmap()");
                        exit(1);
		}
		
		for(i=0;i< (int) uwsgi.cache_max_items;i++) {
			memset(&uwsgi.cache_items[i], 0, sizeof(struct uwsgi_cache_item));
		}

		// the first cache item is always zero
		uwsgi.shared->cache_first_available_item = 1;

		uwsgi.cache_lock = uwsgi_mmap_shared_lock();
        	uwsgi_lock_init(uwsgi.cache_lock);

		uwsgi.p[111] = &uwsgi_cache_plugin;
	}


	uwsgi.current_wsgi_req = simple_current_wsgi_req;


	// call nagios here

#ifdef UWSGI_THREADING
	if (uwsgi.has_threads) {
		uwsgi.current_wsgi_req = threaded_current_wsgi_req;
		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->enable_threads)
				uwsgi.p[i]->enable_threads();
		}
	}
#endif

	if (!uwsgi.no_server) {

		//check for inherited sockets
			if (uwsgi.is_a_reload) {
				for (i = 0; i < uwsgi.sockets_cnt; i++) {
					//a bit overengineering
						if (uwsgi.sockets[i].name != NULL) {

						for (j = 3; j < sysconf(_SC_OPEN_MAX); j++) {
							socket_type_len = sizeof(struct sockaddr_un);
							gsa = (struct sockaddr *) &usa;
							if (!getsockname(j, gsa, &socket_type_len)) {
								if (gsa->sa_family == AF_UNIX) {
									if (!strcmp(usa.sun_path, uwsgi.sockets[i].name)) {
										uwsgi.sockets[i].fd = j;
										uwsgi.sockets[i].family = AF_UNIX;
										uwsgi.sockets[i].bound = 1;
										uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
										uwsgi.sockets_poll[i].events = POLLIN;
										uwsgi_will_starts = 1;
										uwsgi_log("uwsgi socket %d inherited UNIX address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
									}
								} else if (gsa->sa_family == AF_INET) {
									char *computed_addr;
									char computed_port[6];
									isa = (struct sockaddr_in *) &usa;
									char ipv4a[INET_ADDRSTRLEN + 1];
									memset(ipv4a, 0, INET_ADDRSTRLEN + 1);
									memset(computed_port, 0, 6);


									if (snprintf(computed_port, 6, "%d", ntohs(isa->sin_port)) > 0) {
										if (inet_ntop(AF_INET, (const void *) &isa->sin_addr.s_addr, ipv4a, INET_ADDRSTRLEN)) {

											if (!strcmp("0.0.0.0", ipv4a)) {
												computed_addr = uwsgi_concat2(":", computed_port);
											} else {
												computed_addr = uwsgi_concat3(ipv4a, ":", computed_port);
											}
											char *asterisk = strchr(uwsgi.sockets[i].name, '*');
											int match = 1;
											if (asterisk) {
												asterisk[0] = 0;
												match = strncmp(computed_addr, uwsgi.sockets[i].name, strlen(uwsgi.sockets[i].name));
												asterisk[0] = '*';	
											}
											else {
												match = strcmp(computed_addr, uwsgi.sockets[i].name);
											}
											if (!match) {
												uwsgi.sockets[i].fd = j;
												uwsgi.sockets[i].family = AF_INET;
												uwsgi.sockets[i].bound = 1;
												uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
												uwsgi.sockets_poll[i].events = POLLIN;
												uwsgi_will_starts = 1;
												uwsgi_log("uwsgi socket %d inherited INET address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
											}
											free(computed_addr);
										}
									}
								}
							}
						}
					}
				}

				//now close all the unbound fd
				for (j = 3; j < sysconf(_SC_OPEN_MAX); j++) {
					int useless = 1;
#ifdef UWSGI_MULTICAST
					if (j == uwsgi.cluster_fd) continue;
#endif
					socket_type_len = sizeof(struct sockaddr_un);
					gsa = (struct sockaddr *) & usa;
					if (!getsockname(j, gsa, &socket_type_len)) {
						for (i = 0; i < uwsgi.sockets_cnt; i++) {
							if (uwsgi.sockets[i].fd == j && uwsgi.sockets[i].bound) {
								useless = 0;
								break;
							}
						}
					}
					if (useless)
						close(j);
				}
			}
		//now bind all the unbound sockets
		for (i = 0; i < uwsgi.sockets_cnt; i++) {
			if (!uwsgi.sockets[i].bound) {
				char *tcp_port = strchr(uwsgi.sockets[i].name, ':');
				if (tcp_port == NULL) {
					uwsgi.sockets[i].fd = bind_to_unix(uwsgi.sockets[i].name, uwsgi.listen_queue, uwsgi.chmod_socket, uwsgi.abstract_socket);
					uwsgi.sockets[i].family = AF_UNIX;
					uwsgi_log("uwsgi socket %d bound to UNIX address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				} else {
					uwsgi.sockets[i].fd = bind_to_tcp(uwsgi.sockets[i].name, uwsgi.listen_queue, tcp_port);
					uwsgi.sockets[i].family = AF_INET;
					uwsgi_log("uwsgi socket %d bound to TCP address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				}

				if (uwsgi.sockets[i].fd < 0) {
					uwsgi_log("unable to create server socket on: %s\n", uwsgi.sockets[i].name);
					exit(1);
				}
			}
			uwsgi.sockets[i].bound = 1;
			uwsgi.sockets_poll[i].fd = uwsgi.sockets[i].fd;
			uwsgi.sockets_poll[i].events = POLLIN;
			uwsgi_will_starts = 1;
		}

		int zero_used = 0;
		for (i = 0; i < uwsgi.sockets_cnt; i++) {
			if (uwsgi.sockets[i].bound && uwsgi.sockets[i].fd == 0) {
				zero_used = 1;
				break;
			}
		}

		if (!zero_used) {
			socket_type_len = sizeof(struct sockaddr_un);
			gsa = (struct sockaddr *) & usa;
			if (!getsockname(0, gsa, &socket_type_len)) {
				if (uwsgi.sockets_cnt < 8) {
					uwsgi.sockets_cnt++;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].fd = 0;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].bound = 1;
					uwsgi.sockets[uwsgi.sockets_cnt - 1].family = gsa->sa_family;
					//uwsgi.sockets[uwsgi.sockets_cnt - 1].name = uwsgi_get_socket_name(gsa->sa_family, gsa);
					uwsgi.sockets_poll[uwsgi.sockets_cnt - 1].fd = 0;
					uwsgi.sockets_poll[uwsgi.sockets_cnt - 1].events = POLLIN;
					uwsgi_will_starts = 1;
					uwsgi_log("uwsgi socket %d inherited INET address %s fd %d\n", i, uwsgi.sockets[i].name, uwsgi.sockets[i].fd);
				} else {
					uwsgi_log("too many socket defined, i cannot map fd 0\n");
				}
			} else {
				int fd = open("/dev/null", O_RDONLY);
				if (fd < 0) {
					uwsgi_error("open()");
					exit(1);
				}
				if (fd != 0) {
					if (dup2(fd, 0)) {
						uwsgi_error("dup2()");
						exit(1);
					}
				}
			}

		}

	
		// put listening socket i non-blocking state
		for (i = 0; i < uwsgi.sockets_cnt; i++) {
			uwsgi.sockets[i].arg = fcntl(uwsgi.sockets[i].fd, F_GETFL, NULL);
                	if (uwsgi.sockets[i].arg < 0) {
                        	uwsgi_error("fcntl()");
				exit(1);
                	}
                	uwsgi.sockets[i].arg |= O_NONBLOCK;
                	if (fcntl(uwsgi.sockets[i].fd, F_SETFL, uwsgi.sockets[i].arg) < 0) {
                        	uwsgi_error("fcntl()");
                        	exit(1);
                	}
		}
	
#ifdef UWSGI_PROXY
		if (uwsgi.proxy_socket_name) {
			uwsgi.shared->proxy_pid = proxy_start(uwsgi.master_process);
			uwsgi_will_starts = 1;
		}
#endif

#ifdef UWSGI_UDP
		if (uwsgi.udp_socket) {
			uwsgi_will_starts = 1;
		}
#endif


	}
	if (!uwsgi_will_starts && !uwsgi.no_server) {
		uwsgi_log("The -s/--socket option is missing and stdin is not a socket.\n");
		exit(1);
	}
#ifdef UWSGI_DEBUG
	for (i = 0; i < uwsgi.sockets_cnt; i++) {
		so_bufsize_len = sizeof(int);
		if (getsockopt(uwsgi.sockets[i].fd, SOL_SOCKET, SO_RCVBUF, &so_bufsize, &so_bufsize_len)) {
			uwsgi_error("getsockopt()");
		} else {
			uwsgi_debug("uwsgi socket %d SO_RCVBUF size: %d\n", i, so_bufsize);
		}

		so_bufsize_len = sizeof(int);
		if (getsockopt(uwsgi.sockets[i].fd, SOL_SOCKET, SO_SNDBUF, &so_bufsize, &so_bufsize_len)) {
			uwsgi_error("getsockopt()");
		} else {
			uwsgi_debug("uwsgi socket %d SO_SNDBUF size: %d\n", i, so_bufsize);
		}
	}
#endif

#ifdef UWSGI_ASYNC
	if (uwsgi.async > 1) {
#ifdef __linux__
		uwsgi.async_events = uwsgi_malloc(sizeof(struct epoll_event) * uwsgi.async);
#elif defined(__sun__)
		uwsgi.async_events = uwsgi_malloc(sizeof(struct pollfd) * uwsgi.async);
#else
		uwsgi.async_events = uwsgi_malloc(sizeof(struct kevent) * uwsgi.async);
#endif
	}
#endif




#ifndef UNBIT
	uwsgi_log("your server socket listen backlog is limited to %d connections\n", uwsgi.listen_queue);
#endif


	memset(uwsgi.apps, 0, sizeof(uwsgi.apps));

	uwsgi.workers = (struct uwsgi_worker *) mmap(NULL, sizeof(struct uwsgi_worker) * (uwsgi.numproc + 1 + uwsgi.grunt), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
	if (!uwsgi.workers) {
		uwsgi_error("mmap()");
		exit(1);
	}
	memset(uwsgi.workers, 0, sizeof(struct uwsgi_worker) * uwsgi.numproc + 1);

	uwsgi.mypid = getpid();
	masterpid = uwsgi.mypid;

	if (uwsgi.cores > 1) {
		for (i = 1; i < uwsgi.numproc + 1; i++) {
			uwsgi.workers[i].cores = (struct uwsgi_core **) mmap(NULL, sizeof(struct uwsgi_core *) * uwsgi.cores, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
			if (!uwsgi.workers[i].cores) {
				uwsgi_error("mmap()");
				exit(1);
			}
			memset(uwsgi.workers[i].cores, 0, sizeof(struct uwsgi_core *) * uwsgi.cores);

			for (j = 0; j < uwsgi.cores; j++) {
				uwsgi.workers[i].cores[j] = (struct uwsgi_core *) mmap(NULL, sizeof(struct uwsgi_core), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
				if (!uwsgi.workers[i].cores[j]) {
					uwsgi_error("mmap()");
					exit(1);
				}
				memset(uwsgi.workers[i].cores[j], 0, sizeof(struct uwsgi_core));
			}
		}
	}
	uwsgi.workers[0].pid = masterpid;

	/*

	   uwsgi.shared->hooks[0] = uwsgi_request_wsgi;
	   uwsgi.shared->after_hooks[0] = uwsgi_after_request_wsgi;

	   uwsgi.shared->hooks[UWSGI_MODIFIER_ADMIN_REQUEST] = uwsgi_request_admin;	//10
#ifdef UWSGI_SPOOLER
uwsgi.shared->hooks[UWSGI_MODIFIER_SPOOL_REQUEST] = uwsgi_request_spooler;	//17
#endif
uwsgi.shared->hooks[UWSGI_MODIFIER_EVAL] = uwsgi_request_eval;	//22
uwsgi.shared->hooks[UWSGI_MODIFIER_FASTFUNC] = uwsgi_request_fastfunc;	//26

uwsgi.shared->hooks[UWSGI_MODIFIER_MANAGE_PATH_INFO] = uwsgi_request_wsgi;	// 30
uwsgi.shared->after_hooks[UWSGI_MODIFIER_MANAGE_PATH_INFO] = uwsgi_after_request_wsgi;	// 30

uwsgi.shared->hooks[UWSGI_MODIFIER_MESSAGE_MARSHAL] = uwsgi_request_marshal;	//33
uwsgi.shared->hooks[UWSGI_MODIFIER_PING] = uwsgi_request_ping;	//100
*/

	uwsgi_log("*** Operational MODE: ");
	if (uwsgi.threads > 1) {
		if (uwsgi.numproc > 1) {
			uwsgi_rawlog("preforking+threaded");
		} else {
			uwsgi_rawlog("threaded");
		}
	}
#ifdef UWSGI_ASYNC
	else if (uwsgi.async > 1) {
		if (uwsgi.numproc > 1) {
			uwsgi_rawlog("preforking+async");
		} else {
			uwsgi_rawlog("async");
		}
	}
#endif
	else if (uwsgi.numproc > 1) {
		uwsgi_rawlog("preforking");
	} else {
		uwsgi_rawlog("single process");
	}

	uwsgi_rawlog(" ***\n");

#ifdef UWSGI_ERLANG
	if (uwsgi.erlang_node) {
		uwsgi.erlang_nodes = 1;
		uwsgi.erlangfd = init_erlang(uwsgi.erlang_node, uwsgi.erlang_cookie);
	}
#endif

#ifdef UWSGI_SNMP
	if (uwsgi.snmp) {
		//snmp_init();
	}
#endif


	//init apps hook
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->init_apps) {
			uwsgi.p[i]->init_apps();
		}
	}

	/*parse xml for <app> tags */
#ifdef UWSGI_XML
	if (uwsgi.xml_round2 && uwsgi.xml_config != NULL) {
		uwsgi_xml_config(uwsgi.wsgi_req, 1);
	}
#endif

	for (i = 0; i < uwsgi.mounts_cnt; i++) {
		char *what = strchr(uwsgi.mounts[i], '=');
		if (what) {
			what[0] = 0;
			what++;
			uwsgi_log("mounting %s on %s\n", what, uwsgi.mounts[i]);
			for (j = 0; j < 0xFF; j++) {
                		if (uwsgi.p[j]->mount_app) {
                        		if (uwsgi.p[j]->mount_app(uwsgi.mounts[i], what) != -1) break;
                		}
        		}
			what--; what[0] = '=';
		} else {
			uwsgi_log("invalid mountpoint: %s\n", uwsgi.mounts[i]);
			exit(1);
		}
	}


	if (uwsgi.no_server) {
		uwsgi_log("no-server mode requested. Goodbye.\n");
		exit(0);
	}
	//is this a proxy only worker ?

	if (!uwsgi.master_process && uwsgi.numproc == 0) {
		exit(0);
	}
	if (!uwsgi.single_interpreter) {
		uwsgi_log("*** uWSGI is running in multiple interpreter mode ***\n");
	}

	if (uwsgi.master_process) {
		if (uwsgi.is_a_reload) {
			uwsgi_log("gracefully (RE)spawned uWSGI master process (pid: %d)\n", uwsgi.mypid);
		} else {
			uwsgi_log("spawned uWSGI master process (pid: %d)\n", uwsgi.mypid);
		}
	}
#ifdef UWSGI_SPOOLER
	if (uwsgi.spool_dir != NULL && uwsgi.numproc > 0) {
		uwsgi.shared->spooler_pid = spooler_start();
	}
#endif

#ifdef UWSGI_ROUTING
	routing_setup();
#endif

	if (!uwsgi.master_process) {
		if (uwsgi.numproc == 1) {
			uwsgi_log("spawned uWSGI worker 1 (and the only) (pid: %d, cores: %d)\n", masterpid, uwsgi.cores);
		} else {
			uwsgi_log("spawned uWSGI worker 1 (pid: %d, cores: %d)\n", masterpid, uwsgi.cores);
		}
		uwsgi.workers[1].pid = masterpid;
		uwsgi.workers[1].id = 1;
		uwsgi.workers[1].last_spawn = time(NULL);
		uwsgi.workers[1].manage_next_request = 1;
		uwsgi.mywid = 1;
		gettimeofday(&last_respawn, NULL);
		uwsgi.respawn_delta = last_respawn.tv_sec;
	}
	else {
		// setup internal signalling system
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, uwsgi.shared->worker_signal_pipe)) {
                        uwsgi_error("socketpair()\n");
			exit(1);
                }

	}
	for (i = 2 - uwsgi.master_process; i < uwsgi.numproc + 1; i++) {
		pid = fork();
		if (pid == 0) {
			close(uwsgi.shared->worker_signal_pipe[0]);
			uwsgi.mypid = getpid();
			uwsgi.workers[i].pid = uwsgi.mypid;
			uwsgi.workers[i].id = i;
			uwsgi.workers[i].last_spawn = time(NULL);
			uwsgi.workers[i].manage_next_request = 1;
			uwsgi.mywid = i;
			break;
		} else if (pid < 1) {
			uwsgi_error("fork()");
			exit(1);
		} else {
			uwsgi_log("spawned uWSGI worker %d (pid: %d, cores: %d)\n", i, pid, uwsgi.cores);
			//close(uwsgi.workers[i].pipe[1]);
			gettimeofday(&last_respawn, NULL);
			uwsgi.respawn_delta = last_respawn.tv_sec;
		}
	}


	if (getpid() == masterpid && uwsgi.master_process == 1) {
		master_loop(argv, environ);
		//from now on the process is a real worker
	}
	for (i = 0; i < 0xFF; i++) {
		if (uwsgi.p[i]->post_fork) {
			uwsgi.p[i]->post_fork();
		}
	}

	//postpone the queue initialization as kevent
	//do not pass kfd after fork()
#ifdef UWSGI_ASYNC
	if (uwsgi.async > 1) {
		uwsgi.async_queue = async_queue_init(uwsgi.sockets[0].fd);
		if (uwsgi.async_queue < 0) {
			exit(1);
		}
	}
#endif



	uwsgi.async_hvec = uwsgi_malloc(sizeof(struct iovec *) * uwsgi.cores);
	for (i = 0; i < uwsgi.cores; i++) {
		uwsgi.async_hvec[i] = uwsgi_malloc(sizeof(struct iovec) * uwsgi.vec_size);
	}

	if (uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] > 0 && !uwsgi.master_process) {
		signal(SIGALRM, (void *) &harakiri);
	}
	signal(SIGHUP, (void *) &gracefully_kill);
	signal(SIGINT, (void *) &end_me);
	signal(SIGTERM, (void *) &reload_me);


	signal(SIGUSR1, (void *) &stats);

	signal(SIGUSR2, (void *) &what_i_am_doing);


	signal(SIGPIPE, (void *) &warn_pipe);

	//initialization done

	if (uwsgi.chdir2) {
		if (chdir(uwsgi.chdir2)) {
			uwsgi_error("chdir()");
			exit(1);
		}
	}

#ifdef UWSGI_ERLANG
	if (uwsgi.erlang_nodes > 0) {
		if (uwsgi.numproc <= uwsgi.erlang_nodes) {
			uwsgi_log("You do not have enough worker for Erlang. Please respawn with at least %d processes.\n", uwsgi.erlang_nodes + 1);
		} else if (uwsgi.mywid > (uwsgi.numproc - uwsgi.erlang_nodes)) {
			uwsgi_log("Erlang mode enabled for worker %d.\n", uwsgi.mywid);
			erlang_loop(uwsgi.wsgi_req);
			//NEVER HERE
				exit(1);
		}
	}
	close(uwsgi.erlangfd);
#endif

#ifdef UWSGI_ASYNC
		uwsgi.async_running = -1;
#endif



	//re - initialize wsgi_req(can be full of init_uwsgi_app data)
	for (i = 0; i < uwsgi.cores; i++) {
		memset(uwsgi.wsgi_requests[i], 0, sizeof(struct wsgi_request));
	}


	// eventually remap plugins
	if (uwsgi.remap_modifier) {
		char *map = strtok(uwsgi.remap_modifier, ",");
                while (map != NULL) {
			char *colon = strchr(map, ':');
			if (colon) {
				colon[0] = 0;
				int rm_src = atoi(map);
				int rm_dst = atoi(colon+1);
				uwsgi.p[rm_dst] = uwsgi.p[rm_src]; 
			}	
                        map = strtok(NULL, ",");
                }
	}


	if (uwsgi.master_process) {
       		uwsgi.sockets_poll[uwsgi.sockets_cnt].fd = uwsgi.shared->worker_signal_pipe[1];
        	uwsgi.sockets_poll[uwsgi.sockets_cnt].events = POLLIN;
	}


	if (uwsgi.loop) {
		void (*u_loop) (void) = uwsgi_get_loop(uwsgi.loop);
		uwsgi_log("running %s loop %p\n", uwsgi.loop, u_loop);
		u_loop();
		uwsgi_log("done\n");
		goto end;
	} else {
		if (uwsgi.threads > 1) {
			pthread_attr_t pa;
			pthread_t *a_thread;
			int ret;

			ret = pthread_attr_init(&pa);
			if (ret) {
				uwsgi_log("pthread_attr_init() = %d\n", ret);
				exit(1);
			}
			ret = pthread_attr_setdetachstate(&pa, PTHREAD_CREATE_DETACHED);
			if (ret) {
				uwsgi_log("pthread_attr_setdetachstate() = %d\n", ret);
				exit(1);
			}
			if (pthread_key_create(&uwsgi.tur_key, NULL)) {
				uwsgi_error("pthread_key_create()");
				exit(1);
			}
			for (i = 1; i < uwsgi.threads; i++) {
				long j = i;
				a_thread = uwsgi_malloc(sizeof(pthread_t));
				pthread_create(a_thread, &pa, simple_loop, (void *) j);
			}
		}

		if (uwsgi.async < 2) {
			long y = 0;
			simple_loop((void *) y);
		} else {
			complex_loop();
		}

	}

end:
	if (uwsgi.workers[uwsgi.mywid].manage_next_request == 0) {
		reload_me();
	} else {
		goodbye_cruel_world();
	}

	// never here
	return 0;
}

#ifdef UWSGI_PROXY
	pid_t proxy_start(int has_master) {

		pid_t pid;

		char *tcp_port = strchr(uwsgi.proxy_socket_name, ':');

		if (tcp_port == NULL) {
			uwsgi.proxyfd = bind_to_unix(uwsgi.proxy_socket_name, UWSGI_LISTEN_QUEUE, uwsgi.chmod_socket, uwsgi.abstract_socket);
		} else {
			uwsgi.proxyfd = bind_to_tcp(uwsgi.proxy_socket_name, UWSGI_LISTEN_QUEUE, tcp_port);
			tcp_port[0] = ':';
		}

		if (uwsgi.proxyfd < 0) {
			uwsgi_log("unable to create the server socket.\n");
			exit(1);
		}
		if (!has_master && uwsgi.numproc == 0) {
			uwsgi_proxy(uwsgi.proxyfd);
			//never here
				exit(1);
		} else {
			pid = fork();
			if (pid < 0) {
				uwsgi_error("fork()");
				exit(1);
			} else if (pid > 0) {
				close(uwsgi.proxyfd);
				return pid;
				//continue with uWSGI spawn...
			} else {
				uwsgi_proxy(uwsgi.proxyfd);
				//never here
					exit(1);
			}
		}
	}
#endif

	static int manage_base_opt(int i, char *optarg) {

		char *p;

		switch (i) {

		case 0:
			return 1;
#ifdef UWSGI_UDP
		case LONG_ARGS_CLUSTER_RELOAD:
			send_udp_message(98, optarg, "", 0);
			break;
		case LONG_ARGS_CLUSTER_LOG:
			uwsgi_stdin_sendto(optarg, 96, 0);
			break;
#endif
		case LONG_ARGS_VHOSTHOST:
			uwsgi.vhost = 1;
			uwsgi.vhost_host = 1;
			return 1;
		case LONG_ARGS_LOOP:
			uwsgi.loop = optarg;
			return 1;
		case LONG_ARGS_REMAP_MODIFIER:
			uwsgi.remap_modifier = optarg;
			return 1;
		case LONG_ARGS_PLUGINS:
			p = strtok(optarg, ",");
			while (p != NULL) {
#ifdef UWSGI_DEBUG
				uwsgi_debug("loading plugin %s\n", p);
#endif
				uwsgi_load_plugin(-1, p, NULL, 0);
				p = strtok(NULL, ",");
			}
			 build_options();
			return 1;
		case LONG_ARGS_CHDIR:
			uwsgi.chdir = optarg;
			return 1;
		case LONG_ARGS_CHDIR2:
			uwsgi.chdir2 = optarg;
			return 1;
#ifdef UWSGI_HTTP
		case LONG_ARGS_HTTP:
			uwsgi.http = optarg;
			return 1;
#endif
#ifdef UWSGI_LDAP
		case LONG_ARGS_LDAP:
			uwsgi.ldap = optarg;
			return 1;
		case LONG_ARGS_LDAP_SCHEMA:
			uwsgi_ldap_schema_dump();
			return 1;
		case LONG_ARGS_LDAP_SCHEMA_LDIF:
			uwsgi_ldap_schema_dump_ldif();
			return 1;
#endif
		case LONG_ARGS_MODE:
			uwsgi.mode = optarg;
			return 1;
		case LONG_ARGS_ENV:
			if (putenv(optarg)) {
				uwsgi_error("putenv()");
			}
			return 1;
#ifdef UWSGI_THREADING
		case LONG_ARGS_THREADS:
			uwsgi.threads = atoi(optarg);
			return 1;
#endif
#ifdef UWSGI_ASYNC
		case LONG_ARGS_ASYNC:
			uwsgi.async = atoi(optarg);
			return 1;
#endif
		case LONG_ARGS_LOGTO:
			logto(optarg);
			return 1;
		case LONG_ARGS_LOG_SYSLOG:
			log_syslog(optarg);
			uwsgi.log_master = 1;
			return 1;
		case LONG_ARGS_VERSION:
			fprintf(stdout, "uWSGI %s\n", UWSGI_VERSION);
			exit(0);
#ifdef UWSGI_SNMP
		case LONG_ARGS_SNMP:
			uwsgi.snmp = 1;
			return 1;
		case LONG_ARGS_SNMP_COMMUNITY:
			uwsgi.snmp = 1;
			uwsgi.snmp_community = optarg;
			return 1;
#endif
		case LONG_ARGS_PIDFILE:
			uwsgi.pidfile = optarg;
			return 1;
#ifdef UWSGI_UDP
		case LONG_ARGS_UDP:
			uwsgi.udp_socket = optarg;
			uwsgi.master_process = 1;
			return 1;
#endif
#ifdef UWSGI_MULTICAST
		case LONG_ARGS_MULTICAST:
			uwsgi.multicast_group = optarg;
			uwsgi.master_process = 1;
			return 1;
		case LONG_ARGS_CLUSTER:
			uwsgi.cluster = optarg;
			return 1;
#endif
		case LONG_ARGS_CHROOT:
			uwsgi.chroot = optarg;
			return 1;
		case LONG_ARGS_GID:
			uwsgi.gid = atoi(optarg);
			if (!uwsgi.gid) {
				uwsgi.gidname = optarg;
			}
			return 1;
		case LONG_ARGS_UID:
			uwsgi.uid = atoi(optarg);
			if (!uwsgi.uid) {
				uwsgi.uidname = optarg;
			}
			return 1;
		case LONG_ARGS_BINARY_PATH:
			uwsgi.binary_path = optarg;
			return 1;
#ifdef UWSGI_PROXY
		case LONG_ARGS_PROXY_NODE:
			if (uwsgi.cluster_fd >= 0 && !strcmp(optarg, "@self")) {
				uwsgi.proxy_add_me = 1;
			}
			else {
				uwsgi_cluster_simple_add_node(optarg, 1, CLUSTER_NODE_STATIC);
			}
			return 1;
		case LONG_ARGS_PROXY:
			uwsgi.proxy_socket_name = optarg;
			return 1;
#endif
#ifdef UWSGI_ERLANG
		case LONG_ARGS_ERLANG:
			uwsgi.erlang_node = optarg;
			return 1;
		case LONG_ARGS_ERLANG_COOKIE:
			uwsgi.erlang_cookie = optarg;
			return 1;
#endif
#ifdef UWSGI_HTTP
		case LONG_ARGS_HTTP_VAR:
			if (uwsgi.http_vars_cnt < 63) {
				uwsgi.http_vars[uwsgi.http_vars_cnt] = optarg;
				uwsgi.http_vars_cnt++;
			} else {
				uwsgi_log("you can specify at most 64 --http-var options\n");
			}
			return 1;
		case LONG_ARGS_HTTP_MODIFIER1:
			uwsgi.http_modifier1 = (uint8_t) atoi(optarg);
			return 1;
#endif
#ifdef __linux__
		case LONG_ARGS_CGROUP:
			uwsgi.cgroup = optarg;
			return 1;
		case LONG_ARGS_CGROUP_OPT:
			if (uwsgi.cgroup_opt_cnt < 63) {
				uwsgi.cgroup_opt[uwsgi.cgroup_opt_cnt] = optarg;
				uwsgi.cgroup_opt_cnt++;
			} else {
				uwsgi_log("you can specify at most 64 --cgroup_opt options\n");
			}
			return 1;
		case LONG_ARGS_LINUX_NS:
			uwsgi.ns = optarg;
			return 1;
#endif
		case LONG_ARGS_LIMIT_AS:
			uwsgi.rl.rlim_cur = (atoi(optarg)) * 1024 * 1024;
			uwsgi.rl.rlim_max = uwsgi.rl.rlim_cur;
			return 1;
		case LONG_ARGS_LIMIT_POST:
			uwsgi.limit_post = (int) strtol(optarg, NULL, 10);
			return 1;
		case LONG_ARGS_PRIO:
			uwsgi.prio = (int) strtol(optarg, NULL, 10);
			return 1;
		case LONG_ARGS_POST_BUFFERING:
			uwsgi.post_buffering = atoi(optarg);
			return 1;
		case LONG_ARGS_POST_BUFFERING_SIZE:
			uwsgi.post_buffering_bufsize = atoi(optarg);
			return 1;
		case LONG_ARGS_UPLOAD_PROGRESS:
			uwsgi.upload_progress = optarg;
			return 1;
#ifdef UWSGI_YAML
		case 'y':
			uwsgi.yaml = optarg;
			return 1;
#endif
#ifdef UWSGI_INI
		case LONG_ARGS_INI:
			uwsgi.ini = optarg;
			return 1;
/*
		case LONG_ARGS_EVAL_CONFIG:
			uwsgi.eval = optarg;
			return 1;
*/
#ifdef UWSGI_PASTE
		case LONG_ARGS_INI_PASTE:
			uwsgi.ini = optarg;
			if (uwsgi.ini[0] != '/') {
				uwsgi.paste = uwsgi_malloc(7 + strlen(uwsgi.cwd) + 1 + strlen(uwsgi.ini) + 1);
				memset(uwsgi.paste, 0, 7 + strlen(uwsgi.cwd) + strlen(uwsgi.ini) + 1);
				memcpy(uwsgi.paste, "config:", 7);
				memcpy(uwsgi.paste + 7, uwsgi.cwd, strlen(uwsgi.cwd));
				uwsgi.paste[7 + strlen(uwsgi.cwd)] = '/';
				memcpy(uwsgi.paste + 7 + strlen(uwsgi.cwd) + 1, uwsgi.ini, strlen(uwsgi.ini));
			} else {
				uwsgi.paste = uwsgi_malloc(7 + strlen(uwsgi.ini) + 1);
				memset(uwsgi.paste, 0, 7 + strlen(uwsgi.ini) + 1);
				memcpy(uwsgi.paste, "config:", 7);
				memcpy(uwsgi.paste + 7, uwsgi.ini, strlen(uwsgi.ini));
			}
			return 1;
#endif
#endif
#ifdef UWSGI_PASTE
		case LONG_ARGS_PASTE:
			uwsgi.paste = optarg;
			return 1;
#endif
		case LONG_ARGS_CHECK_INTERVAL:
			uwsgi.shared->options[UWSGI_OPTION_MASTER_INTERVAL] = atoi(optarg);
			return 1;
		case LONG_ARGS_CACHE:
			uwsgi.cache_max_items = atoi(optarg);
			return 1;
		case 'A':
			uwsgi.sharedareasize = atoi(optarg);
			return 1;
		case 'L':
			uwsgi.shared->options[UWSGI_OPTION_LOGGING] = 0;
			return 1;
		case LONG_ARGS_LOG_ZERO:
			uwsgi.shared->options[UWSGI_OPTION_LOG_ZERO] = 1;
			return 1;
		case LONG_ARGS_LOG_SLOW:
			uwsgi.shared->options[UWSGI_OPTION_LOG_SLOW] = atoi(optarg);
			return 1;
		case LONG_ARGS_LOG_4xx:
			uwsgi.shared->options[UWSGI_OPTION_LOG_4xx] = 1;
			return 1;
		case LONG_ARGS_LOG_5xx:
			uwsgi.shared->options[UWSGI_OPTION_LOG_5xx] = 1;
			return 1;
		case LONG_ARGS_LOG_BIG:
			uwsgi.shared->options[UWSGI_OPTION_LOG_BIG] = atoi(optarg);
			return 1;
		case LONG_ARGS_LOG_SENDFILE:
			uwsgi.shared->options[UWSGI_OPTION_LOG_SENDFILE] = 1;
			return 1;
		case LONG_ARGS_MOUNT:
			if (uwsgi.mounts_cnt < MAX_APPS) {
				uwsgi.mounts[uwsgi.mounts_cnt] = optarg;
				uwsgi.mounts_cnt++;
			} else {
				uwsgi_log("you can specify at most %d --mount options\n", MAX_APPS);
			}
			return 1;
#ifdef UWSGI_SPOOLER
		case 'Q':
			uwsgi.spool_dir = uwsgi_malloc(PATH_MAX);
			if (access(optarg, R_OK | W_OK | X_OK)) {
				uwsgi_error("[spooler directory] access()");
				exit(1);
			}
			if (!realpath(optarg, uwsgi.spool_dir)) {
				uwsgi_error("realpath()");
				exit(1);
			}
			uwsgi.master_process = 1;
			return 1;
#endif

		case 'd':
			if (!uwsgi.is_a_reload) {
				daemonize(optarg);
			}
			return 1;
		case 's':
			if (uwsgi.sockets_cnt < 8) {
				uwsgi.sockets[uwsgi.sockets_cnt].name = generate_socket_name(optarg);
				uwsgi.sockets_cnt++;
			} else {
				uwsgi_log("you can specify at most 8 --socket options\n");
			}
			return 1;
#ifdef UWSGI_XML
		case 'x':
			uwsgi.xml_config = optarg;
			return 1;
#endif
		case 'l':
			uwsgi.listen_queue = atoi(optarg);
			return 1;
		case 'v':
			uwsgi.max_vars = atoi(optarg);
			uwsgi.vec_size = 4 + 1 + (4 * uwsgi.max_vars);
			return 1;
		case 'p':
			uwsgi.numproc = atoi(optarg);
			return 1;
		case 'r':
			uwsgi.shared->options[UWSGI_OPTION_REAPER] = 1;
			return 1;
		case 'm':
			uwsgi.shared->options[UWSGI_OPTION_MEMORY_DEBUG] = 1;
			return 1;
		case 't':
			uwsgi.shared->options[UWSGI_OPTION_HARAKIRI] = atoi(optarg);
			return 1;
		case 'b':
			uwsgi.buffer_size = atoi(optarg);
			return 1;
		case 'c':
			uwsgi.shared->options[UWSGI_OPTION_CGI_MODE] = 1;
			return 1;
		case 'a':
			uwsgi.abstract_socket = 1;
			return 1;
		case LONG_ARGS_LOG_DATE:
			uwsgi.logdate = 1;
			if (optarg) {
				uwsgi.log_strftime = optarg;
			}
			return 1;
		case 'C':
			uwsgi.chmod_socket = 1;
			if (optarg) {
				if (strlen(optarg) != 3) {
					uwsgi_log("invalid chmod value: %s\n", optarg);
					exit(1);
				}
				for (i = 0; i < 3; i++) {
					if (optarg[i] < '0' || optarg[i] > '7') {
						uwsgi_log("invalid chmod value: %s\n", optarg);
						exit(1);
					}
				}

				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[0] - '0');
				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[1] - '0');
				uwsgi.chmod_socket_value = (uwsgi.chmod_socket_value << 3) + (optarg[2] - '0');
			}
			return 1;
		case 'M':
			uwsgi.master_process = 1;
			return 1;
		case 'R':
			uwsgi.shared->options[UWSGI_OPTION_MAX_REQUESTS] = atoi(optarg);
			return 1;
		case 'z':
			if (atoi(optarg) > 0) {
				uwsgi.shared->options[UWSGI_OPTION_SOCKET_TIMEOUT] = atoi(optarg);
			}
			return 1;
		case 'T':
			uwsgi.has_threads = 1;
			uwsgi.shared->options[UWSGI_OPTION_THREADS] = 1;
			return 1;
		case 'i':
			uwsgi.single_interpreter = 1;
			return 1;
		case 'h':
			fprintf(stdout, "Usage: %s [options...]\n\
\t-s|--socket <name>\t\tpath (or name) of UNIX/TCP socket to bind to\n\
\t-l|--listen <num>\t\tset socket listen queue to <n> (default 64, maximum is system dependent)\n\
\t-z|--socket-timeout <sec>\tset socket timeout to <sec> seconds (default 4 seconds)\n\
\t-b|--buffer-size <n>\t\tset buffer size to <n> bytes\n\
\t-L|--disable-logging\t\tdisable request logging (only errors or server messages will be logged)\n\
\t-x|--xmlconfig <path>\t\tpath of xml config file\n\
\t-w|--module <module>\t\tname of python config module\n\
\t-t|--harakiri <sec>\t\tset harakiri timeout to <sec> seconds\n\
\t-p|--processes <n>\t\tspawn <n> uwsgi worker processes\n\
\t-O|--optimize <n>\t\tset python optimization level to <n>\n\
\t-v|--max-vars <n>\t\tset maximum number of vars/headers to <n>\n\
\t-A|--sharedarea <n>\t\tcreate a shared memory area of <n> pages\n\
\t-c|--cgi-mode\t\t\tset cgi mode\n\
\t-C|--chmod-socket[=NNN]\t\tchmod socket to 666 or NNN\n\
\t-m|--memory-report\t\tenable memory usage report\n\
\t-i|--single-interpreter\t\tsingle interpreter mode\n\
\t-a|--abstract-socket\t\tset socket in the abstract namespace (Linux only)\n\
\t-T|--enable-threads\t\tenable threads support\n\
\t-M|--master\t\t\tenable master process manager\n\
\t-H|--home <path>\t\tset python home/virtualenv\n\
\t-h|--help\t\t\tthis help\n\
\t-r|--reaper\t\t\tprocess reaper (call waitpid(-1,...) after each request)\n\
\t-R|--max-requests\t\tmaximum number of requests for each worker\n\
\t-j|--test\t\t\ttest if uWSGI can import a module\n\
\t-Q|--spooler <dir>\t\trun the spooler on directory <dir>\n\
\t--callable <callable>\t\tset the callable (default 'application')\n\
\t--pidfile <file>\t\twrite the masterpid to <file>\n\
\t--chroot <dir>\t\t\tchroot to directory <dir> (only root)\n\
\t--gid <id/groupname>\t\tsetgid to <id/groupname> (only root)\n\
\t--uid <id/username>\t\tsetuid to <id/username> (only root)\n\
\t--chdir <dir>\t\t\tchdir to <dir> before app loading\n\
\t--chdir2 <dir>\t\t\tchdir to <dir> after module loading\n\
\t--no-server\t\t\tinitialize the uWSGI server then exit. Useful for testing and using uwsgi embedded module\n\
\t--no-defer-accept\t\tdisable the no-standard way to defer the accept() call (TCP_DEFER_ACCEPT, SO_ACCEPTFILTER...)\n\
\t--paste <config:/egg:>\t\tload applications using paste.deploy.loadapp()\n\
\t--check-interval <sec>\t\tset the check interval (in seconds) of the master process\n\
\t--pythonpath <dir>\t\tadd <dir> to PYTHONPATH\n\
\t--python-path <dir>\t\tadd <dir> to PYTHONPATH\n\
\t--pp <dir>\t\t\tadd <dir> to PYTHONPATH\n\
\t--pyargv <args>\t\t\tassign args to python sys.argv\n\
\t--limit-as <MB>\t\t\tlimit the address space of processes to MB megabytes\n\
\t--limit-post <bytes>\t\tlimit HTTP content_length size to <bytes>\n\
\t--post-buffering <bytes>\tbuffer HTTP POST request higher than <bytes> to disk\n\
\t--post-buffering-bufsize <b>\tset the buffer size to <b> bytes for post-buffering\n\
\t--prio <N>\t\t\tset process priority/nice to N\n\
\t--no-orphans\t\t\tautomatically kill workers on master's dead\n\
\t--udp <ip:port>\t\t\tbind master process to udp socket on ip:port\n\
\t--multicast <group>\t\tset multicast group\n\
\t--snmp\t\t\t\tenable SNMP support in the UDP server\n\
\t--snmp-community <value>\tset SNMP community code to <value>\n\
\t--erlang <name@ip>\t\tenable the Erlang server with node name <node@ip>\n\
\t--erlang-cookie <cookie>\tset the erlang cookie to <cookie>\n\
\t--nagios\t\t\tdo a nagios check\n\
\t--binary-path <bin-path>\tset the path for the next reload of uWSGI (needed for chroot environments)\n\
\t--proxy <socket>\t\trun the uwsgi proxy on socket <socket>\n\
\t--proxy-node <socket>\t\tadd the node <socket> to the proxy\n\
\t--proxy-max-connections <n>\tset the max number of concurrent connections mnaged by the proxy\n\
\t--wsgi-file <file>\t\tload the <file> wsgi file\n\
\t--file <file>\t\t\tuse python file instead of python module for configuration\n\
\t--eval <code>\t\t\tevaluate code for app configuration\n\
\t--async <n>\t\t\tenable async mode with n core\n\
\t--logto <logfile|addr>\t\tlog to file/udp\n\
\t--logdate\t\t\tadd timestamp to loglines\n\
\t--log-zero\t\t\tlog requests with 0 response size\n\
\t--log-slow <t>\t\t\tlog requests slower than <t> milliseconds\n\
\t--log-4xx\t\t\tlog requests with status code 4xx\n\
\t--log-5xx\t\t\tlog requests with status code 5xx\n\
\t--log-big <n>\t\t\tlog requests bigger than <n> bytes\n\
\t--log-sendfile\t\t\tlog sendfile() requests\n\
\t--ignore-script-name\t\tdisable uWSGI management of SCRIPT_NAME\n\
\t--no-default-app\t\tdo not fallback unknown SCRIPT_NAME requests\n\
\t--ini <inifile>\t\t\tpath of ini config file\n\
\t--ini-paste <inifile>\t\tpath of ini config file that contains paste configuration\n\
\t--ldap <url>\t\t\turl of LDAP uWSGIConfig resource\n\
\t--ldap-schema\t\t\tdump uWSGIConfig LDAP schema\n\
\t--ldap-schema-ldif\t\tdump uWSGIConfig LDAP schema in LDIF format\n\
\t--grunt\t\t\t\tenable grunt workers\n\
\t--ugreen\t\t\tenable uGreen support\n\
\t--ugreen-stacksize <n>\t\tset uGreen stacksize to <n>\n\
\t--stackless\t\t\tenable usage of tasklet (only on Stackless Python)\n\
\t--no-site\t\t\tdo not import site.py on startup\n\
\t--vhost\t\t\t\tenable virtual hosting\n\
\t--mount MOUNTPOINT=app\t\tadda new app under MOUNTPOINT\n\
\t--routing\t\t\tenable uWSGI advanced routing\n\
\t--http <addr>\t\t\tstart embedded HTTP server on <addr>\n\
\t--http-only\t\t\tstart only the embedded HTTP server\n\
\t--http-var KEY[=VALUE]\t\tadd var KEY to uwsgi requests made by the embedded HTTP server\n\
\t--catch-exceptions\t\tprint exceptions in the browser\n\
\t--mode\t\t\t\tset configuration mode\n\
\t--env KEY=VALUE\t\t\tset environment variable\n\
\t--vacuum\t\t\tclear the environment on exit (remove UNIX sockets and pidfiles)\n\
\t--ping <addr>\t\t\tping a uWSGI server (returns 1 on failure 0 on success)\n\
\t--ping-timeout <n>\t\tset ping timeout to <n>\n\
\t--cgroup <group>\t\trun the server in <group> cgroup (Linux only)\n\
\t--cgroup-opt KEY=VAL\t\tset cgroup option (Linux only)\n\
\t--version\t\t\tprint server version\n\
\t-d|--daemonize <logfile|addr>\tdaemonize and log into <logfile> or udp <addr>\n", uwsgi.binary_path);
			return 0;
		}

		return 0;
	}

	void manage_opt(int i, char *optarg) {

		int j;

		if (manage_base_opt(i, optarg)) {
			add_exported_option( i, optarg );
			return;
		}

		for (j = 0; j < 0xFF; j++) {
			if (uwsgi.p[j]->manage_opt) {
				if (uwsgi.p[j]->manage_opt(i, optarg)) {
					add_exported_option( i, optarg );
					return;
				}
			}
		}

		for (j = 0; j < uwsgi.gp_cnt; j++) {
			if (uwsgi.gp[j]->manage_opt) {
				if (uwsgi.gp[j]->manage_opt(i, optarg)) {
					add_exported_option( i, optarg );
					return;
				}
			}
		}

		//never here
		exit(1);

	}

void uwsgi_cluster_simple_add_node(char *nodename, int workers, int type) {

	int i;
	struct uwsgi_cluster_node *ucn;
	char *tcp_port;

	if (strlen(nodename) > 100) {
		uwsgi_log("invalid cluster node name %s\n", nodename);
		return;
	}

	tcp_port = strchr(nodename, ':');
	if (tcp_port == NULL) {
		fprintf(stdout, "invalid cluster node name %s\n", nodename);
		return;
	}

	// first check for already present node
	for (i = 0; i < MAX_CLUSTER_NODES; i++) {
                ucn = &uwsgi.shared->nodes[i];
		if (ucn->name[0] != 0) {
			if (!strcmp(ucn->name, nodename)) {
				ucn->status = UWSGI_NODE_OK;			
				ucn->last_seen = time(NULL);
				return;
			}
		}
	}

	for (i = 0; i < MAX_CLUSTER_NODES; i++) {
		ucn = &uwsgi.shared->nodes[i];

		if (ucn->name[0] == 0) {
			memcpy(ucn->name, nodename, strlen(nodename) + 1);
			ucn->workers = workers;
			ucn->ucn_addr.sin_family = AF_INET;
			ucn->ucn_addr.sin_port = htons(atoi(tcp_port + 1));
			tcp_port[0] = 0;
			if (nodename[0] == 0) {
				ucn->ucn_addr.sin_addr.s_addr = INADDR_ANY;
			} else {
			uwsgi_log("%s\n", nodename);
				ucn->ucn_addr.sin_addr.s_addr = inet_addr(nodename);
			}

			ucn->type = type;
			// here memory can be freed, as it is allocated by uwsgi_concat2n
			if (type == CLUSTER_NODE_DYNAMIC) {
				free(nodename);
			}
			else {
				tcp_port[0] = ':';
			}
			ucn->last_seen = time(NULL);
			uwsgi_log("[uWSGI cluster] added node %s\n", ucn->name);
			return;
		}
	}

	uwsgi_log("unable to add node %s\n", nodename);
}

void uwsgi_cluster_add_node(struct uwsgi_cluster_node *nucn, int type) {

        int i;
        struct uwsgi_cluster_node *ucn;
        char *tcp_port;

	uwsgi_log("adding node\n");

        tcp_port = strchr(nucn->name, ':');
        if (tcp_port == NULL) {
                fprintf(stdout, "invalid cluster node name %s\n", nucn->name);
                return;
        }

        // first check for already present node
        for (i = 0; i < MAX_CLUSTER_NODES; i++) {
                ucn = &uwsgi.shared->nodes[i];
                if (ucn->name[0] != 0) {
                        if (!strcmp(ucn->name, nucn->name)) {
                                ucn->status = UWSGI_NODE_OK;
                                ucn->last_seen = time(NULL);
				// update requests
				ucn->requests = nucn->requests;
                                return;
                        }
                }
        }

        for (i = 0; i < MAX_CLUSTER_NODES; i++) {
                ucn = &uwsgi.shared->nodes[i];

                if (ucn->name[0] == 0) {
                        memcpy(ucn->name, nucn->name, strlen(nucn->name) + 1);
                        memcpy(ucn->nodename, nucn->nodename, strlen(nucn->nodename) + 1);
                        ucn->workers = nucn->workers;
                        ucn->ucn_addr.sin_family = AF_INET;
                        ucn->ucn_addr.sin_port = htons(atoi(tcp_port + 1));
                        tcp_port[0] = 0;
                        if (nucn->name[0] == 0) {
                                ucn->ucn_addr.sin_addr.s_addr = INADDR_ANY;
                        } else {
                        uwsgi_log("%s\n", nucn->name);
                                ucn->ucn_addr.sin_addr.s_addr = inet_addr(nucn->name);
                        }

                        ucn->type = type;
                        // here memory can be freed, as it is allocated by uwsgi_concat2n
                        if (type != CLUSTER_NODE_DYNAMIC) {
                                tcp_port[0] = ':';
                        }
                        ucn->last_seen = time(NULL);
			ucn->requests = nucn->requests;
                        uwsgi_log("[uWSGI cluster] added node %s\n", ucn->name);
                        return;
                }
        }

        uwsgi_log("unable to add node %s\n", nucn->name);
}



void build_options() {
		int i;
		struct option *lopt,
		*aopt;
		int opt_count = count_options(long_base_options);
		int short_opt_size = strlen(base_short_options);
		char *so_ptr;

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->short_options) {
				short_opt_size += strlen(uwsgi.p[i]->short_options);
			}
		}

		for(i=0;i<uwsgi.gp_cnt;i++) {
			if (uwsgi.gp[i]->short_options) {
				short_opt_size += strlen(uwsgi.gp[i]->short_options);
			}
		}

		if (short_options) {
			free(short_options);
		}
		short_options = uwsgi_malloc(short_opt_size + 1);
		memcpy(short_options, base_short_options, strlen(base_short_options));
		so_ptr = short_options + strlen(base_short_options);

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->short_options) {
				memcpy(so_ptr, uwsgi.p[i]->short_options, strlen(uwsgi.p[i]->short_options));
				so_ptr += strlen(uwsgi.p[i]->short_options);
			}
		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			if (uwsgi.gp[i]->short_options) {
				memcpy(so_ptr, uwsgi.gp[i]->short_options, strlen(uwsgi.gp[i]->short_options));
				so_ptr += strlen(uwsgi.gp[i]->short_options);
			}
		}

		*so_ptr = 0;

		for (i = 0; i < 0xFF; i++) {
			if (uwsgi.p[i]->options) {
				opt_count += count_options(uwsgi.p[i]->options);
			}
		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			if (uwsgi.gp[i]->options) {
				opt_count += count_options(uwsgi.gp[i]->options);
			}
		}

		if (uwsgi.long_options) {
			free(uwsgi.long_options);
		}
		uwsgi.long_options = uwsgi_malloc(sizeof(struct option) * (opt_count + 1));
		opt_count = 0;
		lopt = long_base_options;
		while ((aopt = lopt)) {
			if (!aopt->name)
				break;
			uwsgi.long_options[opt_count].name = aopt->name;
			uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
			uwsgi.long_options[opt_count].flag = aopt->flag;
			uwsgi.long_options[opt_count].val = aopt->val;
			opt_count++;
			lopt++;
		}

		for (i = 0; i < 0xFF; i++) {
			lopt = uwsgi.p[i]->options;
			if (!lopt)
				continue;

			while ((aopt = lopt)) {
				if (!aopt->name)
					break;
				uwsgi.long_options[opt_count].name = aopt->name;
				uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
				uwsgi.long_options[opt_count].flag = aopt->flag;
				uwsgi.long_options[opt_count].val = aopt->val;
				opt_count++;
				lopt++;
			}

		}

		for (i = 0; i < uwsgi.gp_cnt; i++) {
			lopt = uwsgi.gp[i]->options;
			if (!lopt)
				continue;

			while ((aopt = lopt)) {
				if (!aopt->name)
					break;
				uwsgi.long_options[opt_count].name = aopt->name;
				uwsgi.long_options[opt_count].has_arg = aopt->has_arg;
				uwsgi.long_options[opt_count].flag = aopt->flag;
				uwsgi.long_options[opt_count].val = aopt->val;
				opt_count++;
				lopt++;
			}

		}

		uwsgi.long_options[opt_count].name = 0;
		uwsgi.long_options[opt_count].has_arg = 0;
		uwsgi.long_options[opt_count].flag = 0;
		uwsgi.long_options[opt_count].val = 0;
}


void manage_string_opt(char *key, uint16_t keylen, char *val, uint16_t vallen, void *data) {

	struct option *lopt, *aopt;

	// never free this value
	char *key2 = uwsgi_concat2n(key, keylen, "", 0);
	char *val2 = uwsgi_concat2n(val, vallen, "", 0);

	uwsgi_log("%s = %s\n", key2, val2);
	lopt = uwsgi.long_options;
        while ((aopt = lopt)) {
        	if (!aopt->name) break;
                if (!strcmp(key2, aopt->name)) {
                	if (aopt->flag) {
                        	*aopt->flag = aopt->val;
                                add_exported_option(0, key2);
                        }
                        else {
                        	manage_opt(aopt->val, val2);
                        }
                }
                lopt++;
	}
}

#ifdef UWSGI_UDP
int uwsgi_cluster_add_me() {

	const char *key1 = "hostname";
	const char *key2 = "address";
	const char *key3 = "workers";
	const char *key4 = "requests";

	char *ptrbuf ;
	uint16_t ustrlen;
	char numproc[6];

	snprintf(numproc, 6, "%d", uwsgi.numproc);

	size_t len = 2 + strlen(key1) + 2 + strlen(uwsgi.hostname) + 2 + strlen(key2) + 2 + strlen(uwsgi.sockets[0].name) + 2 + strlen(key3) + 2 + strlen(numproc) + 2 + strlen(key4) + 2 + 1;
	char *buf = uwsgi_malloc( len );

	ptrbuf = buf;

	ustrlen = strlen(key1);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, key1, strlen(key1)); ptrbuf+=strlen(key1);

	ustrlen = strlen(uwsgi.hostname);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, uwsgi.hostname, strlen(uwsgi.hostname)); ptrbuf+=strlen(uwsgi.hostname);

	ustrlen = strlen(key2);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, key2, strlen(key2)); ptrbuf+=strlen(key2);

	ustrlen = strlen(uwsgi.sockets[0].name);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, uwsgi.sockets[0].name, strlen(uwsgi.sockets[0].name)); ptrbuf+=strlen(uwsgi.sockets[0].name);


	ustrlen = strlen(key3);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, key3, strlen(key3)); ptrbuf+=strlen(key3);

	ustrlen = strlen(numproc);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, numproc, strlen(numproc)); ptrbuf+=strlen(numproc);

	ustrlen = strlen(key4);
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, key4, strlen(key4)); ptrbuf+=strlen(key4);

	ustrlen = 1;
        *ptrbuf++ = (uint8_t) (ustrlen  & 0xff);
        *ptrbuf++ = (uint8_t) ((ustrlen >>8) & 0xff);
	memcpy(ptrbuf, "0", 1); ptrbuf+=1;


	uwsgi_string_sendto(uwsgi.cluster_fd, 95, 0, (struct sockaddr *) &uwsgi.mc_cluster_addr, sizeof(uwsgi.mc_cluster_addr), buf, len);

	free(buf);

	uwsgi_log("add_me() successfull\n");

	return 0;
}

int uwsgi_cluster_join(char *name) {

	int fd ;
	char *cp;


	fd = bind_to_udp(name, 1);

	if (fd >= 0) {
		cp = strchr(name,':');
                cp[0] = 0;
                uwsgi.mc_cluster_addr.sin_family=AF_INET;
                uwsgi.mc_cluster_addr.sin_addr.s_addr=inet_addr(name);
                uwsgi.mc_cluster_addr.sin_port=htons(atoi(cp+1));
                cp[0] = ':';


		// announce my presence to all the nodes
		uwsgi_string_sendto(fd, 73, 0, (struct sockaddr *) &uwsgi.mc_cluster_addr, sizeof(uwsgi.mc_cluster_addr), uwsgi.hostname, strlen(uwsgi.hostname));
	}

	
	return fd;

}

void uwsgi_stdin_sendto(char *socket_name, uint8_t modifier1, uint8_t modifier2) {

	char buf[4096];
	ssize_t rlen ;
	size_t delta = 4096;
	char *ptr = buf;

	rlen = read(0, ptr, delta);
	while(rlen > 0) {
		uwsgi_log("%.*s\n", rlen, ptr);
		ptr += rlen;
		delta-=rlen;
		if (delta <= 0) break;
		rlen = read(0, ptr, delta);
	}
	
	if (ptr > buf) {
		send_udp_message(modifier1, socket_name, buf, ptr-buf);
		uwsgi_log("sent string \"%.*s\" to cluster node %s", ptr-buf, buf, socket_name);
	}
	
}
#endif


char *uwsgi_cluster_best_node() {
	
	int i;
	int best_node = -1;
	struct uwsgi_cluster_node *ucn;

	for (i = 0; i < MAX_CLUSTER_NODES; i++) {
                ucn = &uwsgi.shared->nodes[i];
                if (ucn->name[0] != 0 && ucn->status == UWSGI_NODE_OK) {
			if (best_node == -1) {
				best_node = i;
			}
			else {
				if (ucn->last_choosen < uwsgi.shared->nodes[best_node].last_choosen) {
					best_node = i;
				}
			}
                }
        }

	if (best_node == -1) {
		return NULL;
	}

	uwsgi.shared->nodes[best_node].last_choosen = time(NULL);
	return uwsgi.shared->nodes[best_node].name;
}
