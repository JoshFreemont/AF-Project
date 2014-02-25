//
//  event_logic.cpp
//  sdltest
//
//  Created by josh freemont on 09/11/2013.
//  Copyright (c) 2013 josh freemont. All rights reserved.
//

#include "event_logic.h"

general_logic::general_logic (int FPS)
{
    IsPaused=false;
    IsSteppingForward=false;
    IsSteppingBackward=false;
    IsRunning=true;
    FrameRate=FPS;
    IsClearScreen=true;
    IsDefib=false;
    IsAblate=false;
}


void general_logic::PausePlay ()
{
    IsPaused=!IsPaused;
    IsSteppingBackward=false;
    IsSteppingForward=false;
}

void general_logic::Pause(){IsPaused=true;}

void general_logic::Continue(){IsPaused=false;}

void general_logic::StepForward ()
{
    IsSteppingForward=true;
    IsPaused=false;
}

void general_logic::StepBackward ()
{
    IsSteppingBackward=true;
    IsPaused=false;
}

void general_logic::Stop () {IsRunning=false;}

void general_logic::ResetAll()
{
    IsPaused=false;
    IsSteppingForward=false;
    IsSteppingBackward=false;
    IsRunning=true;
    IsDefib=false;
    IsAblate=false;
}

bool general_logic::getIsPaused(){return IsPaused;}

bool general_logic::getIsStepping()
{
    if(IsSteppingBackward || IsSteppingForward) {return true;}
    return false;
}

bool general_logic::getIsRunning(){return IsRunning;}

void general_logic::handle_event(SDL_Event& event)
{
    switch(event.type)
    {
        case SDL_QUIT:
            Stop();
            break;

        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
        {
            case SDLK_SPACE:
                PausePlay();
                break;

            case SDLK_RIGHT:
                StepForward();
                break;

            case SDLK_LEFT:
                StepBackward();
                break;

            case SDLK_UP:
                FrameRate += 5;
                break;

            case SDLK_DOWN:
                if(FrameRate>5)FrameRate -= 5;
                break;

            case SDLK_s:
                if(IsPaused){StepForward();}
                break;

            case SDLK_d:
                if(IsPaused){StepForward();}
                break;

            case SDLK_c:
                IsClearScreen=!IsClearScreen;
                break;

            case SDLK_f:
                IsDefib=!IsDefib;
                break;
            case SDLK_a:
                IsAblate=!IsAblate;
                break;
        }
            break;
    }
}

int general_logic::getFrameRate(){return FrameRate;}

bool general_logic::getIsClearScreen()
{
    return IsClearScreen;
}

bool general_logic::getIsDefib()
{
    return IsDefib;
}

bool general_logic::getIsAblate()
{
    return IsAblate;
}

void general_logic::resetIsAblate()
{
    IsAblate = false;
}

void general_logic::resetIsDefib()
{
    IsDefib = false;
}

