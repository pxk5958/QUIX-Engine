#include "MenuDialog.h"

MenuDialog::MenuDialog( wxWindow* parent )
:
GUI_MenuDialog( parent )
{

}

void MenuDialog::OnHost( wxCommandEvent& event )
{
// TODO: Implement OnHost
}

void MenuDialog::OnJoin( wxCommandEvent& event )
{
// TODO: Implement OnJoin
}

void MenuDialog::OnStart( wxCommandEvent& event )
{
// TODO: Implement OnStart
}

void MenuDialog::OnExit( wxCommandEvent& event )
{
// TODO: Implement OnExit
	//EndModal(1);
	Close();
}
