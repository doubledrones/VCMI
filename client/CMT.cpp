// CMT.cpp : Defines the entry point for the console application.
//
#include "../stdafx.h"
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/thread.hpp>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "SDL_Extensions.h"
#include "SDL_framerate.h"
#include "CGameInfo.h"
#include "../mapHandler.h"
#include "../global.h"
#include "CPreGame.h"
#include "CCastleInterface.h"
#include "../CConsoleHandler.h"
#include "CCursorHandler.h"
#include "../lib/CGameState.h"
#include "../CCallback.h"
#include "CPlayerInterface.h"
#include "CAdvmapInterface.h"
#include "../hch/CBuildingHandler.h"
#include "../hch/CVideoHandler.h"
#include "../hch/CHeroHandler.h"
#include "../hch/CCreatureHandler.h"
#include "../hch/CSpellHandler.h"
#include "../hch/CMusicHandler.h"
#include "../hch/CLodHandler.h"
#include "../hch/CDefHandler.h"
#include "../hch/CAmbarCendamo.h"
#include "../hch/CGeneralTextHandler.h"
#include "Graphics.h"
#include "Client.h"
#include "CConfigHandler.h"
#include "../lib/Connection.h"
#include "../lib/VCMI_Lib.h"
#include <cstdlib>
#include "../lib/NetPacks.h"

#if __MINGW32__
#undef main
#endif

/*
 * CMT.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

std::string NAME = NAME_VER + std::string(" (client)"); //application name
SDL_Surface *screen = NULL, *screen2 = NULL; //main screen surface and hlp surface (used to store not-active interfaces layer)

std::queue<SDL_Event*> events;
boost::mutex eventsM;

TTF_Font * TNRB16, *TNR, *GEOR13, *GEORXX, *GEORM, *GEOR16;

void processCommand(const std::string &message, CClient *&client);
static void setScreenRes(int w, int h, int bpp, bool fullscreen);

#ifndef __GNUC__
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{
	tlog0 << "Starting... " << std::endl;
	THC timeHandler tmh, total, pomtime;
	CClient *client = NULL;
	boost::thread *console = NULL;

	std::cout.flags(std::ios::unitbuf);
	logfile = new std::ofstream("VCMI_Client_log.txt");
	::console = new CConsoleHandler;
	*::console->cb = boost::bind(processCommand,_1,boost::ref(client));
	console = new boost::thread(boost::bind(&CConsoleHandler::run,::console));
	tlog0 <<"Creating console and logfile: "<<pomtime.getDif() << std::endl;

	conf.init();
	tlog0 <<"Loading settings: "<<pomtime.getDif() << std::endl;
	tlog0 << NAME << std::endl;

	srand ( time(NULL) );
	CPG=NULL;
	atexit(SDL_Quit);
	CGameInfo * cgi = CGI = new CGameInfo; //contains all global informations about game (texts, lodHandlers, map handler itp.)
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO)==0)
	{
		setScreenRes(800,600,conf.cc.bpp,conf.cc.fullscreen);
		tlog0 <<"\tInitializing screen: "<<pomtime.getDif() << std::endl;
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			int rmask = 0xff000000;int gmask = 0x00ff0000;int bmask = 0x0000ff00;int amask = 0x000000ff;
		#else
			int rmask = 0x000000ff;	int gmask = 0x0000ff00;	int bmask = 0x00ff0000;	int amask = 0xff000000;
		#endif
		CSDL_Ext::std32bppSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 1, 1, 32, rmask, gmask, bmask, amask);
		tlog0 << "\tInitializing minors: " << pomtime.getDif() << std::endl;
		TTF_Init();
		TNRB16 = TTF_OpenFont("Fonts" PATHSEPARATOR "tnrb.ttf",16);
		GEOR13 = TTF_OpenFont("Fonts" PATHSEPARATOR "georgia.ttf",13);
		GEOR16 = TTF_OpenFont("Fonts" PATHSEPARATOR "georgia.ttf",16);
		GEORXX = TTF_OpenFont("Fonts" PATHSEPARATOR "tnrb.ttf",22);
		GEORM = TTF_OpenFont("Fonts" PATHSEPARATOR "georgia.ttf",10);
		if(! (TNRB16 && GEOR16 && GEORXX && GEORM))
		{
			tlog1 << "One of the fonts couldn't be loaded!\n";
			throw "One of the fonts couldn't be loaded!\n";
		}
		atexit(TTF_Quit);
		THC tlog0<<"\tInitializing fonts: "<<pomtime.getDif()<<std::endl;

		//initializing audio
		CAudioHandler * audioh = new CAudioHandler;
		// Note: because of interface button range, volume can only be a
		// multiple of 11, from 0 to 99.
		audioh->initAudio(88);
		cgi->audioh = audioh;
		tlog0<<"\tInitializing sound: "<<pomtime.getDif()<<std::endl;

		tlog0<<"Initializing screen, fonts and sound handling: "<<tmh.getDif()<<std::endl;
		initDLL(::console,logfile);
		CGI->setFromLib();
		cgi->audioh->initCreaturesSounds(CGI->creh->creatures);
		cgi->audioh->initSpellsSounds(CGI->spellh->spells);
		tlog0<<"Initializing VCMI_Lib: "<<tmh.getDif()<<std::endl;
		pomtime.getDif();
		cgi->curh = new CCursorHandler;
		cgi->curh->initCursor();
		cgi->curh->show();
		tlog0<<"Screen handler: "<<pomtime.getDif()<<std::endl;
		pomtime.getDif();
		graphics = new Graphics();
		graphics->loadHeroAnim();
		tlog0<<"\tMain graphics: "<<tmh.getDif()<<std::endl;
		tlog0<<"Initializing game graphics: "<<tmh.getDif()<<std::endl;
		CMessage::init();
		tlog0<<"Message handler: "<<tmh.getDif()<<std::endl;
		CPreGame * cpg = new CPreGame(); //main menu and submenus
		tlog0<<"Initialization CPreGame (together): "<<tmh.getDif()<<std::endl;
		tlog0<<"Initialization of VCMI (together): "<<total.getDif()<<std::endl;

		audioh->playMusic(musicBase::mainMenu, -1);
		StartInfo *options = new StartInfo(cpg->runLoop());

		if(screen->w != conf.cc.resx   ||   screen->h != conf.cc.resy)
		{
			setScreenRes(conf.cc.resx,conf.cc.resy,conf.cc.bpp,conf.cc.fullscreen);
		}
		CClient cl;
		if(options->mode == 0) //new game
		{
			tmh.getDif();
			char portc[10];
			SDL_itoa(conf.cc.port,portc,10);
			CClient::runServer(portc);
			tlog0<<"Preparing shared memory and starting server: "<<tmh.getDif()<<std::endl;

			tmh.getDif();pomtime.getDif();//reset timers

			CConnection *c=NULL;
			//wait until server is ready
			tlog0<<"Waiting for server... ";
			cl.waitForServer();
			tlog0 << tmh.getDif()<<std::endl;
			while(!c)
			{
				try
				{
					tlog0 << "Establishing connection...\n";
					c = new CConnection(conf.cc.server,portc,NAME);
				}
				catch(...)
				{
					tlog1 << "\nCannot establish connection! Retrying within 2 seconds" <<std::endl;
					SDL_Delay(2000);
				}
			}
			THC tlog0<<"\tConnecting to the server: "<<tmh.getDif()<<std::endl;
			cl.newGame(c,options);
			client = &cl;
			audioh->stopMusic();
			boost::thread t(boost::bind(&CClient::run,&cl));
		}
		else //load game
		{
			std::string fname = options->mapname;
			boost::algorithm::erase_last(fname,".vlgm1");
			cl.load(fname);
			client = &cl;
			audioh->stopMusic();
			boost::thread t(boost::bind(&CClient::run,&cl));
		}

		SDL_Event *ev = NULL;
		while(1) //main SDL events loop
		{
			ev = new SDL_Event();

			int ret = SDL_WaitEvent(ev);
			if(ret == 0 || (ev->type==SDL_QUIT)  ||  (ev->type == SDL_KEYDOWN && ev->key.keysym.sym==SDLK_F4 && (ev->key.keysym.mod & KMOD_ALT)))
			{
				LOCPLINT->pim->lock();
				cl.close();
#ifndef __unix__
				::console->killConsole(console->native_handle());
#endif
				SDL_Delay(750);
				tlog0 << "Ending...\n";
				exit(EXIT_SUCCESS);
			}
			else if(ev->type == SDL_KEYDOWN && ev->key.keysym.sym==SDLK_F4)
			{
				boost::unique_lock<boost::recursive_mutex> lock(*LOCPLINT->pim);
				bool full = !(screen->flags&SDL_FULLSCREEN);
				setScreenRes(conf.cc.resx,conf.cc.resy,conf.cc.bpp,full);
				LOCPLINT->totalRedraw();
			}
			eventsM.lock();
			events.push(ev);
			eventsM.unlock();
		}
	}
	else
	{
		tlog1<<"Something was wrong: "<<SDL_GetError()<<std::endl;
		return -1;
	}
}

void processCommand(const std::string &message, CClient *&client)
{
	std::istringstream readed;
	readed.str(message);
	std::string cn; //command name
	readed >> cn;
	int3 src, dst;

//	int heronum;//TODO use me
	int3 dest;

	if(LOCPLINT && LOCPLINT->cingconsole)
		LOCPLINT->cingconsole->print(message);

	if(message==std::string("die, fool"))
		exit(EXIT_SUCCESS);
	else if(cn==std::string("activate"))
	{
		int what;
		readed >> what;
		switch (what)
		{
		case 0:
			LOCPLINT->topInt()->activate();
			break;
		case 1:
			LOCPLINT->adventureInt->activate();
			break;
		case 2:
			LOCPLINT->castleInt->activate();
			break;
		}
	}
	else if(cn=="save")
	{
		std::string fname;
		readed >> fname;
		client->save(fname);
	}
	else if(cn=="load")
	{
		std::string fname;
		readed >> fname;
		client->load(fname);
	}
	else if(cn=="resolution")
	{
		std::map<std::pair<int,int>, config::GUIOptions >::iterator j;
		int i=1, hlp=1;
		tlog4 << "Available screen resolutions:\n";
		for(j=conf.guiOptions.begin(); j!=conf.guiOptions.end(); j++)
			tlog4 << i++ <<". " << j->first.first << " x " << j->first.second << std::endl;
		tlog4 << "Type number from 1 to " << i-1 << " to set appropriate resolution or 0 to cancel.\n";
		std::cin >> i;
		if(!i)
			return;
		else if(i < 0  ||  i > conf.guiOptions.size())
		{
			tlog1 << "Invalid resolution ID! Not a number between 0 and  " << conf.guiOptions.size() << ". No settings changed.\n";
		}
		else
		{
			for(j=conf.guiOptions.begin(); j!=conf.guiOptions.end() && hlp++<i; j++); //move j to the i-th resolution info
			conf.cc.resx = j->first.first;
			conf.cc.resy = j->first.second;
			tlog0 << "Screen resolution set to " << conf.cc.resx << " x " << conf.cc.resy <<". It will be aplied when the game starts.\n";
		}
	}
	else if(message=="get txt")
	{
		boost::filesystem::create_directory("Extracted_txts");
		tlog0<<"Command accepted. Opening .lod file...\t";
		CLodHandler * txth = new CLodHandler;
		txth->init(std::string(DATA_DIR "Data" PATHSEPARATOR "H3bitmap.lod"),"");
		tlog0<<"done.\nScanning .lod file\n";
		int curp=0;
		std::string pattern = ".TXT", pom;
		for(int i=0;i<txth->entries.size(); i++)
		{
			pom = txth->entries[i].nameStr;
			if(boost::algorithm::find_last(pom,pattern))
			{
				txth->extractFile(std::string("Extracted_txts\\")+pom,pom);
			}
			if(i%8) continue;
			int p2 = ((float)i/(float)txth->entries.size())*(float)100;
			if(p2!=curp)
			{
				curp = p2;
				tlog0<<"\r"<<curp<<"%";
			}
		}
		tlog0<<"\rExtracting done :)\n";
	}
	else if(client && client->serv && client->serv->connected) //send to server
	{
		PlayerMessage pm(LOCPLINT->playerID,message);
		*client->serv << &pm;
	}
}

static void setScreenRes(int w, int h, int bpp, bool fullscreen)
{
	if(screen) //screen has been already initialized
		SDL_QuitSubSystem(SDL_INIT_VIDEO);

	SDL_InitSubSystem(SDL_INIT_VIDEO);

	// VCMI will only work with 3 or 4 bytes per pixel
	if (bpp < 24) bpp = 24;
	if (bpp > 32) bpp = 32;

	// Try to use the best screen depth for the display
	if (((bpp = SDL_VideoModeOK(w, h, bpp, SDL_SWSURFACE|(fullscreen?SDL_FULLSCREEN:0))) == 0) ||
		((screen = SDL_SetVideoMode(w, h, bpp, SDL_SWSURFACE|(fullscreen?SDL_FULLSCREEN:0))) == NULL)) {
		tlog1 << "Requested screen resolution is not available (" << w << "x" << h << ")\n";
		throw "Requested screen resolution is not available\n";
	}

	if(screen2)
		SDL_FreeSurface(screen2);
	screen2 = CSDL_Ext::copySurface(screen);
	SDL_EnableUNICODE(1);
	SDL_WM_SetCaption(NAME.c_str(),""); //set window title
	SDL_ShowCursor(SDL_DISABLE);
}