#include "connection.hpp"
#include "frame.h"
#include "main.h"
#include <iostream>
#include <wx/dialog.h>
#include <wx/toplevel.h>

wxConnectionBase *stServer::OnAcceptConnection(const wxString &topic)
{
	const wxString AppName = wxString::Format(wxT("NoteCode-%s"),wxGetUserId().c_str());
	if(topic == AppName)
	{
		wxWindowList::Node *node = wxTopLevelWindows.GetFirst();
		while(node)
		{
			wxDialog *dialog = wxDynamicCast(node->GetData(),wxDialog);
			if(dialog && dialog->IsModal())
			{
				return NULL;
			}
			
			node = node->GetNext();
		}
		return new stConnection();
	}
	else
		return NULL;
}

wxConnectionBase *stClient::OnMakeConnection()
{
	return new stConnection;
}

bool stConnection::OnExecute(const wxString &WXUNUSED(topic),
	const void *data,
	size_t WXUNUSED(size),
	wxIPCFormat WXUNUSED(format))
{
	MyFrame *frame = wxDynamicCast(wxGetApp().GetTopWindow(),MyFrame);
	wxString filename((char *)data);
	if(filename.IsEmpty())
	{
		if(frame)
			frame->Raise();
	}
	else{
		wxFileName file = wxFileName(filename);
		const wxString name = file.GetFullName();
		const wxString directory = file.GetPath();
		frame->CreatePage(name,directory);
		frame->Raise();
	}
}