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
#include "../../common/qremotemonitor.h"

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

#define PIR_PIDFILE            ("/var/run/kalava-master.pid")
#define PIR_CONFIG_FILE        ("../../../master.conf")

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
#define PIR_MAX_BUCKET_SIZE    (100)

#define PIR_DEFAULT_WORKER_INFO_FILE ("__vb.bss")

#define PIR_DEFAULT_DELTA_PEAK (100)
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

#define PIR_RANKING_KEY_MAX    (1024)
#define PIR_DEFAULT_QUERY_NUM  (10)

#define PIR_DEFAULT_OCR_ENABLED (0)
#define PIR_DEFAULT_OCR_SERVER ("http://192.168.1.38:8888")

#define PIR_DEFAULT_LOG_PATH   ("../log")
#define PIR_DEFAULT_LOG_PREFIX (NULL)
#define PIR_DEFAULT_LOG_SCREEN (1)
#define PIR_DEFAULT_LOG_SIZE   (1<<10)
#define PIR_LOG_QUERY_CACHE_SIZE (5<<20)

#define PIR_WATCHDOG_NOW       (1000)
#define PIR_WATCHDOG_INTERVAL  (10000)

#define PIR_DEFAULT_MONITOR_ENABLED (0)
#define PIR_DEFAULT_MONITOR_PORT (28888)

#define PIR_CONN_TIMEOUT       (8000)
#define PIR_RECV_TIMEOUT       (15000)
#define PIR_SEND_TIMEOUT       (15000)

#define PIR_PROC_PREFIX        ("yunshitu")

#define PIR_SOAP_TAG_OK        (1)
#define PIR_SOAP_TAG_ERR       (-1)

#define uSec                   *-1
#define mSec                   *-1000

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

struct searchThreadInfo {
	uint8_t		index;
	int32_t		conn_timeout;
	int32_t		recv_timeout;
	int32_t		send_timeout;
	std::string*	image_path;
	std::vector< std::pair<std::string, float> >* search_res;
	QMutexLock*	search_mutex;
	std::string*	ocr;
	QStopwatch	sw_search;

	searchThreadInfo() :
		conn_timeout(0),
		recv_timeout(0),
		send_timeout(0),
		image_path(NULL),
		search_res(NULL),
		search_mutex(NULL),
		ocr(NULL)
	{}
};

enum ranking_t {
	NoSort		= 0x00,
	AscendingSort	= 0x01,
	DescendingSort	= 0x02
};

struct searchObj {
	int32_t		objId;
	std::string	image_path;
	std::vector< std::pair<std::string, float> > search_res;
	QMutexLock	search_mutex;
	std::string	ocr;
	QStopwatch	search_sw;

	searchObj()
	{}

	void reset()
	{
		image_path.clear();
		search_res.clear();
		ocr.clear();
		search_sw.reset();
	}
};

struct workerInfo {
	std::string	workspace;
	std::string	tpldir;
	std::string	method;
	std::string	option;
	int32_t		state;
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
	int32_t		bucketSize;
	int32_t		bucketMax;
	std::vector<workerInfo> workerInfoVec;
	char*		workerInfoFile;		/* persistence for failover mechanism */
	QQueue<SOAP_SOCKET>* client_queue;
	QTrigger*	client_queue_trigger;
	/* training */
	uint32_t	delta_for_training;
	uint32_t	delta_peak;
	float		delta_ratio;
	time_t		idletime_for_training;
	QMutexLock	mutex_for_training;
	/* image search and relevance ranking */
	QQueue<searchObj*>* client_search_queue;
	ranking_t	ranking;
	int32_t		ranking_key_max;
	int32_t		query_num;
	/* ocr server */
	int32_t		ocr_enabled;
	char*		ocr_server;
	/* Fields used for log */
	char*		log_path;
	char*		log_prefix;
	int32_t		log_screen;
	int32_t		log_size;
	QLogger*	logger;
	int32_t		log_query_cache_size;
	/* watchdog for server status */
	time_t		watchdog_mtime;
	int32_t		watchdog_now;		/* current value can't be 0 */
	int32_t		watchdog_interval;
	QWatchdog	watchdog;
	/* remote monitor */
	int32_t		monitor_enabled;
	int32_t		monitor_port;
	QRemoteMonitor  monitor;
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
int32_t createMonitor(struct pirServer* server);
int32_t initServer(struct pirServer* server);
int32_t listenTcpServer(struct pirServer* server, const char* ip, int32_t port, int32_t tcp_backlog);
int32_t checkTcpBacklogSettings(struct pirServer* server, int32_t tcp_backlog);
int32_t serverMain(struct pirServer* server);
int32_t updateCachedTime(struct pirServer* server);
int32_t setupWatchdog(struct pirServer* server, int32_t now, int32_t interval);
int32_t getServerInfo(struct pirServer* server, char* server_info, int32_t server_info_size);
int32_t getServerLog(struct pirServer* server, const char* day_str, char* log_buf, int32_t log_size, int64_t* log_len);
int32_t freeServer(struct pirServer* server);

int32_t setupSignalHandlers(void);
void sigShutdownHandler(int sig);
void updateServerStatus(int32_t sig);
int32_t checkForDaemonizeMode(int32_t argc, char** argv);
int32_t checkForRecoveryMode(int32_t argc, char** argv);
int32_t checkTcpBacklogSettings(int32_t tcp_backlog);
int32_t getData(const std::string& s, int32_t& bucketSize, std::vector<std::string>& vec);
int32_t initWorkerInfo(const std::string& workspace, const std::string& tpldir, const std::string& method, const std::string& option, std::vector<workerInfo>& workerInfoVec);
int32_t loadWorkerInfo(const char* snapshotFile, std::vector<workerInfo>& workerInfoVec);
int32_t writeWorkerInfo(const std::vector<workerInfo>& workerInfoVec, const char* snapshotFile);
int32_t getThreadState(void* ptr_info);
int32_t http_get(struct soap* soap);
void *process_queue(void *ptr_info);

bool compareKeyAscending(std::pair<std::string, float> key1, std::pair<std::string, float> key2);
bool compareKeyDescending(std::pair<std::string, float> key1, std::pair<std::string, float> key2);

int32_t createPidFile(const char* pidfile);
int32_t daemonize(void);
int32_t setproctitle(int32_t argc, char** argv, char* prefix, char* type);

void usage(void);
void version(void);

#endif // __PIR_H_
