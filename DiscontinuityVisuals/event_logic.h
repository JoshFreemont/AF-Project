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
    bool IsPaused, IsSteppingForward, IsSteppingBackward, IsRunning;
    bool IsClearScreen;
    bool IsDefib, IsAblate;
    int FrameRate;

public:

    general_logic (int);
    void PausePlay ();
    void Pause ();
    void Continue();
    void PlaybackAsBefore();
    void StepForward();
    void StepBackward();
    void Stop ();
    void ResetAll();
    bool getIsPaused();
    bool getIsStepping();
    bool getIsRunning();
    void handle_event(SDL_Event&);
    bool getIsClearScreen();
    int getFrameRate();
    bool getIsDefib();
    bool getIsAblate();
    void resetIsAblate();
    void resetIsDefib();


};






#endif /* defined(__sdltest__event_logic__) */
