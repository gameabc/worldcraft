/*
Copyright (C) 2013 xyz, Ilya Zhuravlev <whatever@xyz.is>

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

#include "touchscreengui.h"
#include "irrlichttypes.h"
#include "irr_v2d.h"
#include "log.h"
#include "keycode.h"
#include "settings.h"
#include "gettime.h"
#include "porting.h"
#include "gettext.h"
#include <iostream>
#include <algorithm>

#include <ISceneCollisionManager.h>

using namespace irr::core;
using namespace irr::video;

extern Settings *g_settings;
//extern android_app *app_global;
//extern JNIEnv *jnienv;

enum {
	first_element_id = 50, // change to random number when shit breaks
	forward_id = first_element_id,
	backward_id,
	left_id,
	right_id,
	jump_id,
	sneak_id,
	punch_id,
	toggle_id,
	after_last_cpad_id,
	chat_id,
	drop_id,
	freemove_id,
	info_id,
	inventory_id,
	after_last_element_id
};

TouchScreenGUI::TouchScreenGUI(IrrlichtDevice *device):
	m_device(device),
	m_guienv(device->getGUIEnvironment()),
	m_camera_yaw(0.0),
	m_camera_pitch(0.0),
	m_down(false),
	m_down_pointer_id(0),
	m_down_since(0),
	m_dig_press_time(0),
	m_digging(false),
	m_rightclick(false),
	m_player_item_changed(false),
	m_player_item(0),
	m_hud_start_y(100000),
	m_buttons_start_x(100000),
	m_visible(false),
	m_visible_buttons(false),
	m_previous_key_time(0),
	m_previous_place_time(0)
{
	m_screensize = m_device->getVideoDriver()->getScreenSize();
}

void TouchScreenGUI::init() {
    ingame_chat_ui_on = false;
    
	u32 control_pad_size = std::min(m_screensize.Y, m_screensize.X) / 2;
	u32 opt_button_size = control_pad_size / 3;
	std::string texture = porting::path_share;
	u32 button_size = 256;
	u32 image_map_size = 1024;
	if (opt_button_size < 80) {
		texture += "/textures/android/cpad_ldpi.png";
		button_size = 64;
		image_map_size = 256;
	}
	else if (opt_button_size < 112) {
		texture += "/textures/android/cpad_mdpi.png";
		button_size = 96;
		image_map_size = 512;
	}
	else if (opt_button_size < 160) {
		texture += "/textures/android/cpad_hdpi.png";
		button_size = 128;
		image_map_size = 512;
	}
	else if (opt_button_size < 224) {
		texture += "/textures/android/cpad_xdpi.png";
		button_size = 192;
	}
	else {
		texture += "/textures/android/cpad_xxdpi.png";
	}
	m_control_pad_rect = rect<s32>(0, m_screensize.Y - 3 * button_size, 3 * button_size, m_screensize.Y);
	u32 margin_size = button_size / 4;
	m_down = false;
	m_digging = false;
	m_rightclick = false;
	m_visible = true;
	m_visible_buttons = false;
	m_dig_press_time = g_settings->getU16("touchscreen_dig_time");
	IGUIButton* cpad[14];
	ITexture* image_map = m_device->getVideoDriver()->getTexture(texture.c_str());

	/*
	draw control pad
	0 1 2
	3 4 5
	6 7 8
	*/
	int n = 0;
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x, ++n) {
			u32 id = 0;
			u32 margin = 0;
			switch (n) {
			case 0:
				if (g_settings->getBool("touchscreen_dig_button"))
					id = punch_id;
				margin = margin_size;
				break;
			case 1:
				id = forward_id;
				break;
			case 3:	
				id = left_id;
				break;
			case 4:
				id = jump_id;
				break;
			case 5:
				id = right_id;
				break;
			case 6:
				id = sneak_id;
				margin = margin_size;
				break;
			case 7:
				id = backward_id;
				break;
			}
			if (id) {
				rect<s32> button_rect(
					x * button_size + margin,
					m_screensize.Y - button_size * (3 - y) + margin,
					(x + 1) * button_size - margin,
					m_screensize.Y - button_size * (2 - y) - margin
				);
				rect<s32> image_rect(
					button_size * x + margin,
					button_size * y + margin,
					button_size * (x + 1) - margin,
					button_size * (y + 1) - margin
				);
				cpad[n] = m_guienv->addButton(button_rect, 0, id, L"");
				cpad[n]->setScaleImage(false);
				cpad[n]->setUseAlphaChannel(true);
				cpad[n]->setDrawBorder(false);
				if(image_map) {
					cpad[n]->setImage(image_map, image_rect);
					cpad[n]->setPressedImage(image_map, image_rect);
				}
			}
		}
	}
	int spacing = m_screensize.Y / 6;
	button_size /= 2;
	m_buttons_start_x = m_screensize.X - button_size;
	n--;
	for (int y = 0; y < 6; ++y, ++n) {
		u32 id = 0;
		switch (n) {
		case 8:
			id = toggle_id;
			break;
		case 9:
			id = inventory_id;
			break;
		case 10:
			id = drop_id;
			break;
		case 11:
			id = freemove_id;
			break;
		case 12:
			id = info_id;
			break;
		case 13:
			id = chat_id;
			break;
		}
		if (id) {
			rect<s32> button_rect(
				m_screensize.X - button_size,
				m_screensize.Y - (spacing * y) - button_size,
				m_screensize.X,
				m_screensize.Y - (spacing * y)
			);
			rect<s32> image_rect(
				image_map_size - button_size,
				button_size * (5 - y),
				image_map_size,
				button_size * (6 - y)
			);
			cpad[n] = m_guienv->addButton(button_rect, 0, id, L"");
			cpad[n]->setScaleImage(false);
			cpad[n]->setUseAlphaChannel(true);
			cpad[n]->setDrawBorder(false);
			if(image_map) {
				cpad[n]->setImage(image_map, image_rect);
				cpad[n]->setPressedImage(image_map, image_rect);
			}
		}
	}
	Toggle(true);
}

TouchScreenGUI::~TouchScreenGUI() {}
#include "hud.h"
extern Hud* g_hud;
void TouchScreenGUI::OnEvent(const SEvent &event) {

	if (event.EventType == EET_MULTI_TOUCH_EVENT)
    {
		keyIsDown.unset(getKeySetting("keymap_forward"));
		keyIsDown.unset(getKeySetting("keymap_backward"));
		keyIsDown.unset(getKeySetting("keymap_left"));
		keyIsDown.unset(getKeySetting("keymap_right"));
		keyIsDown.unset(getKeySetting("keymap_jump"));
		keyIsDown.unset(getKeySetting("keymap_sneak"));

		u32 time = getTimeMs();
		if (event.MultiTouchInput.Event == EMTIE_LEFT_UP) {
			if (time - m_previous_click_time <= 300) {
				// double click
				m_double_click = true;
			} else {
				m_previous_click_time = time;
			}
		}

		bool main_pointer_still_here = false;

		for (int i = 0; i < event.MultiTouchInput.PointerCount; ++i) {
			s32 x = event.MultiTouchInput.X[i];
			s32 y = event.MultiTouchInput.Y[i];
			if (event.MultiTouchInput.ID[i] == m_down_pointer_id)
				m_down_to = v2s32(x, y);
			
			if (!event.MultiTouchInput.Touched[i])
				continue;
			
			if (event.MultiTouchInput.ID[i] == m_down_pointer_id)
				main_pointer_still_here = true;

			bool ignore_click = !m_visible || m_control_pad_rect.isPointInside(v2s32(x, y));
			IGUIElement *element;
			if ((element = m_guienv->getRootGUIElement()->getElementFromPoint(v2s32(x, y)))) {
				std::string key = "";
				switch (element->getID()) {
				case forward_id:
					key = "forward";
					break;
				case left_id:
					key = "left";
					break;
				case right_id:
					key = "right";
					break;
				case backward_id:
					key = "backward";
					break;
				case jump_id:
					key = "jump";
					break;
				case freemove_id:
					if (time - m_previous_key_time > 300)
						key = "freemove";
					break;
				case drop_id:
					key = "drop";
					break;
				case sneak_id:
					key = "sneak";
					break;
				case info_id:
					if (time - m_previous_key_time > 300)
						key = "toggle_time";
					break;
				case toggle_id:
					if (time - m_previous_key_time > 300) {
						m_visible_buttons = !m_visible_buttons;
						Toggle(m_visible);
						ignore_click = true;
						m_previous_key_time = time;
					}
					break;
				case inventory_id:
					if (time - m_previous_place_time > 100) {
						key = "inventory";
						m_previous_place_time = time;
					}
					break;
				case chat_id:
					if (time - m_previous_place_time > 100) {
						key = "chat";
						m_previous_place_time = time;
					}
					break;
				case punch_id:
					m_digging = true;
					ignore_click = true;
					break;
				}

				if (key != "" && m_visible) {
					keyIsDown.set(getKeySetting(("keymap_" + key).c_str()));
					keyWasDown.set(getKeySetting(("keymap_" + key).c_str()));
					ignore_click = true;
					m_previous_key_time = time;
				}
			}

			// perhaps this actually should track a pointer and store its MultiTouchInput.ID[i] somehow
			if (!ignore_click) {
				// update camera_yaw and camera_pitch
				s32 dx = x - event.MultiTouchInput.PrevX[i];
				s32 dy = y - event.MultiTouchInput.PrevY[i];

				float d = g_settings->getFloat("mouse_sensitivity");

				m_camera_yaw -= dx * d;
				m_camera_pitch += dy * d;

				// update shootline
				m_shootline = m_device->getSceneManager()->getSceneCollisionManager()->getRayFromScreenCoordinates(v2s32(x, y));

				if (!m_down) {
					m_down = true;
					main_pointer_still_here = true;
					m_down_pointer_id = event.MultiTouchInput.ID[i];
					m_down_since = time;
					m_down_from = v2s32(x, y);
					m_down_to = m_down_from;
				}
			}

			// check if hud item is pressed
			for (int j = 0; j < m_hud_rects.size(); ++j)
				if (m_hud_rects[j].isPointInside(v2s32(x, y))) {
					m_player_item = j;
					m_player_item_changed = true;
					break;
				}
		}

		if (!main_pointer_still_here) {
			if (m_down_to.Y < m_hud_start_y && m_down && !m_digging
					&& !m_control_pad_rect.isPointInside(v2s32(m_down_to.X, m_down_to.Y))
					&& (m_down_to.X < m_buttons_start_x || !m_visible_buttons)
					&& time - m_previous_place_time > 200
					&& time - m_down_since < 300
					&& m_down_from.getDistanceFromSQ(m_down_to) < 100) {
				m_rightclick = true;
				m_previous_place_time = time;
			}
			m_down = false;
			m_digging = false;
		}
        
        if (g_hud) g_hud->OnEvent(event);
	} else if (event.EventType == EET_KEY_INPUT_EVENT) {
		if (event.KeyInput.PressedDown) {

			// Fixes android issue 43817: Hide keyboard key crashes app
			if (event.KeyInput.Key == KEY_CANCEL) {
                //SKLOG("=====fixme=====");
//				if(porting::displayKeyboard(false, app_global, jnienv))
//					return;				
			}
			if (m_visible) {
				keyIsDown.set(event.KeyInput);
				keyWasDown.set(event.KeyInput);
			}
		} else {
			keyIsDown.unset(event.KeyInput);
		}
	}

}
void TouchScreenGUI::setKeyDown(const KeyPress &keyCode)
{
    if (m_visible) {
        keyIsDown.set(keyCode);
        keyWasDown.set(keyCode);
    }
}


bool TouchScreenGUI::isKeyDown(const KeyPress &keyCode) {
	return keyIsDown[keyCode];
}

bool TouchScreenGUI::wasKeyDown(const KeyPress &keyCode) {
	bool b = keyWasDown[keyCode];
	if (b)
		keyWasDown.unset(keyCode);
	return b;
}

void TouchScreenGUI::step(float dtime) {
	if (m_down && m_dig_press_time) {
		if (getTimeMs() - m_down_since > m_dig_press_time
				&& m_down_from.getDistanceFromSQ(m_down_to) < 100)
			m_digging = true;
	}
    
    _chechChatUi();
}

void TouchScreenGUI::resetHud() {
	m_hud_rects.clear();
	m_hud_start_y = 100000;
}

void TouchScreenGUI::registerHudItem(int index, const rect<s32> &rect) {
	m_hud_start_y = std::min((int)m_hud_start_y, rect.UpperLeftCorner.Y);
	m_hud_rects.push_back(rect);
}

u16 TouchScreenGUI::getPlayerItem() {
	m_player_item_changed = false;
	return m_player_item;
}

s32 TouchScreenGUI::getHotbarImageSize() {
	return m_screensize.Y / 10;
}

void TouchScreenGUI::Toggle(bool visible) {
	m_visible = visible;
	for (int i = first_element_id; i < after_last_cpad_id; ++i) {
		IGUIElement *e = m_guienv->getRootGUIElement()->getElementFromId(i);
		if (e)
			e->setVisible(visible);
	}
	visible = (visible && m_visible_buttons);
	for (int i = after_last_cpad_id; i < after_last_element_id; ++i) {
		IGUIElement *e = m_guienv->getRootGUIElement()->getElementFromId(i);
		if (e)
			e->setVisible(visible);
	}
}

void TouchScreenGUI::Hide() {
	Toggle(false);
}

void TouchScreenGUI::Show() {
	Toggle(true);
}

bool TouchScreenGUI::isSingleClick() {
	bool r = m_previous_click_time && (getTimeMs() - m_previous_click_time > 300);
	return r;
}

bool TouchScreenGUI::isDoubleClick() {
	return m_double_click;
}

void TouchScreenGUI::resetClicks() {
	m_double_click = false;
	m_previous_click_time = 0;
}



// ======
#include <text_input_box.h>
#include "guiFormSpecMenu.h"
#include "client.h"

extern Client* the_client;
void TouchScreenGUI::showChat()
{
    if (ingame_chat_ui_on) return;
    
    ingame_chat_ui_on = true;
    showTextInputBox(L"Command/Chat",L"Send",L"Cancel",L"");
}
void TouchScreenGUI::_chechChatUi()
{
    if (ingame_chat_ui_on)
    {
        if (text_input_on_show)
        {
            return;
        }
        else
        {
            // get the result
            if (text_input_is_cancel == false && text_input_result != L"/" )
            {
                TextDest *dest = new TextDestChat(the_client);
                dest->gotText(text_input_result.c_str() );
                delete dest;
            }
            ingame_chat_ui_on = false;
        }
        return;
    }
}
extern std::wstring statustext;
extern float statustext_time;
const int TIME_MIDNIGHT = 0;
const int TIME_DAWN = 6000;
const int TIME_NOON = 12000;
const int TIME_SUNSET = 18000;
int g_cur_time = TIME_DAWN;
std::wstring g_cur_time_str = L"Dawn";
void nextTime()
{
    g_cur_time += 6000;
    if (g_cur_time >= 24000)
    {
        g_cur_time = 0;
    }
    switch (g_cur_time) {
        case TIME_MIDNIGHT:
            g_cur_time_str = L"Midnight";
            break;
        case TIME_DAWN:
            g_cur_time_str = L"Dawn";
            break;
        case TIME_NOON:
            g_cur_time_str = L"Noon";
            break;
        case TIME_SUNSET:
            g_cur_time_str = L"Sunset";
            break;
    }
    // sent time command
    TextDest *dest = new TextDestChat(the_client);
    char b[32];
    sprintf(b, "/time %d",g_cur_time);
    dest->gotText(_(b));
    delete dest;
}

void TouchScreenGUI::changeTime()
{
    nextTime();
    statustext = L"Change time to: " + g_cur_time_str;
    statustext_time = 0;
}
