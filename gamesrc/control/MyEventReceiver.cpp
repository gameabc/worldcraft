//
//  MyEventReceiver.cpp
//  Mine
//
//  Created by Luke on 10/15/13.
//  Copyright (c) 2013 luke. All rights reserved.
//


#include "MyEventReceiver.h"
#include "hud.h"
#include "config.h"

#include "mainmenumanager.h"
#include "the_game.h"
#include <irrMap.h>
#include "settings.h"
 

using namespace irr::core;
extern Hud* g_hud;
extern Settings *g_settings;

extern bool text_input_on_show;
extern bool noMenuActive();
extern wchar_t* wgettext(const char *str);


ItemSelectChecker item_checker;
void MyEventReceiver::step(float dt)
{
}
void MyEventReceiver::clearInput()
{
    keyIsDown.clear();
    keyWasDown.clear();
    
    leftclicked = false;
    left_active = false;
    right_active = false;
}

MyEventReceiver::MyEventReceiver()
{
    m_enter_lect_active = false;
    m_dx = m_dy = 0;
    m_last_touch_time = -1;
   
    clearInput();
}
void MyEventReceiver::init()
{
    m_move_check_offsetX = the_screensize.Width/150;
    m_move_check_offsetY =  the_screensize.Height/120;
    
    //毫秒
    m_right_click_touch_max_time = g_settings->getS32("right_click_check_time");
    m_lect_click_active_check_time = g_settings->getS32("left_click_check_time");
}

bool MyEventReceiver::onMultiTouchEvent(const irr::SEvent &event)
{
    return false;
}
#include "touchscreengui.h"
extern TouchScreenGUI *touchscreengui;
bool MyEventReceiver::OnEvent(const SEvent& event)
{
    if (event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        keyIsDown.set(event.KeyInput);
        keyWasDown.set(event.KeyInput);
    }
    
    if(noMenuActive() == false && event.EventType == irr::EET_KEY_INPUT_EVENT)
    {
        return g_menumgr.preprocessEvent(event);
    }
    if (touchscreengui)
        touchscreengui->OnEvent(event);
    
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
    {
        m_the_mouse_pos.X = event.MouseInput.X;
        m_the_mouse_pos.Y = event.MouseInput.Y;
    }
    
    return false;
}
