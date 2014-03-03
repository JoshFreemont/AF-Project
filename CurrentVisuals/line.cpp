//
//  line.cpp
//  AF
//
//  Created by josh freemont on 27/02/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#include "line.h"

line::line()
{
    xStart=xEnd=yStart=yEnd = 0;
}

//values of x,y start/end are pixel values, NOT element values.
void line::addLine(int xStartInit, int yStartInit, int xEndInit, int yEndInit)
{
    if(xEndInit-xStartInit<0 && yEndInit-yStartInit<0)
    {xStart = xEndInit; xEnd = xStartInit; yStart = yEndInit; yEnd = yStartInit;}

    else{xStart = xStartInit; xEnd = xEndInit; yStart = yStartInit; yEnd = yEndInit;}
}

void line::drawLine(SDL_Surface* screen)
{
    int deltaY = yEnd-yStart;
    int deltaX = xEnd-xStart;
    double gradient = double(deltaY)/double(deltaX);
    int x,y;
    
    //if gradient is greater than 1.0, the line is "steep" and algoritm must look at steps in y
    if(gradient>1.0)
    {
        //work out y "invGradient"
        double invGradient = double(abs(xEnd-xStart))/double((abs)(yEnd-yStart));
        
        //work out the x value at each y step
        for(int ofsetY = 0; ofsetY<=(yEnd-yStart); ++ofsetY)
        {
            x=(int)(round)((double)ofsetY*invGradient) + xStart;//round to nearest x and plot.
            
            //plot x=ofsetY*invGradient + xStart, y=ofsetY + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + (ofsetY+yStart)*screen->pitch/4 + x;
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
    
    //if gradient is less than 1.0, the line is not "steep" and algorithm takes steps in x.
    else if (gradient>=0.0 && gradient<=1.0)
    {
        for(int ofsetX=0; ofsetX<=(xEnd-xStart); ++ofsetX)
        {
            y=(int)(round((double)ofsetX*gradient)) + yStart;
            
            //plot x=ofsetX + xStart, y=ofsetX*gradient + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + y*screen->pitch/4 + (ofsetX + xStart);
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
    
    else if(gradient<-1.0 && deltaY>=0)
    {
        //work out y "invGradient"
        double invGradient = 1.0/gradient;
        
        //work out the x value at each y step
        for(int ofsetY = 0; ofsetY<=(yEnd-yStart); ++ofsetY)
        {
            x=(int)(round)((double)ofsetY*invGradient) + xStart;//round to nearest x and plot.
            
            //plot x=ofsetY*invGradient + xStart, y=ofsetY + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + (ofsetY+yStart)*screen->pitch/4 + x;
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
    
    else if(gradient<-1.0 && deltaY<0)
    {
        //work out y "invGradient"
        double invGradient = 1.0/gradient;
        
        //work out the x value at each y step
        for(int ofsetY = 0; ofsetY>=(yEnd-yStart); --ofsetY)
        {
            x=(int)(round)((double)ofsetY*invGradient) + xStart;//round to nearest x and plot.
            
            //plot x=ofsetY*invGradient + xStart, y=ofsetY + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + (ofsetY+yStart)*screen->pitch/4 + x;
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
    
    else if(gradient>=-1.0 && gradient<=0.0 && deltaX>0)
    {
        for(int ofsetX=0; ofsetX<=(xEnd-xStart); ++ofsetX)
        {
            y=(int)(round((double)ofsetX*gradient)) + yStart;
            
            //plot x=ofsetX + xStart, y=ofsetX*gradient + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + y*screen->pitch/4 + (ofsetX + xStart);
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
    
    else if(gradient>=-1.0 && gradient<=0.0 && deltaX<0)
    {
        for(int ofsetX=0; ofsetX>=(xEnd-xStart); --ofsetX)
        {
            y=(int)(round((double)ofsetX*gradient)) + yStart;
            
            //plot x=ofsetX + xStart, y=ofsetX*gradient + yStart.
            Uint32* pixels = (Uint32*)screen->pixels;
            Uint32* pixel = pixels + y*screen->pitch/4 + (ofsetX + xStart);
            *pixel = SDL_MapRGB(screen->format, 255, 255, 255);
        }
    }
}


lineGraph::lineGraph(int xOriginInit, int yOriginInit)
{
    xOrigin=xOriginInit;
    yOrigin=yOriginInit;
    lineCoords.reserve(10000);
}

void lineGraph::addLine(int xStart, int yStart, int xEnd, int yEnd)
{
    lineCoords.push_back(new line);
    auto lineCoordsEnd = lineCoords.end();
    (*lineCoordsEnd)->addLine(xStart, yStart, xEnd, yEnd);
    return;
}

void lineGraph::drawLineGraph(SDL_Surface* screen)
{
    for(auto it = lineCoords.begin(); it != lineCoords.end(); ++it)
    {
        (*it)->drawLine(screen);
    }
}
