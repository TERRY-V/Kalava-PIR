/********************************************************************************************
**
** Copyright (C) 2010-2015 Terry Niu (Beijing, China)
** Filename:	pir.h
** Author:	TERRY-V
** Email:	cnbj8607@163.com
** Support:	http://blog.sina.com.cn/terrynotes
** Date:	2015/04/27
**
*********************************************************************************************/

#ifndef __PIR_H_
#define __PIR_H_

#include "./soapH.h"
#include "../../common/qglobal.h"
#include "../../common/qalgorithm.h"
#include "../../common/qconfigreader.h"
#include "../../common/qdatetime.h"
#include "../../common/qfunc.h"
#include "../../common/qlogger.h"
#include "../../common/qqueue.h"

using namespace kalava;

#define PIR_OK                 (0)
#define PIR_ERR                (-1)

#define PIR_VERSION_STR        ("1.0.0")
#define PIR_VERSION            (0x010000)

#define PIR_MODE_UNKNOWN       (0x00)
#define PIR_MODE_SENTINEL      (0x01)
#define PIR_MODE_MASTER        (0x02)
#define PIR_MODE_WORKER        (0x03)

#define PIR_DEFAULT_HZ         (10)
#define PIR_MIN_HZ             (1)
#define PIR_MAX_HZ             (500)

#define PIR_PIDFILE            ("/var/run/kalava-worker.pid")
#define PIR_CONFIG_FILE        ("../../../worker.conf")

#define PIR_SERVER_PORT        (-1)
#define PIR_TCP_BACKLOG        (511)
#define PIR_INVALID_SOCKET     (-1)

#define PIR_STATE_NONE         (0x00)
#define PIR_STATE_INITING      (0x01)
#define PIR_STATE_INITED       (0x02)
#define PIR_STATE_TRAINING     (0x03)
#define PIR_STATE_TRAINED      (0x04)
#define PIR_STATE_STARTING     (0x05)
#define PIR_STATE_READY        (0x06)

#define PIR_STATE_ERROR        (0x7f)

#define PIR_DEFAULT_AUTOSTART  (0)
#define PIR_DEFAULT_DAEMONIZE  (0)
#define PIR_DEFAULT_FAILOVER_POLICY (0)

#define PIR_SETTING_XML_PATH   ("../../setting/")
#define PIR_SETTING_XML        ("setting.xml")

#define PIR_MAX_CLIENTS        (64)
#define PIR_MAX_MEMORY         (0)

#define PIR_DEFAULT_DELTA_PEAK (1000)
#define PIR_DEFAULT_DELTA_RATIO (0.75)
#define PIR_DEFAULT_IDLETIME   (60000)

#define PIR_THREAD_NUM         (8)
#define PIR_THREAD_BUF_SIZE    (0)
#define PIR_THREAD_TIMEOUT     (60000)

#define PIR_THREAD_STATUS_IDLE (0x00)
#define PIR_THREAD_STATUS_BUSY (0x01)

#define PIR_THREAD_NOTSTARTED  (0x00)
#define PIR_THREAD_RUNNING     (0x01)
#define PIR_THREAD_STOPPED     (0x02)

#define PIR_DEFAULT_LOG_PATH   ("../log")
#define PIR_DEFAULT_LOG_PREFIX (NULL)
#define PIR_DEFAULT_LOG_SCREEN (0)
#define PIR_DEFAULT_LOG_SIZE   (1<<10)

#define PIR_LOG_QUERY_CACHE_SIZE (5<<20)

#define PIR_CONN_TIMEOUT       (8000)
#define PIR_RECV_TIMEOUT       (15000)
#define PIR_SEND_TIMEOUT       (15000)

#define PIR_PROC_PREFIX        ("yunshitu")

#define PIR_SOAP_TAG_OK        (1)		/* from old PIR versions */
#define PIR_SOAP_TAG_ERR       (-1)

struct serverType {
	uint8_t		stype;
	const char*	name;
	const char*	str;
	const char*	summary;
	const char*	wsdl;
	int32_t		group;
	const char*	since;
};

struct serverState {
	uint8_t		sstate;
	const char*	str;
	const char*	summary;
	int32_t		group;
	const char*	since;
};

struct threadInfo {
	void*		pthis;
	uint32_t	id:8;
	uint32_t	status:1;
	uint32_t	flag:8;
#if defined (__HAVING_THREAD_BUF__)
	int32_t		buf_size;
	char*		ptr_buf;
#endif
	int32_t		timeout;
	struct soap*	psoap;
	QStopwatch	stopwatch;

	threadInfo() :
		pthis(NULL),
		id(0),
		status(PIR_THREAD_STATUS_IDLE),
		flag(PIR_THREAD_NOTSTARTED),
#if defined (__HAVING_THREAD_BUF__)
		buf_size(PIR_THREAD_BUF_SIZE),
		ptr_buf(NULL),
#endif
		timeout(PIR_THREAD_TIMEOUT),
		psoap(NULL)
	{}
};

struct searchObj {
	int32_t		objId;
	std::string	image_path;
	std::vector< std::pair<std::string, float> > search_res;
	QStopwatch	search_sw;

	searchObj()
	{}

	void reset()
	{
		image_path.clear();
		search_res.clear();
		search_sw.reset();
	}
};

struct pirServer {
	/* General */
	uint32_t	pid;
	char*		pidfile;
	char*		configfile;
	uint8_t		flags;
	time_t		unixtime;
	int32_t         argc;
	char**          argv;
	/* configuration */
	uint32_t	hz:8;
	uint32_t	arch_bits:8;		/* 32 or 64 depending on sizeof(long) */
	uint32_t	serverstate:8;
	uint32_t	autostart:1;
	uint32_t	daemonize:1;
	uint32_t	failover_policy:1;
	uint32_t	shutdown_asap:1;
	float		progressPCT;
	char*		proct_prefix;
	char*		proct_type;
	char*		wsdl_path;
	char*		setting_xml_path;
	char*		setting_xml;
	/* limits */
	uint32_t	maxclients;
	uint64_t	maxmemory;
	/* threads */
	uint8_t		thread_num;
	int32_t		thread_timeout;
	int32_t		thread_buf_size;
	struct threadInfo* thread_info;
	pthread_t*	tid;
	/* Networking */
	struct in_addr	ipv4;
	int32_t		port;
	int32_t		tcp_backlog;
	time_t		lastinteraction;
	struct soap	soap;
	SOAP_SOCKET	master;
	int32_t		conn_timeout;
	int32_t		recv_timeout;
	int32_t		send_timeout;
	std::string	workspace;
	std::string	tpldir;
	std::string	method;
	std::string	option;
	std::string	local_log_path;
	QQueue<SOAP_SOCKET>* client_queue;
	QTrigger*	client_queue_trigger;
	/* training */
	uint32_t	delta_for_training;
	uint32_t	delta_peak;
	float		delta_ratio;
	time_t		idletime_for_training;
	QMutexLock	mutex_for_training;
	/* image search algorithm */
	QQueue<searchObj*>* client_search_queue;
	/* Fields used for log */
	char*		main_log_path;
	char*		main_log_prefix;
	int32_t		main_log_size;
	QLogger*	main_logger;
	int32_t		log_query_cache_size;
	/* Fields used only for stats */
	time_t		stat_starttime;
	uint64_t	stat_numconnections;
	uint64_t	stat_search_conn;
	uint64_t	stat_rejected_conn;
	uint64_t	stat_failed_conn;
};

int32_t initServerConfig(struct pirServer* server);
int32_t initServerOptions(struct pirServer* server, int32_t argc, char** argv);
int32_t loadServerConfig(struct pirServer* server);
int32_t resetServerStats(struct pirServer* server);
int32_t initServer(struct pirServer* server);
int32_t listenTcpServer(struct pirServer* server, const char* ip, int32_t port, int32_t tcp_backlog);
int32_t checkTcpBacklogSettings(struct pirServer* server, int32_t tcp_backlog);
int32_t switchLoggingDirectory(struct pirServer* server);
int32_t serverMain(struct pirServer* server);
int32_t updateCachedTime(struct pirServer* server);
int32_t getServerInfo(struct pirServer* server, char* server_info, int32_t server_info_size);
int32_t getServerLog(struct pirServer* server, const char* day_str, char* log_buf, int32_t log_size, int64_t* log_len);
int32_t getThreadState(struct pirServer* server);
int32_t loadWorkerInfo(struct pirServer* server);
int32_t writeWorkerInfo(struct pirServer* server);
int32_t freeServer(struct pirServer* server);

int32_t setupSignalHandlers(void);
void sigShutdownHandler(int sig);
int32_t checkForDaemonizeMode(int32_t argc, char** argv);
int32_t checkForRecoveryMode(int32_t argc, char** argv);
int32_t checkTcpBacklogSettings(int32_t tcp_backlog);
int32_t http_get(struct soap* soap);
void *process_queue(void *ptr_info);

int32_t createPidFile(const char* pidfile);
int32_t daemonize(void);
int32_t setproctitle(int32_t argc, char** argv, char* prefix, char* type);

void usage(void);
void version(void);

#endif // __PIR_H_
