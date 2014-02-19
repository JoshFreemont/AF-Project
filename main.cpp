#include "main_functions.h"
#include "event_logic.h"
#include "histogram.h"
#include "state.h"
#include "mtrand.h"
#include "patch.h"
#include "array2D.h"
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
    const int S_WIDTH=900;
    const int S_HEIGHT=600;
    
    //AF parameters
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    const double VER=0.35;//0.1477
    const double HOR=0.985;//0.97
    const int GRIDSIZE=300;
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
    array2D<double> inN (G_WIDTH,G_HEIGHT);
    array2D<double> inE (G_WIDTH,G_HEIGHT);
    array2D<double> inS (G_WIDTH,G_HEIGHT);
    array2D<double> inW (G_WIDTH,G_HEIGHT);
    array2D<int> state (G_WIDTH,G_HEIGHT);
    array2D<int> state_update (G_WIDTH,G_HEIGHT);
    array2D<int> exFrame (G_WIDTH,G_HEIGHT);
    array2D<int> exFrameNew (G_WIDTH,G_HEIGHT);
    
    //AF matrix initialization
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            inN(k,l)=VER;
            inS(k,l)=VER;
            inE(k,l)=HOR;
            inW(k,l)=HOR;
            state(k,l)=0;
            state_update(k,l)=0;
            exFrame(k,l)=9999999;
            exFrameNew(k,l)=9999999;
        }
    }
    
    //rotor identification variables
    array2D <int> rotorCellsInit (G_WIDTH,G_HEIGHT,0);
    vector <array2D <int> > rotorCells (MEMLIMIT, rotorCellsInit);//Stores whether a cell is a "rotor" cell or not
    
    array2D <pair <int,int> > excitedBy(G_WIDTH,G_HEIGHT); //Stores coords of cell which excited current cell
    for (int k=0;k<G_WIDTH;k++)
    {
        for (int l=0;l<G_HEIGHT;l++)
        {
            excitedBy(k,l) = make_pair(k,l);
        }
    }
    const int rotorLengthLimit = 2*RP;
    pair <int,int> tempCycleArray[rotorLengthLimit+1];//Temporary array to store values
    //to detect rotors
    int cycleStart = 0;
    int cycleLength = 0;
    int tortoise, hare;

    //Patches
    int size=40;
    double nuIn=0.1;
    Spatch patch1 (size, 100, 100, S_WIDTH, S_HEIGHT, GRIDSIZE, inN, inS, inE, inW, nuIn);
    Spatch patch2 (size, 200, 200, S_WIDTH, S_HEIGHT, GRIDSIZE, inN, inS, inE, inW, nuIn);
    
    
    //excited cell memory declaration + memory reservation
    vector<int> coords;
    coords.reserve(GRIDSIZE*GRIDSIZE);
    vector<vector<int> >exCoords(MEMLIMIT, coords);
    vector<vector<int> >rotorCoords(MEMLIMIT, coords);
    
    //cell coordinates
    int i;
    int j;
    int i_W;
    int i_E;
    int j_N;
    int j_S;
    
    //measures of fibrillation
    int HIGHDISC=SAP;
    
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
            patch1.handleEvent(event);
            patch2.handleEvent(event);
            
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
            
            //free memory in exCoords and rotorCells arrays.
            if(FRAME>MEMLIMIT-1)
            {
                exCoords[cyclicNow].erase(exCoords[cyclicNow].begin(), exCoords[cyclicNow].end());
                rotorCells[cyclicNow] = rotorCellsInit;
            }
            
            
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
                if(i_W>=0 && state(i_W,j)==0 && state_update(i_W,j) != RP && inE(i_W,j)>drand())
                {
                    update_arrays(state_update(i_W,j), exCoords[cyclicNow], exFrameNew(i_W,j), FRAME, RP, i_W, j);
                    excitedBy(i_W,j) = make_pair(i,j);
                }
                
                //east cell checks and excitation
                if(i_E<GRIDSIZE && state(i_E,j)==0 && state_update(i_E,j) != RP && inW(i_E,j)>drand())
                {
                    update_arrays(state_update(i_E,j), exCoords[cyclicNow], exFrameNew(i_E,j), FRAME, RP, i_E, j);
                    excitedBy(i_E,j) = make_pair(i,j);
                }
                
                //north cell checks and excitation
                if(state(i,j_N)==0 && state_update(i,j_N) != RP && inS(i,j_N)>drand())
                {
                    update_arrays(state_update(i,j_N), exCoords[cyclicNow], exFrameNew(i,j_N), FRAME, RP, i, j_N);
                    excitedBy(i,j_N) = make_pair(i,j);
                }
                
                //south cell checks and excitation
                if(state(i,j_S)==0 && state_update(i,j_S) != RP && inN(i,j_S)>drand())
                {
                    update_arrays(state_update(i,j_S), exCoords[cyclicNow], exFrameNew(i,j_S), FRAME, RP, i, j_S);
                    excitedBy(i,j_S) = make_pair(i,j);
                }
            }
            
            //check for rotors
            if (display.getIsRotorView())
            {
                for (auto col = exCoords[cyclicNow].begin(); col!= exCoords[cyclicNow].end(); col+=2)
                {
                    //First, fill up temp array to check for cycles
                    tempCycleArray[0] = excitedBy(*col,*(col+1));
                    
                    for (int k=1;k<rotorLengthLimit;k++)
                    {
                        tempCycleArray[k]=excitedBy(tempCycleArray[k-1].first,tempCycleArray[k-1].second);
                    }
                    
                    //Check if cell is already identified as being a rotor cell
                    //continue to next iteration of loop if yes
                    if (rotorCells[cyclicNow](tempCycleArray[rotorLengthLimit-1].first,tempCycleArray[rotorLengthLimit-1].second)) continue;
                    
                    //Next, check for repeats
                    cycleLength=0;
                    for(tortoise=rotorLengthLimit-1; tortoise>=RP; tortoise--)
                    {
                        for(hare=tortoise-RP; hare>=0; hare--)
                        {
                            if((tempCycleArray[tortoise]==tempCycleArray[hare]) &&
                               (tempCycleArray[hare+1]!=tempCycleArray[hare]))
                            {
                                cycleStart=hare;
                                cycleLength=tortoise-hare;
                                tortoise=-1;
                                break;
                            }
                        }
                    }
                    
                    //if (cycleLength>2)
                    {
                        for (int m=cycleStart, k=0;k<cycleLength;k++)
                        {
                            rotorCells[cyclicNow](tempCycleArray[m+k].first,tempCycleArray[m+k].second)
                            = state_update(tempCycleArray[m+k].first,tempCycleArray[m+k].second)+1;// +1 is for resting cells st. they register in the rotor.
                            
                            rotorCoords[cyclicNow].push_back(tempCycleArray[m+k].first);
                            rotorCoords[cyclicNow].push_back(tempCycleArray[m+k].second);
                            rotorCoords[cyclicNow].push_back(state_update(tempCycleArray[m+k].first,tempCycleArray[m+k].second)+1);
                        }
                    }
                    //else continue;
                }
            }
            
            
            //de-excitation process for refractory cells AND printing process for all cells.
            for (int row = cyclicNow, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(), col_end = exCoords[row].end(); col != col_end; col+=2)
                {
                    //do not de-excite if state has just been excited in frame=cyclicNow.
                    if(row!=cyclicNow)--state_update(*col,*(col+1));
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio, RP);
                }
            }
            //printing for rotors
            for(vector<int>::iterator col = rotorCoords[cyclicNow].begin(), col_end = rotorCoords[cyclicNow].end(); col != col_end; col+=3)
            {
                display.rotor_putpixel(screen, *col, *(col+1), double(*(col+2)/RP), RP);
            }
            
            //pacemaker algorithm.
            if(FRAME%SAP==0)
            {
                pacemaker(screen, state_update, exCoords[cyclicNow], exFrameNew, FRAME, RP, GRIDSIZE, display, excitedBy);
            }
            
            //update loop variables.
            state=state_update;
            exFrame=exFrameNew;
            
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
            //printing loop for rotor data
            for(vector<int>::iterator col = rotorCoords[cyclicRwdNow].begin(), col_end = rotorCoords[cyclicRwdNow].end(); col != col_end; col+=3)
            {
                display.rotor_putpixel(screen, *col, *(col+1), double(*(col+2)/RP), RP);
            }
            
            //logic update.
            if(logic.getIsStepping())logic.Pause();
            if(RWD_FRAME==FRAME-1)logic.GoLive();
        }
        
        //print patches
        patch1.print(screen);
        patch2.print(screen);

        
        //buffer
        if(1000/logic.getFrameRate() > SDL_GetTicks()-start) {SDL_Delay(1000/logic.getFrameRate()-(SDL_GetTicks()-start));}
        //render
        SDL_Flip(screen);
    }
    
    //exit SDL when complete
    SDL_Quit();
    
    return 0;
}





