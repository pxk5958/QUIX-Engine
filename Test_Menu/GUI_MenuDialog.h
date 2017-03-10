///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __GUI_MenuDialog__
#define __GUI_MenuDialog__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class GUI_MenuDialog
///////////////////////////////////////////////////////////////////////////////
class GUI_MenuDialog : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* PlayerNameStatic;
		wxTextCtrl* PlayerNameTextBox;
		wxStaticText* PlayerModelStatic;
		wxTextCtrl* PlayerModelTextBox;
		wxRadioButton* HostRadioButton;
		wxStaticText* MapStatic;
		wxTextCtrl* MapTextBox;
		wxRadioButton* JoinRadioButton;
		wxStaticText* HostIPStatic;
		wxTextCtrl* HostIPTextBox;
		wxButton* StartButton;
		wxButton* ExitButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnHost( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnJoin( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnStart( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GUI_MenuDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Game"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 285,385 ), long style = wxCAPTION|wxSTAY_ON_TOP );
		~GUI_MenuDialog();
	
};

#endif //__GUI_MenuDialog__
