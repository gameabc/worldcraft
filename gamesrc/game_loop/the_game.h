//
//  the_game.h
//  Mine
//
//  Created by Luke on 10/10/13.
//  Copyright (c) 2013 luke. All rights reserved.
//

#ifndef __Mine__the_game__
#define __Mine__the_game__

#include "irrlicht.h"

using namespace irr;
typedef unsigned long Uint32;
class Game
{
public:
    Game();
    ~Game();
    
    void abort()
    {
        m_abort = true;
    }
    void initGame(IrrlichtDevice* dev);
    void quitGame();
    void updateGame();
    
    void setUpdateGame(bool b)
    {
        m_update_game = b;
    }
    
    void pauseGame();
    void resumeGame();
    bool isInGame()
    {
        return m_update_game;
    }
private:
    
    void shareInit(IrrlichtDevice* dev);
    IrrlichtDevice* mDevice;
    bool m_abort;
    bool m_update_game;
    int      m_frame_count;
    Uint32   m_curr_time;
    Uint32   m_prev_time;
    float    getLimitedDt();
};

extern Game* g_the_game;
#endif /* defined(__Mine__the_game__) */
