//
//  event_logic.h
//  sdltest
//
//  Created by josh freemont on 09/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#ifndef __sdltest__event_logic__
#define __sdltest__event_logic__

#include <iostream>
#include <SDL/SDL.h>

class general_logic
{
    bool IsPaused;
    bool IsSteppingForward;
    bool IsSteppingBackward;
    bool IsRWD;
    bool IsFWD;
    bool IsRunning;
    bool IsClearScreen;
    bool IsLive;
    int FrameRate;
    
public:
    general_logic (int);
    void PausePlay ();
    void Pause ();
    void StepForward();
    void StepBackward();
    void Stop ();
    void GoLive();
    bool getIsPaused();
    bool getIsStepping();
    bool getIsStepFor();
    bool getIsStepBack();
    bool getIsRunning();
    void handle_event(SDL_Event&);
    int getFrameRate();
    bool getIsClearScreen();
    bool getIsLive();
    bool getIsFWD();
    bool getIsRWD();
    void setFWD();
    
};






#endif /* defined(__sdltest__event_logic__) */
