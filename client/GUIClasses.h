#ifndef __GUICLASSES_H__
#define __GUICLASSES_H__

#include "../global.h"
#include "GUIBase.h"
#include "FunctionList.h"
#include <set>
#include <list>
#include <boost/thread/mutex.hpp>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

/*
 * GUIClasses.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

class CBonusSystemNode;
class CArtifact;
class CDefEssential;
class AdventureMapButton;
class CHighlightableButtonsGroup;
class CDefHandler;
struct HeroMoveDetails;
class CDefEssential;
class CGHeroInstance;
class CAdvMapInt;
class CCastleInterface;
class CBattleInterface;
class CStack;
class SComponent;
class CCreature;
struct SDL_Surface;
struct CPath;
class CCreatureAnimation;
class CSelectableComponent;
class CCreatureSet;
class CGObjectInstance;
class CGDwelling;
class CSlider;
struct UpgradeInfo;
template <typename T> struct CondSh;
class CInGameConsole;
class CGarrisonInt;
class CInGameConsole;
struct Component;
class CArmedInstance;
class CGTownInstance;
class StackState;
class CPlayerInterface;
class CHeroWindow;
class CArtifact;
class CArtifactsOfHero;
class CResDataBar;
struct SPuzzleInfo;
class CGGarrison;
class CStackInstance;
class IMarket;
class CTextBox;

extern SDL_Color tytulowy, tlo, zwykly ;

class CInfoWindow : public CSimpleWindow //text + comp. + ok button
{ //window able to delete its components when closed
	bool delComps; //whether comps will be deleted

public:
	int ID; //for identification
	CTextBox *text;
	std::vector<AdventureMapButton *> buttons;
	std::vector<SComponent*> components;
	CSlider *slider;

	void setDelComps(bool DelComps);
	virtual void close();

	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to);
	void sliderMoved(int to);

	CInfoWindow(std::string Text, int player, const std::vector<SComponent*> &comps, std::vector<std::pair<std::string,CFunctionList<void()> > > &Buttons, bool delComps); //c-tor
	CInfoWindow(); //c-tor
	~CInfoWindow(); //d-tor

	static void showYesNoDialog( const std::string & text, const std::vector<SComponent*> *components, const CFunctionList<void( ) > &onYes, const CFunctionList<void()> &onNo, bool DelComps = true, int player = 1); //use only before the game starts! (showYesNoDialog in LOCPLINT must be used then)
	static CInfoWindow *create(const std::string &text, int playerID = 1, const std::vector<SComponent*> *components = NULL, bool DelComps = false);
};
class CSelWindow : public CInfoWindow //component selection window
{ //warning - this window deletes its components by closing!
public:
	void selectionChange(unsigned to);
	void madeChoice(); //looks for selected component and calls callback
	CSelWindow(const std::string& text, int player, int charperline ,const std::vector<CSelectableComponent*> &comps, const std::vector<std::pair<std::string,CFunctionList<void()> > > &Buttons, int askID); //c-tor
	CSelWindow(){}; //c-tor
	//notification - this class inherits important destructor from CInfoWindow
};

class CRClickPopup : public CIntObject //popup displayed on R-click
{
public:
	virtual void activate();
	virtual void deactivate();
	virtual void close();
	void clickRight(tribool down, bool previousState);

	CRClickPopup();
	virtual ~CRClickPopup(); //d-tor

	static void createAndPush(const std::string &txt);
	static void createAndPush(const CGObjectInstance *obj, const Point &p, EAlignment alignment = BOTTOMRIGHT);
};

class CRClickPopupInt : public CRClickPopup //popup displayed on R-click
{
public:
	IShowActivable *inner;
	bool delInner;

	void show(SDL_Surface * to);
	CRClickPopupInt(IShowActivable *our, bool deleteInt); //c-tor
	virtual ~CRClickPopupInt(); //d-tor
};

class CInfoPopup : public CRClickPopup
{
public:
	bool free; //TODO: comment me
	SDL_Surface * bitmap; //popup background
	void close();
	void show(SDL_Surface * to);
	CInfoPopup(SDL_Surface * Bitmap, int x, int y, bool Free=false); //c-tor
	CInfoPopup(SDL_Surface * Bitmap, const Point &p, EAlignment alignment, bool Free=false); //c-tor
	CInfoPopup(SDL_Surface *Bitmap = NULL, bool Free = false); //default c-tor

	void init(int x, int y);
	~CInfoPopup(); //d-tor
};

class SComponent : public virtual CIntObject //common popup window component
{
public:
	enum Etype
	{
		primskill, secskill, resource, creature, artifact, experience, secskill44, spell, morale, luck, building, hero, flag
	} type; //component type
	int subtype; //TODO: comment me
	int val; //TODO: comment me

	std::string description; //r-click
	std::string subtitle; //TODO: comment me

	SDL_Surface *img; //our image
	bool free; //should surface be freed on delete
	
	SDL_Surface * setSurface(std::string defName, int imgPos);

	void init(Etype Type, int Subtype, int Val);
	SComponent(Etype Type, int Subtype, int Val, SDL_Surface *sur=NULL, bool freeSur=false); //c-tor
	SComponent(const Component &c); //c-tor
	SComponent(){}; //c-tor
	virtual ~SComponent(); //d-tor

	void clickRight(tribool down, bool previousState); //call-in
	SDL_Surface * getImg();
	virtual void show(SDL_Surface * to);
	virtual void activate();
	virtual void deactivate();
};

class CSelectableComponent : public SComponent, public KeyShortcut
{
public:
	bool selected; //if true, this component is selected
	boost::function<void()> onSelect; //function called on selection change

	void clickLeft(tribool down, bool previousState); //call-in
	void init();
	CSelectableComponent(Etype Type, int Sub, int Val, boost::function<void()> OnSelect = 0); //c-tor
	CSelectableComponent(const Component &c, boost::function<void()> OnSelect = 0); //c-tor
	~CSelectableComponent(); //d-tor
	virtual void show(SDL_Surface * to);
	void activate();
	void deactivate();
	void select(bool on);
};
class CGarrisonInt;
class CGarrisonSlot : public CIntObject
{
public:
	int ID; //for identification
	CGarrisonInt *owner;
	const CStackInstance *myStack; //NULL if slot is empty
	const CCreature *creature;
	int count; //number of creatures
	int upg; //0 - up garrison, 1 - down garrison
	bool active; //TODO: comment me

	virtual void hover (bool on); //call-in
	const CArmedInstance * getObj();
	bool our();
	void clickRight(tribool down, bool previousState);
	void clickLeft(tribool down, bool previousState);
	void activate();
	void deactivate();
	void show(SDL_Surface * to);
	CGarrisonSlot(CGarrisonInt *Owner, int x, int y, int IID, int Upg=0, const CStackInstance * Creature=NULL);
	~CGarrisonSlot(); //d-tor
};

class CGarrisonInt :public CIntObject
{
public:
	int interx; //space between slots
	Point garOffset, //offset between garrisons (not used if only one hero)
	      surOffset; //offset between garrison position on the bg surface and position on the screen
	CGarrisonSlot *highlighted; //chosen slot
	std::vector<AdventureMapButton *> splitButtons; //may be empty if no buttons

	SDL_Surface *&sur; //bg surface
	int p2, //TODO: comment me
	    shiftPos;//1st slot of the second row, set shiftPoint for effect
	bool splitting, pb, 
	     smallIcons, //true - 32x32 imgs, false - 58x64
	     removableUnits,//player can remove units from up
	     twoRows,//slots will be placed in 2 rows
		 ourUp,ourDown;//player owns up or down army

	const CCreatureSet *set1; //top set of creatures
	const CCreatureSet *set2; //bottom set of creatures

	std::vector<CGarrisonSlot*> slotsUp, slotsDown; //slots of upper and lower garrison
	const CArmedInstance *oup, *odown; //upper and lower garrisons (heroes or towns)

	void addSplitBtn(AdventureMapButton * button);
	void createSet(std::vector<CGarrisonSlot*> &ret, const CCreatureSet * set, int posX, int distance, int posY, int Upg );
	
	void activate();
	void createSlots();
	void deleteSlots();
	void recreateSlots();

	void splitClick(); //handles click on split button
	void splitStacks(int am2); //TODO: comment me
	//x, y - position; inx - distance between slots; pomsur - background surface, SurOffset - ?; s1, s2 - top and bottom armies; removableUnits - you can take units from top; smallImgs - units images size 64x58 or 32x32; twoRows - display slots in 2 row (1st row = 4, 2nd = 3)
	CGarrisonInt(int x, int y, int inx, const Point &garsOffset, SDL_Surface *&pomsur, const Point &SurOffset, const CArmedInstance *s1, const CArmedInstance *s2=NULL, bool _removableUnits = true, bool smallImgs = false, bool _twoRows=false); //c-tor
	~CGarrisonInt(); //d-tor
};

class CStatusBar
	: public CIntObject, public IStatusBar
{
public:
	SDL_Surface * bg; //background
	int middlex, middley; //middle of statusbar
	std::string current; //text currently printed

	CStatusBar(int x, int y, std::string name="ADROLLVR.bmp", int maxw=-1); //c-tor
	~CStatusBar(); //d-tor
	void print(const std::string & text); //prints text and refreshes statusbar
	void clear();//clears statusbar and refreshes
	void show(SDL_Surface * to); //shows statusbar (with current text)
	std::string getCurrent(); //getter for current
};

class CLabel
	: public virtual CIntObject
{
public:
	EAlignment alignment;
	EFonts font;
	SDL_Color color;
	std::string text;
	CPicture *bg;
	bool autoRedraw;  //whether control will redraw itself on setTxt
	Point textOffset; //text will be blitted at pos + textOffset with appropriate alignment
	bool ignoreLeadingWhitespace; 

	virtual void setTxt(const std::string &Txt);
	void showAll(SDL_Surface * to); //shows statusbar (with current text)
	CLabel(int x=0, int y=0, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = zwykly, const std::string &Text =  "");
};

//a multi-line label that tries to fit text with given available width and height; if not possible, it creates a slider for scrolling text
class CTextBox
	: public CLabel
{
public:
	int maxW; //longest line of text in px
	int maxH; //total height needed to print all lines

	int sliderStyle;
	bool redrawParentOnScrolling;

	std::vector<std::string> lines;
	CSlider *slider;

	//CTextBox( std::string Text, const Point &Pos, int w, int h, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = zwykly);
	CTextBox(std::string Text, const Rect &rect, int SliderStyle, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = zwykly);
	void showAll(SDL_Surface * to); //shows statusbar (with current text)
	void setTxt(const std::string &Txt);
	void setBounds(int limitW, int limitH);
	void recalculateLines(const std::string &Txt);

	void sliderMoved(int to);
};

class CGStatusBar
	: public CLabel, public IStatusBar
{
	void init();
public:
	IStatusBar *oldStatusBar;

	//statusbar interface overloads
	void print(const std::string & Text); //prints text and refreshes statusbar
	void clear();//clears statusbar and refreshes
	std::string getCurrent(); //returns currently displayed text
	void show(SDL_Surface * to); //shows statusbar (with current text)

	CGStatusBar(int x, int y, EFonts Font = FONT_SMALL, EAlignment Align = CENTER, const SDL_Color &Color = zwykly, const std::string &Text =  "");
	CGStatusBar(CPicture *BG, EFonts Font = FONT_SMALL, EAlignment Align = CENTER, const SDL_Color &Color = zwykly); //given CPicture will be captured by created sbar and it's pos will be used as pos for sbar
	CGStatusBar(int x, int y, std::string name, int maxw=-1); 

	~CGStatusBar();
	void calcOffset();
};

class CFocusable 
	: public virtual CIntObject
{
public:
	bool focus; //only one focusable control can have focus at one moment

	void giveFocus(); //captures focus
	void moveFocus(); //moves focus to next active control (may be used for tab switching)

	static std::list<CFocusable*> focusables; //all existing objs
	static CFocusable *inputWithFocus; //who has focus now
	CFocusable();
	~CFocusable();
};

class CTextInput
	: public CLabel, public CFocusable
{
public:
	CFunctionList<void(const std::string &)> cb;

	void setText(const std::string &nText, bool callCb = false);

	CTextInput(const Rect &Pos, const Point &bgOffset, const std::string &bgName, const CFunctionList<void(const std::string &)> &CB);
	CTextInput(const Rect &Pos, SDL_Surface *srf = NULL);
	~CTextInput();
	void showAll(SDL_Surface * to);
	void clickLeft(tribool down, bool previousState);
	void keyPressed(const SDL_KeyboardEvent & key);
};

class CList : public CIntObject
{
public:
	SDL_Surface * bg; //background bitmap
	CDefHandler *arrup, *arrdo; //button arrows for scrolling list
	SDL_Surface *empty, *selection;
	SDL_Rect arrupp, arrdop; //positions of arrows
	int posw, posh; //position width/height
	int selected, //id of selected position, <0 if none
		from;
	const int SIZE; //size of list
	tribool pressed; //true=up; false=down; indeterminate=none

	CList(int Size = 5); //c-tor
	void clickLeft(tribool down, bool previousState);
	void activate();
	void deactivate();
	virtual void mouseMoved (const SDL_MouseMotionEvent & sEvent)=0; //call-in
	virtual void genList()=0;
	virtual void select(int which)=0;
	virtual void draw(SDL_Surface * to)=0;
	virtual int size() = 0; //how many elements do we have
	void fixPos(); //scrolls list, so the selection will be visible
};
class CHeroList
	: public CList
{
public:
	CDefHandler *mobile, *mana; //mana and movement indicators
	int posmobx, posporx, posmanx, posmoby, pospory, posmany;

	CHeroList(int Size); //c-tor
	int getPosOfHero(const CGHeroInstance* h); //hero's position on list
	void genList();
	void select(int which); //call-in
	void mouseMoved (const SDL_MouseMotionEvent & sEvent); //call-in
	void clickLeft(tribool down, bool previousState); //call-in
	void clickRight(tribool down, bool previousState); //call-in
	void hover (bool on); //call-in
	void keyPressed (const SDL_KeyboardEvent & key); //call-in
	void updateHList(const CGHeroInstance *toRemove=NULL); //removes specific hero from the list or recreates it
	void updateMove(const CGHeroInstance* which); //draws move points bar
	void draw(SDL_Surface * to);
	void show(SDL_Surface * to);
	void init();
	int size(); //how many elements do we have
};

class CTownList
	: public CList
{
public:
	boost::function<void()> fun; //function called on selection change
	int posporx,pospory;

	CTownList(int Size, int x, int y, std::string arrupg, std::string arrdog); //c-tor
	~CTownList(); //d-tor
	void genList();
	void select(int which); //call-in
	void mouseMoved (const SDL_MouseMotionEvent & sEvent);  //call-in
	void clickLeft(tribool down, bool previousState);  //call-in
	void clickRight(tribool down, bool previousState); //call-in
	void hover (bool on);  //call-in
	void keyPressed (const SDL_KeyboardEvent & key);  //call-in
	void draw(SDL_Surface * to);
	void show(SDL_Surface * to);
	int size(); //how many elements do we have
};

class CCreaturePic //draws picture with creature on background, use nextFrame=true to get animation
{
public:
	const CCreature *c; //which creature's picture
	bool big; //big => 100x130; !big => 100x120
	CCreatureAnimation *anim; //displayed animation
	CCreaturePic(const CCreature *cre, bool Big=true); //c-tor
	~CCreaturePic(); //d-tor
	int blitPic(SDL_Surface *to, int x, int y, bool nextFrame); //prints creature on screen
	SDL_Surface * getPic(bool nextFrame); //returns frame of animation
};

class CRecruitmentWindow : public CIntObject
{
public:
	static const int SPACE_BETWEEN = 18;
	static const int CREATURE_WIDTH = 102;
	static const int TOTAL_CREATURE_WIDTH = SPACE_BETWEEN + CREATURE_WIDTH;

	struct creinfo
	{
		SDL_Rect pos;
		CCreaturePic *pic; //creature's animation
		int ID, amount; //creature ID and available amount
		std::vector<std::pair<int,int> > res; //res_id - cost_per_unit
	};
	std::vector<int> amounts; //how many creatures we can afford
	std::vector<creinfo> creatures; //recruitable creatures
	boost::function<void(int,int)> recruit; //void (int ID, int amount) <-- call to recruit creatures
	CSlider *slider; //for selecting amount
	AdventureMapButton *max, *buy, *cancel;
	SDL_Surface *bitmap; //background
	CStatusBar *bar;
	int which; //which creature is active

	const CGDwelling *dwelling;
	int level;
	const CArmedInstance *dst;

	void close();
	void Max();
	void Buy();
	void Cancel();
	void sliderMoved(int to);
	void clickLeft(tribool down, bool previousState);
	void clickRight(tribool down, bool previousState);
	void activate();
	void deactivate();
	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to){show(to);};
	void cleanCres();
	void initCres();
	CRecruitmentWindow(const CGDwelling *Dwelling, int Level, const CArmedInstance *Dst, const boost::function<void(int,int)> & Recruit, int y_offset = 0); //creatures - pairs<creature_ID,amount> //c-tor
	~CRecruitmentWindow(); //d-tor
};

class CSplitWindow : public CIntObject
{
public:
	CGarrisonInt *gar;
	CSlider *slider;
	CCreaturePic *anim; //creature's animation
	AdventureMapButton *ok, *cancel;
	SDL_Surface *bitmap; //background
	int a1, a2, c; //TODO: comment me
	bool which; //which creature is selected
	int last; //0/1/2 - at least one creature must be in the src/dst/both stacks; -1 - no restrictions

	CSplitWindow(int cid, int max, CGarrisonInt *Owner, int Last = -1, int val=0); //c-tor; val - initial amount of second stack
	~CSplitWindow(); //d-tor
	void activate();
	void split();
	void close();
	void deactivate();
	void show(SDL_Surface * to);
	void clickLeft(tribool down, bool previousState); //call-in
	void keyPressed (const SDL_KeyboardEvent & key); //call-in
	void sliderMoved(int to);
};

class CLevelWindow : public CIntObject
{
public:
	int heroPortrait;
	SDL_Surface *bitmap; //background
	std::vector<CSelectableComponent *> comps; //skills to select
	AdventureMapButton *ok;
	boost::function<void(ui32)> cb;

	void close();
	CLevelWindow(const CGHeroInstance *hero, int pskill, std::vector<ui16> &skills, boost::function<void(ui32)> &callback); //c-tor
	~CLevelWindow(); //d-tor
	void activate();
	void deactivate();
	void selectionChanged(unsigned to);
	void show(SDL_Surface * to);
};

class CMinorResDataBar : public CIntObject
{
public:
	SDL_Surface *bg; //background bitmap
	void show(SDL_Surface * to);
	CMinorResDataBar(); //c-tor
	~CMinorResDataBar(); //d-tor
};

class CObjectListWindow : public CIntObject
{
public:

	boost::function<void(int)> onSelect;//called when OK button is pressed, returns id of selected item.
	std::string title,descr;//text for title and description

	CPicture *bg; //background
	CSlider *slider;
	CPicture *titleImage;//title image (castle gate\town portal picture)
	AdventureMapButton *ok, *exit;

	std::vector<Rect> areas;//areas for each visible item
	std::vector<int> items;//id of all items present in list
	int selected;//currently selected item
	int length;//size of list (=9)
	bool init;//true = initialization completed

	/// Callback will be called when OK button is pressed, returns id of selected item. initState = initially selected item
	CObjectListWindow(const std::vector<int> &_items, CPicture * titlePic, std::string _title, std::string _descr,
                      boost::function<void(int)> Callback, int initState=-1); //c-tor
	~CObjectListWindow(); //d-tor

	void elementSelected();//call callback and exit
	void moveList(int which);//called when slider moves
	void clickLeft(tribool down, bool previousState);  //call-in
	void keyPressed (const SDL_KeyboardEvent & key);  //call-in
	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to);
};

class CTradeWindow : public CIntObject //base for markets and altar of sacrifice
{
public:
	enum EType
	{
		RESOURCE, PLAYER, ARTIFACT, CREATURE, CREATURE_PLACEHOLDER,ARTIFACT_PLACEHOLDER
	};
	class CTradeableItem : public CIntObject
	{
	public:
		EType type;
		int id; 
		int serial;
		bool left;
		std::string subtitle; //empty if default

		CFunctionList<void()> callback;
		bool downSelection;

		void showAllAt(const Point &dstPos, const std::string &customSub, SDL_Surface * to);

		void clickRight(tribool down, bool previousState);
		void hover (bool on);
		void showAll(SDL_Surface * to);
		void clickLeft(tribool down, bool previousState);
		SDL_Surface *getSurface();
		std::string getName(int number = -1) const;
		CTradeableItem(EType Type, int ID, bool Left, int Serial);
	};

	const IMarket *market;
	const CGHeroInstance *hero;
	CPicture *bg; //background

	CArtifactsOfHero *arts;
	//all indexes: 1 = left, 0 = right
	std::vector<CTradeableItem*> items[2];
	CTradeableItem *hLeft, *hRight; //highlighted items (NULL if no highlight)
	EType itemsType[2];

	EMarketMode mode;//0 - res<->res; 1 - res<->plauer; 2 - buy artifact; 3 - sell artifact
	AdventureMapButton *ok, *max, *deal;
	CSlider *slider; //for choosing amount to be exchanged
	bool readyToTrade;

	CTradeWindow(const IMarket *Market, const CGHeroInstance *Hero, EMarketMode Mode); //c

	void showAll(SDL_Surface * to);

	void initSubs(bool Left);
	void initTypes();
	void initItems(bool Left);
	std::vector<int> *getItemsIds(bool Left); //NULL if default
	void getPositionsFor(std::vector<Rect> &poss, bool Left, EType type) const;
	void removeItems(const std::set<CTradeableItem *> &toRemove);
	void removeItem(CTradeableItem * t);
	void getEmptySlots(std::set<CTradeableItem *> &toRemove);
	void setMode(EMarketMode Mode); //mode setter

	virtual void getBaseForPositions(EType type, int &dx, int &dy, int &x, int &y, int &h, int &w, bool Right, int &leftToRightOffset) const = 0;
	virtual void selectionChanged(bool side) = 0; //true == left
	virtual Point selectionOffset(bool Left) const = 0;
	virtual std::string selectionSubtitle(bool Left) const = 0;
	virtual void garrisonChanged() = 0; 
	virtual void artifactsChanged(bool left) = 0;
};

class CMarketplaceWindow : public CTradeWindow
{
	bool printButtonFor(EMarketMode M) const;
public:
	int r1, r2; //suggested amounts of traded resources
	bool madeTransaction; //if player made at least one transaction
	CTextBox *traderText;

	void setMax();
	void sliderMoved(int to);
	void makeDeal();
	void selectionChanged(bool side); //true == left
	CMarketplaceWindow(const IMarket *Market, const CGHeroInstance *Hero = NULL, EMarketMode Mode = RESOURCE_RESOURCE); //c-tor
	~CMarketplaceWindow(); //d-tor

	Point selectionOffset(bool Left) const;
	std::string selectionSubtitle(bool Left) const;


	void garrisonChanged(); //removes creatures with count 0 from the list (apparently whole stack has been sold)
	void artifactsChanged(bool left);
	void resourceChanged(int type, int val);

	void getBaseForPositions(EType type, int &dx, int &dy, int &x, int &y, int &h, int &w, bool Right, int &leftToRightOffset) const;
	void updateTraderText();
};

class CAltarWindow : public CTradeWindow
{
public:
	CAltarWindow(const IMarket *Market, const CGHeroInstance *Hero, EMarketMode Mode); //c-tor

	void getExpValues();
	~CAltarWindow(); //d-tor

	std::vector<int> sacrificedUnits, //[slot_nr] -> how many creatures from that slot will be sacrificed
		expPerUnit;

	AdventureMapButton *sacrificeAll, *sacrificeBackpack;
	CLabel *expToLevel, *expOnAltar;


	void selectionChanged(bool side); //true == left
	void SacrificeAll();
	void SacrificeBackpack();

	void putOnAltar(int backpackIndex);
	bool putOnAltar(CTradeableItem* altarSlot, int artID);
	void makeDeal();
	void showAll(SDL_Surface * to);

	void blockTrade();
	void sliderMoved(int to);
	void getBaseForPositions(EType type, int &dx, int &dy, int &x, int &y, int &h, int &w, bool Right, int &leftToRightOffset) const;
	void mimicCres();

	Point selectionOffset(bool Left) const;
	std::string selectionSubtitle(bool Left) const;
	void garrisonChanged(); 
	void artifactsChanged(bool left);
	void calcTotalExp();
	void setExpToLevel();
	void updateRight(CTradeableItem *toUpdate);

	void artifactPicked();
	int firstFreeSlot();
	void moveFromSlotToAltar(int slotID, CTradeableItem* altarSlot, int artID);
};

class CSystemOptionsWindow : public CIntObject
{
private:
	SDL_Surface * background; //background of window
	AdventureMapButton *load, *save, *restart, *mainMenu, *quitGame, *backToMap; //load and restart are not used yet
	CHighlightableButtonsGroup * heroMoveSpeed;
	CHighlightableButtonsGroup * mapScrollSpeed;
	CHighlightableButtonsGroup * musicVolume, * effectsVolume;
public:
	CSystemOptionsWindow(const SDL_Rect & pos, CPlayerInterface * owner); //c-tor
	~CSystemOptionsWindow(); //d-tor

	//functions bound to buttons
	void bsavef(); //save game
	void bquitf(); //quit game
	void breturnf(); //return to game
	void bmainmenuf(); //return to main menu

	void pushSDLEvent(int type, int usercode);

	void activate();
	void deactivate();
	void show(SDL_Surface * to);
};

class CTavernWindow : public CIntObject
{
public:
	class HeroPortrait : public CIntObject
	{
	public:
		std::string hoverName;
		vstd::assigner<int,int> as;
		const CGHeroInstance *h;
		char descr[100];		// "XXX is a level Y ZZZ with N artifacts"

		void clickLeft(tribool down, bool previousState);
		void clickRight(tribool down, bool previousState);
		void hover (bool on);
		HeroPortrait(int &sel, int id, int x, int y, const CGHeroInstance *H);
		void show(SDL_Surface * to);

	} *h1, *h2; //recruitable heroes

	CPicture *bg; //background
	CGStatusBar *bar; //tavern's internal status bar
	int selected;//0 (left) or 1 (right)
	int oldSelected;//0 (left) or 1 (right)

	AdventureMapButton *thiefGuild, *cancel, *recruit;
	const CGObjectInstance *tavernObj;

	CTavernWindow(const CGObjectInstance *TavernObj); //c-tor
	~CTavernWindow(); //d-tor

	void recruitb();
	void close();
	void thievesguildb();
	void show(SDL_Surface * to);
};

class CInGameConsole : public CIntObject
{
private:
	std::list< std::pair< std::string, int > > texts; //<text to show, time of add>
	boost::mutex texts_mx;		// protects texts
	std::vector< std::string > previouslyEntered; //previously entered texts, for up/down arrows to work
	int prevEntDisp; //displayed entry from previouslyEntered - if none it's -1
	int defaultTimeout; //timeout for new texts (in ms)
	int maxDisplayedTexts; //hiw many texts can be displayed simultaneously
public:
	std::string enteredText;
	void activate();
	void deactivate();
	void show(SDL_Surface * to);
	void print(const std::string &txt);
	void keyPressed (const SDL_KeyboardEvent & key); //call-in

	void startEnteringText();
	void endEnteringText(bool printEnteredText);
	void refreshEnteredText();

	CInGameConsole(); //c-tor
};

class HoverableArea: public virtual CIntObject
{
public:
	std::string hoverText;

	virtual void hover (bool on);

	HoverableArea();
	virtual ~HoverableArea();
};

class LRClickableAreaWText: public HoverableArea
{
public:
	std::string text;

	LRClickableAreaWText();
	virtual ~LRClickableAreaWText();

	virtual void clickLeft(tribool down, bool previousState);
	virtual void clickRight(tribool down, bool previousState);
};

class LRClickableAreaWTextComp: public LRClickableAreaWText
{
public:
	int baseType;
	int bonusValue, type;
	virtual void clickLeft(tribool down, bool previousState);
};

class MoraleLuckBox : public LRClickableAreaWTextComp
{
public:
	bool morale; //true if morale, false if luck
	
	void set(const CBonusSystemNode *node);
	void showAll(SDL_Surface * to);

	MoraleLuckBox(bool Morale);
	~MoraleLuckBox();
};

class CHeroArea: public CIntObject
{
public:
	const CGHeroInstance * hero;
	
	CHeroArea(int x, int y, const CGHeroInstance * _hero);
	
	void clickLeft(tribool down, bool previousState);
	void clickRight(tribool down, bool previousState);
	void hover(bool on);
	void showAll(SDL_Surface * to);
};


class LRClickableAreaOpenTown: public LRClickableAreaWTextComp
{
public:
	const CGTownInstance * town;
	void clickLeft(tribool down, bool previousState);
	void clickRight(tribool down, bool previousState);
};

class CCreInfoWindow : public CIntObject
{
public:
	//bool active; //TODO: comment me
	int type;//0 - rclick popup; 1 - normal window
	CPicture *bitmap; //background
	char anf; //animation counter
	std::string count; //creature count in text format

	boost::function<void()> dsm; //dismiss button callback
	CCreaturePic *anim; //related creature's animation
	const CCreature *c; //related creature
	std::vector<SComponent*> upgResCost; //cost of upgrade (if not possible then empty)

	MoraleLuckBox *luck, *morale;

	AdventureMapButton *dismiss, *upgrade, *ok;
	CCreInfoWindow(const CStackInstance &st, int Type = 0, boost::function<void()> Upg = 0, boost::function<void()> Dsm = 0, UpgradeInfo *ui = NULL); //c-tor
	CCreInfoWindow(int Cid, int Type, int creatureCount); //c-tor
	void init(const CCreature *cre, const CStackInstance *stack, int creatureCount);
	void printLine(int nr, const std::string &text, int baseVal, int val=-1, bool range=false);
	~CCreInfoWindow(); //d-tor
	void activate();
	void close();
	void clickRight(tribool down, bool previousState); //call-in
	void dismissF();
	void keyPressed (const SDL_KeyboardEvent & key); //call-in
	void deactivate();
	void show(SDL_Surface * to);
};

class CArtPlace: public LRClickableAreaWTextComp
{
public:
	ui16 slotID; //0   	head	1 	shoulders		2 	neck		3 	right hand		4 	left hand		5 	torso		6 	right ring		7 	left ring		8 	feet		9 	misc. slot 1		10 	misc. slot 2		11 	misc. slot 3		12 	misc. slot 4		13 	ballista (war machine 1)		14 	ammo cart (war machine 2)		15 	first aid tent (war machine 3)		16 	catapult		17 	spell book		18 	misc. slot 5		19+ 	backpack slots

	bool marked;
	bool selectedNo;
	CArtifactsOfHero * ourOwner;
	const CArtifact * ourArt;
	CArtPlace(const CArtifact * Art); //c-tor
	void clickLeft(tribool down, bool previousState);
	void clickRight(tribool down, bool previousState);
	void select ();
	void deselect ();
	void activate();
	void deactivate();
	void showAll(SDL_Surface * to);
	bool fitsHere (const CArtifact * art) const; //returns true if given artifact can be placed here
	bool locked () const;
	void userSelectedNo ();
	~CArtPlace(); //d-tor
};

class CArtifactsOfHero : public CIntObject
{
	CGHeroInstance * curHero; //local copy of hero on which we operate

	std::vector<CArtPlace *> artWorn; // 0 - head; 1 - shoulders; 2 - neck; 3 - right hand; 4 - left hand; 5 - torso; 6 - right ring; 7 - left ring; 8 - feet; 9 - misc1; 10 - misc2; 11 - misc3; 12 - misc4; 13 - mach1; 14 - mach2; 15 - mach3; 16 - mach4; 17 - spellbook; 18 - misc5
	std::vector<CArtPlace *> backpack; //hero's visible backpack (only 5 elements!)
	int backpackPos; //number of first art visible in backpack (in hero's vector)

public:
	struct SCommonPart
	{
		std::set<CArtifactsOfHero *> participants; // Needed to mark slots.
		const CArtifact * srcArtifact;    // Held artifact.
		const CArtifactsOfHero * srcAOH;  // Following two needed to uniquely identify the source.
		int srcSlotID;                    //
		const CArtifactsOfHero * destAOH; // For swapping. (i.e. changing what is held)
		int destSlotID;	                  // Needed to determine what kind of action was last taken in setHero
		const CArtifact * destArtifact;   // For swapping.

		void reset();
	} * commonInfo; //when we have more than one CArtifactsOfHero in one window with exchange possibility, we use this (eg. in exchange window); to be provided externally

	bool updateState; // Whether the commonInfo should be updated on setHero or not.

	AdventureMapButton * leftArtRoll, * rightArtRoll;
	bool allowedAssembling;
	std::multiset<int> artifactsOnAltar; //artifacts id that are technically present in backpack but in GUI are moved to the altar - they'll be ommited in backpack slots

	void setHero(const CGHeroInstance * hero);
	void dispose(); //free resources not needed after closing windows and reset state
	void scrollBackpack(int dir); //dir==-1 => to left; dir==1 => to right

	void safeRedraw();
	void markPossibleSlots (const CArtifact* art);
	void unmarkSlots(bool withRedraw = true);
	void setSlotData (CArtPlace* artPlace, int slotID);
	void eraseSlotData (CArtPlace* artPlace, int slotID);

	CArtifactsOfHero(const Point& position); //c-tor
	~CArtifactsOfHero(); //d-tor
	void updateParentWindow();
	friend class CArtPlace;
};

class CGarrisonWindow : public CWindowWithGarrison
{
public:
	SDL_Surface *bg; //background surface
	AdventureMapButton *quit;

	void close();
	void activate();
	void deactivate();
	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to){show(to);};
	CGarrisonWindow(const CArmedInstance *up, const CGHeroInstance *down, bool removableUnits); //c-tor
	~CGarrisonWindow(); //d-tor
};

class CExchangeWindow : public CWindowWithGarrison
{
	CStatusBar * ourBar; //internal statusbar

	SDL_Surface *bg; //background
	AdventureMapButton * quit, * questlogButton[2];

	std::vector<LRClickableAreaWTextComp *> secSkillAreas[2], primSkillAreas;

	MoraleLuckBox *morale[2], *luck[2];

	LRClickableAreaWText *speciality[2];
	LRClickableAreaWText *experience[2];
	LRClickableAreaWText *spellPoints[2];
	CHeroArea *portrait[2];

public:

	const CGHeroInstance * heroInst[2];
	CArtifactsOfHero * artifs[2];

	void close();
	void activate();
	void deactivate();
	void show(SDL_Surface * to);

	void questlog(int whichHero); //questlog button callback; whichHero: 0 - left, 1 - right

	void prepareBackground(); //prepares or redraws bg

	CExchangeWindow(si32 hero1, si32 hero2); //c-tor
	~CExchangeWindow(); //d-tor
};

class CShipyardWindow : public CIntObject
{
public:
	CStatusBar *bar;
	SDL_Surface *bg; //background
	AdventureMapButton *build, *quit;

	unsigned char frame; //frame of the boat animation
	int boat; //which boat graphic should be used

	void activate();
	void deactivate();
	void show(SDL_Surface * to);
	CShipyardWindow(const std::vector<si32> &cost, int state, int boatType, const boost::function<void()> &onBuy);
	~CShipyardWindow();
};

class CPuzzleWindow : public CIntObject
{
private:
	SDL_Surface * background;
	AdventureMapButton * quitb;
	CResDataBar * resdatabar;

	std::vector<std::pair<SDL_Surface *, SPuzzleInfo *> > puzzlesToPullBack;
	ui8 animCount;

public:
	void activate();
	void deactivate();
	void show(SDL_Surface * to);

	CPuzzleWindow(const int3 &grailPos, float discoveredRatio);
	~CPuzzleWindow();
};

class CTransformerWindow : public CIntObject
{
public:
	class CItem : public CIntObject
	{
	public:
		int id;//position of creature in hero army
		bool left;//position of the item
		int size; //size of creature stack
		CTransformerWindow * parent;

		void move();
		void showAll(SDL_Surface * to);
		void clickLeft(tribool down, bool previousState);
		CItem(CTransformerWindow * _parent, int _size, int _id);
		~CItem();
	};

	const CArmedInstance *army;//object with army for transforming (hero or town)
	const CGHeroInstance *hero;//only if we have hero in town
	const CGTownInstance *town;//market, town garrison is used if hero == NULL
	CPicture *bg; //background
	std::vector<CItem*> items;

	AdventureMapButton *all, *convert, *cancel;
	CGStatusBar *bar;
	void showAll(SDL_Surface * to);
	void makeDeal();
	void addAll();
	CTransformerWindow(const CGHeroInstance * _hero, const CGTownInstance * _town); //c-tor
	~CTransformerWindow(); //d-tor
};

class CUniversityWindow : public CIntObject
{
	class CItem : public CPicture
	{
	public:
		int ID;//id of selected skill
		CUniversityWindow * parent;
		
		void showAll(SDL_Surface * to);
		void clickLeft(tribool down, bool previousState);
		void clickRight(tribool down, bool previousState);
		void hover(bool on);
		int state();//0=can't learn, 1=learned, 2=can learn
		CItem(CUniversityWindow * _parent, int _ID, int X, int Y);
	};

public:
	const CGHeroInstance *hero;
	const IMarket * market;
	
	CPicture * green, * yellow, * red;//colored bars near skills
	CPicture *bg; //background
	std::vector<CItem*> items;

	AdventureMapButton *cancel;
	CGStatusBar *bar;
	
	CUniversityWindow(const CGHeroInstance * _hero, const IMarket * _market); //c-tor
	~CUniversityWindow(); //d-tor
};


class CUnivConfirmWindow : public CIntObject//Confirmation window for University
{
public:
	CUniversityWindow * parent;
	CPicture * bg;
	CGStatusBar *bar;
	AdventureMapButton *confirm, *cancel;

	CUnivConfirmWindow(CUniversityWindow * PARENT, int SKILL, bool available); //c-tor
	void makeDeal(int skill);
};

class CHillFortWindow : public CWindowWithGarrison
{
public:

	int slotsCount;//=7;
	CGStatusBar * bar;
	CDefEssential *resources;
	CPicture *bg; //background surface
	CHeroArea *heroPic;//clickable hero image
	AdventureMapButton *quit,//closes window
	                   *upgradeAll,//upgrade all creatures
	                   *upgrade[7];//upgrade single creature

	const CGObjectInstance * fort;
	const CGHeroInstance * hero;
	std::vector<int> currState;//current state of slot - to avoid calls to getState or updating buttons
	std::vector<std::map<int,int> > costs;// costs [slot ID] [resource ID] = resource count for upgrade
	std::vector<int> totalSumm; // totalSum[resource ID] = value 

	CHillFortWindow(const CGHeroInstance *visitor, const CGObjectInstance *object); //c-tor
	~CHillFortWindow(); //d-tor
	
	void activate();
	void deactivate();
	void showAll (SDL_Surface *to);
	std::string getDefForSlot(int slot);//return def name for this slot
	std::string getTextForSlot(int slot);//return hover text for this slot
	void makeDeal(int slot);//-1 for upgrading all creatures
	int getState(int slot); //-1 = no creature 0=can't upgrade, 1=upgraded, 2=can upgrade
	void updateGarrisons();//update buttons after garrison changes
};

class CThievesGuildWindow : public CIntObject
{
	const CGObjectInstance * owner;

	CGStatusBar * statusBar;
	AdventureMapButton * exitb;
	SDL_Surface * background;
	CMinorResDataBar * resdatabar;

public:
	void activate();
	void show(SDL_Surface * to);

	void bexitf();

	CThievesGuildWindow(const CGObjectInstance * _owner);
	~CThievesGuildWindow();
};

#endif //__GUICLASSES_H__
