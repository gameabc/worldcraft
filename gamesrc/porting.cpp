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

/*
	Random portability stuff

	See comments in porting.h
*/
#if !defined(WIN32)
#  include <sys/stat.h>
#  include <sys/types.h>
// For RemoveDirectory
#else
#  include <direct.h>
#  include <Windows.h>
#endif

/*Needed by the remove directory function */

#ifndef WIN32
#  include <dirent.h>
#endif

#ifdef WIN32
#  include <io.h>
#  include <stdio.h>
#  ifndef __CYGWIN__
#    define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#    define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#  endif
#else
#  include <unistd.h>
#endif

#if defined(linux)
	#include <unistd.h>
#elif defined(__APPLE__)
	#include <unistd.h>
	#include <mach-o/dyld.h>
#elif defined(__FreeBSD__)
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/sysctl.h>
#endif

#include "porting.h"
#include "config.h"
#include "debug.h"
#include "filesys.h"
#include "log.h"
#include "util/string.h"
#include <list>

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#include <mach/thread_act.h>
#endif

#include <irrlicht.h>
using namespace irr;
#ifdef ANDROID
extern std::string g_base_path;
extern std::string path_storage;
#endif

#ifdef ANDROID
#include <EGL/egl.h>
#endif

#include <irrlicht.h>
#include <irr_v2d.h>
using namespace irr;
void show_loading(int percent)
{
    
    video::IVideoDriver* driver = the_device->getVideoDriver();
	v2u32 screensize = driver->getScreenSize();
	core::vector2d<u32> textsize_u(screensize.X * 0.7,screensize.Y * 0.05) ;
	core::vector2d<s32> textsize(textsize_u.X,textsize_u.Y);
	core::vector2d<s32> center(screensize.X/2, screensize.Y/2);
	core::rect<s32> textrect(center - textsize/2, center + textsize/2);
    the_videoDriver->beginScene(true, true, video::SColor(255,140,186,250));
	if (percent >= 0 && percent <= 100) // draw progress bar
	{
        static int w = 0;
        if (w == 0)
        {
            w = (screensize.X*0.3);
        }
        if (w < textsize_u.X)
        {
            w = textsize_u.X * 1.1;
        }
		core::vector2d<s32> barsize(screensize.X * 0.3,textsize_u.Y * 1.2);
		core::rect<s32> barrect(center-barsize/2, center+barsize/2);
		the_videoDriver->draw2DRectangle(video::SColor(255,255,255,255),barrect, NULL); // border
		the_videoDriver->draw2DRectangle(video::SColor(255,64,64,64), core::rect<s32> (
                                                                                       barrect.UpperLeftCorner+1,
                                                                                       barrect.LowerRightCorner-1), NULL); // black inside the bar
		the_videoDriver->draw2DRectangle(video::SColor(255,128,128,128), core::rect<s32> (
                                                                                          barrect.UpperLeftCorner+1,
                                                                                          core::vector2d<s32>(
                                                                                                              barrect.LowerRightCorner.X-(barsize.X-1)+percent*(barsize.X-2)/100,
                                                                                                              barrect.LowerRightCorner.Y-1)), NULL); // the actual progress
	}
	the_device->getGUIEnvironment()->drawAll();
	the_videoDriver->endScene();
	
#ifdef ANDROID
    eglSwapBuffers( eglGetCurrentDisplay(), eglGetCurrentSurface( EGL_DRAW ) );
#endif
}

#ifdef ANDROID
#include <android_native_app_glue.h>
extern android_app *app_global;

std::string g_base_path = "minetest";
int total_copy_file = 1000;
int has_copy = 0;
int update_step = 100;
std::string path_storage;


void copyAssetDirectory(AAssetManager* Mgr, std::string path)
{
    // Android: Copy asset directory to user path
    AAssetDir* AssetDir = AAssetManager_openDir(Mgr, (const char*)path.c_str());
    const char* filename = (const char*)NULL;
    while ((filename = AAssetDir_getNextFileName(AssetDir)) != NULL) {
        std::string fn = path + "/" + std::string(filename);
        AAsset* Asset = AAssetManager_open(Mgr, fn.c_str(), AASSET_MODE_BUFFER);
        char buffer[BUFSIZ];
        int bytes = 0;
        fn = path_storage + "/" + fn;
        FILE* output = fopen(fn.c_str(), "w");
        while ((bytes = AAsset_read(Asset, buffer, BUFSIZ)) > 0)
            fwrite(buffer, bytes, 1, output);
        fclose(output);
        AAsset_close(Asset);
        
        has_copy++;
        if (has_copy % update_step == 0)
        {
            show_loading(has_copy * 100.0 / total_copy_file );
        }
    }
    AAssetDir_close(AssetDir);
}
void extractAssets(android_app* mApplication) {
    
    // Android: Extract minetest resource files
    show_loading(0);
    AAssetManager* Mgr = mApplication->activity->assetManager;
    AAssetDir* AssetDir = AAssetManager_openDir(Mgr, "");
    AAsset* Asset = AAssetManager_open(Mgr, "index.txt", AASSET_MODE_UNKNOWN);
    
    long size = AAsset_getLength(Asset);
    char* buffer = (char*)malloc (sizeof(char) * size);
    
    AAsset_read(Asset, buffer, size);
    AAsset_close(Asset);
    AAssetDir_close(AssetDir);
    
    char* dir = strtok(buffer, "\n");
    while (dir != NULL)	{
        std::string path = std::string(dir);
        mkdir((path_storage + "/" + path).c_str(), 0755);
        
        copyAssetDirectory(Mgr, path);
        dir = strtok(NULL, "\n");
    }
    free (buffer);
}
#include "JniHelper.h"
#include <jni.h>
#define JNI_CLASS "com/abc/GameActivity"
const char* getExternalPathJni()
{
    static std::string ret = "";
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, JNI_CLASS, "getExternalPathJni", "()Ljava/lang/String;"))
    {
        jstring str = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    return ret.c_str();
}

int needCopyResJni(const char* file)
{
    int ret = access(file,F_OK);
    if (0 == ret)
    {
        return 0;
    }
    return 1;
}
void copyResOk(const char* file)
{
    FILE* f = fopen(file,"w");
    fprintf(f,"x");
    fclose(f);
}
#endif



namespace porting
{

/*
	Signal handler (grabs Ctrl-C on POSIX systems)
*/

bool g_killed = false;

bool * signal_handler_killstatus(void)
{
	return &g_killed;
}

#if !defined(_WIN32) // POSIX
	#include <signal.h>

void sigint_handler(int sig)
{
	if(g_killed == false)
	{
		dstream<<DTIME<<"INFO: sigint_handler(): "
				<<"Ctrl-C pressed, shutting down."<<std::endl;

		// Comment out for less clutter when testing scripts
		/*dstream<<DTIME<<"INFO: sigint_handler(): "
				<<"Printing debug stacks"<<std::endl;
		debug_stacks_print();*/

		g_killed = true;
	}
	else
	{
		(void)signal(SIGINT, SIG_DFL);
	}
}

void signal_handler_init(void)
{
	(void)signal(SIGINT, sigint_handler);
}

#else // _WIN32
	#include <signal.h>

	BOOL WINAPI event_handler(DWORD sig)
	{
		switch(sig)
		{
		case CTRL_C_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:

			if(g_killed == false)
			{
				dstream<<DTIME<<"INFO: event_handler(): "
						<<"Ctrl+C, Close Event, Logoff Event or Shutdown Event, shutting down."<<std::endl;
				// Comment out for less clutter when testing scripts
				/*dstream<<DTIME<<"INFO: event_handler(): "
						<<"Printing debug stacks"<<std::endl;
				debug_stacks_print();*/

				g_killed = true;
			}
			else
			{
				(void)signal(SIGINT, SIG_DFL);
			}

			break;
		case CTRL_BREAK_EVENT:
			break;
		}

		return TRUE;
	}

void signal_handler_init(void)
{
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE)event_handler,TRUE);
}

#endif


/*
	Multithreading support
*/
int getNumberOfProcessors() {
#if defined(_SC_NPROCESSORS_ONLN)

	return sysconf(_SC_NPROCESSORS_ONLN);

#elif defined(__FreeBSD__) || defined(__APPLE__)

	unsigned int len, count;
	len = sizeof(count);
	return sysctlbyname("hw.ncpu", &count, &len, NULL, 0);

#elif defined(_GNU_SOURCE)

	return get_nprocs();

#elif defined(_WIN32)

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;

#elif defined(PTW32_VERSION) || defined(__hpux)

	return pthread_num_processors_np();

#else

	return 1;

#endif
}


bool threadBindToProcessor(threadid_t tid, int pnumber) {
#if defined(_WIN32)

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, tid);
	if (!hThread)
		return false;

	bool success = SetThreadAffinityMask(hThread, 1 << pnumber) != 0;

	CloseHandle(hThread);
	return success;

#elif (defined(__FreeBSD__) && (__FreeBSD_version >= 702106)) \
	|| defined(__linux) || defined(linux)

//	cpu_set_t cpuset;
//
//	CPU_ZERO(&cpuset);
//	CPU_SET(pnumber, &cpuset);
//	return pthread_setaffinity_np(tid, sizeof(cpuset), &cpuset) == 0;
    return false;
#elif defined(__sun) || defined(sun)

	return processor_bind(P_LWPID, MAKE_LWPID_PTHREAD(tid), 
						pnumber, NULL) == 0;

#elif defined(_AIX)
	
	return bindprocessor(BINDTHREAD, (tid_t)tid, pnumber) == 0;

#elif defined(__hpux) || defined(hpux)

	pthread_spu_t answer;

	return pthread_processor_bind_np(PTHREAD_BIND_ADVISORY_NP,
									&answer, pnumber, tid) == 0;
	
#elif defined(__APPLE__)

	struct thread_affinity_policy tapol;
	
	thread_port_t threadport = pthread_mach_thread_np(tid);
	tapol.affinity_tag = pnumber + 1;
	return thread_policy_set(threadport, THREAD_AFFINITY_POLICY,
                             (thread_policy_t)&tapol, THREAD_AFFINITY_POLICY_COUNT) == KERN_SUCCESS;

#else

	return false;

#endif
}


bool threadSetPriority(threadid_t tid, int prio) {
#if defined(_WIN32)

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, tid);
	if (!hThread)
		return false;

	bool success = SetThreadPriority(hThread, prio) != 0;

	CloseHandle(hThread);
	return success;
	
#else

	struct sched_param sparam;
	int policy;
	
	if (pthread_getschedparam(tid, &policy, &sparam) != 0)
		return false;
		
	int min = sched_get_priority_min(policy);
	int max = sched_get_priority_max(policy);

	sparam.sched_priority = min + prio * (max - min) / THREAD_PRIORITY_HIGHEST;
	return pthread_setschedparam(tid, policy, &sparam) == 0;
	
#endif
}


/*
	Path mangler
*/

// Default to RUN_IN_PLACE style relative paths
std::string path_share = "..";
std::string path_user = "..";

std::string getDataPath(const char *subpath)
{
	return path_share + DIR_DELIM + subpath;
}

void pathRemoveFile(char *path, char delim)
{
	// Remove filename and path delimiter
	int i;
	for(i = strlen(path)-1; i>=0; i--)
	{
		if(path[i] == delim)
			break;
	}
	path[i] = 0;
}

bool detectMSVCBuildDir(char *c_path)
{
	std::string path(c_path);
	const char *ends[] = {"bin\\Release", "bin\\Build", NULL};
	return (removeStringEnd(path, ends) != "");
}

void initializePaths()
{
#ifdef ANDROID
    std::string sdcard = getExternalPathJni();
    path_storage = sdcard;
    std::string m_root_dir =  sdcard  + g_base_path + "/";
    std::string version_file = m_root_dir + "1.0";
    if (needCopyResJni(version_file.c_str()) == 1)
    {
        (m_root_dir);
        extractAssets(app_global);
        copyResOk(version_file.c_str());
    }
    path_share = m_root_dir;
    path_user  = m_root_dir;
#endif
}
} //namespace porting

