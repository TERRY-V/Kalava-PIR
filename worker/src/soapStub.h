/* soapStub.h
   Generated by gSOAP 2.8.17r from search.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapStub_H
#define soapStub_H
#include <vector>
#include "stdsoap2.h"
#if GSOAP_VERSION != 20817
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef SOAP_TYPE_ns__pc
#define SOAP_TYPE_ns__pc (8)
/* ns:pc */
struct ns__pc
{
public:
	std::string pc;	/* required element of type xsd:string */
	int score;	/* required element of type xsd:int */
public:
	int soap_type() const { return 8; } /* = unique type id SOAP_TYPE_ns__pc */
};
#endif

#ifndef SOAP_TYPE_ns__searchResponse
#define SOAP_TYPE_ns__searchResponse (10)
/* ns:searchResponse */
struct ns__searchResponse
{
public:
	int tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
	std::vector<struct ns__pc >match;	/* optional element of type ns:pc */
	std::string ocr;	/* required element of type xsd:string */
	int consumedTime;	/* required element of type xsd:int */
public:
	int soap_type() const { return 10; } /* = unique type id SOAP_TYPE_ns__searchResponse */
};
#endif

#ifndef SOAP_TYPE_ns__serviceInitResponse
#define SOAP_TYPE_ns__serviceInitResponse (14)
/* ns:serviceInitResponse */
struct ns__serviceInitResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 14; } /* = unique type id SOAP_TYPE_ns__serviceInitResponse */
};
#endif

#ifndef SOAP_TYPE_ns__serviceInit
#define SOAP_TYPE_ns__serviceInit (15)
/* ns:serviceInit */
struct ns__serviceInit
{
public:
	std::string workspace;	/* required element of type xsd:string */
	std::string tpldir;	/* required element of type xsd:string */
	std::string recgmethod;	/* required element of type xsd:string */
	std::string option;	/* required element of type xsd:string */
public:
	int soap_type() const { return 15; } /* = unique type id SOAP_TYPE_ns__serviceInit */
};
#endif

#ifndef SOAP_TYPE_ns__trainResponse
#define SOAP_TYPE_ns__trainResponse (17)
/* ns:trainResponse */
struct ns__trainResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 17; } /* = unique type id SOAP_TYPE_ns__trainResponse */
};
#endif

#ifndef SOAP_TYPE_ns__train
#define SOAP_TYPE_ns__train (18)
/* ns:train */
struct ns__train
{
public:
	int autostart;	/* required element of type xsd:int */
public:
	int soap_type() const { return 18; } /* = unique type id SOAP_TYPE_ns__train */
};
#endif

#ifndef SOAP_TYPE_ns__trainAddResponse
#define SOAP_TYPE_ns__trainAddResponse (20)
/* ns:trainAddResponse */
struct ns__trainAddResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 20; } /* = unique type id SOAP_TYPE_ns__trainAddResponse */
};
#endif

#ifndef SOAP_TYPE_ns__trainAdd
#define SOAP_TYPE_ns__trainAdd (21)
/* ns:trainAdd */
struct ns__trainAdd
{
public:
	std::string imgPath;	/* required element of type xsd:string */
public:
	int soap_type() const { return 21; } /* = unique type id SOAP_TYPE_ns__trainAdd */
};
#endif

#ifndef SOAP_TYPE_ns__trainDeleteResponse
#define SOAP_TYPE_ns__trainDeleteResponse (23)
/* ns:trainDeleteResponse */
struct ns__trainDeleteResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 23; } /* = unique type id SOAP_TYPE_ns__trainDeleteResponse */
};
#endif

#ifndef SOAP_TYPE_ns__trainDelete
#define SOAP_TYPE_ns__trainDelete (24)
/* ns:trainDelete */
struct ns__trainDelete
{
public:
	std::string imgPath;	/* required element of type xsd:string */
public:
	int soap_type() const { return 24; } /* = unique type id SOAP_TYPE_ns__trainDelete */
};
#endif

#ifndef SOAP_TYPE_ns__serviceStartResponse
#define SOAP_TYPE_ns__serviceStartResponse (26)
/* ns:serviceStartResponse */
struct ns__serviceStartResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 26; } /* = unique type id SOAP_TYPE_ns__serviceStartResponse */
};
#endif

#ifndef SOAP_TYPE_ns__serviceStart
#define SOAP_TYPE_ns__serviceStart (27)
/* ns:serviceStart */
struct ns__serviceStart
{
public:
	int soap_type() const { return 27; } /* = unique type id SOAP_TYPE_ns__serviceStart */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__imSearch
#define SOAP_TYPE_ns__imSearch (30)
/* ns:imSearch */
struct ns__imSearch
{
public:
	std::string imgPath;	/* required element of type xsd:string */
public:
	int soap_type() const { return 30; } /* = unique type id SOAP_TYPE_ns__imSearch */
};
#endif

#ifndef SOAP_TYPE_ns__serviceStopResponse
#define SOAP_TYPE_ns__serviceStopResponse (32)
/* ns:serviceStopResponse */
struct ns__serviceStopResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 32; } /* = unique type id SOAP_TYPE_ns__serviceStopResponse */
};
#endif

#ifndef SOAP_TYPE_ns__serviceStop
#define SOAP_TYPE_ns__serviceStop (33)
/* ns:serviceStop */
struct ns__serviceStop
{
public:
	int soap_type() const { return 33; } /* = unique type id SOAP_TYPE_ns__serviceStop */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__serviceRestartResponse
#define SOAP_TYPE_ns__serviceRestartResponse (35)
/* ns:serviceRestartResponse */
struct ns__serviceRestartResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 35; } /* = unique type id SOAP_TYPE_ns__serviceRestartResponse */
};
#endif

#ifndef SOAP_TYPE_ns__serviceRestart
#define SOAP_TYPE_ns__serviceRestart (36)
/* ns:serviceRestart */
struct ns__serviceRestart
{
public:
	int soap_type() const { return 36; } /* = unique type id SOAP_TYPE_ns__serviceRestart */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__stateResponse
#define SOAP_TYPE_ns__stateResponse (38)
/* ns:stateResponse */
struct ns__stateResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 38; } /* = unique type id SOAP_TYPE_ns__stateResponse */
};
#endif

#ifndef SOAP_TYPE_ns__state
#define SOAP_TYPE_ns__state (39)
/* ns:state */
struct ns__state
{
public:
	int soap_type() const { return 39; } /* = unique type id SOAP_TYPE_ns__state */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__progressPCTResponse
#define SOAP_TYPE_ns__progressPCTResponse (43)
/* ns:progressPCTResponse */
struct ns__progressPCTResponse
{
public:
	float *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:float */
public:
	int soap_type() const { return 43; } /* = unique type id SOAP_TYPE_ns__progressPCTResponse */
};
#endif

#ifndef SOAP_TYPE_ns__progressPCT
#define SOAP_TYPE_ns__progressPCT (44)
/* ns:progressPCT */
struct ns__progressPCT
{
public:
	int soap_type() const { return 44; } /* = unique type id SOAP_TYPE_ns__progressPCT */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__clearResponse
#define SOAP_TYPE_ns__clearResponse (46)
/* ns:clearResponse */
struct ns__clearResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 46; } /* = unique type id SOAP_TYPE_ns__clearResponse */
};
#endif

#ifndef SOAP_TYPE_ns__clear
#define SOAP_TYPE_ns__clear (47)
/* ns:clear */
struct ns__clear
{
public:
	int soap_type() const { return 47; } /* = unique type id SOAP_TYPE_ns__clear */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__pidResponse
#define SOAP_TYPE_ns__pidResponse (49)
/* ns:pidResponse */
struct ns__pidResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 49; } /* = unique type id SOAP_TYPE_ns__pidResponse */
};
#endif

#ifndef SOAP_TYPE_ns__pid
#define SOAP_TYPE_ns__pid (50)
/* ns:pid */
struct ns__pid
{
public:
	int soap_type() const { return 50; } /* = unique type id SOAP_TYPE_ns__pid */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef SOAP_TYPE_ns__quitResponse
#define SOAP_TYPE_ns__quitResponse (52)
/* ns:quitResponse */
struct ns__quitResponse
{
public:
	int *tag;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:int */
public:
	int soap_type() const { return 52; } /* = unique type id SOAP_TYPE_ns__quitResponse */
};
#endif

#ifndef SOAP_TYPE_ns__quit
#define SOAP_TYPE_ns__quit (53)
/* ns:quit */
struct ns__quit
{
public:
	int soap_type() const { return 53; } /* = unique type id SOAP_TYPE_ns__quit */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (54)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	int soap_type() const { return 54; } /* = unique type id SOAP_TYPE_SOAP_ENV__Header */
#ifdef WITH_NOEMPTYSTRUCT
private:
	char dummy;	/* dummy member to enable compilation */
#endif
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (55)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
public:
	int soap_type() const { return 55; } /* = unique type id SOAP_TYPE_SOAP_ENV__Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (57)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
public:
	int soap_type() const { return 57; } /* = unique type id SOAP_TYPE_SOAP_ENV__Detail */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (60)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 60; } /* = unique type id SOAP_TYPE_SOAP_ENV__Reason */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (61)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
public:
	int soap_type() const { return 61; } /* = unique type id SOAP_TYPE_SOAP_ENV__Fault */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Server-Side Operations                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 ns__serviceInit(struct soap*, std::string workspace, std::string tpldir, std::string recgmethod, std::string option, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__train(struct soap*, int autostart, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__trainAdd(struct soap*, std::string imgPath, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__trainDelete(struct soap*, std::string imgPath, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__serviceStart(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__imSearch(struct soap*, std::string imgPath, struct ns__searchResponse *ret);

SOAP_FMAC5 int SOAP_FMAC6 ns__serviceStop(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__serviceRestart(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__state(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__progressPCT(struct soap*, float *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__clear(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__pid(struct soap*, int *tag);

SOAP_FMAC5 int SOAP_FMAC6 ns__quit(struct soap*, int *tag);

/******************************************************************************\
 *                                                                            *
 * Server-Side Skeletons to Invoke Service Operations                         *
 *                                                                            *
\******************************************************************************/

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceInit(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__train(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__trainAdd(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__trainDelete(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceStart(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__imSearch(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceStop(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceRestart(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__state(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__progressPCT(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__clear(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__pid(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__quit(struct soap*);

#endif

/* End of soapStub.h */