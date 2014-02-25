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
    IsLive=true;
    IsRWD=false;
    IsFWD=true;
}


void general_logic::PausePlay ()
{
    IsPaused=!IsPaused;
    IsSteppingBackward=false;
    IsSteppingForward=false;
}

void general_logic::Pause(){IsPaused=true;}

void general_logic::StepForward ()
{
    IsSteppingForward=true;
    IsPaused=false;
}


void general_logic::StepBackward ()
{
    IsSteppingBackward=true;
    IsLive=false;
    IsPaused=false;
}


void general_logic::Stop () {IsRunning=false;}


void general_logic::GoLive()
{
    IsLive=true;
    if(IsSteppingForward)IsPaused=true;
    else IsPaused=false;
}

bool general_logic::getIsPaused(){return IsPaused;}

bool general_logic::getIsStepping()
{
    if(IsSteppingBackward || IsSteppingForward) {return true;}
    return false;
}

bool general_logic::getIsStepFor()
{
    return IsSteppingForward;
}

bool general_logic::getIsStepBack()
{
    return IsSteppingBackward;
}

bool general_logic::getIsRunning(){return IsRunning;}

bool general_logic::getIsClearScreen(){return IsClearScreen;}

bool general_logic::getIsLive(){return IsLive;}

bool general_logic::getIsFWD(){return IsFWD;}

bool general_logic::getIsRWD(){return IsRWD;}

void general_logic::setFWD()
{
    IsFWD=true;
    IsRWD=false;
    return;
}

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
                IsFWD=true;
                IsRWD=false;
                break;
                
            case SDLK_LEFT:
                StepBackward();
                IsRWD=true;
                IsFWD=false;
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

        }
            break;
    }
}

int general_logic::getFrameRate(){return FrameRate;}






