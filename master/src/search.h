//gsoap ns service name: Imsearch
//gsoap ns service protocol: SOAP
//gsoap ns service encoding: encoded
//gsoap ns service executable: Imsearch.cgi
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

//gsoap ns2 service name: textDetect
//gsoap ns2 service protocol: SOAP
//gsoap ns2 service encoding: encoded
//gsoap ns2 service executable: textDetect.cgi
//gsoap ns2 service namespace: http://localhost:8888/textDetect.wsdl
//gsoap ns2 service location: http://localhost:8888

//gsoap ns2 schema namespace: urn:textDetect

class xsd__base64Binary {
	public:
		unsigned char *__ptr;
		int __size;
};

class box {
	public:
		int x;
		int width;
		int y;
		int height;
		std::vector<std::string> words;
};

class result {
	public:
		int state;
		std::vector<box> boxes;
		xsd__base64Binary image;
};

int ns2__setParams(int input, int *output);

int ns2__textDetect(xsd__base64Binary input, result *output);

