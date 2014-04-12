/*
Part of Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
Copyright (C) 2013 Ciaran Gultnieks <ciaran@ciarang.com>

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

#include "guiPasswordChange.h"
#include "debug.h"
#include "serialization.h"
#include <string>
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIButton.h>
#include <IGUIStaticText.h>
#include <IGUIFont.h>
 
#include "gettext.h"
#include "GUINativeEditBox.h"
const int ID_oldPassword = 256;
const int ID_newPassword1 = 257;
const int ID_newPassword2 = 258;
const int ID_change = 259;
const int ID_message = 260;

GUIPasswordChange::GUIPasswordChange(gui::IGUIEnvironment* env,
		gui::IGUIElement* parent, s32 id,
		IMenuManager *menumgr,
		Client* client
):
	GUIModalMenu(env, parent, id, menumgr),
	m_client(client)
{
//#ifdef ANDROID
       
//#endif
}

GUIPasswordChange::~GUIPasswordChange()
{
//#ifdef ANDROID
       
//#endif
	removeChildren();
}

void GUIPasswordChange::removeChildren()
{
	{
		gui::IGUIElement *e = getElementFromId(ID_oldPassword);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(ID_newPassword1);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(ID_newPassword2);
		if(e != NULL)
			e->remove();
	}
	{
		gui::IGUIElement *e = getElementFromId(ID_change);
		if(e != NULL)
			e->remove();
	}
}
extern int g_the_font_height;
void GUIPasswordChange::regenerateGui(v2u32 screensize)
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
    s32 font_height = g_the_font_height * 1.2;
 
	s32 ypos = size.Y * 0.1;
	const s32 btn_height = size.Y * 0.1;
    const s32 btn_width  = size.X * 0.3;
    const s32 btn_gap = size.Y * 0.05 + btn_height;
	{
		core::rect<s32> rect(0, 0, size.X, font_height);
		rect +=   v2s32(size.X * 0.2, ypos+6);
		
		Environment->addStaticText(_("Old Password"), rect, false, true, this, -1);
		
	}
	
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect +=  v2s32(size.X/2 + 20, ypos);
		gui::IGUIEditBox *e = 
		Environment->addEditBox(L"", rect, true, this, ID_oldPassword);
		Environment->setFocus(e);
		e->setPasswordBox(true);
	}
	ypos += btn_gap;
	
	{
		core::rect<s32> rect(0, 0, size.X, font_height);
		rect +=  v2s32(size.X * 0.2, ypos+6);
		
		Environment->addStaticText(_("New Password"), rect, false, true, this, -1);
		
	}
	
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect +=  v2s32(size.X/2 + 20, ypos);
		gui::IGUIEditBox *e = 
		Environment->addEditBox(L"", rect, true, this, ID_newPassword1);
		e->setPasswordBox(true);
	}
	ypos += btn_gap;
	
	{
		core::rect<s32> rect(0, 0, size.X, font_height);
		rect +=  v2s32(size.X * 0.2, ypos+6);
		
		Environment->addStaticText(_("Confirm Password"), rect, false, true, this, -1);
		
	}
	
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect +=  v2s32(size.X/2 + 20, ypos);
		gui::IGUIEditBox *e = 
		Environment->addEditBox(L"", rect, true, this, ID_newPassword2);
		e->setPasswordBox(true);
	}

	ypos += btn_gap;
	
	{
		core::rect<s32> rect(0, 0, btn_width, btn_height);
		rect = rect + v2s32(size.X/2-size.X * 0.3/2, size.Y * 0.65);
		
		Environment->addButton(rect, this, ID_change, _("Change"));
		
	}

	ypos += btn_gap;
	{
		core::rect<s32> rect(0, 0, size.X, font_height);
		rect +=   v2s32(size.X * 0.2, size.Y * 0.75);
		
		IGUIElement *e = 
		Environment->addStaticText(
			_("Passwords do not match!"),
			rect, false, true, this, ID_message);
		e->setVisible(false);
		
	}
	

}

void GUIPasswordChange::drawMenu()
{
	gui::IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;
	video::IVideoDriver* driver = Environment->getVideoDriver();
	
	video::SColor bgcolor(140,0,0,0);
	driver->draw2DRectangle(bgcolor, AbsoluteRect, &AbsoluteClippingRect);

	gui::IGUIElement::draw();
}

bool GUIPasswordChange::acceptInput()
{
		std::wstring oldpass;
		std::wstring newpass;
		gui::IGUIElement *e;
		e = getElementFromId(ID_oldPassword);
		if(e != NULL)
			oldpass = e->getText();
		e = getElementFromId(ID_newPassword1);
		if(e != NULL)
			newpass = e->getText();
		e = getElementFromId(ID_newPassword2);
		if(e != NULL && newpass != e->getText())
		{
			e = getElementFromId(ID_message);
			if(e != NULL)
				e->setVisible(true);
			return false;
		}
		m_client->sendChangePassword(oldpass, newpass);
		return true;
}

bool GUIPasswordChange::OnEvent(const SEvent& event)
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
			if(acceptInput())
				quitMenu();
			return true;
		}
	}
	if(event.EventType==EET_GUI_EVENT)
	{
		if(event.GUIEvent.EventType==gui::EGET_ELEMENT_FOCUS_LOST
				&& isVisible())
		{
			if(!canTakeFocus(event.GUIEvent.Element))
			{
				dstream<<"GUIPasswordChange: Not allowing focus change."
						<<std::endl;
				// Returning true disables focus change
				return true;
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_BUTTON_CLICKED)
		{
			switch(event.GUIEvent.Caller->getID())
			{
			case ID_change:
				if(acceptInput())
					quitMenu();
				return true;
			}
		}
		if(event.GUIEvent.EventType==gui::EGET_EDITBOX_ENTER)
		{
			switch(event.GUIEvent.Caller->getID())
			{
			case ID_oldPassword:
			case ID_newPassword1:
			case ID_newPassword2:
				if(acceptInput())
					quitMenu();
				return true;
			}
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}

