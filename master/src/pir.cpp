#include "pir.h"

struct serverType server_type[] =
{
	{0x00, "PIR Unknown", "unknown process", NULL, NULL, 0, "1.0.0"},
	{0x01, "PIR Sentinel", "sentinel process", NULL, "../../src/serlaunch.wsdl", 1, "1.0.0"},
	{0x02, "PIR Master", "master process", NULL, "../../src/Imsearch.wsdl", 1, "1.0.0"},
	{0x03, "PIR Worker", "worker process", NULL, "../../src/Imsearch.wsdl", 1, "1.0.0"}
};

struct serverState server_state[] =
{
	{0x00, "pir_state_none", "the server is waiting for initialization.", 0, "1.0.0"},
	{0x01, "pir_state_initing", "the server is initializing configuration parameters.", 1, "1.0.0"},
	{0x02, "pir_state_inited", "the server is successfully inited.", 1, "1.0.0"},
	{0x03, "pir_state_training", "the server is trainning template images.", 2, "1.0.0"},
	{0x04, "pir_state_trained", "the server is successfully trained.", 2, "1.0.0"},
	{0x05, "pir_state_starting", "the server is statring up.", 3, "1.0.0"},
	{0x06, "pir_state_ready", "the server is ready for retrieval.", 3, "1.0.0"}
};

int32_t initServerConfig(struct pirServer* server)
{
	server->pid=getpid();
	server->pidfile=NULL;
	server->configfile=strdup(PIR_CONFIG_FILE);
	server->flags=PIR_MODE_MASTER;
	server->unixtime=time(NULL);
	server->argc=0;
	server->argv=NULL;
	server->hz=PIR_DEFAULT_HZ;
	server->arch_bits=(sizeof(long)==8)?64:32;
	server->serverstate=PIR_STATE_NONE;
	server->autostart=PIR_DEFAULT_AUTOSTART;
	server->daemonize=PIR_DEFAULT_DAEMONIZE;
	server->failover_policy=PIR_DEFAULT_FAILOVER_POLICY;
	server->shutdown_asap=0;
	server->progressPCT=0.0;
	server->proct_prefix=strdup(PIR_PROC_PREFIX);
	server->proct_type=NULL;
	server->wsdl_path=NULL;
	server->setting_xml_path=strdup(PIR_SETTING_XML_PATH);
	server->setting_xml=strdup(PIR_SETTING_XML);
	server->maxclients=PIR_MAX_CLIENTS;
	server->maxmemory=PIR_MAX_MEMORY;
	server->thread_num=PIR_THREAD_NUM;
	server->thread_timeout=PIR_THREAD_TIMEOUT;
	server->thread_buf_size=PIR_THREAD_BUF_SIZE;
	server->thread_info=NULL;
	server->tid=NULL;
	server->ipv4.s_addr=htons(INADDR_ANY);
	server->port=PIR_SERVER_PORT;
	server->tcp_backlog=PIR_TCP_BACKLOG;
	server->lastinteraction=time(NULL);
	server->master=PIR_INVALID_SOCKET;
	server->conn_timeout=PIR_CONN_TIMEOUT;
	server->recv_timeout=PIR_RECV_TIMEOUT;
	server->send_timeout=PIR_SEND_TIMEOUT;
	server->bucketSize=0;
	server->bucketMax=PIR_MAX_BUCKET_SIZE;
	server->workerInfoFile=strdup(PIR_DEFAULT_WORKER_INFO_FILE);
	server->client_queue=NULL;
	server->client_queue_trigger=NULL;
	server->client_search_queue=NULL;
	server->ranking=DescendingSort;
	server->ranking_key_max=PIR_RANKING_KEY_MAX;
	server->query_num=PIR_DEFAULT_QUERY_NUM;
	server->ocr_enabled=PIR_DEFAULT_OCR_ENABLED;
	server->ocr_server=NULL;
	server->delta_for_training=0;
	server->delta_peak=PIR_DEFAULT_DELTA_PEAK;
	server->delta_ratio=PIR_DEFAULT_DELTA_RATIO;
	server->idletime_for_training=PIR_DEFAULT_IDLETIME;
	server->log_path=NULL;
	server->log_prefix=NULL;
	server->log_screen=PIR_DEFAULT_LOG_SCREEN;
	server->log_size=PIR_DEFAULT_LOG_SIZE;
	server->logger=NULL;
	server->log_query_cache_size=PIR_LOG_QUERY_CACHE_SIZE;
	server->watchdog_mtime=0;
	server->watchdog_now=PIR_WATCHDOG_NOW;
	server->watchdog_interval=PIR_WATCHDOG_INTERVAL;
	server->monitor_enabled=PIR_DEFAULT_MONITOR_ENABLED;
	server->monitor_port=PIR_DEFAULT_MONITOR_PORT;
	server->stat_starttime=time(NULL);
	server->stat_numconnections=0;
	server->stat_search_conn=0;
	server->stat_rejected_conn=0;
	server->stat_failed_conn=0;
	return PIR_OK;
}

int32_t initServerOptions(struct pirServer* server, int32_t argc, char** argv)
{
	server->argc=argc;
	server->argv=(char**)q_malloc(server->argc*sizeof(char*));
	if(server->argv==NULL)
		return PIR_ERR;

	for(int32_t i=0; i<server->argc; ++i)
		server->argv[i]=strdup(argv[i]);

	server->daemonize=checkForDaemonizeMode(server->argc, server->argv);
	server->failover_policy=checkForRecoveryMode(server->argc, server->argv);

	return PIR_OK;
}

int32_t loadServerConfig(struct pirServer* server)
{
	QConfigReader config;
	int32_t ret=0;

	if(config.init(server->configfile)<0)
		return PIR_ERR;

	int32_t flag=0;
	ret=config.getFieldYesNo("daemonize", flag);
	if(ret<0)
		return PIR_ERR;

	if(flag)
		server->daemonize=1;

	ret=config.getFieldString("pidfile", server->pidfile);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("tcp-backlog", server->tcp_backlog);
	if(ret<0)
		return PIR_ERR;

#if 0
	flag=0;
	ret=config.getFieldYesNo("failover-policy", flag);
	if(ret<0)
		return PIR_ERR;

	if(flag)
		server->failover_policy=1;
#endif

	ret=config.getFieldUint32("max-clients", server->maxclients);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldUint8("thread-num", server->thread_num);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("thread-timeout", server->thread_timeout);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("conn-timeout", server->conn_timeout);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("recv-timeout", server->recv_timeout);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("send-timeout", server->send_timeout);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("query-num", server->query_num);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldYesNo("ocr-enabled", server->ocr_enabled);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldString("ocr-server", server->ocr_server);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("watchdog-now", server->watchdog_now);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("watchdog-interval", server->watchdog_interval);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldYesNo("monitor-enabled", server->monitor_enabled);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("monitor-port", server->monitor_port);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldString("log-path", server->log_path);
	if(ret<0)
		return PIR_ERR;

#if 0
	ret=config.getFieldString("log-prefix", server->log_prefix);
	if(ret<0)
		return PIR_ERR;
#endif

	ret=config.getFieldYesNo("log-screen", server->log_screen);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("log-size", server->log_size);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("log-query-cache-size", server->log_query_cache_size);
	if(ret<0)
		return PIR_ERR;

	return PIR_OK;
}

int32_t resetServerStats(struct pirServer* server)
{
	server->stat_numconnections=0;
	server->stat_search_conn=0;
	server->stat_rejected_conn=0;
	server->stat_failed_conn=0;
	return PIR_OK;
}

int32_t initServer(struct pirServer* server)
{
	int32_t ret=0;

	//signal(SIGHUP, SIG_IGN);
	//signal(SIGPIPE, SIG_IGN);
	setupSignalHandlers();

	server->flags|=PIR_MODE_MASTER;
	server->unixtime=time(NULL);
	server->proct_type=strdup(server_type[server->flags].str);
	server->wsdl_path=strdup(server_type[server->flags].wsdl);

	server->logger=q_new<QLogger>();
	if(!server->logger)
		return PIR_ERR;

	ret=server->logger->init(server->log_path, server->log_prefix, server->log_size);
	if(ret<0)
		return PIR_ERR;

	server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
			"Starting YUNSHITU master on Port [%d]...", \
			server->port);

	server->client_queue=q_new< QQueue<SOAP_SOCKET> >();
	if(!server->client_queue) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->client_queue is null!");
		return PIR_ERR;
	}

	ret=server->client_queue->init(server->maxclients);
	if(ret<0) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->client_queue init error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	server->client_queue_trigger=q_new<QTrigger>();
	if(!server->client_queue_trigger) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->client_queue_trigger is null!");
		return PIR_ERR;
	}

	server->client_search_queue=q_new< QQueue<searchObj*> >();
	if(!server->client_search_queue) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->client_search_queue is null!");
		return PIR_ERR;
	}

	ret=server->client_search_queue->init(server->maxclients);
	if(ret<0) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->client_search_queue init error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	/* This function must be called for blocking SIGPIPE signal during socket communication */
	ret=q_init_socket();
	if(ret<0) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: init socket error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	if(server->failover_policy && (access(server->workerInfoFile, F_OK)==0)) {
		ret=loadWorkerInfo(server->workerInfoFile, server->workerInfoVec);
		if(ret<0) {
			server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"initServer: loadWorkerInfo error, ret = (%d)!", ret);
			return PIR_ERR;
		}

		server->bucketSize=server->workerInfoVec.size();
	}

	struct searchObj* search_object=NULL;
	for(int32_t i=0; i<server->maxclients-1; ++i)
	{
		search_object=q_new<searchObj>();
		if(search_object==NULL) {
			server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"initServer: search_object is null!");
			return PIR_ERR;
		}

		search_object->objId=i;
		search_object->search_res.reserve(1<<10);
		search_object->search_sw.reset();

		server->client_search_queue->push(search_object);
	}

	server->thread_info=q_new_array<struct threadInfo>(server->thread_num);
	if(!server->thread_info) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->thread_info is null!");
		return PIR_ERR;
	}

	server->tid=q_new_array<pthread_t>(server->thread_num);
	if(!server->tid) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: server->tid is null!");
		return PIR_ERR;
	}

	return PIR_OK;
}

int32_t createMonitor(struct pirServer* server)
{
	int32_t ret=0;

	ret=server->monitor.init(server->monitor_port, getThreadState, (void*)server, 1);
	if(ret<0) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: init monitor error!");
		return PIR_ERR;
	}

	return PIR_OK;
}

int32_t listenTcpServer(struct pirServer* server, const char* ip, int32_t port, int32_t tcp_backlog)
{
	checkTcpBacklogSettings(server, tcp_backlog);

	soap_init(&server->soap);
	server->soap.recv_timeout=server->recv_timeout;
	server->soap.send_timeout=server->send_timeout;
	server->soap.socket_flags=MSG_NOSIGNAL;
	server->soap.fget=http_get;

	server->master=soap_bind(&server->soap, ip, port, tcp_backlog);
	if(!soap_valid_socket(server->master)) {
		soap_print_fault(&server->soap, stderr);
		return PIR_ERR;
	}

	return PIR_OK;
}

int32_t checkTcpBacklogSettings(struct pirServer* server, int32_t tcp_backlog)
{
#ifdef WIN32
	return PIR_OK;
#else
	FILE* fp=fopen("/proc/sys/net/core/somaxconn", "r");
	if(fp==NULL) return PIR_OK;
	char buf[1<<10]={0};
	if(fgets(buf, sizeof(buf), fp)!=NULL)
	{
		int32_t somaxconn=atoi(buf);
		if(somaxconn>0&&somaxconn<tcp_backlog) {
			server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"The TCP backlog setting of %d can not be enfored because /proc/sys/net/core/somaxconn is set to the lower value of %d!", \
					tcp_backlog, \
					somaxconn);
		}
	}
	fclose(fp);
	return PIR_OK;
#endif
}

int32_t serverMain(struct pirServer* server)
{
	SOAP_SOCKET sock;
	int32_t i=0;
	int32_t ret=0;

	/* setup server state watchdog */
	ret=setupWatchdog(server, server->watchdog_now, server->watchdog_interval);
	if(ret<0) {
		server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: setup watchdog error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	// Threads must be initialized after listenTcpServer() function, or a soap bug emerges!
	for(i=0; i<server->thread_num; ++i)
	{
		server->thread_info[i].pthis=server;
		server->thread_info[i].id=i;
		server->thread_info[i].status=PIR_THREAD_STATUS_IDLE;
		server->thread_info[i].flag=PIR_THREAD_NOTSTARTED;
#if defined (__HAVING_THREAD_BUF__)
		server->thread_info[i].buf_size=server->thread_buf_size;
		server->thread_info[i].ptr_buf=q_new_array<char>(server->thread_info[i].buf_size);
		Q_CHECK_PTR(server->thread_info[i].ptr_buf);
#endif
		server->thread_info[i].timeout=server->thread_timeout;
		server->thread_info[i].psoap=soap_copy((struct soap*)&server->soap);

		server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"initServer: starting thread (%02d)...", i);

		if(q_create_thread(&server->tid[i], process_queue, (void*)&server->thread_info[i])) {
			server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"initServer: thread (%02d) create error!", i);
			q_sleep(1);
		}

		while(!server->thread_info[i].flag&PIR_THREAD_RUNNING)
			q_sleep(1);

		if(server->thread_info[i].flag&PIR_THREAD_STOPPED) {
			server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"initServer: thread (%02d) start error!", i);
			return PIR_ERR;
		}
	}

	while(!server->shutdown_asap)
	{
		sock=soap_accept(&server->soap);
		if(!soap_valid_socket(sock))
		{
			if(server->soap.errnum) {
				server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
						"Retrying sock (%d)...", \
						sock);
				soap_print_fault(&server->soap, stderr);
				continue;
			}
			server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"Socket accept timed out error, sock = (%d)", \
					sock);
			break;
		}

		++server->stat_numconnections;

#if defined (__VERBOSE_MODE)
		server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"Request task (%d), slave socket = (%d), IP = (%d.%d.%d.%d)...", \
				server->stat_numconnections, \
				sock, \
				(int32_t)(server->soap.ip>>24)&0xFF, \
				(int32_t)(server->soap.ip>>16)&0xFF, \
				(int32_t)(server->soap.ip>>8)&0xFF, \
				(int32_t)server->soap.ip&0xFF);
#endif

		server->client_queue->push(sock);
		server->client_queue_trigger->signal();
	}

	for(i=0; i<server->thread_num; ++i)
	{
		q_thread_join(server->tid[i]);
		server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
				"Thread (%d) terminated...", i);

#if defined (__HAVING_THREAD_BUF__)
		q_delete_array<char>(server->thread_info[i].ptr_buf);
#endif
		soap_done(server->thread_info[i].psoap);
		free(server->thread_info[i].psoap);
	}

	return PIR_OK;
}

int32_t updateCachedTime(struct pirServer* server)
{
	server->unixtime=time(NULL);
	return PIR_OK;
}

int32_t setupWatchdog(struct pirServer* server, int32_t now, int32_t interval)
{
	server->watchdog.set_now(now);
	server->watchdog.set_interval(interval);

	if(server->watchdog.start(updateServerStatus))
		return PIR_ERR;

	return PIR_OK;
}

int32_t setupSignalHandlers(void)
{
	struct sigaction act;
	if(sigemptyset(&act.sa_mask)==-1)
		return PIR_ERR;
	act.sa_flags=0;
	act.sa_handler=sigShutdownHandler;
	if(sigaction(SIGTERM, &act, NULL)==-1)
		return PIR_ERR;
	if(sigaction(SIGINT, &act, NULL)==-1)
		return PIR_ERR;
	return PIR_OK;
}

int32_t checkForDaemonizeMode(int32_t argc, char** argv)
{
	for(int32_t i=0; i<argc; ++i) {
		if(strcmp(argv[i], "-d")==0 || strcmp(argv[i], "--daemon")==0)
			return 1;
	}
	return 0;
}

int32_t checkForRecoveryMode(int32_t argc, char** argv)
{
	for(int32_t i=0; i<argc; ++i) {
		if(strcmp(argv[i], "-r")==0 || strcmp(argv[i], "--recovery")==0)
			return 1;
	}
	return 0;
}

int32_t getServerInfo(struct pirServer* server, char* server_info, int32_t server_info_size)
{
	if(server_info==NULL||server_info_size<=0)
		return PIR_ERR;

	char kernel_info[1<<10]={0};
	struct tm* pst=NULL;
	struct tm* pli=NULL;
	uint64_t used_bytes=0;
	uint64_t total_bytes=0;
	uint64_t used_mem_bytes=0;
	uint64_t total_mem_bytes=0;

	char* ptr_info=server_info;
	char* ptr_end=server_info+server_info_size;
	int32_t ret=0;

	updateCachedTime(server);

	ret=q_get_kernel_info(kernel_info, sizeof(kernel_info));
	if(ret<0)
		return PIR_ERR;

	ret=q_get_disk_usage((char*)"./", &used_bytes, &total_bytes);
	if(ret<0)
		return PIR_ERR;

	ret=q_get_mem_usage(&used_mem_bytes, &total_mem_bytes);
	if(ret<0)
		return PIR_ERR;

	/* master info */
	ret=q_snprintf(ptr_info, server_info_size, \
			"# Server\r\n"
			"os: %s\r\n"
			"arch_bits: %d\r\n"
			"processors: %d\r\n"
			"load_average: %.2f\r\n"
			"disk_usage: %s/%s\r\n"
			"mem_usage: %s/%s\r\n"
			"gcc_version: %d.%d.%d\r\n"
			"process_id: %ld\r\n"
			"server_type: %s\r\n"
			"server_version: %s\r\n"
			"server_state: %d(%s)\r\n"
			"thread_state: %d\r\n"
			"server_ip: %s\r\n"
			"tcp_port: %d\r\n"
			"hz: %d\r\n"
			"unixtime: %s\r\n"
			"starttime: %s\r\n"
			"lastinteraction: %s\r\n"
			"total_conns: %u\r\n"
			"search_conns: %u\r\n"
			"rejected_conns: %u\r\n"
			"failed_conns: %u\r\n\r\n",
			kernel_info,
			server->arch_bits,
			q_get_cpu_processors(),
			q_get_load_avg()/100.0f,
			q_format_byte_size(used_bytes).c_str(),
			q_format_byte_size(total_bytes).c_str(),
			q_format_byte_size(used_mem_bytes).c_str(),
			q_format_byte_size(total_mem_bytes).c_str(),
#ifdef __GNUC__
			__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__,
#else
			0,0,0,
#endif
			(int64_t)server->pid,
			server_type[server->flags].str,
			PIR_VERSION_STR,
			server->serverstate, 
			server_state[server->serverstate].summary,
			getThreadState(server),
			q_local_ip(),
			server->port,
			server->hz,
			QDateTime(server->unixtime).to_string().c_str(),
			QDateTime(server->stat_starttime).to_string().c_str(),
			QDateTime(server->lastinteraction).to_string().c_str(),
			server->stat_numconnections,
			server->stat_search_conn,
			server->stat_rejected_conn,
			server->stat_failed_conn);
	if(ret<0||ptr_info+ret>=ptr_end)
		return PIR_ERR;
	else
		ptr_info+=ret;

	/* worker info */
	for(int32_t i=0; i<server->bucketSize; ++i)
	{
		ret=q_snprintf(ptr_info, ptr_end-ptr_info, \
				"# Worker: %d\r\n"
				"workspace: %s\r\n"
				"tpldir: %s\r\n"
				"method: %s\r\n"
				"option: %s\r\n"
				"state: %d(%s)\r\n\r\n",
				i,
				server->workerInfoVec[i].workspace.c_str(),
				server->workerInfoVec[i].tpldir.c_str(),
				server->workerInfoVec[i].method.c_str(),
				server->workerInfoVec[i].option.c_str(),
				server->workerInfoVec[i].state,
				server_state[server->serverstate].summary);
		if(ret<0||ptr_info+ret>=ptr_end)
			return PIR_ERR;
		else
			ptr_info+=ret;
	}

	/* ocr info */
	ret=q_snprintf(ptr_info, ptr_end-ptr_info, \
			"# OCR\r\n"
			"ocr_enabled: %s\r\n"
			"ocr_server: %s\r\n\r\n",
			(server->ocr_enabled==1)?"yes":"no",
			server->ocr_server);
	if(ret<0||ptr_info+ret>=ptr_end)
		return PIR_ERR;
	else
		ptr_info+=ret;

	return PIR_OK;
}

int32_t getServerLog(struct pirServer* server, const char* day_str, char* log_buf, int32_t log_size, int64_t* log_len)
{
	if(day_str==NULL||log_buf==NULL||log_size<=0||log_len==NULL)
		return PIR_ERR;

	char log_file[1<<8]={0};
	FILE* logfp=NULL;

	sprintf(log_file, "%s/%s.log", server->log_path, day_str);

	if((logfp=fopen(log_file, "rb"))==NULL)
		return PIR_ERR;

	if(fseek(logfp, 0, SEEK_END)==-1)
		goto err;

	*log_len=ftell(logfp);
	if(*log_len==-1||*log_len>=log_size)
		goto err;

	if(fseek(logfp, 0, SEEK_SET)==-1)
		goto err;

	if(fread(log_buf, *log_len, 1, logfp)!=1)
		goto err;

	log_buf[*log_len+1]='\0';
	fclose(logfp);
	return PIR_OK;

err:
	fclose(logfp);
	return PIR_ERR;
}

int32_t getThreadState(void* ptr_info)
{
	struct pirServer* server=reinterpret_cast<struct pirServer*>(ptr_info);
	Q_CHECK_PTR(server);

	/* monitor thread timout status */
	int32_t count=0;
	for(int32_t i=0; i<server->thread_num; ++i) {
		if(!server->thread_info[i].status&PIR_THREAD_STATUS_BUSY)
			continue;

		server->thread_info[i].stopwatch.stop();
		if(server->thread_info[i].stopwatch.elapsed_ms()>server->thread_info[i].timeout)
			++count;
	}

	return count;
}

int32_t freeServer(struct pirServer* server)
{
	q_free(server->pidfile);
	q_free(server->configfile);

	for(int32_t i=0; i<server->argc; ++i)
		q_free(server->argv[i]);
	q_free(server->argv);

	q_free(server->proct_prefix);
	q_free(server->proct_type);
	q_free(server->wsdl_path);
	q_free(server->setting_xml_path);
	q_free(server->setting_xml);

	q_delete_array<struct threadInfo>(server->thread_info);
	q_delete_array<pthread_t>(server->tid);

	soap_done(&server->soap);
	server->master=PIR_INVALID_SOCKET;

	q_delete< QQueue<SOAP_SOCKET> >(server->client_queue);
	q_delete<QTrigger>(server->client_queue_trigger);

	struct searchObj* search_object=NULL;
	while(!server->client_search_queue->pop_non_blocking(search_object))
		q_delete<searchObj>(search_object);
	q_delete< QQueue<searchObj*> >(server->client_search_queue);

	q_free(server->ocr_server);

	q_free(server->log_path);
	q_free(server->log_prefix);
	q_delete<QLogger>(server->logger);

	return PIR_OK;
}

void *process_queue(void *ptr_info)
{
	struct threadInfo* thread_info=reinterpret_cast<threadInfo*>(ptr_info);
	Q_CHECK_PTR(thread_info);

	struct pirServer* server=reinterpret_cast<struct pirServer*>(thread_info->pthis);
	Q_CHECK_PTR(server);

	thread_info->flag|=PIR_THREAD_RUNNING;
	while(!server->shutdown_asap)
	{
		thread_info->status&=~PIR_THREAD_STATUS_BUSY;

		server->client_queue_trigger->wait();

		thread_info->status|=PIR_THREAD_STATUS_BUSY;
		while(server->client_queue->pop_non_blocking(thread_info->psoap->socket)==0)
		{
			if(!soap_valid_socket(thread_info->psoap->socket)) {
				server->logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
						"Soap socket is invalid, which may have closed!");
				q_lock_inc(&server->stat_failed_conn);
				continue;
			}

			thread_info->stopwatch.start();

			soap_serve(thread_info->psoap);
			soap_destroy(thread_info->psoap);
			soap_end(thread_info->psoap);

			thread_info->stopwatch.stop();
			server->lastinteraction=time(NULL);

#if defined (__VERBOSE_MODE)
			server->logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server->log_screen, \
					"Task processing finished, which conusumed (%dms)!", \
					thread_info->stopwatch.elapsed_ms());
#endif
		}
	}

	thread_info->flag&=~PIR_THREAD_RUNNING;
	thread_info->flag|=PIR_THREAD_STOPPED;

	return (NULL);
}

int32_t getData(const std::string& s, int32_t& bucketSize, std::vector<std::string>& vec)
{
	int32_t beg=0;
	int32_t end=s.find('|');

	std::string head=s.substr(0, end);
	bucketSize=atoi(head.c_str());
	if(bucketSize<=0)
		return PIR_ERR;

	beg=end+1;
	for(int32_t i=0; i<bucketSize; ++i) {
		end=s.find('|', beg);
		vec.push_back(s.substr(beg, end-beg));
		beg=end+1;
	}

	return PIR_OK;;
}

int32_t initWorkerInfo(const std::string& workspace, const std::string& tpldir, const std::string& method, const std::string& option, std::vector<workerInfo>& workerInfoVec)
{
	std::vector<std::string> workspaceVec;
	std::vector<std::string> tpldirVec;
	std::vector<std::string> methodVec;
	std::vector<std::string> optionVec;

	int32_t bucketSize=0;

	if(getData(workspace, bucketSize, workspaceVec)==PIR_ERR)
		return PIR_ERR;

	if(getData(tpldir, bucketSize, tpldirVec)==PIR_ERR)
		return PIR_ERR;

	if(getData(method, bucketSize, methodVec)==PIR_ERR)
		return PIR_ERR;

	if(getData(option, bucketSize, optionVec)==PIR_ERR)
		return PIR_ERR;

	if(workspaceVec.size()!=bucketSize||tpldirVec.size()!=bucketSize||methodVec.size()!=bucketSize||optionVec.size()!=bucketSize)
		return PIR_ERR;

	for(int32_t i=0; i<bucketSize; ++i)
	{
		struct workerInfo wi;
		wi.workspace	= workspaceVec[i];
		wi.tpldir	= tpldirVec[i];
		wi.method	= methodVec[i];
		wi.option	= optionVec[i];
		wi.state	= PIR_STATE_NONE;
		workerInfoVec.push_back(wi);
	}

	return PIR_OK;
}

int32_t loadWorkerInfo(const char* snapshotFile, std::vector<workerInfo>& workerInfoVec)
{
	if(snapshotFile==NULL)
		return PIR_ERR;

	QConfigReader reader;
	char buf[1<<10]={0};
	int32_t worker_num=0;
	int32_t ret=0;

	if(reader.init(snapshotFile)<0)
		return PIR_ERR;

	ret=reader.getFieldInt32("worker-num", worker_num);
	if(ret<0)
		return PIR_ERR;

	for(int32_t i=0; i<worker_num; ++i)
	{
		struct workerInfo wi;
		int32_t flag=0;

		sprintf(buf, "workspace-%02d", i);
		ret=reader.getFieldString(buf, wi.workspace);
		if(ret<0)
			return PIR_ERR;

		sprintf(buf, "tpldir-%02d", i);
		ret=reader.getFieldString(buf, wi.tpldir);
		if(ret<0)
			return PIR_ERR;

		sprintf(buf, "method-%02d", i);
		ret=reader.getFieldString(buf, wi.method);
		if(ret<0)
			return PIR_ERR;

		sprintf(buf, "option-%02d", i);
		ret=reader.getFieldString(buf, wi.option);
		if(ret<0)
			return PIR_ERR;

		sprintf(buf, "enable-%02d", i);
		ret=reader.getFieldYesNo(buf, flag);
		if(ret<0)
			return PIR_ERR;

		if(flag)
			workerInfoVec.push_back(wi);
	}

	return PIR_OK;
}

int32_t writeWorkerInfo(const std::vector<workerInfo>& workerInfoVec, const char* snapshotFile)
{
	FILE* fp=NULL;
	int32_t worker_num=workerInfoVec.size();

	fp=fopen(snapshotFile, "wb");
	if(fp==NULL)
		return PIR_ERR;

	fprintf(fp, "# Server:\n");
	fprintf(fp, "worker-num = %d\n", workerInfoVec.size());
	fprintf(fp, "\n");

	for(int32_t i=0; i<workerInfoVec.size(); ++i)
	{
		fprintf(fp, "# Worker: %02d\n", i);
		fprintf(fp, "workspace-%02d = %s\n", i, workerInfoVec[i].workspace.c_str());
		fprintf(fp, "tpldir-%02d = %s\n", i, workerInfoVec[i].tpldir.c_str());
		fprintf(fp, "method-%02d = %s\n", i, workerInfoVec[i].method.c_str());
		fprintf(fp, "option-%02d = %s\n", i, workerInfoVec[i].option.c_str());
		fprintf(fp, "enable-%02d = yes\n", i);
		fprintf(fp, "\n");
	}

	fclose(fp);
	fp=NULL;

	return PIR_OK;
}

bool compareKeyAscending(std::pair<std::string, float> key1, std::pair<std::string, float> key2)
{
	return key1.second<key2.second;
}

bool compareKeyDescending(std::pair<std::string, float> key1, std::pair<std::string, float> key2)
{
	return key1.second>key2.second;
}

int32_t createPidFile(const char* pidfile)
{
	FILE* fp=fopen(pidfile, "wb");
	if(fp) {
#ifdef WIN32
		fprintf(fp, "%d\n", (int32_t)_getpid());
#else
		fprintf(fp, "%d\n", (int32_t)getpid());
#endif
		fclose(fp);
	}
	return PIR_OK;
}

int32_t daemonize(void)
{
#ifdef WIN32
	return PIR_OK;
#else
	int32_t pid;
	int32_t fd;

	//::signal(SIGTTOU, SIG_IGN);
	//::signal(SIGTTIN, SIG_IGN);
	//::signal(SIGTSTP, SIG_IGN);
	//::signal(SIGHUP, SIG_IGN);
	//::signal(SIGCHLD, SIG_IGN);

	if((pid=::fork())) {
		exit(EXIT_SUCCESS);
	} else if(pid<0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	::setsid();

	if((fd=open("/dev/null", O_RDWR, 0))!=-1) {
		dup2(fd, STDIN_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		if(fd>STDERR_FILENO) close(fd);
	}

	return PIR_OK;
#endif
}

int32_t setproctitle(int32_t argc, char** argv, char* prefix, char* type)
{
	char proctitle[1<<10] = {0};

	strcat(proctitle, prefix);
	strcat(proctitle, ":");
	strcat(proctitle, " ");

	strcat(proctitle, type);
	strcat(proctitle, " ");

	for(int32_t i=0; i<argc; ++i) {
		strcat(proctitle, argv[i]);
		if(i<argc-1)
			strcat(proctitle, " ");
	}

	strcpy(argv[0], proctitle);
	return PIR_OK;
}

void usage(void)
{
	fprintf(stderr, 
			"PIR [option]...\n"
			"  -p,--port          Run service on specified TCP port...\n"
			"  -d,--daemon        Run as a daemon...\n"
			"  -h,--help          Display a help message and exit...\n"
			"  -r,--recovery      Recovery the service...\n"
			"  -v,--version       Output version information and exit...\n"
	       );
}

void version(void)
{
	fprintf(stdout, "Kalava version = %s\n", PIR_VERSION_STR);
	exit(0);
}
