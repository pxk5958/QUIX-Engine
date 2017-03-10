#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#include "vld.h"
#endif

#include "wx/wxprec.h"

#include "MenuDialog.h"

class MyApp : public wxApp
{
public:
	bool OnInit();
};

IMPLEMENT_APP( MyApp );

bool MyApp :: OnInit()
{
	MenuDialog *dialog = new MenuDialog(NULL);
	dialog->ShowModal();
	dialog->Destroy();

	return true;
}