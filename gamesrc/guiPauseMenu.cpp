/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

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

#include "guiPauseMenu.h"
#include "debug.h"
#include "serialization.h"
#include "porting.h"
#include "config.h"
#include "main.h"
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>

#include "util/string.h"
 
#include "gettext.h"
#include "settings.h"
#include "guiMainMenu.h"
GUIPauseMenu::GUIPauseMenu(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IGameCallback *gamecallback,
		IMenuManager *menumgr,
		bool simple_singleplayer_mode):
	GUIModalMenu(env, parent, id, menumgr),
	m_gamecallback(gamecallback),
	m_simple_singleplayer_mode(simple_singleplayer_mode)
{
//#ifdef ANDROID
       
//#endif
}

GUIPauseMenu::~GUIPauseMenu()
{
//#ifdef ANDROID
       
//#endif
	removeChildren();
}

void GUIPauseMenu::removeChildren()
{
	{
		gui::IGUIElement *e = getElementFromId(256);
		if(e != NULL)
			e->remove();
	}
//	{
//		gui::IGUIElement *e = getElementFromId(257);
//		if(e != NULL)
//			e->remove();
//	}
	{
		gui::IGUIElement *e = getElementFromId(258);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(259);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(260);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(261);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(262);
		if(e != NULL)
			e->remove();
	}
    {
        //文本:ip/port
		gui::IGUIElement *e = getElementFromId(100);
		if(e != NULL)
			e->remove();
	}
}

//
//IGUIButton* addGreenColorButton(IGUIEnvironment* env,const core::rect<s32>& rectangle,
//                           IGUIElement* parent, s32 id, const wchar_t* text, const wchar_t *tooltiptext)
//{
//    irr::gui::IGUIButton* button = env->addButton(rectangle, parent, id,text,tooltiptext);
//    return button;
//}
extern int g_the_font_height;
extern MainMenuData menudata;
void GUIPauseMenu::regenerateGui(v2u32 screensize)
{
	/*
		Remove stuff
	*/
	removeChildren();
	
	/*
		Calculate new sizes and positions
	*/
    core::rect<s32> rect(
                         screensize.X/2 - screensize.X * 0.4,
                         screensize.Y/2 - screensize.Y * 0.3,
                         screensize.X/2 + screensize.X * 0.4,
                         screensize.Y/2 + screensize.Y * 0.3
                         );
	
	DesiredRect = rect;
	recalculateAbsolutePosition(false);

	v2s32 size = rect.getSize();

	/*
		Add stuff
	*/
	const s32 btn_height = size.Y * 0.1;
    const s32 btn_width  = size.X * 0.3;
	const s32 btn_gap = size.Y * 0.05;
	const s32 btn_num = m_simple_singleplayer_mode ? 4 : 5;
	s32 btn_y = size.Y/2-((btn_num*btn_height+(btn_num-1)*btn_gap))/2 +btn_height;
	
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect = rect + v2s32(size.X/2-btn_width/2, btn_y);
		
		Environment->addButton(rect, this, 256,
			_("Continue"));
		
	}
	btn_y += btn_height + btn_gap;
	if(!m_simple_singleplayer_mode)
	{
		{
			core::rect<s32> rect(0, 0, btn_width, btn_height);
			rect = rect + v2s32(size.X/2-btn_width/2, btn_y);
			
			Environment->addButton(rect, this, 261,
				_("Change Password"));
			
		}
		btn_y += btn_height + btn_gap;
	}
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect = rect + v2s32(size.X/2-btn_width/2, btn_y);
		
		Environment->addButton(rect, this, 262,
			_("Settings"));
		
	}
	btn_y += btn_height + btn_gap;
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect = rect + v2s32(size.X/2-btn_width/2, btn_y);
		
		Environment->addButton(rect, this, 260,
			_("Exit to Menu"));
	}
	btn_y += btn_height + btn_gap;

    if (menudata.simple_singleplayer_mode)
    {
        std::string text = ("Server IP/Port: ");
        text += g_settings->get("local_server_ip");
        text += ":";
        text += g_settings->get("local_server_port");
    
        int width = Environment->getSkin()->getFont()->getDimension(_(text.c_str())).Width;
        int x = (rect.getWidth()-width)/2;
        int y = rect.getHeight() * 0.02;
        core::rect<s32> text_rect(x, y, x + width, y + g_the_font_height * 1.2);
        Environment->addStaticText( _(text.c_str()), text_rect, false,
                               true, this,100);
    }
	
}


void GUIPauseMenu::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();
	
	video::SColor bgcolor(140,0,0,0);
	driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);
	gui::IGUIElement::draw();
}
#include "touchscreengui.h"
bool GUIPauseMenu::OnEvent(const SEvent& event)
{

	if(event.EventType==EET_KEY_INPUT_EVENT)
	{
		if(event.KeyInput.PressedDown)
		{
			if(event.KeyInput.Key==KEY_ESCAPE)
			{
				quitMenu();
				return true;
			}
			else if(event.KeyInput.Key==KEY_RETURN)
			{
				quitMenu();
				return true;
			}
		}
	}
	if(event.EventType==EET_GUI_EVENT)
	{
		if(event.GUIEvent.EventType==gui::EGET_ELEMENT_FOCUS_LOST
				&& isVisible())
		{
			if(!canTakeFocus(event.GUIEvent.Element))
			{
				dstream<<"GUIPauseMenu: Not allowing focus change."
						<<std::endl;
				// Returning true disables focus change
				return true;
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
		{
			switch(event.GUIEvent.Caller->getID())
			{
			case 256: // continue
				quitMenu();
				// ALWAYS return immediately after quitMenu()
				return true;
			case 261:
				quitMenu();
				m_gamecallback->changePassword();
				return true;
			case 262:
				quitMenu();
				m_gamecallback->changeVolume();
				return true;
			case 260: // disconnect
                    if (touchscreengui) {
                        touchscreengui->Hide();
                    }
				m_gamecallback->disconnect();
				quitMenu();
				return true;
//			case 257: // exit
//				m_gamecallback->exitToOS();
//				quitMenu();
//				return true;
			}
		}
	}
	
	return Parent ? Parent->OnEvent(event) : false;
}

