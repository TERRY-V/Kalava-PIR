#include "soapH.h"
#include "serlaunch.nsmap"

#include "../../common/qglobal.h"
#include "../../common/qconfigreader.h"
#include "../../common/qdir.h"
#include "../../common/qlogger.h"

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

#define PIR_PIDFILE            ("/var/run/kalava.pid")
#define PIR_CONFIG_FILE        ("../../../sentinel.conf")

#define PIR_SERVER_PORT        (29999)
#define PIR_TCP_BACKLOG        (511)
#define PIR_INVALID_SOCKET     (-1)

#define PIR_DEFAULT_CMD_SIZE   (1<<8)
#define PIR_DEFAULT_PATH_SIZE  (1<<8)

#define PIR_PROC_PREFIX        ((char*)"yunshitu")
#define PIR_PROC_TYPE          ((char*)"sentinel process")

/* sentinel config */
struct config_t {
	char		pidfile[PIR_DEFAULT_PATH_SIZE];
	int32_t		daemonize;
	int32_t		port;
	int32_t		tcp_backlog;
} config;

/* service version */
struct version_t {
	int32_t		type;
	const char*	str;
	const char*	summary;
	const char*	since;
} vers[] = {
	{0x00, NULL, NULL},
	{0x01, "split2.0", "master process", "1.0.0"},
	{0x02, "server3.0", "worker process", "1.0.0"}
};

int32_t initConfig()
{
	QConfigReader conf;
	if(conf.init(PIR_CONFIG_FILE)<0)
		return PIR_ERR;

	int32_t ret=0;

	ret=conf.getFieldString("pidfile", config.pidfile, sizeof(config.pidfile));
	if(ret<0)
		return PIR_ERR;

	ret=conf.getFieldYesNo("daemonize", config.daemonize);
	if(ret<0)
		return PIR_ERR;

	ret=conf.getFieldInt32("port", config.port);
	if(ret<0)
		return PIR_ERR;

	ret=conf.getFieldInt32("tcp-backlog", config.tcp_backlog);
	if(ret<0)
		return PIR_ERR;

	return PIR_OK;
}

int ns__launch(struct soap *soap, std::string workspace, std::string tpldir, std::string ip, std::string port, std::string version, std::string recgdir, int *tag)
{
	char cmd[PIR_DEFAULT_CMD_SIZE] = {0};
	int32_t status=0;

	Q_LOG(INFO, "workspace = (%s)", workspace.c_str());
	Q_LOG(INFO, "tpldir    = (%s)", tpldir.c_str());
	Q_LOG(INFO, "ipaddr    = (%s:%s)", ip.c_str(), port.c_str());
	Q_LOG(INFO, "version   = (%s)", version.c_str());
	Q_LOG(INFO, "recgdir   = (%s)", recgdir.c_str());

	try {
		if(!QDir::mkdir(workspace.c_str())) {
			Q_LOG(ERROR, "mkdir workspace (%s) error, (%d:%s)", workspace.c_str(), errno, strerror(errno));
			throw -1;
		}

		if(!QDir::chmod777(workspace.c_str())) {
			Q_LOG(ERROR, "chmod workspace (%s) error, (%d:%s)", workspace.c_str(), errno, strerror(errno));
			throw -2;
		}

		if(!QDir::mkdir(tpldir.c_str()) && (errno!=30)) {
			Q_LOG(ERROR, "mkdir tpldir (%s) error, (%d:%s)", tpldir.c_str(), errno, strerror(errno));
			throw -3;
		}

		if(!QDir::chmod777(tpldir.c_str()) && (errno!=30)) {
			Q_LOG(ERROR, "chmod tpldir (%s) error, (%d:%s)", tpldir.c_str(), errno, strerror(errno));
			throw -4;
		}

		if(!QDir::mkdir(recgdir.c_str())) {
			Q_LOG(ERROR, "mkdir recgdir (%s) error, (%d:%s)", recgdir.c_str(), errno, strerror(errno));
			throw -5;
		}

		if(!QDir::chmod777(recgdir.c_str())) {
			Q_LOG(ERROR, "chmod recgdir (%s) error, (%d:%s)", recgdir.c_str(), errno, strerror(errno));
			throw -6;
		}

		/*
		 * Reset errno for checking system function.
		 */
		errno=0;
		if(strcmp(version.c_str(), vers[1].str)==0) {
			snprintf(cmd, sizeof(cmd), "cd ../../../master/build/bin/; ./recgservice -p %s -d", port.c_str());

			// system() executes a command specified in command by calling /bin/sh -c command, and returns 
			// after the command has been completed. During execution of the command, SIGCHLD will be blocked, 
			// and SIGINT and SIGQUIT will be ignored.
			if(system(cmd)) {
				Q_LOG(ERROR, "start (%s) error, (%s)!", cmd, strerror(errno));
				throw -7;
			}
		} else if(strcmp(version.c_str(), vers[2].str)==0) {
			snprintf(cmd, sizeof(cmd), "cd ../../../worker/build/bin/; ./recgservice -p %s -d", port.c_str());

			if(system(cmd)) {
				Q_LOG(ERROR, "start (%s) error, (%s)!", cmd, strerror(errno));
				throw -8;
			}
		} else {
			Q_LOG(ERROR, "start error, version (%s) not supported!", version.c_str());
			throw -9;
		}

		Q_LOG(INFO, "Service (%s) starts okay!", cmd);
	} catch(const int32_t err) {
		status=err;
		Q_LOG(ERROR, "Service (%s) starts failed, errno = (%d)", cmd, status);
	}

	*tag=status;
	return SOAP_OK;
}

int http_get(struct soap* soap)
{
	FILE* fp=NULL;
	char* ptr=NULL;
	size_t len=0;

	ptr=strchr(soap->path, '?');
	if(ptr&&!strcmp(ptr, "?wsdl"))
	{
		fp=::fopen("../../src/serlaunch.wsdl", "rb");
		if(!fp) {
			::fclose(fp);
			return 404;
		}

		soap->http_content="text/xml";
		soap_response(soap, SOAP_FILE);

		for(;;) {
			len=fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fp);
			if(len<=0)
				goto err;

			if(soap_send_raw(soap, soap->tmpbuf, len))
				goto err;
		}

		::fclose(fp);
	} else {
		soap->http_content="text/plain";
		soap_response(soap, SOAP_FILE);

		char tmpbuf[1<<10]={0};
		strcpy(tmpbuf, "Sentinel: OK!");

		if(soap_send_raw(soap, tmpbuf, strlen(tmpbuf)))
			goto err;
	}

err:
	soap_end_send(soap);
	return SOAP_OK;
}

int32_t daemonize()
{
#ifdef WIN32
	return 0;
#else
	int32_t pid;
	int32_t fd;

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

	return 0;
#endif
}

int32_t setproctitle(int32_t argc, char** argv, char* prefix, char* type)
{
	char title[1<<10]={0};

	strcat(title, prefix);
	strcat(title, ":");
	strcat(title, " ");

	strcat(title, type);
	strcat(title, " ");

	for(int32_t i=0; i<argc; ++i)
	{
		strcat(title, argv[i]);
		if(i<argc-1) strcat(title, " ");
	}

	strcpy(argv[0], title);
	return 0;
}

int main(int argc, char **argv)
{
	struct timeval tv;
	setlocale(LC_COLLATE, "");
	srand(time(NULL)^getpid());
	gettimeofday(&tv, NULL);

	SOAP_SOCKET master, slave;
	struct soap soap;
	char* local_ip=NULL;

	/* init config */
	if(initConfig()==PIR_ERR) {
		Q_LOG(ERROR, "Sentinel: init config error...");
		return EXIT_FAILURE;
	}

	/* daemonize */
	if(config.daemonize)
		daemonize();

	/* lcoal ip */
	if((local_ip=q_local_ip())==NULL) {
		Q_LOG(ERROR, "Sentinel: get local ip error...");
		return EXIT_FAILURE;
	}

	/* bind and listen */
	Q_LOG(INFO, "Starting YUNSHITU sentinel on IP [%s:%d]......", local_ip, config.port);

	soap_init(&soap);
	soap.fget=http_get;

	master=soap_bind(&soap, local_ip, config.port, config.tcp_backlog);
	if(!soap_valid_socket(master)) {
		Q_LOG(ERROR, "Master socket (%d) is invalid.....", master);
		return EXIT_FAILURE;
	} else {
		Q_LOG(INFO, "Socket port (%d) binds successful: master socket = (%d)", config.port, master);
	}

#if __USE_SET_TITLE
	/* set proc title */
	setproctitle(argc, argv, PIR_PROC_PREFIX, PIR_PROC_TYPE);
#endif

	/* accept connections */
	for(int32_t req=1; ; ++req)
	{
		slave=soap_accept(&soap);
		if(!soap_valid_socket(slave)) {
			Q_LOG(ERROR, "Slave socket (%d) is invalid, waiting for retrying...", slave);
			continue;
		}

		Q_LOG(INFO, "---------------------------------------------------------------------------");
		Q_LOG(INFO, "Request task (%d), slave socket = (%d), IP = (%d.%d.%d.%d)...", \
				req, \
				slave, \
				(int32_t)(soap.ip>>24)&0xFF, \
				(int32_t)(soap.ip>>16)&0xFF, \
				(int32_t)(soap.ip>>8)&0xFF, \
				(int32_t)soap.ip&0xFF);

		if(soap_serve(&soap)!=SOAP_OK) {
			Q_LOG(ERROR, "Slave socket (%d) is invalid, soap_serve error...", slave);
			continue;
		}

		soap_destroy(&soap);
		soap_end(&soap);
	}

	soap_done(&soap);
	return EXIT_SUCCESS;
}
