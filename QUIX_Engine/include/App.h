#ifndef APP_H
#define APP_H

class App : public wxApp
{
	public:
		bool OnInit();
		int OnExit();

	private:
		WinFrame* frame;
};

DECLARE_APP(App)

#endif