/* soapServer.cpp
   Generated by gSOAP 2.8.17r from search.h

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under one of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.8.17r 2016-09-20 02:59:00 GMT")


extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (soap->max_keep_alive > 0 && !--k)
			soap->keep_alive = 0;
#endif
		if (soap_begin_serve(soap))
		{	if (soap->error >= SOAP_STOP)
				continue;
			return soap->error;
		}
		if (soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:serviceInit"))
		return soap_serve_ns__serviceInit(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:train"))
		return soap_serve_ns__train(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:trainAdd"))
		return soap_serve_ns__trainAdd(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:trainDelete"))
		return soap_serve_ns__trainDelete(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:serviceStart"))
		return soap_serve_ns__serviceStart(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:imSearch"))
		return soap_serve_ns__imSearch(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:serviceStop"))
		return soap_serve_ns__serviceStop(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:serviceRestart"))
		return soap_serve_ns__serviceRestart(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:state"))
		return soap_serve_ns__state(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:progressPCT"))
		return soap_serve_ns__progressPCT(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:clear"))
		return soap_serve_ns__clear(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:pid"))
		return soap_serve_ns__pid(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:quit"))
		return soap_serve_ns__quit(soap);
	if (!soap_match_tag(soap, soap->tag, "ns2:setParams"))
		return soap_serve_ns2__setParams(soap);
	if (!soap_match_tag(soap, soap->tag, "ns2:textDetect"))
		return soap_serve_ns2__textDetect(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceInit(struct soap *soap)
{	struct ns__serviceInit soap_tmp_ns__serviceInit;
	struct ns__serviceInitResponse soap_tmp_ns__serviceInitResponse;
	int soap_tmp_int;
	soap_default_ns__serviceInitResponse(soap, &soap_tmp_ns__serviceInitResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__serviceInitResponse.tag = &soap_tmp_int;
	soap_default_ns__serviceInit(soap, &soap_tmp_ns__serviceInit);
	if (!soap_get_ns__serviceInit(soap, &soap_tmp_ns__serviceInit, "ns:serviceInit", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__serviceInit(soap, soap_tmp_ns__serviceInit.workspace, soap_tmp_ns__serviceInit.tpldir, soap_tmp_ns__serviceInit.recgmethod, soap_tmp_ns__serviceInit.option, soap_tmp_ns__serviceInitResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__serviceInitResponse(soap, &soap_tmp_ns__serviceInitResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__serviceInitResponse(soap, &soap_tmp_ns__serviceInitResponse, "ns:serviceInitResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__serviceInitResponse(soap, &soap_tmp_ns__serviceInitResponse, "ns:serviceInitResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__train(struct soap *soap)
{	struct ns__train soap_tmp_ns__train;
	struct ns__trainResponse soap_tmp_ns__trainResponse;
	int soap_tmp_int;
	soap_default_ns__trainResponse(soap, &soap_tmp_ns__trainResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__trainResponse.tag = &soap_tmp_int;
	soap_default_ns__train(soap, &soap_tmp_ns__train);
	if (!soap_get_ns__train(soap, &soap_tmp_ns__train, "ns:train", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__train(soap, soap_tmp_ns__train.autostart, soap_tmp_ns__trainResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__trainResponse(soap, &soap_tmp_ns__trainResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__trainResponse(soap, &soap_tmp_ns__trainResponse, "ns:trainResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__trainResponse(soap, &soap_tmp_ns__trainResponse, "ns:trainResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__trainAdd(struct soap *soap)
{	struct ns__trainAdd soap_tmp_ns__trainAdd;
	struct ns__trainAddResponse soap_tmp_ns__trainAddResponse;
	int soap_tmp_int;
	soap_default_ns__trainAddResponse(soap, &soap_tmp_ns__trainAddResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__trainAddResponse.tag = &soap_tmp_int;
	soap_default_ns__trainAdd(soap, &soap_tmp_ns__trainAdd);
	if (!soap_get_ns__trainAdd(soap, &soap_tmp_ns__trainAdd, "ns:trainAdd", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__trainAdd(soap, soap_tmp_ns__trainAdd.imgPath, soap_tmp_ns__trainAddResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__trainAddResponse(soap, &soap_tmp_ns__trainAddResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__trainAddResponse(soap, &soap_tmp_ns__trainAddResponse, "ns:trainAddResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__trainAddResponse(soap, &soap_tmp_ns__trainAddResponse, "ns:trainAddResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__trainDelete(struct soap *soap)
{	struct ns__trainDelete soap_tmp_ns__trainDelete;
	struct ns__trainDeleteResponse soap_tmp_ns__trainDeleteResponse;
	int soap_tmp_int;
	soap_default_ns__trainDeleteResponse(soap, &soap_tmp_ns__trainDeleteResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__trainDeleteResponse.tag = &soap_tmp_int;
	soap_default_ns__trainDelete(soap, &soap_tmp_ns__trainDelete);
	if (!soap_get_ns__trainDelete(soap, &soap_tmp_ns__trainDelete, "ns:trainDelete", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__trainDelete(soap, soap_tmp_ns__trainDelete.imgPath, soap_tmp_ns__trainDeleteResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__trainDeleteResponse(soap, &soap_tmp_ns__trainDeleteResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__trainDeleteResponse(soap, &soap_tmp_ns__trainDeleteResponse, "ns:trainDeleteResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__trainDeleteResponse(soap, &soap_tmp_ns__trainDeleteResponse, "ns:trainDeleteResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceStart(struct soap *soap)
{	struct ns__serviceStart soap_tmp_ns__serviceStart;
	struct ns__serviceStartResponse soap_tmp_ns__serviceStartResponse;
	int soap_tmp_int;
	soap_default_ns__serviceStartResponse(soap, &soap_tmp_ns__serviceStartResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__serviceStartResponse.tag = &soap_tmp_int;
	soap_default_ns__serviceStart(soap, &soap_tmp_ns__serviceStart);
	if (!soap_get_ns__serviceStart(soap, &soap_tmp_ns__serviceStart, "ns:serviceStart", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__serviceStart(soap, soap_tmp_ns__serviceStartResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__serviceStartResponse(soap, &soap_tmp_ns__serviceStartResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__serviceStartResponse(soap, &soap_tmp_ns__serviceStartResponse, "ns:serviceStartResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__serviceStartResponse(soap, &soap_tmp_ns__serviceStartResponse, "ns:serviceStartResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__imSearch(struct soap *soap)
{	struct ns__imSearch soap_tmp_ns__imSearch;
	struct ns__searchResponse ret;
	soap_default_ns__searchResponse(soap, &ret);
	soap_default_ns__imSearch(soap, &soap_tmp_ns__imSearch);
	if (!soap_get_ns__imSearch(soap, &soap_tmp_ns__imSearch, "ns:imSearch", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__imSearch(soap, soap_tmp_ns__imSearch.imgPath, &ret);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__searchResponse(soap, &ret);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__searchResponse(soap, &ret, "ns:searchResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__searchResponse(soap, &ret, "ns:searchResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceStop(struct soap *soap)
{	struct ns__serviceStop soap_tmp_ns__serviceStop;
	struct ns__serviceStopResponse soap_tmp_ns__serviceStopResponse;
	int soap_tmp_int;
	soap_default_ns__serviceStopResponse(soap, &soap_tmp_ns__serviceStopResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__serviceStopResponse.tag = &soap_tmp_int;
	soap_default_ns__serviceStop(soap, &soap_tmp_ns__serviceStop);
	if (!soap_get_ns__serviceStop(soap, &soap_tmp_ns__serviceStop, "ns:serviceStop", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__serviceStop(soap, soap_tmp_ns__serviceStopResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__serviceStopResponse(soap, &soap_tmp_ns__serviceStopResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__serviceStopResponse(soap, &soap_tmp_ns__serviceStopResponse, "ns:serviceStopResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__serviceStopResponse(soap, &soap_tmp_ns__serviceStopResponse, "ns:serviceStopResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__serviceRestart(struct soap *soap)
{	struct ns__serviceRestart soap_tmp_ns__serviceRestart;
	struct ns__serviceRestartResponse soap_tmp_ns__serviceRestartResponse;
	int soap_tmp_int;
	soap_default_ns__serviceRestartResponse(soap, &soap_tmp_ns__serviceRestartResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__serviceRestartResponse.tag = &soap_tmp_int;
	soap_default_ns__serviceRestart(soap, &soap_tmp_ns__serviceRestart);
	if (!soap_get_ns__serviceRestart(soap, &soap_tmp_ns__serviceRestart, "ns:serviceRestart", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__serviceRestart(soap, soap_tmp_ns__serviceRestartResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__serviceRestartResponse(soap, &soap_tmp_ns__serviceRestartResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__serviceRestartResponse(soap, &soap_tmp_ns__serviceRestartResponse, "ns:serviceRestartResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__serviceRestartResponse(soap, &soap_tmp_ns__serviceRestartResponse, "ns:serviceRestartResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__state(struct soap *soap)
{	struct ns__state soap_tmp_ns__state;
	struct ns__stateResponse soap_tmp_ns__stateResponse;
	int soap_tmp_int;
	soap_default_ns__stateResponse(soap, &soap_tmp_ns__stateResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__stateResponse.tag = &soap_tmp_int;
	soap_default_ns__state(soap, &soap_tmp_ns__state);
	if (!soap_get_ns__state(soap, &soap_tmp_ns__state, "ns:state", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__state(soap, soap_tmp_ns__stateResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__stateResponse(soap, &soap_tmp_ns__stateResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__stateResponse(soap, &soap_tmp_ns__stateResponse, "ns:stateResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__stateResponse(soap, &soap_tmp_ns__stateResponse, "ns:stateResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__progressPCT(struct soap *soap)
{	struct ns__progressPCT soap_tmp_ns__progressPCT;
	struct ns__progressPCTResponse soap_tmp_ns__progressPCTResponse;
	float soap_tmp_float;
	soap_default_ns__progressPCTResponse(soap, &soap_tmp_ns__progressPCTResponse);
	soap_default_float(soap, &soap_tmp_float);
	soap_tmp_ns__progressPCTResponse.tag = &soap_tmp_float;
	soap_default_ns__progressPCT(soap, &soap_tmp_ns__progressPCT);
	if (!soap_get_ns__progressPCT(soap, &soap_tmp_ns__progressPCT, "ns:progressPCT", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__progressPCT(soap, soap_tmp_ns__progressPCTResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__progressPCTResponse(soap, &soap_tmp_ns__progressPCTResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__progressPCTResponse(soap, &soap_tmp_ns__progressPCTResponse, "ns:progressPCTResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__progressPCTResponse(soap, &soap_tmp_ns__progressPCTResponse, "ns:progressPCTResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__clear(struct soap *soap)
{	struct ns__clear soap_tmp_ns__clear;
	struct ns__clearResponse soap_tmp_ns__clearResponse;
	int soap_tmp_int;
	soap_default_ns__clearResponse(soap, &soap_tmp_ns__clearResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__clearResponse.tag = &soap_tmp_int;
	soap_default_ns__clear(soap, &soap_tmp_ns__clear);
	if (!soap_get_ns__clear(soap, &soap_tmp_ns__clear, "ns:clear", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__clear(soap, soap_tmp_ns__clearResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__clearResponse(soap, &soap_tmp_ns__clearResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__clearResponse(soap, &soap_tmp_ns__clearResponse, "ns:clearResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__clearResponse(soap, &soap_tmp_ns__clearResponse, "ns:clearResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__pid(struct soap *soap)
{	struct ns__pid soap_tmp_ns__pid;
	struct ns__pidResponse soap_tmp_ns__pidResponse;
	int soap_tmp_int;
	soap_default_ns__pidResponse(soap, &soap_tmp_ns__pidResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__pidResponse.tag = &soap_tmp_int;
	soap_default_ns__pid(soap, &soap_tmp_ns__pid);
	if (!soap_get_ns__pid(soap, &soap_tmp_ns__pid, "ns:pid", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__pid(soap, soap_tmp_ns__pidResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__pidResponse(soap, &soap_tmp_ns__pidResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__pidResponse(soap, &soap_tmp_ns__pidResponse, "ns:pidResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__pidResponse(soap, &soap_tmp_ns__pidResponse, "ns:pidResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__quit(struct soap *soap)
{	struct ns__quit soap_tmp_ns__quit;
	struct ns__quitResponse soap_tmp_ns__quitResponse;
	int soap_tmp_int;
	soap_default_ns__quitResponse(soap, &soap_tmp_ns__quitResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns__quitResponse.tag = &soap_tmp_int;
	soap_default_ns__quit(soap, &soap_tmp_ns__quit);
	if (!soap_get_ns__quit(soap, &soap_tmp_ns__quit, "ns:quit", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__quit(soap, soap_tmp_ns__quitResponse.tag);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns__quitResponse(soap, &soap_tmp_ns__quitResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__quitResponse(soap, &soap_tmp_ns__quitResponse, "ns:quitResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__quitResponse(soap, &soap_tmp_ns__quitResponse, "ns:quitResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns2__setParams(struct soap *soap)
{	struct ns2__setParams soap_tmp_ns2__setParams;
	struct ns2__setParamsResponse soap_tmp_ns2__setParamsResponse;
	int soap_tmp_int;
	soap_default_ns2__setParamsResponse(soap, &soap_tmp_ns2__setParamsResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_ns2__setParamsResponse.output = &soap_tmp_int;
	soap_default_ns2__setParams(soap, &soap_tmp_ns2__setParams);
	if (!soap_get_ns2__setParams(soap, &soap_tmp_ns2__setParams, "ns2:setParams", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns2__setParams(soap, soap_tmp_ns2__setParams.input, soap_tmp_ns2__setParamsResponse.output);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns2__setParamsResponse(soap, &soap_tmp_ns2__setParamsResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__setParamsResponse(soap, &soap_tmp_ns2__setParamsResponse, "ns2:setParamsResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns2__setParamsResponse(soap, &soap_tmp_ns2__setParamsResponse, "ns2:setParamsResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns2__textDetect(struct soap *soap)
{	struct ns2__textDetect soap_tmp_ns2__textDetect;
	struct ns2__textDetectResponse soap_tmp_ns2__textDetectResponse;
	result soap_tmp_result;
	soap_default_ns2__textDetectResponse(soap, &soap_tmp_ns2__textDetectResponse);
	soap_tmp_result.soap_default(soap);
	soap_tmp_ns2__textDetectResponse.output = &soap_tmp_result;
	soap_default_ns2__textDetect(soap, &soap_tmp_ns2__textDetect);
	if (!soap_get_ns2__textDetect(soap, &soap_tmp_ns2__textDetect, "ns2:textDetect", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns2__textDetect(soap, soap_tmp_ns2__textDetect.input, soap_tmp_ns2__textDetectResponse.output);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = "";
	soap_serializeheader(soap);
	soap_serialize_ns2__textDetectResponse(soap, &soap_tmp_ns2__textDetectResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns2__textDetectResponse(soap, &soap_tmp_ns2__textDetectResponse, "ns2:textDetectResponse", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns2__textDetectResponse(soap, &soap_tmp_ns2__textDetectResponse, "ns2:textDetectResponse", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapServer.cpp */
