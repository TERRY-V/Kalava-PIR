#include "soapH.h"
#include "Imsearch.nsmap"

#include "pir.h"

struct pirServer server;

void *search_thread(void* ptr_info)
{
	struct searchThreadInfo* search_info=reinterpret_cast<struct searchThreadInfo*>(ptr_info);
	Q_CHECK_PTR(search_info);

	struct soap soap;
	int32_t state=0;
	struct ns__searchResponse resp;

	/* soap init */
	soap_init(&soap);
	soap.connect_timeout=search_info->conn_timeout mSec;
	soap.recv_timeout=search_info->recv_timeout mSec;
	soap.send_timeout=search_info->send_timeout mSec;

	search_info->sw_search.start();

	/* image search */
	soap_call_ns__imSearch(&soap, \
			server.workerInfoVec[search_info->index].method.c_str(), \
			"", \
			search_info->image_path->c_str(), \
			&resp);

	if(soap.error) {
		server.logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server [%02d] (%s) search error...", \
				search_info->index, \
				server.workerInfoVec[search_info->index].method.c_str());
	} else {
		search_info->search_mutex->lock();
		for(int32_t i=0; i<resp.match.size(); ++i)
			search_info->search_res->push_back(std::pair<std::string, float>(resp.match[i].pc, resp.match[i].score));
		search_info->search_mutex->unlock();

		search_info->sw_search.stop();

#if defined (__VERBOSE_MODE)
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server [%02d] (%s) finds (%d) similar pictures, which consumed (%dms)!", \
				search_info->index, \
				server.workerInfoVec[search_info->index].method.c_str(), \
				resp.match.size(), \
				search_info->sw_search.elapsed_ms());
#endif
	}

	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);

	return NULL;
}

int setParams(const char* ocr_server, int intput, int *output)
{
	int32_t result=0;
	struct soap soap;
	soap_init(&soap);

	soap_call_ns2__setParams(&soap, ocr_server, "", intput, output);
	if(soap.error) {
		server.logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"Soap setParams error: %d, %s, %s", \
				soap.error, \
				*soap_faultcode(&soap), \
				*soap_faultstring(&soap));
		result=soap.error;
	}

	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	return result;
}

int textDetect(const char* ocr_server, xsd__base64Binary input, result *output)
{
	int32_t result=0;
	struct soap soap;
	soap_init(&soap);

	soap_call_ns2__textDetect(&soap, ocr_server, "", input, output);
	if(soap.error) {
		server.logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"Soap textDetect error: %d, %s, %s", \
				soap.error, \
				*soap_faultcode(&soap), \
				*soap_faultstring(&soap));
		result=soap.error;
	}

	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	return result;
}

void *textDetect_thread(void* ptr_info)
{
	struct searchThreadInfo* search_info=reinterpret_cast<struct searchThreadInfo*>(ptr_info);
	Q_CHECK_PTR(search_info);

	struct soap soap;
	int32_t input=0;
	int32_t output=0;
	xsd__base64Binary image_in;
	result result_out;
	FILE* inFp=NULL;

	struct soap *psoap=soap_new();
	Q_CHECK_PTR(psoap);

	search_info->sw_search.start();

	/* soap init */
	soap_init(psoap);
	psoap->connect_timeout=search_info->conn_timeout mSec;
	psoap->recv_timeout=search_info->recv_timeout mSec;
	psoap->send_timeout=search_info->send_timeout mSec;

	inFp=fopen(search_info->image_path->c_str(), "rb");
	if(inFp==NULL)
		goto end;

	fseek(inFp, 0L, SEEK_END);
	image_in.__size=::ftell(inFp);
	fseek(inFp, 0L, SEEK_SET);
	image_in.__ptr=(unsigned char*)soap_malloc(&soap, image_in.__size);
	fread(image_in.__ptr, 1, image_in.__size, inFp);

	fclose(inFp);
	inFp=NULL;

	if(setParams(server.ocr_server, input, &output)!=0)
		goto end;

	if(textDetect(server.ocr_server, image_in, &result_out)!=0)
		goto end;

	search_info->ocr->clear();
	for(int32_t i=0; i<result_out.boxes.size(); ++i)
	{
		char tmp[1<<10]={0};
		q_snprintf(tmp, sizeof(tmp), "[%d:%d:%d:%d] ", result_out.boxes[i].x, result_out.boxes[i].width, \
				result_out.boxes[i].y, \
				result_out.boxes[i].height);
		search_info->ocr->append(tmp);
#if 0
		Q_INFO("Box %d:", i);
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"Box %02d: x = (%d), w = (%d), y = (%d), h = (%d)", \
				i, \
				result_out.boxes[i].x, \
				result_out.boxes[i].width, \
				result_out.boxes[i].y, \
				result_out.boxes[i].height);
#endif
		for(int j=0; j<result_out.boxes[i].words.size(); ++j)
		{
			std::string trimmed_str=q_trim(result_out.boxes[i].words[j]);
			search_info->ocr->append(trimmed_str);
			search_info->ocr->append(" ");
		}
	}

#if 0
	FILE *outFp=::fopen("out.img", "wb");
	::fwrite(result_out.image.__ptr, 1, result_out.image.__size, outFp);
	::fclose(outFp);
	outFp = NULL;
#endif

end:
	soap_destroy(psoap);
	soap_end(psoap);
	soap_done(psoap);
	soap_free(psoap);

	search_info->sw_search.stop();

#if defined (__VERBOSE_MODE)
	server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
			"server index (%d) search consumed (%dms)", \
			search_info->index, \
			search_info->sw_search.elapsed_ms());
#endif
	return NULL;
}

int ns__imSearch(struct soap* soap, std::string imgPath, struct ns__searchResponse* ret)
{
	struct ns__searchResponse resp;
	resp.tag=PIR_SOAP_TAG_OK;

	struct ns__pc __pc;
	pthread_t* ptr_tid=NULL;
	struct searchThreadInfo* search_info=NULL;
	struct searchObj* search_object=NULL;
	int32_t query_num=0;

#ifdef __VERBOSE_MODE
	server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
			"server search (%s) starts...", \
			imgPath.c_str());
#endif

	if(server.serverstate==PIR_STATE_READY)
	{
		ptr_tid=q_new_array<pthread_t>(server.bucketSize+1);
		Q_CHECK_PTR(ptr_tid);

		search_info=q_new_array<struct searchThreadInfo>(server.bucketSize+1);
		Q_CHECK_PTR(search_info);

		search_object=server.client_search_queue->pop();
		Q_CHECK_PTR(search_object);

		search_object->image_path=imgPath;
		search_object->search_res.clear();
		search_object->search_sw.start();

		q_lock_inc(&server.stat_search_conn);

		/* create search threads */
		for(int32_t i=0; i<server.bucketSize; ++i)
		{
			search_info[i].index=i;
			search_info[i].conn_timeout=server.conn_timeout;
			search_info[i].recv_timeout=server.recv_timeout;
			search_info[i].send_timeout=server.send_timeout;
			search_info[i].image_path=&search_object->image_path;
			search_info[i].search_res=&search_object->search_res;
			search_info[i].search_mutex=&search_object->search_mutex;

			while(q_create_thread(&ptr_tid[i], search_thread, (void*)&search_info[i])<0)
				q_sleep(1);
		}

		/* ocr threads */
		if(server.ocr_enabled)
		{
			for(int32_t i=server.bucketSize; i<server.bucketSize+1; ++i)
			{
				search_info[i].index=i;
				search_info[i].conn_timeout=server.conn_timeout;
				search_info[i].recv_timeout=server.recv_timeout;
				search_info[i].send_timeout=server.send_timeout;
				search_info[i].image_path=&search_object->image_path;
				search_info[i].ocr=&search_object->ocr;

				while(q_create_thread(&ptr_tid[i], textDetect_thread, (void*)&search_info[i])<0)
					q_sleep(1);
			}
		}

		/* join with terminated search threads */
		for(int32_t i=0; i<server.bucketSize; ++i)
		{
			q_thread_join(ptr_tid[i]);
		}

		/* join with terminated ocr thread */
		if(server.ocr_enabled)
		{
			for(int32_t i=server.bucketSize; i<server.bucketSize+1; ++i)
			{
				q_thread_join(ptr_tid[i]);
			}
		}

		q_delete_array<pthread_t>(ptr_tid);
		q_delete_array<searchThreadInfo>(search_info);

#if defined (__TEST_SORT)
		QStopwatch sw_sort;
		sw_sort.start();
#endif

		/* use stl sort algorithm */
		if(search_object->search_res.size())
		{
			switch(server.ranking)
			{
				case AscendingSort:
					std::sort(search_object->search_res.begin(), search_object->search_res.end(), compareKeyAscending);
					break;
				case DescendingSort:
					std::sort(search_object->search_res.begin(), search_object->search_res.end(), compareKeyDescending);
					break;
				default:
					std::sort(search_object->search_res.begin(), search_object->search_res.end(), compareKeyDescending);
					break;
			}
		}

#if defined (__TEST_SORT)
		sw_sort.stop();
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server search sort consumed (%dms)...", \
				sw_sort.elapsed_ms());
#endif

		resp.tag=PIR_SOAP_TAG_OK;
		query_num=(server.query_num<=search_object->search_res.size())?server.query_num:search_object->search_res.size();
		for(int32_t i=0; i<query_num; ++i)
		{
			__pc.pc=search_object->search_res[i].first;
			__pc.score=search_object->search_res[i].second;
			resp.match.push_back(__pc);
		}

		if(server.ocr_enabled)
			resp.ocr=search_object->ocr;

		search_object->search_sw.stop();
		resp.consumedTime=search_object->search_sw.elapsed_ms();

#ifdef __VERBOSE_MODE
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server search finds (%d) simpliar pictures totally, which consumed (%dms)...", \
				resp.match.size(), \
				search_object->search_sw.elapsed_ms());
#endif

		search_object->reset();
		server.client_search_queue->push(search_object);
	} else {
		resp.tag=PIR_STATE_ERROR;
		q_lock_inc(&server.stat_failed_conn);

		server.logger->log(LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server state is not (%d)!", \
				PIR_STATE_READY);
	}

	*ret=resp;
	return SOAP_OK;
}

int ns__serviceInit(struct soap* soap, std::string workspace, std::string tpldir, std::string method, std::string option, int* tag)
{
	int32_t ret=PIR_SOAP_TAG_OK;

	server.serverstate=PIR_STATE_INITING;

	if(initWorkerInfo(workspace, tpldir, method, option, server.workerInfoVec)==PIR_ERR)
		goto end;

	server.bucketSize=server.workerInfoVec.size();

	for(int32_t i=0; i<server.bucketSize; ++i)
	{
		struct soap soap;
		soap_init(&soap);

#if defined (__VERBOSE_MODE)
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server index (%d) init begins...", i);
#endif
		soap_call_ns__serviceInit(&soap, \
				server.workerInfoVec[i].method.c_str(), \
				"", \
				server.workerInfoVec[i].workspace.c_str(), \
				server.workerInfoVec[i].tpldir.c_str(), \
				server.workerInfoVec[i].method.c_str(), \
				server.workerInfoVec[i].option.c_str(), \
				&ret);
		if(soap.error) {
			soap_print_fault(&soap, stderr);
			ret=PIR_SOAP_TAG_ERR;
		}

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);

		if(ret==PIR_SOAP_TAG_ERR)
			goto end;
	}

	server.serverstate=PIR_STATE_INITED;

	/* point-in-time snapshot of worker info */
	if(writeWorkerInfo(server.workerInfoVec, server.workerInfoFile)==PIR_ERR)
		goto end;

end:
	*tag=ret;
	return SOAP_OK;
}

float getMYprogressPCT(int32_t index)
{
	float resp=0;
	struct soap soap;
	soap_init(&soap);

	soap_call_ns__progressPCT(&soap, server.workerInfoVec[index].method.c_str(), "", &resp);
	if(soap.error) {
		soap_print_fault(&soap, stderr);
		resp=1.f;
	}

	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	return resp;
}

void *train_thread(void*)
{
	int32_t completeTrainNum=0;
	int32_t state=0;

	for(int32_t i=0; i<server.bucketSize; ++i)
	{
		int resp;
		struct soap soap;
		soap_init(&soap);

#if defined (__VERBOSE_MODE)
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server (%02d) training begins...", i);
#endif

		soap_call_ns__train(&soap, server.workerInfoVec[i].method.c_str(), "", 0, &resp);
		if(soap.error)
			soap_print_fault(&server.soap, stderr);

		while(getMYprogressPCT(i)<1.0)
			q_sleep(3000);

		soap_call_ns__state(&soap, server.workerInfoVec[i].method.c_str(), "", &state);

		if(soap.error||state!=PIR_STATE_TRAINED) {
			soap_print_fault(&soap, stderr);
			soap_destroy(&soap);
			soap_end(&soap);
			soap_done(&soap);
			return NULL;
		} else {
			completeTrainNum++;
		}

#if defined (__VERBOSE_MODE)
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server (%02d) training ends, state = (%d)...", \
				i, \
				state);
#endif

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	return NULL;
}

int ns__train(struct soap* soap, int autostart, int* tag)
{
	server.autostart=autostart;
	while(q_create_thread(train_thread, NULL)) {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"serviceTrain create thread error...");
		q_sleep(1000);
	}

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__trainAdd(struct soap* soap, std::string imgPath, int* tag)
{
	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__trainDelete(struct soap* soap, std::string imgPath, int* tag)
{
	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

void *serviceStart_thread(void*)
{
	for(int32_t i=0; i<server.bucketSize; i++)
	{
		int32_t resp;
		struct soap soap;
		soap_init(&soap);

#if defined (__VERBOSE_MODE)
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"server (%02d) start begins...", i);
#endif
		soap_call_ns__serviceStart(&soap, server.workerInfoVec[i].method.c_str(), "", &resp);
		if(soap.error)
			soap_print_fault(&soap, stderr);

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}
	return NULL;
}

int ns__serviceStart(struct soap* soap, int* tag)
{
	while(q_create_thread(serviceStart_thread, NULL)) {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"serviceStart create serviceStart thread error...");
		q_sleep(1);
	}

	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

void clear()
{
	for(int32_t i=0 ; i<server.bucketSize; ++i)
	{
		int32_t resp;
		struct soap soap;
		soap_init(&soap);

		soap_call_ns__serviceStop(&soap, server.workerInfoVec[i].method.c_str(), "", &resp);
		if(soap.error)
			soap_print_fault(&soap, stderr);

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}
}

int ns__serviceStop(struct soap* soap, int* tag)
{
	clear();
	server.serverstate=PIR_STATE_TRAINED;
	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__serviceRestart(struct soap *soap ,int *tag)
{
	clear();
	server.serverstate=PIR_STATE_TRAINED;
	while(q_create_thread(serviceStart_thread, NULL)) {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
				"serviceStart create thread error...");
		q_sleep(1);
	}
	*tag=PIR_SOAP_TAG_OK;
	return SOAP_OK;
}

int ns__state(struct soap* soap, int* tag)
{
	*tag=server.serverstate;
	return SOAP_OK;
}

int ns__progressPCT(struct soap* soap, float* tag)
{
	float ret=0;
	for(int32_t i=0; i<server.bucketSize; ++i)
	{
		float resp;
		struct soap soap;
		soap_init(&soap);

		soap_call_ns__progressPCT(&soap, server.workerInfoVec[i].method.c_str(), "", &resp);
		if(soap.error) {
			soap_print_fault(&soap, stderr);
		} else {
			ret+=resp;
		}

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	ret/=server.bucketSize;
	server.progressPCT=ret;
	*tag=server.progressPCT;
	return SOAP_OK;
}

int ns__clear(struct soap* soap, int* tag)
{
	clear();
	server.serverstate=PIR_STATE_INITED;
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
	for(int32_t i=0; i<server.bucketSize; ++i)
	{
		int32_t resp;
		struct soap soap;
		soap_init(&soap);

		soap_call_ns__quit(&soap, server.workerInfoVec[i].method.c_str(), "", &resp);
		if(soap.error)
			soap_print_fault(&soap, stderr);

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	*tag=PIR_SOAP_TAG_OK;
	soap_done(soap);
	exit(0);
	return SOAP_OK;
}

int ns2__setParams(struct soap *soap, int input, int *output)
{
	return SOAP_NO_METHOD;
}

int ns2__textDetect(struct soap *soap, xsd__base64Binary input, result *output)
{
	return SOAP_NO_METHOD;
}

int32_t http_get(struct soap* soap)
{
	FILE* fp=NULL;
	char* ptr=NULL;
	size_t len=0;

	ptr=strchr(soap->path, '?');
	if(ptr&&!strcmp(ptr, "?wsdl"))
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

	/* SIGINT is often delivered via Ctrl+C in an interactive session.
	 * If we receive the signal the second time, we interpret this as
	 * the user really wanting to quit ASAP without waiting to persist
	 * on disk. */
	if(server.shutdown_asap && sig==SIGINT) {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, "You inisit... exiting now.");
		exit(1); /* Exit with an error since this was not a clean shutdown. */
	}

	server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, "%s", msg);
	server.shutdown_asap=1; /* Exit with a clean shutdown */
	return;
}

void updateServerStatus(int sig)
{
	Q_NOTUSED(sig);

	int32_t state=0x7fffffff;
	int32_t ret=0;

	struct stat buf;
	if(stat(server.workerInfoFile, &buf)<0)
		return;

	/* check whether bss file was modified, a mutex lock is necessary */
	if(buf.st_mtime>server.watchdog_mtime)
	{
		server.watchdog_mtime=buf.st_mtime;

		server.bucketSize=0;
		server.workerInfoVec.clear();

		/* populate the workerInfoVec */
		ret=loadWorkerInfo(server.workerInfoFile, server.workerInfoVec);
		if(ret<0) {
			server.logger->log(LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
					"loadWorkerInfo error, ret = (%d)!", ret);
			exit(0);
		}

		/* update bucketSize */
		server.bucketSize=server.workerInfoVec.size();
	}

	/* compute server status */
	for(int32_t i=0; i<server.bucketSize; ++i)
	{
		struct soap soap;

		soap_init(&soap);
		soap.recv_timeout=server.recv_timeout;
		soap.send_timeout=server.send_timeout;
		soap.socket_flags=MSG_NOSIGNAL;

		soap_call_ns__state(&soap, server.workerInfoVec[i].method.c_str(), "", &server.workerInfoVec[i].state);
		if(soap.error) {
			server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
					"connect server (%s) error...", \
					server.workerInfoVec[i].method.c_str());
			server.serverstate=PIR_STATE_ERROR;
			goto end;
		}

		if(server.workerInfoVec[i].state<state)
			state=server.workerInfoVec[i].state;

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	if(state==0x7fffffff)
		server.serverstate=PIR_STATE_ERROR;
	else
		server.serverstate=state;

end:
#if defined(__VERBOSE_MODE) && defined (__WATCHDOG_LOG)
	server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, \
			"server state = (%d)...", \
			server.serverstate);
#endif
	return;
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
	/* pidfile */
	if(server.daemonize) createPidFile(server.pidfile);
#endif

#if defined (__USE_SET_TITLE)
	/* set proc titile */
	setproctitle(argc, argv, server.proct_prefix, server.proct_type);
#endif

	/* monitor */
	if(server.monitor_enabled) createMonitor(&server);
	
	/* listen */
	if(listenTcpServer(&server, inet_ntoa(server.ipv4), server.port, server.tcp_backlog)==PIR_ERR) {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, "Port (%d) binds failed.....", server.port);
		exit(1);
	} else {
		server.logger->log(LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, server.log_screen, "Port (%d) binds successful, master socket = (%d)", \
				server.port, server.master);
	}

	/* event loop */
	serverMain(&server);

	/* free */
	freeServer(&server);
	return 0;
}
