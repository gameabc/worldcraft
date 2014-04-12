/*
 Part of Minetest
 Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
 Copyright (C) 2013 Ciaran Gultnieks <ciaran@ciarang.com>
 Copyright (C) 2013 RealBadAngel, Maciej Kasatkin <mk@realbadangel.pl>
 
 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "guiSettings.h"
#include "debug.h"
#include "serialization.h"
#include <string>
#include <IGUICheckBox.h>
#include <IGUIButton.h>
#include <IGUIScrollBar.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
#include "main.h"

#include "gettext.h"
#include "MyEventReceiver.h"
#include "settings.h"
 

const int ID_soundText1 = 100;
const int ID_soundText2 = 101;
const int ID_soundSlider = 102;

const int ID_lookaroundText1 = 103;
const int ID_lookaroundText2 = 104;
const int ID_lookaroundSlider = 105;

const int ID_leftClickText1 = 106;
const int ID_leftClickText2 = 107;
const int ID_leftClickSlider = 308;

const int ID_rightClickText1 = 109;
const int ID_rightClickText2 = 110;
const int ID_rightClickSlider = 111;

const int ID_settingExitButton = 200;

//
const float mouse_sensitivity_full = 6.0;

// 500到1000
const int left_click_check_time_min = 500;
const int left_click_check_time_max = 1000;

const int right_click_check_time_min = 100;
const int right_click_check_time_max = 500;

GUISettings::GUISettings(gui::IGUIEnvironment* env,
                                 gui::IGUIElement* parent, s32 id,
                                 IMenuManager *menumgr,
                                 Client* client
                                 ):
GUIModalMenu(env, parent, id, menumgr),
m_client(client)
{
       
}

GUISettings::~GUISettings()
{
	removeChildren();
        
}

void GUISettings::removeChildren()
{
    const core::list<gui::IGUIElement*> &children = getChildren();
	core::list<gui::IGUIElement*> children_copy;
	for(core::list<gui::IGUIElement*>::ConstIterator
        i = children.begin(); i != children.end(); i++)
	{
		children_copy.push_back(*i);
	}
	for(core::list<gui::IGUIElement*>::Iterator
        i = children_copy.begin();
        i != children_copy.end(); i++)
	{
		(*i)->remove();
	}
    
    finishSetting();
}
extern int g_the_font_height;
void GUISettings::regenerateGui(v2u32 screensize)
{
	/*
     Remove stuff
     */
	removeChildren();
	
	/*
     Calculate new sizes and positions
     */
	core::rect<s32> rect(
                         screensize.X/2 - screensize.X * 33/80,
                         screensize.Y/2 - screensize.Y*10/54,
                         screensize.X/2 + screensize.X * 33/80,
                         screensize.Y/2 + screensize.Y*10/54
                         );
	
	DesiredRect = rect;
	recalculateAbsolutePosition(false);
    
	v2s32 size = rect.getSize();
	
	/*
     Add stuff
     */
	
    int scrollbarW = screensize.X*30/80;
    int scrollbarH = screensize.Y*0.06;
    int theY = size.Y * 0.1;
    int theX = size.X/2-scrollbarW * 0.2;
    int theMarginX = 20;
    int rightTextW = Environment->getSkin()->getFont()->getDimension(L"100").Width;
    int textH = g_the_font_height * 1.5;
    int theTextY = theY + (scrollbarH-textH);
	{
//        theTextY = theTextY * 1.8;
        int volume=(int)(g_settings->getFloat("sound_volume")*100);
        core::stringw text = _("Sound Volume");
        text += _(": ");
        int width = Environment->getSkin()->getFont()->getDimension(text.c_str()).Width;
        core::rect<s32> rect(0, 0, width, textH);
		rect = rect + v2s32(theX - width - theMarginX, theTextY);
		Environment->addStaticText(text.c_str(), rect, false,
                                   true, this, ID_soundText1);
        
		core::rect<s32> rect2(0, 0, rightTextW, textH);
		rect2 = rect2 + v2s32( theX + scrollbarW + theMarginX,theTextY );
		Environment->addStaticText(core::stringw(volume).c_str(), rect2, false,
                                   true, this, ID_soundText2);

		core::rect<s32> rect3(0, 0,scrollbarW ,scrollbarH );
		rect3 = rect3 + v2s32(theX, theY);
		gui::IGUIScrollBar *e = Environment->addScrollBar(true,
                                                          rect3, this, ID_soundSlider);
		e->setMax(100);
		e->setPos(volume);
	}
    
//    theY += scrollbarH * 2;
//    theTextY = theY + (scrollbarH-textH);
//    {
//        int sensitivity =(int)(g_settings->getFloat("mouse_sensitivity") /  mouse_sensitivity_full *100);
//        core::stringw text = _("Look Sensitivity");
//        text += _(": ");
//        int width = Environment->getSkin()->getFont()->getDimension(text.c_str()).Width;
//        core::rect<s32> rect(0, 0, width, textH);
//		rect = rect + v2s32(theX - width - theMarginX, theTextY);
//		Environment->addStaticText(text.c_str(), rect, false,
//                                   true, this, ID_lookaroundText1);
//        
//		core::rect<s32> rect2(0, 0, rightTextW, textH);
//		rect2 = rect2 + v2s32( theX + scrollbarW + theMarginX,theTextY );
//		Environment->addStaticText(core::stringw(sensitivity).c_str(), rect2, false,
//                                   true, this, ID_lookaroundText2);
//        
//		core::rect<s32> rect3(0, 0,scrollbarW ,scrollbarH );
//		rect3 = rect3 + v2s32(theX, theY);
//		gui::IGUIScrollBar *e = Environment->addScrollBar(true,
//                                                          rect3, this, ID_lookaroundSlider);
//		e->setMax(100);
//		e->setPos(sensitivity);
//	}
//    
//    theY += scrollbarH * 2;
//    theTextY = theY + (scrollbarH-textH);
//    {
//        int current = g_settings->getS32("left_click_check_time");
//        
//        int pos = (current - left_click_check_time_min) * 1.0 / (left_click_check_time_max - left_click_check_time_min) * 100;
//        core::stringw text = _("Dig Block Sensitivity");
//        text += _(":");
//        int width = Environment->getSkin()->getFont()->getDimension(text.c_str()).Width;
//        core::rect<s32> rect(0, 0, width, textH);
//		rect = rect + v2s32(theX - width - theMarginX, theTextY);
//		Environment->addStaticText(text.c_str(), rect, false,
//                                   true, this, ID_leftClickText1);
//        
//		core::rect<s32> rect2(0, 0, rightTextW, textH);
//		rect2 = rect2 + v2s32( theX + scrollbarW + theMarginX,theTextY );
//		Environment->addStaticText(core::stringw(pos).c_str(), rect2, false,
//                                   true, this, ID_leftClickText2);
//        
//		core::rect<s32> rect3(0, 0,scrollbarW ,scrollbarH );
//		rect3 = rect3 + v2s32(theX, theY);
//		gui::IGUIScrollBar *e = Environment->addScrollBar(true,
//                                                          rect3, this, ID_leftClickSlider);
//		e->setMax(100);
//		e->setPos(pos);
//	}
//    
//    theY += scrollbarH * 2;
//    theTextY = theY + (scrollbarH-textH);
//    {
//        int current = g_settings->getS32("right_click_check_time");
//        
//        int pos = (current - right_click_check_time_min) * 1.0 / (right_click_check_time_max - right_click_check_time_min) * 100;
//        core::stringw text = _("Place Block Sensitivity");
//        text += _(":");
//        int width = Environment->getSkin()->getFont()->getDimension(text.c_str()).Width;
//        core::rect<s32> rect(0, 0, width, textH);
//		rect = rect + v2s32(theX - width - theMarginX, theTextY);
//		Environment->addStaticText(text.c_str(), rect, false,
//                                   true, this, ID_rightClickText1);
//        
//		core::rect<s32> rect2(0, 0, rightTextW, textH);
//		rect2 = rect2 + v2s32( theX + scrollbarW + theMarginX,theTextY );
//		Environment->addStaticText(core::stringw(pos).c_str(), rect2, false,
//                                   true, this, ID_rightClickText2);
//        
//		core::rect<s32> rect3(0, 0,scrollbarW ,scrollbarH );
//		rect3 = rect3 + v2s32(theX, theY);
//		gui::IGUIScrollBar *e = Environment->addScrollBar(true,
//                                                          rect3, this, ID_rightClickSlider);
//		e->setMax(100);
//		e->setPos(pos);
//	}
    
    
    {
		core::rect<s32> rect(0, 0, screensize.X/7, scrollbarH);
		rect = rect + v2s32(size.X/2 - screensize.X/14 ,  size.Y/2 * 1.5 );
		
        Environment->addButton(rect, this, ID_settingExitButton,
                               _("Ok"));
		
	}
	
}

void GUISettings::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();
	video::SColor bgcolor(140,0,0,0);
	driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);
	gui::IGUIElement::draw();
}

bool GUISettings::OnEvent(const SEvent& event)
{
	if(event.EventType==EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.Key==KEY_ESCAPE && event.KeyInput.PressedDown)
		{
			quitMenu();
			return true;
		}
		if(event.KeyInput.Key==KEY_RETURN && event.KeyInput.PressedDown)
		{
			quitMenu();
			return true;
		}
	}
	if(event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
    {
        if (event.GUIEvent.Caller->getID() == ID_settingExitButton)
        {
            quitMenu();
            return true;
        }
    }
	if(event.GUIEvent.EventType==gui::EGET_SCROLL_BAR_CHANGED)
    {
		if (event.GUIEvent.Caller->getID() == ID_soundSlider)
        {
            s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
            g_settings->setFloat("sound_volume",(float)pos/100);
            gui::IGUIElement *e = getElementFromId(ID_soundText2);
            e->setText( core::stringw(pos).c_str() );
            return true;
        }
        else if (event.GUIEvent.Caller->getID() == ID_lookaroundSlider)
        {
            s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
            float current = mouse_sensitivity_full * pos / 100;
            g_settings->setFloat("mouse_sensitivity",current);
            gui::IGUIElement *e = getElementFromId(ID_lookaroundText2);
            e->setText( core::stringw(pos).c_str() );
//            //SKLOG("  %d,%f", pos,current);
            return true;
        }
        else if (ID_leftClickSlider == event.GUIEvent.Caller->getID())
        {
            s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
            int current = (left_click_check_time_max-left_click_check_time_min) * pos / 100 + left_click_check_time_min;
            g_settings->setS32("left_click_check_time",current);
            gui::IGUIElement *e = getElementFromId(ID_leftClickText2);
            e->setText( core::stringw(pos).c_str() );
//            //SKLOG("  %d,%d", pos,current);
            return true;
        }
        else if (ID_rightClickSlider == event.GUIEvent.Caller->getID())
        {
            s32 pos = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
            int current = (right_click_check_time_max-right_click_check_time_min) * pos / 100 + right_click_check_time_min;
            g_settings->setS32("right_click_check_time",current);
            gui::IGUIElement *e = getElementFromId(ID_rightClickText2);
            e->setText( core::stringw(pos).c_str() );
//            //SKLOG("  %d,%d", pos,current);
            return true;
        }
    }
	return Parent ? Parent->OnEvent(event) : false;
}
void GUISettings::finishSetting()
{
    //更新！！
    my_event_receiver.init();
}
