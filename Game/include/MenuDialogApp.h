#include "wx/wxprec.h"
#include "MenuDialog.h"

class MenuDialogApp : public wxApp
{
public:
	bool OnInit();
};

bool MenuDialogApp :: OnInit()
{
	MenuDialog *m_menuDialog = new MenuDialog(NULL);
	
	m_menuDialog->ShowModal();
	
	m_menuDialog->Destroy();

	return true;
}