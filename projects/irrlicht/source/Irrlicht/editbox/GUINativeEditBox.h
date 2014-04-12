//
//  GUINativeEditBox.h
//  Mine
//
//  Created by Luke on 10/18/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#ifndef __Mine__GUINativeEditBox__
#define __Mine__GUINativeEditBox__

#include "CGUIEditBox.h"

using namespace irr::core;

namespace irr
{
    namespace gui
    {
        class GUINativeEditBox:public CGUIEditBox
        {
        public:
            bool m_in_show;
        public:
            GUINativeEditBox(const wchar_t* text, bool border, IGUIEnvironment* environment,
                             IGUIElement* parent, s32 id, const core::rect<s32>& rectangle)
            :CGUIEditBox(text,border,environment,parent,id,rectangle)
            {
                m_in_show = false;
                m_title = "Input Message";
                m_yes = "Ok";
                m_no = "Cancle";
            }

           //! destructor
            virtual ~GUINativeEditBox()
            {
            }
            
            virtual bool OnEvent(const SEvent& event);
            virtual void draw();
            void setEditBoxInfo(const char*& title,const char*& yes,const char*& no)
            {
                m_title = title;
                m_yes = yes;
                m_no = no;
            }
            stringw m_title,m_yes,m_no,m_result;
        };

}
}
#endif /* defined(__Mine__GUINativeEditBox__) */
