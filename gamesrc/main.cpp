/*
Minetest
Copyright (C) 2010-2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifdef NDEBUG
	/*#ifdef _WIN32
		#pragma message ("Disabling unit tests")
	#else
		#warning "Disabling unit tests"
	#endif*/
	// Disable unit tests
	#define ENABLE_TESTS 0
#else
	// Enable unit tests
	#define ENABLE_TESTS 1
#endif

#ifdef _MSC_VER
#ifndef SERVER // Dedicated server isn't linked with Irrlicht
	#pragma comment(lib, "Irrlicht.lib")
	// This would get rid of the console window
	//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif
	#pragma comment(lib, "zlibwapi.lib")
	#pragma comment(lib, "Shell32.lib")
#endif

#include "irrlicht.h" // createDevice

#include "main.h"
#include "mainmenumanager.h"
#include <iostream>
#include <fstream>
#include <locale.h>
#include "irrlichttypes_extrabloated.h"
#include "debug.h"
#include "test.h"
#include "clouds.h"
#include "server.h"
#include "constants.h"
#include "porting.h"
#include "gettime.h"
//#include "guiMessageMenu.h"
#include "filesys.h"
#include "config.h"
#include "version.h"
#include "guiMainMenu.h"
#include "game.h"
#include "keycode.h"
#include "tile.h"
#include "chat.h"
#include "defaultsettings.h"
#include "gettext.h"
#include "settings.h"
#include "profiler.h"
#include "log.h"
#include "mods.h"

#include "util/string.h"
#include "subgame.h"
#include "quicktune.h"
#include "serverlist.h"
#include "guiEngine.h"
#include "mapsector.h"

#include "database-sqlite3.h"
#ifdef USE_LEVELDB
#include "database-leveldb.h"
#endif

#include "MyEventReceiver.h"

#include "the_game.h"


 
#include <IGUISkin.h>

using namespace irr::gui;
using namespace irr;


using namespace irr;
using namespace irr::gui;

/*
	Settings.
	These are loaded from the config file.
*/
Settings main_settings;
Settings *g_settings = &main_settings;
std::string g_settings_path;

// Global profiler
Profiler main_profiler;
Profiler *g_profiler = &main_profiler;

// Menu clouds are created later
Clouds *g_menuclouds = 0;
irr::scene::ISceneManager *g_menucloudsmgr = 0;

/*
	Debug streams
*/

// Connection
std::ostream *dout_con_ptr = &dummyout;
std::ostream *derr_con_ptr = &verbosestream;

// Server
std::ostream *dout_server_ptr = &infostream;
std::ostream *derr_server_ptr = &errorstream;

// Client
std::ostream *dout_client_ptr = &infostream;
std::ostream *derr_client_ptr = &errorstream;

#ifndef SERVER
/*
	Random stuff
*/

/* mainmenumanager.h */

gui::IGUIEnvironment* guienv = NULL;
gui::IGUIStaticText *guiroot = NULL;
MainMenuManager g_menumgr;

bool noMenuActive()
{
	return (g_menumgr.menuCount() == 0);
}

// Passed to menus to allow disconnecting and exiting
MainGameCallback *g_gamecallback = NULL;
#endif

/*
	gettime.h implementation
*/

#ifdef SERVER

u32 getTimeMs()
{
	/* Use imprecise system calls directly (from porting.h) */
	return porting::getTime(PRECISION_MILLI);
}

u32 getTime(TimePrecision prec)
{
	return porting::getTime(prec);
}

#else

// A small helper class
class TimeGetter
{
public:
	virtual u32 getTime(TimePrecision prec) = 0;
};

// A precise irrlicht one
class IrrlichtTimeGetter: public TimeGetter
{
public:
	IrrlichtTimeGetter(IrrlichtDevice *device):
		m_device(device)
	{}
	u32 getTime(TimePrecision prec)
	{
		if (prec == PRECISION_MILLI) {
			if(m_device == NULL)
				return 0;
			return m_device->getTimer()->getRealTime();
		} else {
			return porting::getTime(prec);
		}
	}
private:
	IrrlichtDevice *m_device;
};
// Not so precise one which works without irrlicht
class SimpleTimeGetter: public TimeGetter
{
public:
	u32 getTime(TimePrecision prec)
	{
		return porting::getTime(prec);
	}
};

// A pointer to a global instance of the time getter
// TODO: why?
TimeGetter *g_timegetter = NULL;

u32 getTimeMs()
{
	if(g_timegetter == NULL)
		return 0;
	return g_timegetter->getTime(PRECISION_MILLI);
}

u32 getTime(TimePrecision prec) {
	if (g_timegetter == NULL)
		return 0;
	return g_timegetter->getTime(prec);
}
#endif

class StderrLogOutput: public ILogOutput
{
public:
	/* line: Full line with timestamp, level and thread */
	void printLog(const std::string &line)
	{
		std::cerr<<line<<std::endl;
	}
} main_stderr_log_out;

class DstreamNoStderrLogOutput: public ILogOutput
{
public:
	/* line: Full line with timestamp, level and thread */
	void printLog(const std::string &line)
	{
		dstream_no_stderr<<line<<std::endl;
	}
} main_dstream_no_stderr_log_out;

#ifndef SERVER


/*
	Separated input handler
*/

//class RealInputHandler : public InputHandler
//{
//public:
//	RealInputHandler(IrrlichtDevice *device, MyEventReceiver *receiver):
//    m_device(device),
//    m_receiver(receiver)
//	{
//	}
//	virtual bool isKeyDown(const KeyPress &keyCode)
//	{
//		return m_receiver->IsKeyDown(keyCode);
//	}
//	virtual bool wasKeyDown(const KeyPress &keyCode)
//	{
//		return m_receiver->WasKeyDown(keyCode);
//	}
//	virtual v2s32 getMousePos()
//	{
//		return  my_event_receiver.getMousePos();
//	}
//	virtual void setMousePos(s32 x, s32 y)
//	{
//		;//m_device->getCursorControl()->setPosition(x, y);
//	}
//    
//	virtual bool getLeftState()
//	{
//		return m_receiver->left_active;
//	}
//	virtual bool getRightState()
//	{
//		return m_receiver->right_active;
//	}
//	
//    //	virtual bool getLeftClicked()
//    //	{
//    //		return m_receiver->leftclicked;
//    //	}
//    //	virtual bool getRightClicked()
//    //	{
//    //		return m_receiver->rightclicked;
//    //	}
//    //	virtual void resetLeftClicked()
//    //	{
//    //		m_receiver->leftclicked = false;
//    //	}
//    //	virtual void resetRightClicked()
//    //	{
//    //		m_receiver->rightclicked = false;
//    //	}
//    //
//    //	virtual bool getLeftReleased()
//    //	{
//    //		return m_receiver->leftreleased;
//    //	}
//    //	virtual bool getRightReleased()
//    //	{
//    //		return m_receiver->rightreleased;
//    //	}
//    //	virtual void resetLeftReleased()
//    //	{
//    //		m_receiver->leftreleased = false;
//    //	}
//    //	virtual void resetRightReleased()
//    //	{
//    //		m_receiver->rightreleased = false;
//    //	}
//    
//    virtual void resetRightState()
//    {
//        m_receiver->right_active = false;
//    }
//    //	virtual s32 getMouseWheel()
//    //	{
//    //		return m_receiver->getMouseWheel();
//    //	}
//    
//	void clear()
//	{
//		m_receiver->clearInput();
//	}
//    virtual void step(float dt)
//    {
//        m_receiver->step(dt);
//    }
//private:
//	IrrlichtDevice *m_device;
//	MyEventReceiver *m_receiver;
//};

//class RandomInputHandler : public InputHandler
//{
//public:
//	RandomInputHandler()
//	{
//		leftdown = false;
//		rightdown = false;
//		leftclicked = false;
//		rightclicked = false;
//		leftreleased = false;
//		rightreleased = false;
//		keydown.clear();
//	}
//	virtual bool isKeyDown(const KeyPress &keyCode)
//	{
//		return keydown[keyCode];
//	}
//	virtual bool wasKeyDown(const KeyPress &keyCode)
//	{
//		return false;
//	}
//	virtual v2s32 getMousePos()
//	{
//		return mousepos;
//	}
//	virtual void setMousePos(s32 x, s32 y)
//	{
//		mousepos = v2s32(x,y);
//	}
//
//	virtual bool getLeftState()
//	{
//		return leftdown;
//	}
//	virtual bool getRightState()
//	{
//		return rightdown;
//	}
//
//	virtual bool getLeftClicked()
//	{
//		return leftclicked;
//	}
//	virtual bool getRightClicked()
//	{
//		return rightclicked;
//	}
//	virtual void resetLeftClicked()
//	{
//		leftclicked = false;
//	}
//	virtual void resetRightClicked()
//	{
//		rightclicked = false;
//	}
//
//	virtual bool getLeftReleased()
//	{
//		return leftreleased;
//	}
//	virtual bool getRightReleased()
//	{
//		return rightreleased;
//	}
//	virtual void resetLeftReleased()
//	{
//		leftreleased = false;
//	}
//	virtual void resetRightReleased()
//	{
//		rightreleased = false;
//	}
//
//	virtual s32 getMouseWheel()
//	{
//		return 0;
//	}
//
//	virtual void step(float dtime)
//	{
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 40);
//				keydown.toggle(getKeySetting("keymap_jump"));
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 40);
//				keydown.toggle(getKeySetting("keymap_special1"));
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 40);
//				keydown.toggle(getKeySetting("keymap_forward"));
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 40);
//				keydown.toggle(getKeySetting("keymap_left"));
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 20);
//				mousespeed = v2s32(Rand(-20,20), Rand(-15,20));
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 30);
//				leftdown = !leftdown;
//				if(leftdown)
//					leftclicked = true;
//				if(!leftdown)
//					leftreleased = true;
//			}
//		}
//		{
//			static float counter1 = 0;
//			counter1 -= dtime;
//			if(counter1 < 0.0)
//			{
//				counter1 = 0.1*Rand(1, 15);
//				rightdown = !rightdown;
//				if(rightdown)
//					rightclicked = true;
//				if(!rightdown)
//					rightreleased = true;
//			}
//		}
//		mousepos += mousespeed;
//	}
//
//	s32 Rand(s32 min, s32 max)
//	{
//		return (myrand()%(max-min+1))+min;
//	}
//private:
//	KeyList keydown;
//	v2s32 mousepos;
//	v2s32 mousespeed;
//	bool leftdown;
//	bool rightdown;
//	bool leftclicked;
//	bool rightclicked;
//	bool leftreleased;
//	bool rightreleased;
//};
//
#endif // !SERVER

// These are defined global so that they're not optimized too much.
// Can't change them to volatile.
s16 temp16;
f32 tempf;
v3f tempv3f1;
v3f tempv3f2;
std::string tempstring;
std::string tempstring2;

void SpeedTests()
{
	{
		infostream<<"The following test should take around 20ms."<<std::endl;
		TimeTaker timer("Testing std::string speed");
		const u32 jj = 10000;
		for(u32 j=0; j<jj; j++)
		{
			tempstring = "";
			tempstring2 = "";
			const u32 ii = 10;
			for(u32 i=0; i<ii; i++){
				tempstring2 += "asd";
			}
			for(u32 i=0; i<ii+1; i++){
				tempstring += "asd";
				if(tempstring == tempstring2)
					break;
			}
		}
	}
	
	infostream<<"All of the following tests should take around 100ms each."
			<<std::endl;

	{
		TimeTaker timer("Testing floating-point conversion speed");
		tempf = 0.001;
		for(u32 i=0; i<4000000; i++){
			temp16 += tempf;
			tempf += 0.001;
		}
	}
	
	{
		TimeTaker timer("Testing floating-point vector speed");

		tempv3f1 = v3f(1,2,3);
		tempv3f2 = v3f(4,5,6);
		for(u32 i=0; i<10000000; i++){
			tempf += tempv3f1.dotProduct(tempv3f2);
			tempv3f2 += v3f(7,8,9);
		}
	}

	{
		TimeTaker timer("Testing std::map speed");
		
		std::map<v2s16, f32> map1;
		tempf = -324;
		const s16 ii=300;
		for(s16 y=0; y<ii; y++){
			for(s16 x=0; x<ii; x++){
				map1[v2s16(x,y)] =  tempf;
				tempf += 1;
			}
		}
		for(s16 y=ii-1; y>=0; y--){
			for(s16 x=0; x<ii; x++){
				tempf = map1[v2s16(x,y)];
			}
		}
	}

	{
		infostream<<"Around 5000/ms should do well here."<<std::endl;
		TimeTaker timer("Testing mutex speed");
		
		JMutex m;
		m.Init();
		u32 n = 0;
		u32 i = 0;
		do{
			n += 10000;
			for(; i<n; i++){
				m.Lock();
				m.Unlock();
			}
		}
		// Do at least 10ms
		while(timer.getTimerTime() < 10);

		u32 dtime = timer.stop();
		u32 per_ms = n / dtime;
		infostream<<"Done. "<<dtime<<"ms, "
				<<per_ms<<"/ms"<<std::endl;
	}
}

static void print_worldspecs(const std::vector<WorldSpec> &worldspecs,
		std::ostream &os)
{
	for(u32 i=0; i<worldspecs.size(); i++){
		std::string name = worldspecs[i].name;
		std::string path = worldspecs[i].path;
		if(name.find(" ") != std::string::npos)
			name = std::string("'") + name + "'";
		path = std::string("'") + path + "'";
		name = padStringRight(name, 14);
		os<<"  "<<name<<" "<<path<<std::endl;
	}
}


std::vector<WorldSpec> worldspecs;
MenuMusicFetcher main_soundfetcher;
ISoundManager *main_sound = NULL;
s32 handle = -1;

SubgameSpec gamespec;
WorldSpec worldspec;
bool simple_singleplayer_mode = false;

// These are set up based on the menu and other things
std::string current_playername = "inv£lid";
std::string current_password = "";
std::string current_address = "does-not-exist";
std::string address = "127.0.0.1";
std::string playername = "";
u16 port = 30000;
int current_port = 0;
std::string commanded_world = "";
std::string commanded_worldname = "";
bool skip_main_menu = false;
std::string password;
SubgameSpec commanded_gamespec;
MainMenuData menudata;
//MenuTextures menutextures;
SubgameSpec *menugame = NULL;
GUIMainMenu* menu = NULL;

u32 main_menu_lasttime;
float fps_max;
std::wstring error_message = L"";

extern IrrlichtDevice* the_device;
video::IVideoDriver* the_driver = NULL;
extern scene::ISceneManager* the_smgr;
InputHandler *input = NULL;
std::string g_settings_configpath = "";
int g_the_font_height = 25;
irr::gui::IGUIFont*font = NULL;
extern bool g_is_show_chat_ui;
extern void showIngameChatUi(bool isShow);



enum GameType {
    GY_Singleplayer,
    GY_Multiplayer_client_only,
};
bool game_type_on_init_has_error(GameType type);

void setConfig2g_settings()
{
    //SKLOG("setConfig2g_settings");
    // Save settings
//    g_settings->setS32("selected_mainmenu_tab", menudata.selected_tab);
//    g_settings->setS32("selected_serverlist", menudata.selected_serverlist);
//    g_settings->set("selected_mainmenu_game", menudata.selected_game);
//    g_settings->set("new_style_leaves", itos(menudata.fancy_trees));
//    g_settings->set("smooth_lighting", itos(menudata.smooth_lighting));
//    g_settings->set("enable_3d_clouds", itos(menudata.clouds_3d));
//    g_settings->set("opaque_water", itos(menudata.opaque_water));
//    
//    g_settings->set("mip_map", itos(menudata.mip_map));
//    g_settings->set("anisotropic_filter", itos(menudata.anisotropic_filter));
//    g_settings->set("bilinear_filter", itos(menudata.bilinear_filter));
//    g_settings->set("trilinear_filter", itos(menudata.trilinear_filter));
//    // enable_shaders不支持设定
//    //    g_settings->setS32("enable_shaders", menudata.enable_shaders);
//    g_settings->set("using_hd_texure", itos(menudata.using_hd_texure));
//    //    g_settings->set("preload_item_visuals", itos(menudata.preload_item_visuals));
//    g_settings->set("preload_item_visuals", "true");
//    g_settings->set("enable_particles", itos(menudata.enable_particles));
//    g_settings->set("liquid_finite", itos(menudata.liquid_finite));
//    
//    g_settings->set("creative_mode", itos(menudata.creative_mode));
//    g_settings->set("enable_damage", itos(menudata.enable_damage));
//    //不会广播服务器信息到minetest.net
//    //    g_settings->set("server_announce", itos(menudata.enable_public));
//    g_settings->set("name", playername);
//    g_settings->set("address", address);
//    g_settings->set("port", itos(port));
//    if(menudata.selected_world != -1)
//        g_settings->set("selected_world_path",
//                        worldspecs[menudata.selected_world].path);
}
std::string the_random_name = "";
void save_player_name_if_needed()
{
    if (menudata.simple_singleplayer_mode == true)
    {
        menudata.store_player_name = menudata.name;
        menudata.name = "Admin";
    }
}
void restore_player_name_if_needed()
{
    if (menudata.simple_singleplayer_mode == true)
    {
        menudata.name = menudata.store_player_name;
        g_settings->set("name", menudata.name);
    }
}
bool main_main_menu_on_quit_has_error()
{
    the_smgr->clear();
    if(menudata.errormessage != ""){
        error_message = narrow_to_wide(menudata.errormessage);
        return true;
    }
    save_player_name_if_needed();
    
    //update worldspecs (necessary as new world may have been created)
    worldspecs = getAvailableWorlds();
    
    
    if (menudata.name == "")
    {
        if (the_random_name == "") {
            the_random_name = std::string("Guest") + itos(myrand_range(1000,9999));
        }
        menudata.name = the_random_name;
    }
    
    playername = menudata.name;
    
    password = translatePassword(playername, narrow_to_wide(menudata.password));
    //infostream<<"Main: password hash: '"<<password<<"'"<<std::endl;
    
    address = menudata.address;
    int newport = stoi(menudata.port);
    if(newport != 0)
        port = newport;
    
    simple_singleplayer_mode = false;// menudata.simple_singleplayer_mode;
    if (menudata.simple_singleplayer_mode == true)
    {
        //从singleplayer的tab过来的
        int newport = stoi(menudata.local_server_port);
        if(newport != 0)
            port = newport;
    }
    
    // Save settings
    g_settings->set("name", playername);
    g_settings->set("address", address);
    g_settings->set("port", itos(port));
    
    if((menudata.selected_world >= 0) &&
       (menudata.selected_world < (int)worldspecs.size()))
        g_settings->set("selected_world_path",
                        worldspecs[menudata.selected_world].path);
    
//    // Break out of menu-game loop to shut down cleanly
//    if(device->run() == false || kill == true)
//        break;
    
    current_playername = playername;
    current_password = password;
    current_address = address;
    current_port = port;
    
    
    // If using simple singleplayer mode, override
    if(simple_singleplayer_mode){
        current_playername = "singleplayer";
        current_password = "";
        current_address = "";
        current_port = myrand_range(49152, 65535);
    }
    else if (address != "")
    {
        ServerListSpec server;
        server["name"] = menudata.servername;
        server["address"] = menudata.address;
        server["port"] = menudata.port;
        server["description"] = menudata.serverdescription;
        ServerList::insert(server);
    }
    
    // Set world path to selected one
    if ((menudata.selected_world >= 0) &&
        (menudata.selected_world < (int)worldspecs.size())) {
        worldspec = worldspecs[menudata.selected_world];
        infostream<<"Selected world: "<<worldspec.name
        <<" ["<<worldspec.path<<"]"<<std::endl;
    }
    
    // If local game
    if(current_address == "")
    {
        if(menudata.selected_world == -1){
            error_message = narrow_to_wide("No world selected and no address "
                                     "provided. Nothing to do.");
            errorstream<<wide_to_narrow(error_message)<<std::endl;
            return true;
        }
        // Load gamespec for required game
        gamespec = findWorldSubgame(worldspec.path);
        if(!gamespec.isValid() && !commanded_gamespec.isValid()){
            error_message = narrow_to_wide("Could not find or load game \"")
            + narrow_to_wide(worldspec.gameid) + L"\"";
            errorstream<< wide_to_narrow(error_message)<<std::endl;
            return true;
        }
        if(commanded_gamespec.isValid() &&
           commanded_gamespec.id != worldspec.gameid){
            errorstream<<"WARNING: Overriding gamespec from \""
            <<worldspec.gameid<<"\" to \""
            <<commanded_gamespec.id<<"\""<<std::endl;
            gamespec = commanded_gamespec;
        }
        
        if(!gamespec.isValid()){
            error_message = narrow_to_wide("Invalid gamespec.");
            error_message += L" (world_gameid="
            +narrow_to_wide(worldspec.gameid)+L")";
            errorstream<<wide_to_narrow(error_message)<<std::endl;
            return true;
        }
    }
    
    if (false == simple_singleplayer_mode)
    {
        if ((current_address == "127.0.0.1" || current_address == "localhost"))
        {
            error_message = L"can't connect to server(127.0.0.1) in multiplayer mode!";
            return true;
        }
    }
    return false;
}




void _init_game(IrrlichtDevice* device);

void main_initGame(IrrlichtDevice* device)
{
    _init_game(device);
}

#include "touchscreengui.h"
extern TouchScreenGUI *touchscreengui;


//第一次初始化执行
void main_init_on_login(IrrlichtDevice* device)
{
    guienv = device->getGUIEnvironment();
    guiroot = guienv->addStaticText(L"",
                                    core::rect<s32>(0, 0, 10000, 10000));
    
    the_device = device;
    the_smgr = device->getSceneManager();
    the_driver = device->getVideoDriver();
    
	porting::signal_handler_init();
	porting::initializePaths();
    
	// Create user data directory
	fs::CreateDir(porting::path_user);

    // Initialize default settings
	set_default_settings(g_settings);
	
	// Initialize sockets
	sockets_init();
	atexit(sockets_cleanup);
    
    std::vector<std::string> filenames;
    filenames.push_back(porting::path_user +
                        DIR_DELIM + "settings.conf");
#if RUN_IN_PLACE
    // Try also from a lower level (to aid having the same configuration
    // for many RUN_IN_PLACE installs)
    filenames.push_back(porting::path_user +
                        DIR_DELIM + ".." + DIR_DELIM + ".." + DIR_DELIM + "settings.conf");
#endif
    
    for(u32 i=0; i<filenames.size(); i++)
    {
        bool r = g_settings->readConfigFile(filenames[i].c_str());
        if(r)
        {
            g_settings_configpath = filenames[i];
            break;
        }
    }
    
    // If no path found, use the first one (menu creates the file)
    if(g_settings_configpath == "")
        g_settings_configpath = filenames[0];
    
    // Initialize random seed
	srand(time(0));
	mysrand(time(0));
    
    my_event_receiver.init();
    g_timegetter = new IrrlichtTimeGetter(device);
	
	// Create game callback for menus
	g_gamecallback = new MainGameCallback(device);
    
    bool random_input = false;//g_settings->getBool("random_input");
    
//	if(random_input)
//		input = new RandomInputHandler();
//	else
    {
        touchscreengui = new TouchScreenGUI(device);
        input = touchscreengui;
    }

    
	scene::ISceneManager* smgr = device->getSceneManager();
    //这里保存下原有的skin。从游戏内返回的时候需要重新加载
    IGUISkin* skin = guienv->getSkin();
    //设定一下一些控件的大小
    skin->setSize(EGDS_SCROLLBAR_SIZE, device->getVideoDriver()->getScreenSize().Width * 0.06);
    skin->setSize(EGDS_CHECK_BOX_WIDTH, device->getVideoDriver()->getScreenSize().Width * 0.03);
    skin->setSize(EGDS_BUTTON_WIDTH, device->getVideoDriver()->getScreenSize().Width * 0.1);
    skin->setSize(EGDS_BUTTON_HEIGHT, device->getVideoDriver()->getScreenSize().Width * 0.03);
    skin->setSize(EGDS_WINDOW_BUTTON_WIDTH, device->getVideoDriver()->getScreenSize().Width * 0.05);
    font = skin->getFont();
	if(font)
		skin->setFont(font);
	else
		errorstream<<"WARNING: Font file was not found."
        " Using default font."<<std::endl;
	// If font was not found, this will get us one
    //	font = skin->getFont();
	assert(font);
    
	//skin->setColor(gui::EGDC_BUTTON_TEXT, video::SColor(255,0,0,0));
	skin->setColor(gui::EGDC_BUTTON_TEXT, video::SColor(255,255,255,255));
	//skin->setColor(gui::EGDC_3D_HIGH_LIGHT, video::SColor(0,0,0,0));
	//skin->setColor(gui::EGDC_3D_SHADOW, video::SColor(0,0,0,0));
    //	skin->setColor(gui::EGDC_3D_HIGH_LIGHT, video::SColor(255,0,0,0));
    skin->setColor(gui::EGDC_3D_HIGH_LIGHT, video::SColor(255,52,55,64));
	skin->setColor(gui::EGDC_3D_SHADOW, video::SColor(255,0,0,0));
    
    //	skin->setColor(gui::EGDC_HIGH_LIGHT, video::SColor(255,70,100,50));
    skin->setColor(gui::EGDC_HIGH_LIGHT, video::SColor(255,  161 , 164 , 179));
	skin->setColor(gui::EGDC_HIGH_LIGHT_TEXT, video::SColor(255,255,255,255));
    
#if (IRRLICHT_VERSION_MAJOR >= 1 && IRRLICHT_VERSION_MINOR >= 8) || IRRLICHT_VERSION_MAJOR >= 2
	// Irrlicht 1.8 input colours
	skin->setColor(gui::EGDC_EDITABLE, video::SColor(255,128,128,128));
	skin->setColor(gui::EGDC_FOCUSED_EDITABLE, video::SColor(255,96,134,49));
#endif

    
    if (!g_menucloudsmgr)
		g_menucloudsmgr = smgr->createNewSceneManager();
	if (!g_menuclouds)
		g_menuclouds = new Clouds(g_menucloudsmgr->getRootSceneNode(),
                                  g_menucloudsmgr, -1, rand(), 100);
	g_menuclouds->update(v2f(0, 0), video::SColor(255,200,200,255));
	scene::ICameraSceneNode* camera;
	camera = g_menucloudsmgr->addCameraSceneNode(0,
                                                 v3f(0,0,0), v3f(0, 60, 100));
	camera->setFarValue(10000);
    
    
    guienv->clear();
    guiroot = guienv->addStaticText(L"",
                                   core::rect<s32>(0, 0, 10000, 10000));
}
ChatBackend chat_backend;



#if 0
#prama mark main_menu_loop
#endif


void main_draw_main_menu(float dt,IrrlichtDevice* device)
{
    if (false == device->run())
    {
        return;
    }
    if (menu != NULL) {
        if (menu->m_startgame == false)
        {
            menu->run();
        }
        else
        {
//            // menu quit
//            if (false == main_main_menu_on_quit_has_error())
//            {
//                delete menu;
//                menu = NULL;
//                g_the_game->setUpdateGame(true);
//                main_initGame(device);
//            }
//            else
//            {
//                //有错误
//                menu->m_startgame = false;
//                menu->getScriptIface()->setMainMenuErrorMessage(wide_to_narrow(error_message));
//                error_message = L"";
//                menudata.errormessage = "";
//            }
            
            // menu quit
            delete menu;
            menu = NULL;
            if (false == main_main_menu_on_quit_has_error())
            {
                error_message = L"";
                menudata.errormessage = "";
                g_the_game->setUpdateGame(true);
                main_initGame(device);
            }
            else
            {
                //有错误
//                menu->m_startgame = false;
//                menu->getScriptIface()->setMainMenuErrorMessage(wide_to_narrow(error_message));
//                error_message = L"";
//                menudata.errormessage = "";
            }
        }
        return;
    }
    else
    {
        if (noMenuActive())
        {
            // FIXME
            if (error_message != L"")
            {
                menudata.errormessage = wide_to_narrow(error_message);
            }
            menu = new GUIMainMenu(the_device,guiroot,&g_menumgr,the_smgr,&menudata);
        }
        else
        {
            the_driver->beginScene(true, true,
                                   video::SColor(255,128,128,128));
            guienv->drawAll();
            the_driver->endScene();
            sleep_ms(25);
        }
    }
}
void main_updateGame(IrrlichtDevice* device)
{
    the_game_loop(device);
}
void main_cleanupGame()
{
    if (g_settings_configpath != "")
    {
        g_settings->updateConfigFile(g_settings_configpath.c_str());
    }
    //TODO: 原来的main函数还有一些需要调用
    the_game_end(the_device);
}

void _init_game(IrrlichtDevice* device)
{
    
    the_device = device;
    bool kill = false;
    bool random_input = false;
//    the_game(
//             kill,
//             random_input,
//             input,
//             device,
//             font,
//             worldspec.path,
//             current_playername,
//             current_password,
//             current_address,
//             current_port,
//             error_message,
//             g_settings_configpath,
//             chat_backend,
//             gamespec,
//             simple_singleplayer_mode
//             );
    
     the_game(kill,
        random_input,
        input,
        device,
        font,
        worldspec.path,
        current_playername,
        current_password,
        current_address,
        current_port,
        error_message,
        chat_backend,
        gamespec, // Used for local game
        simple_singleplayer_mode
        );
}
std::string getGUIDir()
{
    return porting::path_share + "gui/";
}
irr::video::ITexture* getTexture(std::string s)
{
    return the_videoDriver->getTexture(s.c_str());
}


