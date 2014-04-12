//
//  GUINativeEditBox.cpp
//  Mine
//
//  Created by Luke on 10/18/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#include "GUINativeEditBox.h"
#include "text_input_box.h"
namespace irr
{
    namespace gui
    {
        bool GUINativeEditBox::OnEvent(const irr::SEvent &event)
        {
            bool ret  = false;
            // go here
            if (isEnabled()
                    && event.EventType == EET_GUI_EVENT
                    && event.GUIEvent.EventType == EGET_ELEMENT_FOCUSED && m_in_show == false)
            {
                m_in_show = true;
                showTextInputBox(m_title.c_str(), m_yes.c_str(), m_no.c_str(), getText(),isPasswordBox());
                ret = true;
            }
            if (ret)
            {
                return true;
            }
            return CGUIEditBox::OnEvent(event);
        }
        
        void GUINativeEditBox::draw()
        {
            CGUIEditBox::draw();
            // do some check
            if ( m_in_show) {
                if (text_input_on_show)
                {
                    return;
                }
                else
                {
                    m_in_show = false;
                    if (text_input_is_cancel == false)
                    {
                        setText(text_input_result.c_str() );
                    }
                }
            }
        } 
    }
}