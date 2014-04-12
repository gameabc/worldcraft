/** Example 027 Helloworld_Android
	This example shows a simple application for Android.
*/

#include <irrlicht.h>

#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include "the_game.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


IrrlichtDevice *startup(android_app* app)
{
	// create device
	SIrrlichtCreationParameters param;
	param.DriverType = EDT_OGLES1;				// android:glEsVersion in AndroidManifest.xml should be "0x00010000"
	//param.DriverType = EDT_OGLES2;				// android:glEsVersion in AndroidManifest.xml should be "0x00020000"
	param.WindowSize = dimension2d<u32>(0,0);	// using 0,0 it will automatically set it to the maximal size
	param.PrivateData = app;
	param.Bits = 24;
	param.ZBufferBits = 16;
	param.AntiAlias  = 0;

	/* Logging is written to a file. So your application should disable all logging when you distribute your
       application or it can fill up that file over time.
	*/
#ifndef _DEBUG
	param.LoggingLevel = ELL_NONE;	
#endif	
	
	return createDeviceEx(param);
}
extern void ensure_jni_env();
extern void dispatch_pending_runnables();
int example_helloworld(android_app* app)
{
	// create device
	IrrlichtDevice *device = startup(app);
	if (device == 0)
       	return 1;

    g_the_game = new Game();
    g_the_game->initGame(device);
    
    
    while (true)
    {
    	dispatch_pending_runnables();
        g_the_game->updateGame();
    }
	//run(device);

	device->setEventReceiver(0);
	device->closeDevice();
	device->drop();

	return 0;
}
android_app* app_global;
void android_main(android_app* app)
{
	app_global = app;
	app_dummy();
	ensure_jni_env();
	example_helloworld(app);
}

#endif	// defined(_IRR_ANDROID_PLATFORM_)

/*
**/
