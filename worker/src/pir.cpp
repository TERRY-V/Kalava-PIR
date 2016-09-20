#include "pir.h"
#include <string>
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
	server->flags=PIR_MODE_WORKER;
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
	server->setting_xml_path=NULL;
	server->setting_xml=NULL;
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
	server->workspace="";
	server->tpldir="";
	server->method="";
	server->option="";
	server->client_queue=NULL;
	server->client_queue_trigger=NULL;
	server->client_search_queue=NULL;
	server->delta_for_training=0;
	server->delta_peak=PIR_DEFAULT_DELTA_PEAK;
	server->delta_ratio=PIR_DEFAULT_DELTA_RATIO;
	server->idletime_for_training=PIR_DEFAULT_IDLETIME;
	server->main_log_path=NULL;
	server->main_log_prefix=NULL;
	server->main_log_size=PIR_DEFAULT_LOG_SIZE;
	server->main_logger=NULL;
	server->log_query_cache_size=PIR_LOG_QUERY_CACHE_SIZE;
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

	if(flag==1)
		server->daemonize=1;

	ret=config.getFieldString("pidfile", server->pidfile);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt32("tcp-backlog", server->tcp_backlog);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldString("setting-xml-path", server->setting_xml_path);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldString("setting-xml", server->setting_xml);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldUint32("delta-peak", server->delta_peak);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldFloat("delta-ratio", server->delta_ratio);
	if(ret<0)
		return PIR_ERR;

	ret=config.getFieldInt64("idletime-for-training", server->idletime_for_training);
	if(ret<0)
		return PIR_ERR;

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

	ret=config.getFieldString("log-path", server->main_log_path);
	if(ret<0)
		return PIR_ERR;
/*

	ret=config.getFieldString("log-prefix", server->main_log_prefix);
	if(ret<0)
		return PIR_ERR;
*/

	ret=config.getFieldInt32("log-size", server->main_log_size);
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

	server->flags|=PIR_MODE_WORKER;
	server->unixtime=time(NULL);
	server->proct_type=strdup(server_type[server->flags].str);
	server->wsdl_path=strdup(server_type[server->flags].wsdl);

	server->main_logger=q_new<QLogger>();
	if(!server->main_logger)
		return PIR_ERR;

	ret=server->main_logger->init(server->main_log_path, server->main_log_prefix, server->main_log_size);
	if(ret<0)
		return PIR_ERR;

	server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"Starting YUNSHITU worker on Port [%d]......", \
			server->port);

	server->client_queue=q_new< QQueue<SOAP_SOCKET> >();
	if(!server->client_queue) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->client_queue is null!");
		return PIR_ERR;
	}

	ret=server->client_queue->init(server->maxclients);
	if(ret<0) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->client_queue init error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	server->client_queue_trigger=q_new<QTrigger>();
	if(!server->client_queue_trigger) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->client_queue_trigger is null!");
		return PIR_ERR;
	}

	server->client_search_queue=q_new< QQueue<searchObj*> >();
	if(!server->client_search_queue) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->client_search_queue is null!");
		return PIR_ERR;
	}

	ret=server->client_search_queue->init(server->maxclients);
	if(ret<0) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->client_search_queue init error, ret = (%d)!", ret);
		return PIR_ERR;
	}

	/* This function must be called for blocking SIGPIPE signal during socket communication */
	/*
	ret=q_init_socket();
	if(ret<0) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: init socket error, ret = (%d)!", ret);
		return PIR_ERR;
	}
	*/

	if(server->failover_policy) {
		/* auto start */
		server->autostart=1;

		if(loadWorkerInfo(server)==PIR_ERR) {
			server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"loadWorkerInfo: load error!");
			return PIR_ERR;
		}

		if(switchLoggingDirectory(server)==PIR_ERR)
			return PIR_ERR;
	}

	struct searchObj* search_object=NULL;
	for(int32_t i=0; i<server->maxclients-1; ++i)
	{
		search_object=q_new<searchObj>();
		if(search_object==NULL) {
			server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"initServer: search_object is null!");
			return PIR_ERR;
		}

		search_object->objId=i;
		search_object->search_res.reserve(1<<10);	/* to accelerate */
		search_object->search_sw.reset();
		server->client_search_queue->push(search_object);
	}

	server->thread_info=q_new_array<struct threadInfo>(server->thread_num);
	if(!server->thread_info) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->thread_info is null!");
		return PIR_ERR;
	}

	server->tid=q_new_array<pthread_t>(server->thread_num);
	if(!server->tid) {
		server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: server->tid is null!");
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
			server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"The TCP backlog setting of %d can not be enfored because /proc/sys/net/core/somaxconn is set to the lower value of %d!", \
					tcp_backlog, \
					somaxconn);
		}
	}
	fclose(fp);
	return PIR_OK;
#endif
}

int32_t switchLoggingDirectory(struct pirServer* server)
{
	if(server==NULL)
		return PIR_ERR;

	int32_t ret=0;

	if(server->main_logger)
		q_delete<QLogger>(server->main_logger);

	server->main_logger=q_new<QLogger>();
	if(server->main_logger==NULL)
		return PIR_ERR;

	ret=server->main_logger->init(server->workspace.c_str(), server->main_log_prefix, server->main_log_size);
	if(ret<0) {
		q_delete<QLogger>(server->main_logger);
		return PIR_ERR;
	}

	return PIR_OK;
}

int32_t serverMain(struct pirServer* server)
{
	SOAP_SOCKET sock;
	int32_t i=0;

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

		server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"initServer: starting thread (%02d)...", i);

		if(q_create_thread(&server->tid[i], process_queue, (void*)&server->thread_info[i])) {
			server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"initServer: thread (%02d) create error!", i);
			q_sleep(1);
		}

		while(!server->thread_info[i].flag&PIR_THREAD_RUNNING)
			q_sleep(1);

		if(server->thread_info[i].flag&PIR_THREAD_STOPPED) {
			server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
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
				server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
						"Retrying sock (%d)...", sock);
				soap_print_fault(&server->soap, stderr);
				continue;
			}
			server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"Socket accept timed out error, sock = (%d)", sock);
			break;
		}

		++server->stat_numconnections;

#if defined (__VERBOSE_MODE)
		server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
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
		server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
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

	updateCachedTime(server);

	ret=q_get_kernel_info(kernel_info, sizeof(kernel_info));
	if(ret<0)
		return PIR_ERR;

	ret=q_snprintf(server_info, server_info_size, \
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
			"workspace: %s\r\n"
			"tpldir: %s\r\n"
			"method: %s\r\n"
			"option: %s\r\n"
			"unixtime: %s\r\n"
			"starttime: %s\r\n"
			"lastinteraction: %s\r\n"
			"total_conns: %u\r\n"
			"search_conns: %u\r\n"
			"rejected_conns: %u\r\n"
			"failed_conns: %u\r\n",
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
			server_type[server->flags].name,
			PIR_VERSION_STR,
			server->serverstate, server_state[server->serverstate].summary,
			getThreadState(server),
			q_local_ip(),
			server->port,
			server->hz,
			server->workspace.c_str(),
			server->tpldir.c_str(),
			server->method.c_str(),
			server->option.c_str(),
			QDateTime(server->unixtime).to_string().c_str(),
			QDateTime(server->stat_starttime).to_string().c_str(),
			QDateTime(server->lastinteraction).to_string().c_str(),
			server->stat_numconnections,
			server->stat_search_conn,
			server->stat_rejected_conn,
			server->stat_failed_conn);
	if(ret<0||server_info+ret>=server_info+server_info_size)
		return PIR_ERR;

	return PIR_OK;
}

int32_t getServerLog(struct pirServer* server, const char* day_str, char* log_buf, int32_t log_size, int64_t* log_len)
{
	if(day_str==NULL||log_buf==NULL||log_size<=0||log_len==NULL)
		return PIR_ERR;

	char log_file[1<<8]={0};
	FILE* logfp=NULL;

	if(server->workspace.empty()) {
		sprintf(log_file, "%s/%s.log", server->main_log_path, day_str);
	} else if(server->workspace[server->workspace.length()-1]=='/') {
		sprintf(log_file, "%s%s.log", server->workspace.c_str(), day_str);
	} else {
		sprintf(log_file, "%s/%s.log", server->workspace.c_str(), day_str);
	}

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

int32_t getThreadState(struct pirServer* server)
{
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

int32_t loadWorkerInfo(struct pirServer* server)
{
	if(server==NULL)
		return PIR_ERR;

	QConfigReader reader;
	char rdb_file[1<<10]={0};
	int32_t ret=0;

	q_snprintf(rdb_file, sizeof(rdb_file), "__%d.rdb", server->port);

	if(reader.init(rdb_file)<0)
		return PIR_ERR;

	ret=reader.getFieldString("workspace", server->workspace);
	if(ret<0)
		return PIR_ERR;

	ret=reader.getFieldString("tpldir", server->tpldir);
	if(ret<0)
		return PIR_ERR;

	ret=reader.getFieldString("method", server->method);
	if(ret<0)
		return PIR_ERR;

	ret=reader.getFieldString("option", server->option);
	if(ret<0)
		return PIR_ERR;

	return PIR_OK;
}

int32_t writeWorkerInfo(struct pirServer* server)
{
	FILE* fp=NULL;
	char rdb_file[1<<10]={0};

	q_snprintf(rdb_file, sizeof(rdb_file), "__%d.rdb", server->port);

	fp=fopen(rdb_file, "wb");
	if(fp==NULL)
		return PIR_ERR;

	fprintf(fp, "port = %d\n", server->port);
	fprintf(fp, "workspace = %s\n", server->workspace.c_str());
	fprintf(fp, "tpldir = %s\n", server->tpldir.c_str());
	fprintf(fp, "method = %s\n", server->method.c_str());
	fprintf(fp, "option = %s\n\n", server->option.c_str());

	fclose(fp);
	fp=NULL;

	return PIR_OK;
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

	q_free(server->main_log_path);
	q_free(server->main_log_prefix);
	q_delete<QLogger>(server->main_logger);

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
				server->main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
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

			server->main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"Task processing finished, which conusumed (%dms)!", \
					thread_info->stopwatch.elapsed_ms());

#endif
		}
	}

	thread_info->flag&=~PIR_THREAD_RUNNING;
	thread_info->flag|=PIR_THREAD_STOPPED;

	return (NULL);
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
