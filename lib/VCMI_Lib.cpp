#define VCMI_DLL
#include "../global.h"
#include "VCMI_Lib.h"
#include "CArtHandler.h"
#include "CCreatureHandler.h"
#include "CDefObjInfoHandler.h"
#include "CHeroHandler.h"
#include "CLodHandler.h"
#include "CObjectHandler.h"
#include "CTownHandler.h"
#include "CBuildingHandler.h"
#include "CSpellHandler.h"
#include "CGeneralTextHandler.h"
#include "IGameEventsReceiver.h"

/*
 * VCMI_Lib.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

class CLodHandler;
LibClasses * VLC = NULL;
DLL_EXPORT CLodHandler *bitmaph = NULL, 
	*spriteh = NULL,
	*bitmaph_ab = NULL;


DLL_EXPORT CLogger tlog0(0);
DLL_EXPORT CLogger tlog1(1);
DLL_EXPORT CLogger tlog2(2);
DLL_EXPORT CLogger tlog3(3);
DLL_EXPORT CLogger tlog4(4);
DLL_EXPORT CLogger tlog5(5);
DLL_EXPORT CLogger tlog6(-2);

DLL_EXPORT CConsoleHandler *console = NULL;
DLL_EXPORT std::ostream *logfile = NULL
;
DLL_EXPORT void initDLL(CConsoleHandler *Console, std::ostream *Logfile)
{
	console = Console;
	logfile = Logfile;
	VLC = new LibClasses;
	try
	{
		VLC->init();
	} HANDLE_EXCEPTION;
}

DLL_EXPORT void loadToIt(std::string &dest, const std::string &src, int &iter, int mode)
{
	switch(mode)
	{
	case 0:
		{
			int hmcr = 0;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\t')
					++hmcr;
				if(hmcr==1)
					break;
			}
			++iter;

			int befi=iter;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\t')
					break;
			}
			dest = src.substr(befi, iter-befi);
			++iter;

			hmcr = 0;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\r')
					++hmcr;
				if(hmcr==1)
					break;
			}
			iter+=2;
			break;
		}
	case 1:
		{
			int hmcr = 0;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\t')
					++hmcr;
				if(hmcr==1)
					break;
			}
			++iter;

			int befi=iter;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\r')
					break;
			}
			dest = src.substr(befi, iter-befi);
			iter+=2;
			break;
		}
	case 2:
		{
			int befi=iter;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\t')
					break;
			}
			dest = src.substr(befi, iter-befi);
			++iter;

			int hmcr = 0;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\r')
					++hmcr;
				if(hmcr==1)
					break;
			}
			iter+=2;
			break;
		}
	case 3:
		{
			int befi=iter;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\r')
					break;
			}
			dest = src.substr(befi, iter-befi);
			iter+=2;
			break;
		}
	case 4:
		{
			int befi=iter;
			for(; iter<src.size(); ++iter)
			{
				if(src[iter]=='\t')
					break;
			}
			dest = src.substr(befi, iter-befi);
			iter++;
			break;
		}
	}
}


DLL_EXPORT void loadToIt(si32 &dest, const std::string &src, int &iter, int mode)
{
	std::string pom;
	loadToIt(pom,src,iter,mode);
	dest = atol(pom.c_str());
}

void LibClasses::init()
{
	timeHandler pomtime;

	generaltexth = new CGeneralTextHandler;
	generaltexth->load();
	tlog0<<"\tGeneral text handler: "<<pomtime.getDif()<<std::endl;

	heroh = new CHeroHandler;
	heroh->loadHeroes();
	heroh->loadObstacles();
	heroh->loadPuzzleInfo();
	tlog0 <<"\tHero handler: "<<pomtime.getDif()<<std::endl;

	arth = new CArtHandler;
	arth->loadArtifacts(false);
	tlog0<<"\tArtifact handler: "<<pomtime.getDif()<<std::endl;

	creh = new CCreatureHandler();
	creh->loadCreatures();
	tlog0<<"\tCreature handler: "<<pomtime.getDif()<<std::endl;

	townh = new CTownHandler;
	townh->loadStructures();
	tlog0<<"\tTown handler: "<<pomtime.getDif()<<std::endl;

	objh = new CObjectHandler;
	objh->loadObjects();
	tlog0<<"\tObject handler: "<<pomtime.getDif()<<std::endl;

	dobjinfo = new CDefObjInfoHandler;
	dobjinfo->load();
	tlog0<<"\tDef information handler: "<<pomtime.getDif()<<std::endl;

	buildh = new CBuildingHandler;
	buildh->loadBuildings();
	tlog0<<"\tBuilding handler: "<<pomtime.getDif()<<std::endl;

	spellh = new CSpellHandler;
	spellh->loadSpells();
	tlog0<<"\tSpell handler: "<<pomtime.getDif()<<std::endl;

	IS_AI_ENABLED = false;
}

void LibClasses::clear()
{
	delete generaltexth;
	delete heroh;
	delete arth;
	delete creh;
	delete townh;
	delete objh;
	delete dobjinfo;
	delete buildh;
	delete spellh;
	makeNull();
}

void LibClasses::makeNull()
{
	generaltexth = NULL;
	heroh = NULL;
	arth = NULL;
	creh = NULL;
	townh = NULL;
	objh = NULL;
	dobjinfo = NULL;
	buildh = NULL;
	spellh = NULL;
}

LibClasses::LibClasses()
{
	//load .lod archives
	timeHandler pomtime;
	spriteh = new CLodHandler();
	spriteh->init(DATA_DIR "/Data/H3sprite.lod", DATA_DIR "/Sprites");
	bitmaph = new CLodHandler;
	bitmaph->init(DATA_DIR "/Data/H3bitmap.lod", DATA_DIR "/Data");
	bitmaph_ab = new CLodHandler();
	bitmaph_ab->init(DATA_DIR "/Data/H3ab_bmp.lod", DATA_DIR "/Data");
	tlog0<<"Loading .lod files: "<<pomtime.getDif()<<std::endl;

	//init pointers to handlers
	makeNull();
}

void LibClasses::callWhenDeserializing()
{
	generaltexth = new CGeneralTextHandler;
	generaltexth->load();
	arth->loadArtifacts(true);
}
