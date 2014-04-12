//
//  the_game.cpp
//  Mine
//
//  Created by Luke on 10/10/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#include "the_game.h"
#include <assert.h>
#include "main.h"
 
#include "settings.h"
#include "MyEventReceiver.h"
#include "guiMainMenu.h"
#include "guiEngine.h"
Game* g_the_game = NULL;
MyEventReceiver my_event_receiver;

Game::Game():m_abort(false),m_frame_count(0),m_update_game(false)
{
    g_the_game = this;
    m_curr_time = 0;
    m_prev_time = 0;
}
Game::~Game()
{
}
scene::ISceneManager * the_smgr = NULL;
video::IVideoDriver  * the_videoDriver = NULL;
IrrlichtDevice       * the_device = NULL;
core::dimension2d<u32>   the_screensize;
void Game::shareInit(IrrlichtDevice* m_device)
{
    the_device              = m_device;
    the_smgr = m_device->getSceneManager();
    the_videoDriver  = m_device->getVideoDriver();
    the_screensize = the_videoDriver->getScreenSize();
    the_device->setEventReceiver(&my_event_receiver);
}
float Game::getLimitedDt()
{
    IrrlichtDevice* device = the_device;
    m_prev_time = m_curr_time;
    float dt;
    while( 1 )
    {
        m_curr_time = device->getTimer()->getRealTime();
        dt = (float)(m_curr_time - m_prev_time);
        static const float max_elapsed_time = 3.0f*1.0f/60.0f*1000.0f;
        if(dt > max_elapsed_time) dt=max_elapsed_time;
        const int max_fps = 60;
        const int current_fps = (int)(1000.0f/dt);
        if( current_fps > max_fps )
        {
            int wait_time = 1000/max_fps - 1000/current_fps;
            if(wait_time < 1) wait_time = 1;
            
            device->sleep(wait_time);
        }
        else break;
    }
    dt *= 0.001f;
    return dt;
}


void Game::initGame(IrrlichtDevice* device)
{
    mDevice = device;
    shareInit(device);
    main_init_on_login(device);
}
void Game::quitGame()
{
    main_cleanupGame();
}
extern std::string g_settings_configpath;
float current_volume = 1.0;
bool isOnPause = false;
extern GUIMainMenu *menu;
extern void setConfig2g_settings();
void Game::pauseGame()
{
}
void Game::resumeGame()
{
}
void Game::updateGame()
{
    m_prev_time = m_curr_time;
    float dt   = getLimitedDt();
    
    if (!m_abort)
    {
        if(!m_abort)
        {
            if (false == m_update_game)
            {
                main_draw_main_menu(dt,the_device);
                return;
            }
            else
            {
                main_updateGame(the_device);
            }
        }

    }
}
