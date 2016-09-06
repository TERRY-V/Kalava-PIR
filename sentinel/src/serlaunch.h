//gsoap ns service name: serlaunch
//gsoap ns service namespace: http://localhost:29999/serlaunch.wsdl
//gsoap ns service location: http://localhost:29999
//gsoap ns service executable: serlaunch.cgi
//gsoap ns service encoding: encoded
//gsoap ns schema namespace: urn:serlaunch

int ns__launch(std::string workspace, std::string tpldir, std::string ip, std::string port, std::string version, std::string recgdir, int *tag);

