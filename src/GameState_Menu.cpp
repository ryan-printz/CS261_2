// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Menu.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	implementation of the 'menu' game state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------

#include "main.h"
#include "GameState_Menu.h"
#include "GameState_NetworkMenu.h"
#include "GameState_Play.h"

#define TESTING

#ifdef TESTING
#include "GameState_NetworkPlay.h"
#endif

#include "curl\curl.h"
#include "json\json.h"

// ---------------------------------------------------------------------------

GameState_Menu::GameState_Menu()
{}

GameState_Menu::~GameState_Menu()
{}

void GameState_Menu::load(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

GameState_Menu::tweet::tweet(const std::string & _user, const std::string &_text)
	: user(_user), text(_text)
{}

static std::string tweets;

size_t writeCallback(char * buffer, size_t size, size_t nmemb, void * up)
{
	for( int i = 0; i < size * nmemb; i++ )
		tweets.push_back(buffer[i]);

	return size * nmemb;
}

void GameState_Menu::init(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_curl = curl_easy_init();

	curl_easy_setopt(m_curl, CURLOPT_URL, "http://search.twitter.com/search.json?q=%23cs261asteroids&rpp=5");
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 1L);

	curl_easy_perform(m_curl);

	Json::Value root;
	Json::Reader reader;

	bool result = reader.parse(tweets, root, false);
	
	if( result )
		for( auto rtweet = root["results"].begin(); rtweet != root["results"].end(); ++rtweet )
			m_tweets.push_back(tweet((*rtweet)["from_user"].asString(), (*rtweet)["text"].asString()));

	m_cursor = 0;
}

// ---------------------------------------------------------------------------

void GameState_Menu::update(void)
{
	if(AEInputCheckTriggered(DIK_UP))
		m_cursor--;
	if(AEInputCheckTriggered(DIK_DOWN))
		m_cursor++;
	
	m_cursor = (m_cursor < 0) ? 0 : ((m_cursor > 2) ? 2 : m_cursor);

#ifdef TESTING
	GameReplicationInfo game;
	PlayerReplicationInfo localPlayer;
	std::vector<PlayerReplicationInfo> players;
#endif

	if(AEInputCheckTriggered(DIK_SPACE))
		switch(m_cursor)
		{
#ifdef TESTING
		case 0:
			sprintf(localPlayer.m_name, "player");
			localPlayer.m_lives = 3;
			localPlayer.m_x = 10;
			localPlayer.m_y = 10;
			players.push_back(localPlayer);
			localPlayer.m_x = 20;
			localPlayer.m_y = 20;
			players.push_back(localPlayer);
			localPlayer.m_x = 0;
			localPlayer.m_y = 10;
			players.push_back(localPlayer);
			m_gsm->nextState(new GameState_NetworkPlay(game, players, nullptr));
			break;
#else
		case 0:
			m_gsm->nextState(new GameState_Play());
			break;
#endif

		case 1:
			m_gsm->nextState(new GameState_NetworkMenu());
			break;

		default:
			m_gsm->quit(true);
			break;
		};
}

// ---------------------------------------------------------------------------

void wrapPrint(int x, int & y, int color, std::string & string)
{
	static const int max_line = 60;

	std::list<std::string> substrs;
	int lines = std::ceil((float)string.length() / max_line);

	for( int i = 0; i < lines; ++i )
		substrs.push_back(string.substr(i * max_line, max_line)); 

	for( auto line = substrs.rbegin(); line != substrs.rend(); ++line )	
	{
		AEGfxPrint(x, y, color, (s8*)line->c_str());
		y -= 25;
	}
}

void GameState_Menu::draw(void)
{
	AEGfxPrint(10, 20, 0xFFFFFFFF, "<> ASTEROID <>");
	AEGfxPrint(40, 60, 0xFFFFFFFF, "Join Game");
	AEGfxPrint(40, 90, 0xFFFFFFFF, "Network Menu");
	AEGfxPrint(40, 120, 0xFFFFFFFF, "Quit");

	if (gAEFrameCounter & 0x0008)
		AEGfxPrint(10, 60 + 30 * m_cursor, 0xFFFFFFFF, ">>");

	int y = 565;

	for( auto rtweet = m_tweets.rbegin(); rtweet != m_tweets.rend(); ++rtweet )
	{
		wrapPrint(30, y, 0xCCCCCCFF, rtweet->text);
		AEGfxPrint(10, y + 15, 0xF09090FF, "@");
		AEGfxPrint(20, y+10, 0xAAAAAAFF, (s8*)rtweet->user.c_str());
		y-=30;
	}
}

// ---------------------------------------------------------------------------

void GameState_Menu::free(void)
{
	curl_easy_cleanup(m_curl);
	curl_global_cleanup();

	m_tweets.clear();
}

// ---------------------------------------------------------------------------

void GameState_Menu::unload(void)
{
}
