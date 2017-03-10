#include "QUIX.h"

Network::Network( void (*HandleNetworkMessageFunction)( ReceivedMessage *msg ) )
{	
	enet_initialize ();
		
	// Initialise the critical sections.
	//InitializeCriticalSection( &m_playerCS );
	//InitializeCriticalSection( &m_messageCS );

	// Create the player list.
	m_players = new LinkedList< PlayerInfo >;

	// Create the network message list.
	m_messages = new LinkedList< ReceivedMessage >;

	// Initially the network is not allowed to receive application specific messages.
	m_receiveAllowed = false;

	// Set the network message handler.
	HandleNetworkMessage = HandleNetworkMessageFunction;

	generatedId = 1;

	m_init = false;
}

Network::~Network()
{
	Terminate();

	enet_deinitialize ();

	// Destroy the player list.
	SAFE_DELETE( m_players );

	// Destroy the network message list.
	SAFE_DELETE( m_messages );

	// Delete the critical sections.
	//DeleteCriticalSection( &m_playerCS );
	//DeleteCriticalSection( &m_messageCS );
}

bool Network::Init( PlayerInfo *info, char* hostAddress, bool ishost )
{
	//_CrtDumpMemoryLeaks();

	m_receiveAllowed = false;
	m_ishost = false;

	server = NULL;
	myself = NULL;

	m_players->Empty();
	m_messages->Empty();
	generatedId = 1;

	//myinfo = NULL;
	if( info == NULL )
		myinfo = new PlayerInfo;
	else
		myinfo = info;
	
	m_id = myinfo->id;

	m_address = new char[strlen(hostAddress) + 1];
	strcpy( m_address, hostAddress);

	// Load the network settings.
	Script *settings = new Script( "NetworkSettings.txt" );
	if( settings->GetNumberData( "processing_time" ) == NULL )
	{
		m_port = 2509;
		m_connectionTimeOut = 5000;
		m_processingTime = 100;
		m_maxclients = 64;
		m_client_incoming_bandwidth = 57600;
		m_client_outgoing_bandwidth = 14400;
	}
	else
	{
		m_port = *settings->GetNumberData( "port" );
		m_connectionTimeOut = *settings->GetNumberData( "connection_timeout" );
		m_processingTime = *settings->GetNumberData( "processing_time" );
		m_maxclients = *settings->GetNumberData( "max_clients" );
		m_client_incoming_bandwidth = *settings->GetNumberData( "client_incoming_bandwidth" );
		m_client_outgoing_bandwidth = *settings->GetNumberData( "client_outgoing_bandwidth" );
		/*
		char temp_add[MAX_PATH];
		wcstombs( temp_add, settings->GetStringData( _T("address") ), MAX_PATH );
		m_address = new char[strlen( temp_add ) + 1];
		memcpy( m_address, temp_add, ( strlen( temp_add ) + 1 ) * sizeof( char ) );
		*/
	}
	SAFE_DELETE( settings );


	if( ishost == true )
	{
		address.host = ENET_HOST_ANY;
		address.port = m_port;

		myself = enet_host_create (&address,
                             m_maxclients,   /* number of clients */
                             2,    /* number of channels */
                             0,    /* Any incoming bandwith */
                             0);   /* Any outgoing bandwith */
		
		if (myself == NULL)
		{
			SAFE_DELETE_ARRAY( m_address );
			return false;
		}

		m_ishost = true;
		m_id = 0;
		myinfo->id = 0;

		m_receiveAllowed = true;

		//add host's info to its list of players
		m_players->Add( myinfo );
		
		// the host should handle itself (new player) in game-specific message handler
		if( HandleNetworkMessage != NULL )
		{
			PlayerInfoMessage *newmsg = new PlayerInfoMessage;
			newmsg->msgid = MSGID_CREATE_PLAYER;
			newmsg->id = m_id;
			newmsg->data = *myinfo;

			ReceivedMessage *rmsg = (ReceivedMessage*) newmsg;

			//EnterCriticalSection( &network->m_messageCS );
			m_messages->Add( rmsg );
			//LeaveCriticalSection( &network->m_messageCS );
		}

		m_init = true;
		return true;
	}	
	else
	{
		myself = enet_host_create (NULL /* create a client host */,
				    1 /* only allow 1 outgoing connection */,
				   2 /* allow up 2 channels to be used, 0 and 1 */,
				  m_client_incoming_bandwidth / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
				  m_client_outgoing_bandwidth / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

		if (myself == NULL)
		{
			SAFE_DELETE (myinfo);
			SAFE_DELETE_ARRAY( m_address );
			return false;
		}

		//enet_address_set_host (&address, m_address);
		enet_address_set_host (&address, m_address);
		address.port = m_port;

		server = enet_host_connect (myself,
                            &address,    /* address to connect to */
                             2,           /* number of channels */
                             0);          /* user data supplied to the receiving host */

		if (server == NULL)
		{
			enet_host_destroy(myself);
			myself = NULL;
			SAFE_DELETE (myinfo);
			SAFE_DELETE_ARRAY( m_address );
			return false;
		}

		if (enet_host_service (myself, &eve, m_connectionTimeOut) > 0 && eve.type == ENET_EVENT_TYPE_CONNECT)
		{
			m_init = true;
			return true;
		}
		else
		{
			enet_peer_reset (server);
			server = NULL;
			enet_host_destroy(myself);
			myself = NULL;
			SAFE_DELETE (myinfo);
			SAFE_DELETE_ARRAY( m_address );
			return false;
		}
	}

	//_CrtDumpMemoryLeaks();
}

void Network::Update()
{
	//_CrtDumpMemoryLeaks();

	NetworkMessageHandler();

	//EnterCriticalSection( &m_messageCS );

	ReceivedMessage *message = m_messages->GetFirst();

	unsigned long endTime = timeGetTime() + m_processingTime;
	while( endTime > timeGetTime() && message != NULL )
	{
		HandleNetworkMessage( message );
		m_messages->Remove( &message );
		message = m_messages->GetFirst();
	}

	//LeaveCriticalSection( &m_messageCS );

	//_CrtDumpMemoryLeaks();
}

void Network::Terminate()
{
	//_CrtDumpMemoryLeaks();

	if( m_init == false )
		return;

	m_init = false;

	if( m_ishost )
	{
		NetworkMessage newmsg;
		newmsg.msgid = MSGID_TERMINATE_SESSION;
		newmsg.id = m_id;

		ENetPacket *packet = enet_packet_create (&newmsg, sizeof(NetworkMessage), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast (myself, 0, packet);
		
		while (enet_host_service (myself, &eve, m_connectionTimeOut) > 0)
		{
			switch (eve.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy (eve.packet);
					break;
			}
		}
	
		if( myself )
		{
			enet_host_destroy (myself);
			myself = NULL;
		}
	}
	else
	{
		if( server )
		{
			enet_peer_disconnect (server, 0);

			while (enet_host_service (myself, &eve, m_connectionTimeOut) > 0)
			{
				switch (eve.type)
				{
					case ENET_EVENT_TYPE_RECEIVE:
						enet_packet_destroy (eve.packet);
						break;
				}
			}
			/* We've arrived here, so the disconnect attempt didn't */
			/* succeed yet.  Force the connection down.             */
			enet_peer_reset (server);
			server = NULL;
		}

		if( myself )
		{
			enet_host_destroy (myself);
			myself = NULL;
		}
	}

	// Save the network settings.
	Script *settings = new Script( "NetworkSettings.txt" );
	//TCHAR temp_add[MAX_PATH];
	//mbstowcs( temp_add, m_address, MAX_PATH*sizeof(TCHAR) );
	if( settings->GetNumberData( "processing_time" ) == NULL )
	{
		settings->AddVariable( "port", VARIABLE_NUMBER, &m_port );
		settings->AddVariable( "connection_timeout", VARIABLE_NUMBER, &m_connectionTimeOut );
		settings->AddVariable( "processing_time", VARIABLE_NUMBER, &m_processingTime );
		settings->AddVariable( "max_clients", VARIABLE_NUMBER, &m_maxclients );
		settings->AddVariable( "client_incoming_bandwidth", VARIABLE_NUMBER, &m_client_incoming_bandwidth );
		settings->AddVariable( "client_outgoing_bandwidth", VARIABLE_NUMBER, &m_client_outgoing_bandwidth );
		//settings->AddVariable( _T("address"), VARIABLE_STRING, temp_add );
	}
	else
	{
		settings->SetVariable( "port", &m_port );
		settings->SetVariable( "connection_timeout", &m_connectionTimeOut );
		settings->SetVariable( "processing_time", &m_processingTime );
		settings->SetVariable( "max_clients", &m_maxclients );
		settings->SetVariable( "client_incoming_bandwidth", &m_client_incoming_bandwidth );
		settings->SetVariable( "client_outgoing_bandwidth", &m_client_outgoing_bandwidth );
		//settings->SetVariable( _T("address"), temp_add );
	}

SAFE_DELETE_ARRAY( m_address );

	settings->SaveScript();
	SAFE_DELETE( settings );

	//_CrtDumpMemoryLeaks();
}

void Network::SetReceiveAllowed( bool allowed )
{
	m_receiveAllowed = allowed;
}

void Network::Send( void *data, unsigned long size, bool reliable ) //check what parameters are needed
{
	ENetPacket *packet = NULL;

	if (reliable)
		packet = enet_packet_create (data, size, ENET_PACKET_FLAG_RELIABLE);
	else
		packet = enet_packet_create (data, size, 0);
							
	if(m_ishost)
	{
		//server should process its own messages too in game-specific message handler
		if( HandleNetworkMessage != NULL )
		{
			ReceivedMessage* msg = new ReceivedMessage;
			memcpy( msg, packet->data, packet->dataLength );
					
			m_messages->Add( msg );
		}

		enet_host_broadcast (myself, 0, packet);
	}
	else
	{
		enet_peer_send (server, 0, packet);
	}
}

void Network::NetworkMessageHandler()
{
	if ( m_init == false )
		return;


	/*
	// server or client should check for connection timeouts from time to time
	static unsigned long endTime = timeGetTime() + m_connectionTimeOut;

	if ( endTime > timeGetTime() )
	{
		if(m_ishost)
		{
			ENetPeer * currentPeer;

			for (currentPeer = myself -> peers; currentPeer < & myself -> peers [myself -> peerCount]; ++ currentPeer)
			{
				if (currentPeer -> state != ENET_PEER_STATE_CONNECTED)
				{
					PlayerInfo *found = NULL;
					
					m_players->Iterate( true );
						while( m_players->Iterate() )
						{
							if( (unsigned int)currentPeer->data > 0 && m_players->GetCurrent()->id == (unsigned int)currentPeer->data )
							{
								NetworkMessage newmsg;
								newmsg.msgid = MSGID_DESTROY_PLAYER;
								newmsg.id = (unsigned int)currentPeer->data;

								ENetPacket *packet = enet_packet_create (&newmsg, sizeof(NetworkMessage), ENET_PACKET_FLAG_RELIABLE);
								enet_host_broadcast (myself, 0, packet);

								//m_players->Remove( (PlayerInfo**)m_players->GetCurrent() );
								found = m_players->GetCurrent();
								m_players->Remove( &found );
							
								//now add it to message queue
								if( HandleNetworkMessage != NULL )
								{							
									NetworkMessage* nmsg = new NetworkMessage;
									nmsg->msgid = MSGID_DESTROY_PLAYER;
									nmsg->id = (unsigned int)currentPeer->data;
	
									ReceivedMessage* msg = (ReceivedMessage*) nmsg;
							
									m_messages->Add( msg );

									break;
								}

								currentPeer->data = NULL;
							}
						}
				}
			}
		}

		else
		{
			if( server!= NULL && server->state != ENET_PEER_STATE_CONNECTED )
			{
				server = NULL;

				if( HandleNetworkMessage != NULL )
				{
					NetworkMessage* newmsg = new NetworkMessage;
					newmsg->msgid = MSGID_TERMINATE_SESSION;
					newmsg->id = 0;
							
					ReceivedMessage* msg = (ReceivedMessage*) newmsg;
				
					m_messages->Add( msg );
				}
			}
		}

		endTime = timeGetTime() + m_connectionTimeOut;
	}
	*/


	int result = enet_host_service (myself, & eve, 0);

	if(result > 0)
	{
			if( m_ishost )
			{
				switch (eve.type)
				{
					case ENET_EVENT_TYPE_CONNECT:
						{
							eve.peer->data = (void*) generatedId;
							NetworkMessage newmsg;
							newmsg.msgid = MSGID_ASSIGN_ID;
							newmsg.id = generatedId;
							generatedId++;

							//send msg to client
							ENetPacket *packet = enet_packet_create (&newmsg, sizeof(NetworkMessage), ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send (eve.peer, 0, packet);

							break;
						}

					case ENET_EVENT_TYPE_RECEIVE:
						{
							ReceivedMessage* msg = new ReceivedMessage;
							memcpy( msg, eve.packet->data, eve.packet->dataLength );

							switch (msg->msgid)
							{
								case MSGID_CREATE_PLAYER:
								{
									PlayerInfoMessage newmsg;
									newmsg.msgid = MSGID_CREATE_PLAYER;
									//newmsg.id = m_id;

									/*
									m_players->Iterate( true );
									while( m_players->Iterate() )
									{
										newmsg.id = m_players->GetCurrent()->id;
										newmsg.data = *m_players->GetCurrent();
										ENetPacket *packet = enet_packet_create (&newmsg, sizeof(PlayerInfoMessage), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send (eve.peer, 0, packet);
									}
									*/

									PlayerInfo *tempLast = m_players->GetLast();
									while( tempLast != NULL )
									{
										newmsg.id = tempLast->id;
										newmsg.data = *tempLast;
										ENetPacket *packet = enet_packet_create (&newmsg, sizeof(PlayerInfoMessage), ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send (eve.peer, 0, packet);

										tempLast = m_players->GetPrev(tempLast);
									}

									//void VLDDisable (void); 
									
									PlayerInfoMessage* pmsg = (PlayerInfoMessage*)msg;
									PlayerInfo* temp = new PlayerInfo;
									*temp =	pmsg->data;
									m_players->Add( temp );

									//void VLDEnable (void); 

									if( HandleNetworkMessage == NULL )
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}

								/*
								case MSGID_DESTROY_PLAYER:
								{
									if( HandleNetworkMessage == NULL || m_receiveAllowed == false)
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}
								*/

								default:
								{
									if( HandleNetworkMessage == NULL )
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}
							}

							/* Tell all clients about this message */
							enet_host_broadcast (myself, 0, eve.packet);

							//SAFE_DELETE( msg);

							break;
						}
           
					case ENET_EVENT_TYPE_DISCONNECT:
						{
							//printf ("%s disconected.\n", eve.peer -> data);
							PlayerInfo *found = NULL;
							m_players->Iterate( true );
							while( m_players->Iterate() )
							{
								if( m_players->GetCurrent()->id == (unsigned int)eve.peer->data )
								{
									NetworkMessage newmsg;
									newmsg.msgid = MSGID_DESTROY_PLAYER;
									newmsg.id = (unsigned int)eve.peer->data;

									ENetPacket *packet = enet_packet_create (&newmsg, sizeof(NetworkMessage), ENET_PACKET_FLAG_RELIABLE);
									enet_host_broadcast (myself, 0, packet);

									//m_players->Remove( (PlayerInfo**)m_players->GetCurrent() );
									found = m_players->GetCurrent();
									m_players->Remove( &found );
																		
									//now add it to message queue
									if( HandleNetworkMessage != NULL )
									{
										NetworkMessage* nmsg = new NetworkMessage;
										nmsg->msgid = MSGID_DESTROY_PLAYER;
										nmsg->id = (unsigned int)eve.peer->data;

										ReceivedMessage* msg = (ReceivedMessage*) nmsg;
									
										m_messages->Add( msg );
									}

									eve.peer->data = NULL;

									break;
								}
							}

							break;
						}
				}
			}
			else
			{
				switch (eve.type)
				{
					case ENET_EVENT_TYPE_RECEIVE:
						{
							ReceivedMessage* msg = new ReceivedMessage;
							memcpy( msg, eve.packet->data, eve.packet->dataLength );


							switch (msg->msgid)
							{
								case MSGID_ASSIGN_ID:
								{
									NetworkMessage* tmpmsg = (NetworkMessage*)msg;
									m_id = tmpmsg->id;
									myinfo->id = m_id;

									
									PlayerInfoMessage newmsg;
									newmsg.msgid = MSGID_CREATE_PLAYER;
									newmsg.id = m_id;
									newmsg.data = *myinfo;

									//prepare msg to send to server
									ENetPacket *packet = enet_packet_create (&newmsg, sizeof(PlayerInfoMessage), ENET_PACKET_FLAG_RELIABLE);
									

									//add myinfo to players list
									m_players->Add( myinfo );

									//add to it's own message queue if possible so that it can be processed in game-specific message handler
									if( HandleNetworkMessage != NULL )
									{
										ReceivedMessage *rmsg = new ReceivedMessage;
										memcpy( rmsg, packet->data, packet->dataLength );

										//EnterCriticalSection( &network->m_messageCS );
										m_messages->Add( rmsg );
										//LeaveCriticalSection( &network->m_messageCS );
									}


									//send to server
									enet_peer_send (server, 0, packet);																		


									SAFE_DELETE(msg);

									break;
								}

								case MSGID_CREATE_PLAYER:
								{
									PlayerInfoMessage* newmsg = (PlayerInfoMessage*)msg;


							if(newmsg->id == m_id)
							{
								SAFE_DELETE(msg);
								break;
							}


									PlayerInfo* temp = new PlayerInfo;
									*temp = newmsg->data;
									m_players->Add( temp );

									if( HandleNetworkMessage == NULL )
									{
										SAFE_DELETE(msg);
										break;
									}
	
									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}

								case MSGID_DESTROY_PLAYER:
								{
									NetworkMessage* newmsg = (NetworkMessage*) msg;

									PlayerInfo *found = NULL;
									m_players->Iterate( true );
									while( m_players->Iterate() )
									{
										if( m_players->GetCurrent()->id == newmsg->id )
										{
											//m_players->Remove( (PlayerInfo**)m_players->GetCurrent() );
											found = m_players->GetCurrent();
											m_players->Remove( &found );
											break;
										}
									}

									if( HandleNetworkMessage == NULL )
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}

								case MSGID_TERMINATE_SESSION:
								{
									if( server )
									{
										enet_peer_reset (server);
										server = NULL;
									}

									if( HandleNetworkMessage == NULL )
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );

									break;
								}

								default:
								{
									if( HandleNetworkMessage == NULL || m_receiveAllowed == false)
									{
										SAFE_DELETE(msg);
										break;
									}

									//EnterCriticalSection( &network->m_messageCS );
									m_messages->Add( msg );
									//LeaveCriticalSection( &network->m_messageCS );
	
									break;
								}
							}

							enet_packet_destroy (eve.packet);

							//SAFE_DELETE( msg);

							break;
						}

					case ENET_EVENT_TYPE_DISCONNECT:
						{
							if( server )
							{
								enet_peer_reset (server);
								server = NULL;
							}

							if( HandleNetworkMessage != NULL )
							{
								NetworkMessage* newmsg = new NetworkMessage;
								newmsg->msgid = MSGID_TERMINATE_SESSION;
								newmsg->id = 0;
							
								ReceivedMessage* msg = (ReceivedMessage*) newmsg;
							
								m_messages->Add( msg );
							}

							break;
						}
				}
			}
	}
}

unsigned long Network::GetNumberOfPlayers()
{
	//EnterCriticalSection( &m_playerCS );

	unsigned long number = m_players->GetTotalElements();

	//LeaveCriticalSection( &m_playerCS );

	return number;
}

PlayerInfo* Network::GetPlayer( unsigned int id )
{
	//EnterCriticalSection( &m_playerCS );

	m_players->Iterate( true );
	while( m_players->Iterate() )
	{
		if( m_players->GetCurrent()->id == id )
		{
			//LeaveCriticalSection( &m_playerCS );

			return m_players->GetCurrent();
		}
	}

	//LeaveCriticalSection( &m_playerCS );

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns the local player's DirectPlay ID.
//-----------------------------------------------------------------------------
unsigned int Network::GetID()
{
	return m_id;
}