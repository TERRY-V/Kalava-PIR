//gsoap ns service name: Imsearch
//gsoap ns service protocol: SOAP
//gsoap ns service encoding: encoded
//gsoap ns service executable: Imsearchserver.cgi
//gsoap ns service namespace: http://localhost:8888/Imsearch.wsdl
//gsoap ns service location: http://localhost:8888

//gsoap ns schema namespace: urn:Imsearch

#import "stlvector.h"

struct ns__pc {
	std::string pc;
	int score;
};

struct ns__searchResponse {
	int tag; 
	std::vector<struct ns__pc> match;
	std::string ocr;
	int consumedTime;
};

int ns__serviceInit(std::string workspace, std::string tpldir, std::string recgmethod, std::string option, int *tag);

int ns__train(int autostart, int* tag);

int ns__trainAdd(std::string imgPath, int* tag);

int ns__trainDelete(std::string imgPath, int* tag);

int ns__serviceStart(int* tag);

int ns__imSearch(std::string imgPath, struct ns__searchResponse* ret);

int ns__serviceStop(int* tag);

int ns__serviceRestart(int* tag);

int ns__state(int* tag);

int ns__progressPCT(float* tag);

int ns__clear(int* tag);

int ns__pid(int* tag);

int ns__quit(int* tag);

