/*
Minetest
Copyright (C) 2010-2013 kwolekr, Ryan Kwolek <kwolekr@minetest.net>

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

#ifndef HUD_HEADER
#define HUD_HEADER

#include "irrlichttypes_extrabloated.h"
#include <string>

#define HUD_DIR_LEFT_RIGHT 0
#define HUD_DIR_RIGHT_LEFT 1
#define HUD_DIR_TOP_BOTTOM 2
#define HUD_DIR_BOTTOM_TOP 3

#define HUD_CORNER_UPPER  0
#define HUD_CORNER_LOWER  1
#define HUD_CORNER_CENTER 2

#define HUD_FLAG_HOTBAR_VISIBLE    (1 << 0)
#define HUD_FLAG_HEALTHBAR_VISIBLE (1 << 1)
#define HUD_FLAG_CROSSHAIR_VISIBLE (1 << 2)
#define HUD_FLAG_WIELDITEM_VISIBLE (1 << 3)
#define HUD_FLAG_BREATHBAR_VISIBLE (1 << 4)

#define HUD_PARAM_HOTBAR_ITEMCOUNT 1
#define HUD_PARAM_HOTBAR_IMAGE 2
#define HUD_PARAM_HOTBAR_SELECTED_IMAGE 3

#define HUD_HOTBAR_ITEMCOUNT_DEFAULT 5
#define HUD_HOTBAR_ITEMCOUNT_MAX     6

enum HudElementType {
	HUD_ELEM_IMAGE     = 0,
	HUD_ELEM_TEXT      = 1,
	HUD_ELEM_STATBAR   = 2,
	HUD_ELEM_INVENTORY = 3
};

enum HudElementStat {
	HUD_STAT_POS = 0,
	HUD_STAT_NAME,
	HUD_STAT_SCALE,
	HUD_STAT_TEXT,
	HUD_STAT_NUMBER,
	HUD_STAT_ITEM,
	HUD_STAT_DIR,
	HUD_STAT_ALIGN,
	HUD_STAT_OFFSET
};

struct HudElement {
	HudElementType type;
	v2f pos;
	std::string name;
	v2f scale;
	std::string text;
	u32 number;
	u32 item;
	u32 dir;
	v2f align;
	v2f offset;
};

#ifndef SERVER

#include <vector>
#include <IGUIFont.h>
#include "irr_aabb3d.h"

class IGameDef;
class ITextureSource;
class Inventory;
class InventoryList;
class LocalPlayer;
struct ItemStack;
#define HUD_UI_TYPE_BIG 0
#define HUD_UI_TYPE_SMALL 1
class Hud {
public:
	video::IVideoDriver *driver;
	gui::IGUIEnvironment *guienv;
	gui::IGUIFont *font;
	u32 text_height;
	IGameDef *gamedef;
	LocalPlayer *player;
	Inventory *inventory;
	ITextureSource *tsrc;

	v2u32 screensize;
	v2s32 displaycenter;
	s32 hotbar_imagesize;
	
	video::SColor crosshair_argb;
	video::SColor selectionbox_argb;
	bool use_crosshair_image;
	std::string hotbar_image;
	bool use_hotbar_image;
	std::string hotbar_selected_image;
	bool use_hotbar_selected_image;
	
	Hud(video::IVideoDriver *driver, gui::IGUIEnvironment* guienv,
		gui::IGUIFont *font, u32 text_height, IGameDef *gamedef,
		LocalPlayer *player, Inventory *inventory,v2u32 screen_size);
	
	void drawItem(v2s32 upperleftpos, s32 imgsize, s32 itemcount,
		InventoryList *mainlist, u16 selectitem, u16 direction);
	void drawLuaElements();
	void drawStatbar(v2s32 pos, u16 corner, u16 drawdir,
					 std::string texture, s32 count, v2s32 offset);
	
	void drawHotbar(v2s32 centerlowerpos, s32 halfheartcount, u16 playeritem, s32 breath);
	void resizeHotbar();
	
	void drawCrosshair();
	void drawSelectionBoxes(std::vector<aabb3f> &hilightboxes);
    
    ~Hud();
    // HUD_UI_TYPE_BIG or HUD_UI_TYPE_SMALL
	void setUiType(int type);
    
    bool OnEvent(const SEvent& event);

    void updateChatUi();
    //仅仅在这个区域内的点击才算是有效的left/right点击
    core::rect<s32> m_valid_click_area;
	core::rect<s32> m_button_items_area;
private:
    bool checkClickEvent(int index,const core::rect<s32>& pos,const v2s32& mouse_pos);
    void drawAndCheckBackButton();
    
    void drawAndCheckChatCommand();
    void drawAndCheckTime();
    void drawAndCheckListItems();
    
    void drawAndCheckPause();
    
    void drawAndCheckFly();
    void drawAndCheckMoreButtons();
    bool m_stop_draw_buttons;
    
    bool m_is_creative_mode;
    core::rect<s32> m_pos_back;
    core::rect<s32> m_pos_chat;
    core::rect<s32> m_pos_left_null;
    core::rect<s32> m_pos_left_2;
    core::rect<s32> m_pos_left_3;
    core::rect<s32> m_pos_items;
    core::rect<s32> m_pos_time;
    core::rect<s32> m_pos_pause;
    core::rect<s32> m_pos_fly;
//    core::rect<s32> m_pos_noclip;
    
    core::rect<s32> m_pos_more;
    core::rect<s32> m_pos_more1_help,m_pos_more2_sneak,m_pos_more3_noclip,m_pos_more4_control;
    bool m_is_more_on;
};

void drawItemStack(video::IVideoDriver *driver,
		gui::IGUIFont *font,
		const ItemStack &item,
		const core::rect<s32> &rect,
		const core::rect<s32> *clip,
		IGameDef *gamedef);

class Client;
extern Client* the_client;
//当前是否有ui正在显示。在main.cpp中实现
bool noMenuActive();

#endif

#endif
