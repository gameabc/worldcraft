//
//  text_input_box.cpp
//  iOS
//
//  Created by Luke on 10/24/13.
//
//

#ifdef ANDROID
#include  "text_input_box.h"
#include <jni.h>
#include "JniHelper.h"
#define CLASS_NAME "com/abc/GameActivity"
#include "irrString.h"
irr::core::stringw text_input_result;
bool text_input_on_show = false;
bool text_input_is_cancel = false;

extern "C"
{
    JNIEXPORT void JNICALL Java_com_abc_GameActivity_nativeSetEditBoxResult(JNIEnv * env, jobject obj, jbyteArray text,jboolean isCancel) {
    	text_input_on_show = false;
    	text_input_is_cancel = (bool)isCancel;
        jsize  size = env->GetArrayLength(text);
        
        if (size > 0) {
            jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
            char* pBuf = (char*)malloc(size+1);
            if (pBuf != NULL) {
                memcpy(pBuf, data, size);
                pBuf[size] = '\0';
                // pass data to edittext's delegate
                text_input_result = char_to_wide(pBuf);
//                if (s_pfEditTextCallback) s_pfEditTextCallback(pBuf, s_ctx);
//                free(pBuf);
            }
            env->ReleaseByteArrayElements(text, data, 0);
        } else {
//            if (s_pfEditTextCallback) s_pfEditTextCallback("", s_ctx);
            text_input_result = L"";
        }
    }

    
    void showEditTextDialogJNI(const char* pszTitle, const char* pszMessage, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength) {
        if (pszMessage == NULL) {
            return;
        }
        text_input_on_show = true;
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIII)V")) {
            jstring stringArg1;
            
            if (!pszTitle) {
                stringArg1 = t.env->NewStringUTF("");
            } else {
                stringArg1 = t.env->NewStringUTF(pszTitle);
            }
            
            jstring stringArg2 = t.env->NewStringUTF(pszMessage);
            
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, nInputMode, nInputFlag, nReturnType, nMaxLength);
            
            t.env->DeleteLocalRef(stringArg1);
            t.env->DeleteLocalRef(stringArg2);
            t.env->DeleteLocalRef(t.classID);
        }
    }
    
}
int kEditBoxInputModeSingleLine = 6;
int kEditBoxInputFlagPassword = 0;
int kEditBoxInputFlagSensitive = 1;
int kKeyboardReturnTypeDone = 1;
extern char* wide_to_char(const wchar_t* input);
const char* wchat2chat(const wchar_t* s)
{
    return wide_to_char(s);
}



void showTextInputBox(const wchar_t* t,const wchar_t* yes,const wchar_t* no,const wchar_t* initStr,bool is_password)
{
    showEditTextDialogJNI(wchat2chat(t), wchat2chat(initStr),
            kEditBoxInputModeSingleLine,
            is_password ? kEditBoxInputFlagPassword : kEditBoxInputFlagSensitive,
    		kKeyboardReturnTypeDone,
            1024);
}
#endif
