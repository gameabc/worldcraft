//
//  MyEventReceiver.h
//  Mine
//
//  Created by Luke on 10/9/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#ifndef Mine_MyEventReceiver_h
#define Mine_MyEventReceiver_h

#include <IEventReceiver.h>
#include "keycode.h"
#include "porting.h"
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace irr;
using namespace irr::core;
class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event);
    void step(float dt);
    
	bool IsKeyDown(const KeyPress &keyCode) const
	{
		return keyIsDown[keyCode];
	}
	
	// Checks whether a key was down and resets the state
	bool WasKeyDown(const KeyPress &keyCode)
	{
		bool b = keyWasDown[keyCode];
		if (b)
			keyWasDown.unset(keyCode);
		return b;
	}
    void setKeyDown(const KeyPress& keyCode)
    {
        keyIsDown.set(keyCode);
        keyWasDown.set(keyCode);
    }
    
//	s32 getMouseWheel()
//	{
//		s32 a = mouse_wheel;
//		mouse_wheel = 0;
//		return a;
//	}
    
	void clearInput();
	
 
	MyEventReceiver();
    void init();
//        
    irr::core::vector2di getMousePos()
    {
        return m_the_mouse_pos;
    }
    s32 getDx()
    {
        return m_dx;
    }
    s32 getDy()
    {
        return m_dy;
    }
    void disableDXY()
    {
        m_dx = m_dy = 0;
    }
    bool leftclicked;
	bool left_active;
	bool right_active;
    
    void setRightClickCheckTime(u32 newTime)
    {
        m_right_click_touch_max_time = newTime;
    }
    void setLeftClickCheckTime(u32 newTime)
    {
        m_lect_click_active_check_time = newTime;
    }
private:
    bool onMultiTouchEvent(const SEvent& event);
  
    bool m_enter_lect_active;
    /*
       在判断左右击事件时，如果发生move事件，只要移动距离小于这两个值就认为没有发生移动
     */
    int m_move_check_offsetX;
    int m_move_check_offsetY;
    
    //右击最多持续的事件
    u32 m_right_click_touch_max_time;
    //如果手指按住持续的时间大于这个值，进入left click 验证。显然，m_left_click_begin_check_time > m_right_click_touch_max_time
    //左击有效了！！ m_lect_click_active_check_time > m_left_click_begin_check_time
    u32 m_lect_click_active_check_time;
    
    u32 m_last_touch_time;
    
    s32 m_dx,m_dy;
    irr::core::vector2di m_pos;
    
    //不参与逻辑。只记录鼠标最后的位置
	irr::core::vector2di m_the_mouse_pos;
	// The current state of keys
	KeyList keyIsDown;
	// Whether a key has been pressed or not
	KeyList keyWasDown;
    
//    u32 m_press_down_time,m_press_move_time;
//    u32 m_move_time_cnt;
};
#define DOUBLE_CLICK_CHECK_TIME_MS 220
extern const char* SELECT_ITEM_SINGLE;
extern const char* SELECT_ITEM_ALL;
class gui::IGUIStaticText;
class ItemSelectChecker
{
public:
    bool m_is_move_one;
    core::stringw m_status_text;
    void setEnable(bool b)
    {
        m_running = b;
    }
    ItemSelectChecker();
private:
    friend class MyEventReceiver;
    u32 m_last_click_time;
    bool m_running;
    
};
extern ItemSelectChecker item_checker;
extern MyEventReceiver my_event_receiver;

#endif
