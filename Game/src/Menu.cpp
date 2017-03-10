#include "Main.h"

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_INITDIALOG:
		{
			char* m_playerName;
			char* m_playerModel;
			char* m_map;
			char* m_hostIP;

			// Load the settings
			Script *settings = new Script( "MenuSettings.txt" );
			if( settings->GetStringData( "player_name" ) == NULL )
			{
				m_playerName = new char[strlen("Unknown Player")+1];
				m_playerModel = new char[strlen("Marine")+1];
				m_map = new char[strlen("Abandoned City")+1];
				m_hostIP = new char[strlen("localhost")+1];

				strcpy( m_playerName, "Unknown Player" );
				strcpy( m_playerModel, "Marine" );
				strcpy( m_map, "Abandoned City" );
				strcpy( m_hostIP, "localhost" );
			}
			else
			{
				m_playerName = new char[strlen(settings->GetStringData( "player_name" ))+1];
				m_playerModel = new char[strlen(settings->GetStringData( "player_model" ))+1];
				m_map = new char[strlen(settings->GetStringData( "map" ))+1];
				m_hostIP = new char[strlen(settings->GetStringData( "host_address" ))+1];

				strcpy( m_playerName, settings->GetStringData( "player_name" ) );
				strcpy( m_playerModel, settings->GetStringData( "player_model" ) );
				strcpy( m_map, settings->GetStringData( "map" ) );
				strcpy( m_hostIP, settings->GetStringData( "host_address" ) );
			}
			SAFE_DELETE( settings );

			SetDlgItemTextA(hwnd, IDC_EDIT1, m_playerName);
			SendDlgItemMessageA(hwnd, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)m_playerModel);
			SendDlgItemMessageA(hwnd, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)m_map);
			SetDlgItemTextA(hwnd, IDC_EDIT2, m_hostIP);

			SAFE_DELETE_ARRAY( m_playerName );
			SAFE_DELETE_ARRAY( m_playerModel );
			SAFE_DELETE_ARRAY( m_map );
			SAFE_DELETE_ARRAY( m_hostIP );

			return TRUE;
		}

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_RADIO1:
                {
                    switch (HIWORD(wParam))
                    {
                        case BN_CLICKED:
                            if (SendDlgItemMessageA(hwnd, IDC_RADIO1, BM_GETCHECK, 0, 0) == 1) 
                            {
                                SendDlgItemMessageA(hwnd, IDC_RADIO1, BM_SETCHECK, 1, 0);
                                SendDlgItemMessageA(hwnd, IDC_RADIO2, BM_SETCHECK, 0, 0);
                            }
                        break; 
                    }
					break;
                }

                case IDC_RADIO2:
                {
                    switch (HIWORD(wParam))
                    {
                        case BN_CLICKED:
                            if (SendDlgItemMessageA(hwnd, IDC_RADIO2, BM_GETCHECK, 0, 0) == 1) 
                            {
                                SendDlgItemMessageA(hwnd, IDC_RADIO2, BM_SETCHECK, 1, 0);
                                SendDlgItemMessageA(hwnd, IDC_RADIO1, BM_SETCHECK, 0, 0);
                            }
                        break; 
                    }
					break;
                }
                

				case IDOK:
				{
					if( SendDlgItemMessageA(hwnd,IDC_RADIO1, BM_GETCHECK, 0, 0) == false && SendDlgItemMessageA(hwnd,IDC_RADIO2, BM_GETCHECK, 0, 0)==false )
						return FALSE;

					int len;
					PlayerInfo *info = new PlayerInfo;
					info->id = 1;
					len = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT1));
					GetDlgItemTextA(hwnd, IDC_EDIT1, info->name, len + 1);
					SendDlgItemMessageA(hwnd,IDC_LIST1, LB_GETTEXT, 0, (LPARAM)info->model);
					SendDlgItemMessageA(hwnd,IDC_LIST2, LB_GETTEXT, 0, (LPARAM)info->map);
				
					len = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT2));
					if(len > 0)
					{
						char* buf;

						buf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemTextA(hwnd, IDC_EDIT2, buf, len + 1);

						if( g_engine->GetNetwork()->Init( info, buf, SendDlgItemMessageA(hwnd,IDC_RADIO1, BM_GETCHECK, 0, 0) ) == false )
							return FALSE;

						GlobalFree((HANDLE)buf);
					}

					// Save the settings
					char* namebuf;
					char* ipbuf;
					char* modelbuf;
					char* mapbuf;
					len = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT1));
					if(len > 0)
					{
						namebuf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemTextA(hwnd, IDC_EDIT1, namebuf, len + 1);
					}
					len = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT2));
					if(len > 0)
					{
						ipbuf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemTextA(hwnd, IDC_EDIT2, ipbuf, len + 1);
					}
					len = SendDlgItemMessageA(hwnd,IDC_LIST1, LB_GETTEXTLEN, 0, 0);
					if(len > 0)
					{
						modelbuf = (char*)GlobalAlloc(GPTR, len + 1);
						SendDlgItemMessageA(hwnd,IDC_LIST1, LB_GETTEXT, 0, (LPARAM)modelbuf);
					}
					len = SendDlgItemMessageA(hwnd,IDC_LIST2, LB_GETTEXTLEN, 0, 0);
					if(len > 0)
					{
						mapbuf = (char*)GlobalAlloc(GPTR, len + 1);
						SendDlgItemMessageA(hwnd,IDC_LIST2, LB_GETTEXT, 0, (LPARAM)mapbuf);
					}

					Script *settings = new Script( "MenuSettings.txt" );
					if( settings->GetStringData( "player_name" ) == NULL )
					{
						settings->AddVariable( "player_name", VARIABLE_STRING, namebuf );
						settings->AddVariable( "player_model", VARIABLE_STRING, modelbuf );
						settings->AddVariable( "map", VARIABLE_STRING, mapbuf );
						settings->AddVariable( "host_address", VARIABLE_STRING, ipbuf );
					}
					else
					{
						settings->SetVariable( "player_name", namebuf );
						settings->SetVariable( "player_model", modelbuf );
						settings->SetVariable( "map", mapbuf );
						settings->SetVariable( "host_address", ipbuf );
					}
					settings->SaveScript();
					SAFE_DELETE( settings );

					GlobalFree((HANDLE)namebuf);
					GlobalFree((HANDLE)ipbuf);
					GlobalFree((HANDLE)modelbuf);
					GlobalFree((HANDLE)mapbuf);

					EndDialog(hwnd, IDOK);
					break;
				}

				case IDCANCEL:
				{
					// Save the settings
					char* namebuf;
					char* ipbuf;
					char* modelbuf;
					char* mapbuf;
					int len = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT1));
					if(len > 0)
					{
						namebuf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemTextA(hwnd, IDC_EDIT1, namebuf, len + 1);
					}
					len = GetWindowTextLengthA(GetDlgItem(hwnd, IDC_EDIT2));
					if(len > 0)
					{
						ipbuf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemTextA(hwnd, IDC_EDIT2, ipbuf, len + 1);
					}
					len = SendDlgItemMessageA(hwnd,IDC_LIST1, LB_GETTEXTLEN, 0, 0);
					if(len > 0)
					{
						modelbuf = (char*)GlobalAlloc(GPTR, len + 1);
						SendDlgItemMessageA(hwnd,IDC_LIST1, LB_GETTEXT, 0, (LPARAM)modelbuf);
					}
					len = SendDlgItemMessageA(hwnd,IDC_LIST2, LB_GETTEXTLEN, 0, 0);
					if(len > 0)
					{
						mapbuf = (char*)GlobalAlloc(GPTR, len + 1);
						SendDlgItemMessageA(hwnd,IDC_LIST2, LB_GETTEXT, 0, (LPARAM)mapbuf);
					}

					Script *settings = new Script( "MenuSettings.txt" );
					if( settings->GetStringData( "player_name" ) == NULL )
					{
						settings->AddVariable( "player_name", VARIABLE_STRING, namebuf );
						settings->AddVariable( "player_model", VARIABLE_STRING, modelbuf );
						settings->AddVariable( "map", VARIABLE_STRING, mapbuf );
						settings->AddVariable( "host_address", VARIABLE_STRING, ipbuf );
					}
					else
					{
						settings->SetVariable( "player_name", namebuf );
						settings->SetVariable( "player_model", modelbuf );
						settings->SetVariable( "map", mapbuf );
						settings->SetVariable( "host_address", ipbuf );
					}
					settings->SaveScript();
					SAFE_DELETE( settings );

					GlobalFree((HANDLE)namebuf);
					GlobalFree((HANDLE)ipbuf);
					GlobalFree((HANDLE)modelbuf);
					GlobalFree((HANDLE)mapbuf);

					g_engine->Shutdown();

					EndDialog(hwnd, IDCANCEL);
					break;
				}
			}
			break;
		}
		
		default:
			return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// Menu class constructor.
//-----------------------------------------------------------------------------
Menu::Menu() : State( STATE_MENU )
{
	// Does nothing but set's the state's ID.
}

//-----------------------------------------------------------------------------
// Update the menu state.
//-----------------------------------------------------------------------------
void Menu::Update( float elapsed )
{
	ShowWindow(g_engine->m_window, SW_HIDE);


		int ret = DialogBoxA(GetModuleHandleA(NULL), 
                MAKEINTRESOURCEA(IDD_DIALOG1), g_engine->m_window, AboutDlgProc);

		/*
		PlayerInfo *info = new PlayerInfo;
		info->id = 1;
		strcpy( info->name, "Unknown" );
		strcpy( info->model, "Marine" );
		strcpy( info->map, "Abandoned City" );

		int value = g_engine->GetNetwork()->Init( info, _T("localhost"), true );
		*/

		if( g_engine->GetFullScreen() == true )
			ShowWindow(g_engine->m_window, SW_SHOWMAXIMIZED);
		else
			ShowWindow(g_engine->m_window, SW_SHOW);

		if( ret == IDOK )
		{
			g_engine->GetRenderer()->DeInit();
			g_engine->GetRenderer()->Init();

			g_engine->ChangeState( STATE_GAME );
		}

}