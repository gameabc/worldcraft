//
//  text_input_box.h
//  Mine
//
//  Created by Luke on 10/17/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#ifndef __Mine__text_input_box__
#define __Mine__text_input_box__


#include "irrString.h"
void showTextInputBox(const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t* initStr,bool is_password = false);
void showCancelDialog();
extern bool text_input_is_cancel;
extern bool text_input_on_show;
extern  irr::core::stringw text_input_result;
extern wchar_t* char_to_wide(const char* input);
extern char* wide_to_char(const wchar_t* input);
#endif /* defined(__Mine__text_input_box__) */
