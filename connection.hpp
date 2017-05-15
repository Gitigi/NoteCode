#include "wx/ipc.h"

#define NOTECODE_PORT "5048"

class stServer : public wxServer
{
public:
	wxConnectionBase *OnAcceptConnection(const wxString &topic);
};

class stClient : public wxClient
{
public:
	stClient(){};
	wxConnectionBase *OnMakeConnection();
};

class stConnection : public wxConnection
{
public:
	stConnection(){}
	~stConnection(){}
	bool OnExecute(const wxString &topic,const void *data,size_t size,wxIPCFormat format);
};