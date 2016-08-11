#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/cmdargs.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/image.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/fs_filter.h>


#include "frame.h"

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{

    {
        wxCMD_LINE_SWITCH, "h", "help","displays help on the command line parameters"
    },
    {
        wxCMD_LINE_SWITCH, "v", "version", "print version"
    },

    {
        wxCMD_LINE_PARAM,NULL,NULL,"Input file",
        wxCMD_LINE_VAL_STRING,wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE
    },
    {
        wxCMD_LINE_NONE
    }
};

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();

    wxFileSystem::AddHandler(new wxArchiveFSHandler());
    wxFileSystem::AddHandler(new wxFilterFSHandler());
    wxFileSystem::AddHandler(new wxInternetFSHandler());
    wxFileSystem::AddHandler(new wxMemoryFSHandler());
    wxFileSystem::AddHandler(new wxZipFSHandler());

    wxString name = wxT("Unammed");
    wxString directory = wxEmptyString;

    wxString cmd_file_name;
    wxCmdLineParser cmd_parser(g_cmdLineDesc,argc,argv);
    cmd_parser.Parse(false);

    if(cmd_parser.Found(wxT("h")))
    {
        wxLog::SetActiveTarget(new wxLogStderr);
        wxLogMessage("NoteCode input_file");
        return false;
    }

    if(cmd_parser.Found(wxT("v")))
    {
        wxLog::SetActiveTarget(new wxLogStderr);
        wxLogMessage("Version 0.1");
        return false;
    }

    if(cmd_parser.GetParamCount() > 0)
    {
        cmd_file_name = cmd_parser.GetParam(0);


        wxFileName fname(cmd_file_name);
        fname.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|
                        wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);

        name = fname.GetFullName();
        directory = fname.GetPath();

    }


	#ifdef __WXMSW__
	wxString enviro_variable(wxT("NoteCode"));
    wxString enviro_variable_value;
    wxGetEnv(enviro_variable,&enviro_variable_value);
    wxSetWorkingDirectory(enviro_variable_value);
    #endif // __WXWINDOWS__
    InitStyles();

    MyFrame *frame = new MyFrame("NoteCode",name,directory);
    frame->Show(true);
    return true;
}
