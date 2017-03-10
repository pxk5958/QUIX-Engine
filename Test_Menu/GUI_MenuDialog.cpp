///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "GUI_MenuDialog.h"

///////////////////////////////////////////////////////////////////////////

GUI_MenuDialog::GUI_MenuDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 13, 0, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	PlayerNameStatic = new wxStaticText( this, wxID_ANY, wxT("Player Name"), wxDefaultPosition, wxDefaultSize, 0 );
	PlayerNameStatic->Wrap( -1 );
	fgSizer1->Add( PlayerNameStatic, 0, wxALL, 5 );
	
	PlayerNameTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 256,-1 ), 0 );
	fgSizer1->Add( PlayerNameTextBox, 0, wxALL, 5 );
	
	PlayerModelStatic = new wxStaticText( this, wxID_ANY, wxT("Player Model"), wxDefaultPosition, wxDefaultSize, 0 );
	PlayerModelStatic->Wrap( -1 );
	fgSizer1->Add( PlayerModelStatic, 0, wxALL, 5 );
	
	PlayerModelTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( PlayerModelTextBox, 0, wxALL|wxEXPAND, 5 );
	
	HostRadioButton = new wxRadioButton( this, wxID_ANY, wxT("HOST"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	fgSizer1->Add( HostRadioButton, 0, wxALL, 5 );
	
	MapStatic = new wxStaticText( this, wxID_ANY, wxT("Map"), wxDefaultPosition, wxDefaultSize, 0 );
	MapStatic->Wrap( -1 );
	fgSizer1->Add( MapStatic, 0, wxALL, 5 );
	
	MapTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( MapTextBox, 0, wxALL|wxEXPAND, 5 );
	
	JoinRadioButton = new wxRadioButton( this, wxID_ANY, wxT("JOIN"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( JoinRadioButton, 0, wxALL, 5 );
	
	HostIPStatic = new wxStaticText( this, wxID_ANY, wxT("Host IP Address"), wxDefaultPosition, wxDefaultSize, 0 );
	HostIPStatic->Wrap( -1 );
	fgSizer1->Add( HostIPStatic, 0, wxALL, 5 );
	
	HostIPTextBox = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( HostIPTextBox, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	StartButton = new wxButton( this, wxID_ANY, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( StartButton, 0, wxALL, 5 );
	
	ExitButton = new wxButton( this, wxID_ANY, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( ExitButton, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	HostRadioButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUI_MenuDialog::OnHost ), NULL, this );
	JoinRadioButton->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUI_MenuDialog::OnJoin ), NULL, this );
	StartButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUI_MenuDialog::OnStart ), NULL, this );
	ExitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUI_MenuDialog::OnExit ), NULL, this );
}

GUI_MenuDialog::~GUI_MenuDialog()
{
	// Disconnect Events
	HostRadioButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUI_MenuDialog::OnHost ), NULL, this );
	JoinRadioButton->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GUI_MenuDialog::OnJoin ), NULL, this );
	StartButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUI_MenuDialog::OnStart ), NULL, this );
	ExitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GUI_MenuDialog::OnExit ), NULL, this );
	
}
