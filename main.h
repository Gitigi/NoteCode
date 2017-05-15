#include "wx/app.h"
#include <wx/snglinst.h>

class stServer;

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
	~MyApp()
	{
		delete m_server;
		delete m_singleInstanceChecker;
	}
private:
	stServer *m_server;
	wxSingleInstanceChecker *m_singleInstanceChecker;
};

DECLARE_APP(MyApp)