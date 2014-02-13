#include "main_functions.h"
#include "event_logic.h"
#include "histogram.h"
#include "state.h"
#include "mtrand.h"
#include "patch.h"
#include <ctime>
//TO DO:
//make experimental view when paused/beginning the program- use this view to set parameters for experiment.

//CODE INFORMATION:
//FREE MEMORY IN "exCoords" ARRAY
//MEMORY OF EXCITED CELLS: define vector containing all excited coordinates within the frame storage limit. within each of these "frame" vectors is stored the coordinates of each excited cell during the frame. The format of this storage is a long list of ints, where the first int is the i coordinate of the first registered excited cell, the second coordinate is the j coordinate of the first registered excited cell. The pattern continues s.t. the coordinates of the nth registered excited cell are: i=exCoords[frame][(n-1)*2], j=exCoords[frame][(n*2)-1].
//IF MEMLIMIT is reached then delete the oldest column. Find oldest column by making the erasure on the (frame%MEMLIMIT) frame.
//"EXCITED SCAN" ALGORITHM
//Note, always update the current frame= "FRAME" and always check the previous frame.
//EXCITED CELL PRINTING PROCEDURE + STATE DE-EXCITATION PROCESS: Use iterator to iterate back through the exCoords array from "FRAME-1" all the way to "FRAME-RP" (where applicable). Use the "current frame" and the scanned FRAME to determine refractoriness and use this to colour the pixels accordingly.

using namespace std;

int main(int argc, char** argv)
{
//******************************************Declarations and Initialization******************************************//
    //surface parameters
	const int FPS = 50;//initial frame rates
    const int S_WIDTH=1200;
    const int S_HEIGHT=400;
    
    //AF parameters
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    const double VER=0.35;//0.1477
    const double HOR=0.985;//0.97
    const int GRIDSIZE=400;
    const int G_HEIGHT=GRIDSIZE;
    const int G_WIDTH=GRIDSIZE;
    
    //frame and memory variables
    const int MEMLIMIT=50*RP;
    const int MAXFRAME=1000000;
    int FRAME=0;
    int cyclicNow=0;
    int cyclicOld=0;
    int cyclicBackRP=0;
    int RWD_FRAME=FRAME;
    int cyclicRwdNow=0;
    int cyclicRwdOld=0;
    int cyclicRwdBackRP=0;
    
    //other parameters
    MTRand drand(time(NULL));//seed rnd generator
    Uint32 start;
    
    //AF matrix declarations
    double inN [G_WIDTH][G_HEIGHT];
    double inE [G_WIDTH][G_HEIGHT];
    double inS [G_WIDTH][G_HEIGHT];
    double inW [G_WIDTH][G_HEIGHT];
    int state [G_WIDTH][G_HEIGHT];
    int state_update [G_WIDTH][G_HEIGHT];
    int exFrame [G_WIDTH][G_HEIGHT];
    int exFrameNew [G_WIDTH][G_HEIGHT];
    
    //AF matrix initialization
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            inN [k][l]=VER;
            inS [k][l]=VER;
            inE [k][l]=HOR;
            inW [k][l]=HOR;
            state[k][l]=0;
            state_update[k][l]=0;
            exFrame[k][l]=9999999;
            exFrameNew[k][l]=9999999;
        }
    }
    
    //Declare + initialize patches
    //square patches
    /*Spatch patch1 (40, 100, 100, S_WIDTH, S_HEIGHT, GRIDSIZE);
    Spatch patch2 (40, 200, 200, S_WIDTH, S_HEIGHT, GRIDSIZE);
    for(int k=0; k<40; ++k)
    {
        for(int l=0; l<40; ++l)
        {
            inN[k+80][l+80]=0.1;
            inS[k+80][l+80]=0.1;
            inN[k+180][l+180]=0.1;
            inS[k+180][l+180]=0.1;
        }
    }
    */
    //circle patches
    int radius1=20;
    double nuIn=0.1;
    int x_orig1=100;
    int y_orig1=100;
    int radius2=20;
    int x_orig2=250;
    int y_orig2=250;
    Cpatch patch3 (radius1, x_orig1, y_orig1, S_WIDTH, S_HEIGHT, GRIDSIZE);
    Cpatch patch4 (radius2, x_orig2, y_orig2, S_WIDTH, S_HEIGHT, GRIDSIZE);
    for(int k=-radius1; k<radius1; ++k)
    {
        for(int l=-radius1; l<radius1; ++l)
        {
            int r2= k*k + l*l;
            if(r2<=(radius1*radius1))
            {
                inN[k+x_orig1][l+y_orig1]=nuIn;
                inS[k+x_orig1][l+y_orig1]=nuIn;
            }
        }
    }
    
    for(int k=-radius2; k<radius2; ++k)
    {
        for(int l=-radius2; l<radius2; ++l)
        {
            int r2= k*k + l*l;
            if(r2<=(radius2*radius2))
            {
                inN[k+x_orig2][l+y_orig2]=nuIn;
                inS[k+x_orig2][l+y_orig2]=nuIn;
            }
        }
    }
    
    //excited cell memory declaration + memory reservation
    vector<int> coords;
    coords.reserve(GRIDSIZE*GRIDSIZE);
    vector<vector<int> >exCoords(MEMLIMIT, coords);
    
    //cell coordinates
    int i;
    int j;
    int i_W;
    int i_E;
    int j_N;
    int j_S;

    //measures of fibrillation
    int exCount=0;
    int discCount=0;
    int discCountNow;
    int h_DiscCountNow;
    int v_DiscCountNow;
    int LOWDISC=0;
    int HIGHDISC=SAP;
    int disc=0;
    int maxDiscId=0;
    int maxBreakId=0;
    
    //display + event classes *****make display controller class which automatically carries out positioning of elements on the screen.*****
    histogram histo (0, 0, 100 , 100, HIGHDISC, 500);
    state_display display(0, S_HEIGHT, GRIDSIZE, S_WIDTH, S_HEIGHT, GRIDSIZE, histo.getIsBinSelectArray(), histo.getIsAllSelect());
    general_logic logic(FPS);
    
    //Window initialization
    SDL_Init(SDL_INIT_EVERYTHING);//initialize sdl
	SDL_Surface *screen;//declare screen (sdl surface)
	screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE);
    
    //Christensen model initialization
    /*double defect_density = 0.3;
    double vertical_connections = 0.2;
    double firing_prob = 0.95;
    init_Kishan(&inN, &inE, &inS, &inW, GRIDSIZE, defect_density, vertical_connections, firing_prob);
    test(inN);*/
   
//******************************************AF Experimentation Loop******************************************//
    
    while(logic.getIsRunning())
    {
        start = SDL_GetTicks();
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            logic.handle_event(event);
            display.handle_event(event);
            
        }
    
        //LIVE LOOP.
        if(!logic.getIsPaused() && logic.getIsLive())//if not paused and is live then carry out loop.
        {
            //clear screen
            if(logic.getIsClearScreen())SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 20, 20, 20));
            
            //update frame and cyclic frame values
            cyclicNow = FRAME%MEMLIMIT;
            cyclicOld = (FRAME+MEMLIMIT-1)%MEMLIMIT;
            cyclicBackRP = (FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
            RWD_FRAME = FRAME;
        
            //free memory in exCoords array.
            if(FRAME>MEMLIMIT-1){exCoords[cyclicNow].erase(exCoords[cyclicNow].begin(), exCoords[cyclicNow].end());}

            //begin excited scan process to calculate new excited cells
            for(vector<int>::iterator it = exCoords[cyclicOld].begin(), it_end = exCoords[cyclicOld].end(); it != it_end; it+=2)
            {
                i= *it;
                j= *(it+1);
                j_N=(j-1+GRIDSIZE)%GRIDSIZE;
                i_E=i+1;
                j_S=(j+1)%GRIDSIZE;
                i_W=i-1;
                
                //west cell checks and excitation
                if(i_W>=0 && state[i_W][j]==0 && state_update[i_W][j] != RP && inE[i_W][j]>drand())
                {
                    update_arrays(state_update[i_W][j], exCoords[cyclicNow], exFrameNew[i_W][j], FRAME, RP, i_W, j);
                }
            
                //east cell checks and excitation
                if(i_E<GRIDSIZE && state[i_E][j]==0 && state_update[i_E][j] != RP && inW[i_E][j]>drand())
                {
                    update_arrays(state_update[i_E][j], exCoords[cyclicNow], exFrameNew[i_E][j], FRAME, RP, i_E, j);
                }
            
                //north cell checks and excitation
                if(state[i][j_N]==0 && state_update[i][j_N] != RP && inS[i][j_N]>drand())
                {
                    update_arrays(state_update[i][j_N], exCoords[cyclicNow], exFrameNew[i][j_N], FRAME, RP, i, j_N);
                }
            
                //south cell checks and excitation
                if(state[i][j_S]==0 && state_update[i][j_S] != RP && inN[i][j_S]>drand())
                {
                    update_arrays(state_update[i][j_S], exCoords[cyclicNow], exFrameNew[i][j_S], FRAME, RP, i, j_S);
                }
            }


            //de-excitation process for refractory cells AND printing process for all cells.
            for (int row = cyclicNow, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(), col_end = exCoords[row].end(); col != col_end; col+=2)
                {
                    //do not de-excite if state has just been excited in frame=cyclicNow.
                    if(row!=cyclicNow)--state_update[*col][*(col+1)];
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio, RP);
                }
            }
            
            //pacemaker algorithm.
            if(FRAME%SAP==0)pacemaker(screen, state_update[0], exCoords[cyclicNow], exFrameNew[0], FRAME, RP, GRIDSIZE, display);
    
            //update loop variables.
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    state[k][l]=state_update[k][l];
                    exFrame[k][l]=exFrameNew[k][l];
                }
            }
            
            //logic update
            if(logic.getIsStepping())logic.Pause();//framestep logic
            if(FRAME==MAXFRAME)logic.Stop();//stop at MAXFRAME
            ++FRAME;
        }
        
        

        //NON LIVE LOOP
        //use dummy frame RWD_FRAME.
        else if(!logic.getIsLive() && !logic.getIsPaused())
        {
            //if memory limit is reached- pause, set fwd and exit.
            if(RWD_FRAME%MEMLIMIT == (FRAME+RP+1)%MEMLIMIT && logic.getIsRWD())
            {
                logic.Pause();
                logic.setFWD();
                continue;
            }
            
            //check if rwd or fwd and change frame accordingly
            if(logic.getIsRWD() && RWD_FRAME>0)--RWD_FRAME;
            if(logic.getIsFWD())++RWD_FRAME;
            
            //update cyclic frames depending on new RWD_FRAME
            cyclicRwdNow = RWD_FRAME%MEMLIMIT;
            cyclicRwdOld = (RWD_FRAME+MEMLIMIT-1)%MEMLIMIT;
            cyclicRwdBackRP = (RWD_FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
            
            //clear screen
            if(logic.getIsClearScreen())SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 20, 20, 20));
        
            //printing loop for previous state data
            for (int row = cyclicRwdNow, rowEnd = cyclicRwdBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(RWD_FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(), col_end = exCoords[row].end(); col != col_end; col+=2)
                {
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio, RP);
                }
            }
            
            //logic update.
            if(logic.getIsStepping())logic.Pause();
            if(RWD_FRAME==FRAME-1)logic.GoLive();
        }
    
    //print patches
    //patch1.print(screen);
    //patch2.print(screen);
    patch3.print(screen);
    patch4.print(screen);
        
    //buffer
    if(1000/logic.getFrameRate() > SDL_GetTicks()-start) {SDL_Delay(1000/logic.getFrameRate()-(SDL_GetTicks()-start));}
    //render
    SDL_Flip(screen);
    }
    
//exit SDL when complete
SDL_Quit();

return 0;
}



















/*if(event.type==SDL_KEYDOWN)
 {
 if(event.key.keysym.sym==SDLK_a)
 {
 for(int k=-radius1; k<radius1; ++k)
 {
 for(int l=-radius1; l<radius1; ++l)
 {
 int r2= k*k + l*l;
 if(r2<=(radius1*radius1))
 {
 inN[k+x_orig1][l+y_orig1]=0.0;
 inS[k+x_orig1][l+y_orig1]=0.0;
 inE[k+x_orig1][l+y_orig1]=0.0;
 inW[k+x_orig1][l+y_orig1]=0.0;
 }
 }
 }
 
 for(int k=-radius2; k<radius2; ++k)
 {
 for(int l=-radius2; l<radius2; ++l)
 {
 int r2= k*k + l*l;
 if(r2<=(radius2*radius2))
 {
 inN[k+x_orig2][l+y_orig2]=0.0;
 inS[k+x_orig2][l+y_orig2]=0.0;
 inE[k+x_orig2][l+y_orig2]=0.0;
 inW[k+x_orig2][l+y_orig2]=0.0;
 }
 }
 }
 }
 }*/
