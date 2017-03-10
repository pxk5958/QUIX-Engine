#ifndef NETWORK_H
#define NETWORK_H


#include <enet/enet.h>


//-----------------------------------------------------------------------------
// Standard Message Defines
//-----------------------------------------------------------------------------
#define MSGID_ASSIGN_ID 0x12001
#define MSGID_CREATE_PLAYER 0x12002
#define MSGID_DESTROY_PLAYER 0x12003
#define MSGID_TERMINATE_SESSION 0x12004
#define MSGID_CONNECTION_CHECK 0x12005

struct PlayerInfo
{
	unsigned int id; // player ID.
	char name[30];
	char model[30];
	char map[30];
	char team[30];

	PlayerInfo()
	{
		id = 1;
		name[0] = 0;
		model[0] = 0;
		map[0] = 0;
		team[0] = 0;
	}
};

//check this later about virtual destructors
struct NetworkMessage
{
	unsigned long msgid; // Message ID.
	unsigned int id; // player ID.
};

struct PlayerInfoMessage : public NetworkMessage
{
	PlayerInfo data;
};

struct ReceivedMessage : public NetworkMessage
{
	char data[ sizeof(PlayerInfo) ]; // Message data.
};


class Network
{
	public:

		Network( void (*HandleNetworkMessageFunction)( ReceivedMessage *msg ) );
		~Network();

		bool Init( PlayerInfo *info, char* hostAddress, bool ishost = false );
		void Update();
		void Terminate();

		void SetReceiveAllowed( bool allowed );

		void Send( void *data, unsigned long size, bool reliable = true );

		void NetworkMessageHandler();
		


/*
// ONLY FOR TESTING. REMOVE IN FINAL VERSION, ESPECIALLY WHEN MULTITHREADING
LinkedList< PlayerInfo > *GetPlayerList()
{
	return m_players;
}
//
*/


	
		PlayerInfo *GetPlayer( unsigned int id );
		unsigned long GetNumberOfPlayers();
		unsigned int GetID();

		bool IsHost() { return m_ishost; }

	private:

PlayerInfo* myinfo;
unsigned int generatedId;

ENetAddress address;
ENetEvent eve;

ENetHost *myself;
ENetPeer *server;

bool m_init;

		unsigned int m_maxclients;

		char*  m_address;
		unsigned int m_port; // Port for network communication.
		unsigned long m_connectionTimeOut; // Timeout for trying to connect to server.
		unsigned long m_processingTime; // Allowed time period for processing received network messages.
		unsigned long m_client_incoming_bandwidth;
		unsigned long m_client_outgoing_bandwidth;

		unsigned int m_id; // ID of the local player.
		//unsigned int m_idHost; // ID of the host player.

		bool m_ishost;
		
		//CRITICAL_SECTION m_sessionCS; // Enumerated session list critical section.
		//LinkedList< SessionInfo > *m_sessions; // Linked list of enumerated sessions.

		//CRITICAL_SECTION m_playerCS; // Player list critical section.
		LinkedList< PlayerInfo > *m_players; // Linked list of players.

		bool m_receiveAllowed; // Inidcates if the network is allowed to receive application specific messages.
		//CRITICAL_SECTION m_messageCS; // Network message list critical section.
		LinkedList< ReceivedMessage > *m_messages; // Linked list of network messages.
		void (*HandleNetworkMessage)(ReceivedMessage *msg ); // Pointer to an application specific network message handler.
};

#endif