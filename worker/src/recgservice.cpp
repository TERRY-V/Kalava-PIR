#include "soapH.h"
#include "Imsearch.nsmap"

#include "pir.h"
#include "imageMatchLib.h"

struct pirServer server;

void *serviceStart_thread(void*);

int ns__imSearch(struct soap *soap, std::string imgPath, struct ns__searchResponse* ret)
{
	struct ns__searchResponse resp;
	resp.tag=PIR_SOAP_TAG_ERR;

	struct ns__pc __pc;

	q_lock_inc(&server.stat_search_conn);
	if(server.serverstate==PIR_STATE_READY)
	{
		struct searchObj* search_object=server.client_search_queue->pop();
		Q_CHECK_PTR(search_object);

		search_object->image_path=imgPath;
		search_object->search_sw.start();

		search_object->search_res=search_image(search_object->image_path.c_str(), search_object->objId);

		resp.tag=PIR_SOAP_TAG_OK;
		for(int32_t i=0; i<search_object->search_res.size(); ++i)
		{
			__pc.pc=search_object->search_res[i].first;
			__pc.score=search_object->search_res[i].second;
			resp.match.push_back(__pc);
		}

		search_object->search_sw.stop();
		resp.consumedTime=search_object->search_sw.elapsed_ms();

#ifdef __VERBOSE_MODE
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"HYY finds (%d) similar pictures for (%s), which consumed (%dms)!", \
				search_object->search_res.size(), \
				search_object->image_path.c_str(), \
				search_object->search_sw.elapsed_ms());
#endif

		search_object->reset();
		server.client_search_queue->push(search_object);
	} else {
		resp.tag=PIR_STATE_ERROR;
		q_lock_inc(&server.stat_failed_conn);

#ifdef __VERBOSE_MODE
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"Server state is not (%d)!", \
				PIR_STATE_READY);
#endif
	}

	*ret=resp;
	return SOAP_OK;
}

int32_t copysetting(const std::string& workspace, const std::string& option)
{
	char *ptr_buf=NULL;
	uint32_t file_size=0;
	FILE *fp=NULL;

	// read
	std::string setting_path;
	setting_path.append(server.setting_xml_path);
	setting_path.append(option);
	
	fp=fopen(setting_path.c_str(), "r");
	if(fp==NULL) {
		server.main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting: open (%s) error!", \
				setting_path.c_str());
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	file_size=ftell(fp);
	rewind(fp);

	ptr_buf=(char*)malloc(sizeof(char)*file_size+1);
	if(ptr_buf==NULL) {
		server.main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting: malloc error!");
		return -2;
	}

	if(fread(ptr_buf, file_size, 1, fp)!=1) {
		server.main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting: fread error!");
		fclose(fp);
		free(ptr_buf);
		return -3;
	}

	ptr_buf[file_size]='\0';
	fclose(fp);

	// write
	setting_path.clear();
	setting_path.append(server.workspace);
	setting_path.append(server.setting_xml);

	fp=fopen(setting_path.c_str(), "wb");
	if(fp==NULL) {
		server.main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting: open (%s) error!", \
				setting_path.c_str());
		free(ptr_buf);
		return -4;
	}

	if(fwrite(ptr_buf, file_size, 1, fp)!=1) {
		server.main_logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting: fwrite error!");
		fclose(fp);
		free(ptr_buf);
		return -5;
	}

	fclose(fp);
	fp=NULL;

	free(ptr_buf);
	ptr_buf=NULL;

	return 0;
}

int ns__serviceInit(struct soap* soap, std::string workspace, std::string tpldir, std::string method, std::string option, int* tag)
{
	int32_t ret=0;

	server.serverstate=PIR_STATE_INITING;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceInit starts...");

	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceInit starts, workspace = (%s)...", workspace.c_str());
	server.workspace=workspace;
	server.tpldir=tpldir;
	server.method=method;
	server.option=option;

	/* switch logging directory */
	if(switchLoggingDirectory(&server)==PIR_ERR) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	/* copy setting */
	ret=copysetting(server.workspace, server.option);
	if(ret<0) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"copysetting error, ret = (%d)", ret);
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	server.serverstate=PIR_STATE_INITED;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceInit okay...");

	/* bakup for recoverty */
	if(writeWorkerInfo(&server)==PIR_ERR) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

end:
	if(ret==0) {
		*tag=PIR_SOAP_TAG_OK;
	} else {
		*tag=ret;
	}
	return SOAP_OK;
}

void *train_thread(void*)
{
	server.serverstate=PIR_STATE_TRAINING;

	clear();

	float steps=7.0;
	server.progressPCT=0.0/steps;

	init_lib(server.workspace.c_str());
	server.progressPCT=1.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] init_lib over......", \
			server.progressPCT*100);
	
	import_image(server.tpldir.c_str());
	server.progressPCT=2.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] import_image over......", \
			server.progressPCT*100);

	cast_feature_bySave();
	server.progressPCT=3.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] cast_feature_bySave over......", \
			server.progressPCT*100);

	compute_word_byLoad();
	server.progressPCT=4.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] compute_word_byLoad over......", \
			server.progressPCT*100);

	build_index_bySave();
	server.progressPCT=5.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] build_index_bySave over......", \
			server.progressPCT*100);

	cluster_feature2word_bySave();
	server.progressPCT=6.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] cluster_feature2word_bySave over......", \
			server.progressPCT*100);

	clear();
	server.progressPCT=7.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain [%.2f\%] cluster_feature2word_bySave over......", \
			server.progressPCT*100);

	server.serverstate=PIR_STATE_TRAINED;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain okay......");

	if(server.autostart==1)
		serviceStart_thread(NULL);

	return NULL;
}

int ns__train(struct soap* soap, int autostart, int* tag)
{
	server.serverstate=PIR_STATE_TRAINING;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain starts, server.autostart = (%d)......", \
			autostart);

	server.progressPCT=0.f;
	server.autostart=autostart;

	while(q_create_thread(train_thread, NULL)) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"serviceTrain create train thread error......");
		q_sleep(1000);
	}

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__trainAdd(struct soap* soap, std::string imgPath, int* tag)
{
	int32_t ret=PIR_SOAP_TAG_OK;
	if(!imgPath.length() || server.serverstate!=PIR_STATE_READY) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	/* Guess the following three function is non-reentrant, thus a mutex is needed. */
	server.mutex_for_training.lock();
	if(add_train_image(imgPath.c_str())==false) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	if(update_train_file()==false) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	++server.delta_for_training;

	/* Check whether it's time for rebuilding the index */
	updateCachedTime(&server);
	if((server.delta_for_training>=server.delta_peak*server.delta_ratio&&(server.unixtime-server.lastinteraction)*1000>server.idletime_for_training) \
			||server.delta_for_training>=server.delta_peak)
	{
		server.autostart=1;
		while(q_create_thread(train_thread, NULL)) {
			server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"trainAdd create train thread error......");
			q_sleep(1000);
		}
	}

end:
	server.mutex_for_training.unlock();
	*tag=ret;
	return SOAP_OK;
}

int ns__trainDelete(struct soap* soap, std::string imgPath, int* tag)
{
	int32_t ret=PIR_SOAP_TAG_OK;
	if(!imgPath.length() || server.serverstate!=PIR_STATE_READY) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	/* Guess the following three function is non-reentrant, thus a mutex is needed. */
	server.mutex_for_training.lock();
	if(delete_train_image(imgPath.c_str())==false) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	if(update_train_file()==false) {
		ret=PIR_SOAP_TAG_ERR;
		goto end;
	}

	--server.delta_for_training;

end:
	server.mutex_for_training.unlock();
	*tag=ret;
	return SOAP_OK;
}

void *serviceStart_thread(void*)
{
	server.serverstate=PIR_STATE_STARTING;
	q_sleep(1);

	clear();

	float steps=6.0;
	server.progressPCT=0.0/steps;

	init_lib(server.workspace.c_str());
	server.progressPCT=1.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] init_lib over!", \
			server.progressPCT*100);

	cast_feature_byLoad();
	server.progressPCT=2.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] cast_feature_byLoad over!", \
			server.progressPCT*100);

	compute_word_byLoad();
	server.progressPCT=3.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] compute_word_byLoad over!", \
			server.progressPCT*100);

	build_index_byLoad();
	server.progressPCT=4.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] build_index_byLoad over!", \
			server.progressPCT*100);

	cluster_feature2word_byLoad();
	server.progressPCT=5.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] cluster_feature2word_byLoad over!", \
			server.progressPCT*100);

	search_image_init();
	server.progressPCT=6.0/steps;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart [%.2f\%] search_image_init over!", \
			server.progressPCT*100);

	server.serverstate=PIR_STATE_READY;
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"ServiceStart okay......");

	return (NULL);
}

int ns__serviceStart(struct soap* soap, int* tag)
{
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceStart starts...");

	if(server.serverstate==PIR_STATE_INITED||server.serverstate==PIR_STATE_TRAINED||server.serverstate==PIR_STATE_READY)
	{
		server.serverstate=PIR_STATE_STARTING;
		server.progressPCT=0.f;

		while(q_create_thread(serviceStart_thread, NULL)) {
			server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"serviceStart create thread error...");
			q_sleep(1000);
		}
	}

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__serviceStop(struct soap* soap, int* tag)
{
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "serviceStop...");

	server.serverstate=PIR_STATE_TRAINED;
	clear();

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__serviceRestart(struct soap *soap ,int *tag)
{
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "serviceRestart...");

	server.serverstate=PIR_STATE_TRAINED;
	clear();

	if(server.serverstate==PIR_STATE_INITED||server.serverstate==PIR_STATE_TRAINED||server.serverstate==PIR_STATE_READY)
	{
		server.serverstate|=PIR_STATE_STARTING;
		server.progressPCT=0.f;

		while(q_create_thread(serviceStart_thread, NULL)) {
			server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
					"create serviceStart thread error...");
			q_sleep(1000);
		}
	}

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__state(struct soap* soap, int* tag)
{
#ifdef __VERBOSE_MODE
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "server state = (%d)", \
			server.serverstate);
#endif //__VERBOSE_MODE

	*tag=server.serverstate;
	return SOAP_OK;
}

int ns__progressPCT(struct soap* soap, float* tag)
{
	*tag=server.progressPCT;
	return SOAP_OK;
}

int ns__clear(struct soap* soap, int* tag)
{
	server.serverstate=PIR_STATE_INITED;
	clear();

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__pid(struct soap *soap ,int *tag)
{
	*tag=server.pid;
	return SOAP_OK;
}

int ns__quit(struct soap* soap, int* tag)
{
	server.shutdown_asap=1;
	soap_done(soap);
	q_sleep(3000);
	exit(0);

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int32_t http_get(struct soap* soap)
{
	FILE* fp=NULL;
	char* ptr=NULL;
	size_t len=0;

	ptr=strchr(soap->path, '?');
	if(ptr && !strcmp(ptr, "?wsdl"))
	{
		fp=::fopen(server.wsdl_path, "rb");
		if(!fp) {
			::fclose(fp);
			return 404;
		}

		soap->http_content="text/xml";
		soap_response(soap, SOAP_FILE);

		for(;;) {
			len=fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fp);
			if(len<=0)
				goto end;

			if(soap_send_raw(soap, soap->tmpbuf, len))
				goto end;
		}

		::fclose(fp);
	} else if(ptr && !strncmp(ptr, "?log=", 5)) {
		soap->http_content="text/plain";
		soap_response(soap, SOAP_FILE);

		char* logQueryBuf=q_new_array<char>(server.log_query_cache_size);
		Q_CHECK_PTR(logQueryBuf);

		int64_t len=0;

		if(getServerLog(&server, ptr+5, logQueryBuf, server.log_query_cache_size, &len)<0) {
			q_delete_array<char>(logQueryBuf);
			goto end;
		}

		if(soap_send_raw(soap, logQueryBuf, len)) {
			q_delete_array<char>(logQueryBuf);
			goto end;
		}

		q_delete_array<char>(logQueryBuf);
	} else {
		soap->http_content="text/plain";
		soap_response(soap, SOAP_FILE);

		char tmpbuf[10<<10]={0};
		len=getServerInfo(&server, tmpbuf, sizeof(tmpbuf));
		if(len<0)
			goto end;

		if(soap_send_raw(soap, tmpbuf, strlen(tmpbuf)))
			goto end;
	}

end:
	soap_end_send(soap);
	return SOAP_OK;
}

void sigShutdownHandler(int sig)
{
	char* msg;

	switch(sig) {
		case SIGINT:
			msg=(char*)"Received SIGINT scheduling shutdown...";
			break;
		case SIGTERM:
			msg=(char*)"Received SIGTERM scheduling shutdown...";
			break;
		default:
			msg=(char*)"Received shutdown signal, scheduling shutdown...";
	};

#if defined(__BLOCK_SIGNAL_SAFELY)
	/* SIGINT is often delivered via Ctrl+C in an interactive session.
	 * If we receive the signal the second time, we interpret this as
	 * the user really wanting to quit ASAP without waiting to persist
	 * on disk. */
	if(server.shutdown_asap && sig==SIGINT) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "You inisit... exiting now.");
		exit(1); /* Exit with an error since this was not a clean shutdown. */
	}
#endif

	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "%s", msg);
	server.shutdown_asap=1; /* Exit with a clean shutdown */
	return;
}

int32_t serviceTrain()
{
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceTrain starts: server state = (%d)...", \
			server.serverstate);

	server.serverstate=PIR_STATE_TRAINING;
	server.progressPCT=0.f;

	while(q_create_thread(train_thread, NULL)) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"serviceTrain create thread error...");
		q_sleep(1000);
	}

	return PIR_OK;
}

int32_t serviceStart()
{
	server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
			"serviceStart starts: server state = (%d)...", \
			server.serverstate);

	server.serverstate=PIR_STATE_STARTING;
	server.progressPCT=0.f;

	while(q_create_thread(serviceStart_thread, NULL)) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, \
				"serviceStart create thread error...");
		q_sleep(1000);
	}

	return PIR_OK;
}

int32_t main(int32_t argc, char **argv)
{
	struct timeval tv;
	setlocale(LC_COLLATE, "");
	srand(time(NULL)^getpid());
	gettimeofday(&tv, NULL);

	/* init config */
	initServerConfig(&server);

	/* init options */
	initServerOptions(&server, argc, argv);

	/* load config */
	if(loadServerConfig(&server)==PIR_ERR)
		exit(0);

	if(argc>=2) {
		/* Handle special options --help and --version */
		if(strcmp(argv[1], "-v")==0 || strcmp(argv[1], "--version")==0) {
			version();
			exit(0);
		} else if(strcmp(argv[1], "-h")==0 || strcmp(argv[1], "--help")==0) {
			usage();
			exit(0);
		}
		/* Handle special options -p or --port */
		else if(strcmp(argv[1], "-p")==0 || strcmp(argv[1], "--port")==0) {
			if(argc>2) {
				server.port=atoi(argv[2]);
			} else {
				usage();
				exit(0);
			}
		} else {
			usage();
			exit(0);
		}
	} else {
		usage();
		exit(0);
	}

	/* init */
	if(initServer(&server)==PIR_ERR)
		exit(0);

	/* daemonize */
	if(server.daemonize) daemonize();

#if defined (__USE_PIDFILE)
	if(server.daemonize) createPidFile(server.pidfile);
#endif

#if defined (__USE_SET_TITLE)
	/* set proc titile */
	setproctitle(argc, argv, server.proct_prefix, server.proct_type);
#endif
	
	/* listen */
	if(listenTcpServer(&server, inet_ntoa(server.ipv4), server.port, server.tcp_backlog)==PIR_ERR) {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "Port (%d) binds failed.....", server.port);
		exit(1);
	} else {
		server.main_logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, PIR_DEFAULT_LOG_SCREEN, "Port (%d) binds successfully, master socket = (%d)", \
				server.port, server.master);
	}

	/* autostart */
	if(server.failover_policy==1)
		serviceStart();
	else if(server.failover_policy==2)
		serviceTrain();

	/* event loop */
	serverMain(&server);

	/* free */
	freeServer(&server);
	return 0;
}
