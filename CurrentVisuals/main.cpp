#include "main_functions.h"
#include "event_logic.h"
#include "histogram.h"
#include "state.h"
#include "mtrand.h"
#include "patch.h"
#include "array2D.h"
#include "line.h"
#include "network.h"
#include "data_structs.h"
#include <ctime>
#include <unordered_map>

//Decide whether rotor edges should be formed on a probabilistic basis- ie. can a rotor be formed by two independent rotors?



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
    const int S_WIDTH=1250;
    const int S_HEIGHT=450;
    
    //AF parameters
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    const double VER=0.13;//0.1477
    const double HOR=0.985;//0.97
    const int GRIDSIZE=200;
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
    Uint32 start;//timer.
    
    //AF matrix declarations
    array2D<double> inN (G_WIDTH,G_HEIGHT);
    array2D<double> inE (G_WIDTH,G_HEIGHT);
    array2D<double> inS (G_WIDTH,G_HEIGHT);
    array2D<double> inW (G_WIDTH,G_HEIGHT);
    array2D<int> state (G_WIDTH,G_HEIGHT);//stores state of cell in old frame
    array2D<int> state_update (G_WIDTH,G_HEIGHT);//stores update of state of cell.
    array2D<int> exFrame (G_WIDTH,G_HEIGHT);//stores frame when state was last excited in old frame
    array2D<int> exFrameNew (G_WIDTH,G_HEIGHT);//stores update of frame when state was last excited.
    array2D <pair <int,int> > excitedBy(G_WIDTH,G_HEIGHT); //Stores coords of cell which excited current cell
    
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
            excitedBy(k,l) = make_pair(k,l);
        }
    }
    
    //rotor variables
    const int rotorLengthLimit = 2*RP;
    array2D <bool> isRotorInit (G_WIDTH,G_HEIGHT,false);
    vector <array2D <bool> > isRotor (MEMLIMIT, isRotorInit);//Stores whether a cell is a "rotor" cell or not
    
    //rotor id variables
    array2D<int> activeRotorId (G_WIDTH, G_HEIGHT, 0);//stores all rotor ids for active rotors.
    array2D<int> inheritedRotorId (G_WIDTH, G_HEIGHT, 0);//stores all rotor ids for all cells, whether in a rotor or not.
    unordered_map<int, int> tempRotorIdFrequency;//stores temp frequency count of ids
    double rotorIdThresh = 0.01, tempRotorIdRatio;//threshold for rotor id inheritence. 1.0=100% of current rotor cells must have same id.
    int maxFreq;//counts the maximum frequency rotor id within rotor
    int tempRotorId;//creates a temp rotor id variable.
    int maxRotorId=-1;//global maximum rotor id counter.
    vector<int> rotorIdDuration;
    vector< pair <int,int> > rotorIdAverageCoords;
    int iSum, jSum, iMean, jMean;
    int maxIdNode = 10000;
    
    //rotorIdNetwork
    network rotorIdNetwork(maxIdNode);
    
    
    
    //cycle data variables.
    pair <int,int> tempCycleArray[rotorLengthLimit+1];//Temporary array to store values to detect rotors
    int cycleStart = 0;
    int cycleLength = 0;
    int tortoise, hare;
    bool isCycle;

    //Patches
    int size=40;
    double nuIn=0.05;
    Spatch patch1 (size, 50, 50, 400, 400, GRIDSIZE, inN, inS, inE, inW, nuIn, false);
    Spatch patch2 (size, 150, 150, 400, 400, GRIDSIZE, inN, inS, inE, inW, nuIn, false);
    
    
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
    int HIGH_DURATION=1000;

    //display + event classes *****make display controller class which automatically carries out positioning of elements on the screen.*****
    histogram duration (450, 400, 750, 350, HIGH_DURATION, HIGH_DURATION/2, 50);
    histogram histo (0, 0, 100 , 100, HIGHDISC, HIGHDISC, 500);
    state_display display(0, S_HEIGHT, GRIDSIZE, 400, 400, GRIDSIZE, histo.getIsBinSelectArray(), histo.getIsAllSelect());
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
                isRotor[cyclicNow] = isRotorInit;
                rotorCoords[cyclicNow].erase(rotorCoords[cyclicNow].begin(), rotorCoords[cyclicNow].end());
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
                    inheritedRotorId(i_W, j)=inheritedRotorId(i,j);
                }
                
                //east cell checks and excitation
                if(i_E<GRIDSIZE && state(i_E,j)==0 && state_update(i_E,j) != RP && inW(i_E,j)>drand())
                {
                    update_arrays(state_update(i_E,j), exCoords[cyclicNow], exFrameNew(i_E,j), FRAME, RP, i_E, j);
                    excitedBy(i_E,j) = make_pair(i,j);
                    inheritedRotorId(i_E, j)=inheritedRotorId(i,j);
                }
                
                //north cell checks and excitation
                if(state(i,j_N)==0 && state_update(i,j_N) != RP && inS(i,j_N)>drand())
                {
                    update_arrays(state_update(i,j_N), exCoords[cyclicNow], exFrameNew(i,j_N), FRAME, RP, i, j_N);
                    excitedBy(i,j_N) = make_pair(i,j);
                    inheritedRotorId(i, j_N)=inheritedRotorId(i,j);
                }
                
                //south cell checks and excitation
                if(state(i,j_S)==0 && state_update(i,j_S) != RP && inN(i,j_S)>drand())
                {
                    update_arrays(state_update(i,j_S), exCoords[cyclicNow], exFrameNew(i,j_S), FRAME, RP, i, j_S);
                    excitedBy(i,j_S) = make_pair(i,j);
                    inheritedRotorId(i, j_S)=inheritedRotorId(i,j);

                }
            }
            
            //check for rotors
            if (display.getIsRotorView())
            {
                for (auto col = exCoords[cyclicNow].begin(); col!= exCoords[cyclicNow].end(); col+=2)
                {
                    //First, fill up temp array to check for cycles
                    tempCycleArray[0] = excitedBy(*col,*(col+1));
                    int parentRotorId = inheritedRotorId(*col, *(col+1));
                    
                    
                    for (int k=1; k<rotorLengthLimit; ++k)
                    {
                        tempCycleArray[k]=excitedBy(tempCycleArray[k-1].first,tempCycleArray[k-1].second);
                    }
                    
                    //Check if cell is already identified as being a rotor cell
                    //continue to next iteration of loop if yes
                    if (isRotor[cyclicNow](tempCycleArray[rotorLengthLimit-1].first,tempCycleArray[rotorLengthLimit-1].second)) continue;
                    if (abs(tempCycleArray[rotorLengthLimit-1].second - *(col+1)) > 15) continue;
                        
                        
                    //Next, check for repeats
                    cycleLength=0;
                    isCycle=false;
                    for(tortoise=rotorLengthLimit-1; tortoise>=RP; --tortoise)
                    {
                        for(hare=tortoise-RP; hare>=0; hare--)
                        {
                            if((tempCycleArray[tortoise]==tempCycleArray[hare]) &&
                               (tempCycleArray[hare+1]!=tempCycleArray[hare]))
                            {
                                cycleStart=hare;
                                cycleLength=tortoise-hare;
                                tortoise=-1;
                                isCycle=true;
                                break;
                            }
                        }
                    }
                    
                    //if no cycle then continue through next iteration.
                    if(!isCycle)continue;
                    
                    //Rotor Id allocation
                    //fill rotor frequency map with previous rotor ids for rotor cells
                    for (int m=cycleStart, k=0;k<cycleLength;++k)
                    {
                        int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second;
                        tempRotorIdFrequency[activeRotorId(i,j)]++;
                    }
                    
                    //now find highest rotor id frequency in map
                    maxFreq=0;
                    for(unordered_map<int, int>::iterator it = tempRotorIdFrequency.begin(); it != tempRotorIdFrequency.end(); ++it)
                    {
                        if(it->second > maxFreq && it->first != 0)
                        {
                            maxFreq = it->second;
                            tempRotorId = it->first;
                        }
                    }
                    tempRotorIdFrequency.clear();//clear rotor id frequency map after use.
                    
                    //now check if tempRotorIdRatio > rotorIdThresh if so then consider rotor the same as tempRotorIdRatio.
                    //create new id if all current ids are zeros.
                    tempRotorIdRatio=(double)maxFreq/cycleLength;
                    iSum=0;
                    jSum=0;
                    if(tempRotorIdRatio >= rotorIdThresh && tempRotorId !=0)
                    {
                        //put data in memory, [0]=i, [1]=j, [2]=state, [3]=rotorid
                        for (int m=cycleStart, k=0;k<cycleLength;++k)
                        {
                            int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second, state = state_update(i,j)+1;

                            rotorCoords[cyclicNow].push_back(i);
                            rotorCoords[cyclicNow].push_back(j);
                            rotorCoords[cyclicNow].push_back(state);
                            rotorCoords[cyclicNow].push_back(tempRotorId);
                            activeRotorId(i,j)=tempRotorId;
                            isRotor[cyclicNow](i,j)=true;
                            iSum += i;
                            jSum += j;
                            inheritedRotorId(i,j)=tempRotorId;
                        }
                        //add one to id duration counter.
                        rotorIdDuration[tempRotorId]++;
                        
                        //calculate and update rotorIdAverageCoords array.
                        /*iMean = (int)(iSum/cycleLength + 0.5);
                        jMean = (int)(jSum/cycleLength + 0.5);
                        rotorIdAverageCoords[tempRotorId] = make_pair(iMean, jMean);*/
                        
                        //do not add edge to rotorId network as this rotor is considered to be the same as tempRotorId.
                    }
                    
                    else
                    {
                        //add to max rotor id counter and assign this new id to rotor.
                        maxRotorId++;
                        
                        //put data in memory, [0]=i, [1]=j, [2]=state, [3]=rotorid
                        for (int m=cycleStart, k=0;k<cycleLength;++k)
                        {
                            int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second, state = state_update(i,j)+1;
                            
                            rotorCoords[cyclicNow].push_back(i);
                            rotorCoords[cyclicNow].push_back(j);
                            rotorCoords[cyclicNow].push_back(state);
                            rotorCoords[cyclicNow].push_back(maxRotorId);
                            activeRotorId(i,j)=maxRotorId;
                            isRotor[cyclicNow](i,j)=true;
                            iSum += i;
                            jSum += j;
                            inheritedRotorId(i,j)=maxRotorId;
                        }
                        //add one to id duration counter
                        rotorIdDuration.push_back(1);
                        
                        //calculate and update rotorIdAverageCoords array.
                        /*iMean = (int)(iSum/cycleLength + 0.5);
                        jMean = (int)(jSum/cycleLength + 0.5);
                        rotorIdAverageCoords.push_back(make_pair(iMean, jMean));*/
                        
                        
                        //Update rotorIdNetwork
                        //add node at maxRotorId
                        rotorIdNetwork.addNode(maxRotorId);
                        
                        //add edge between parentRotorId and inheritedRotorId if parentRotorId != 0.
                        if(parentRotorId)
                        {
                            rotorIdNetwork.addEdge(parentRotorId, maxRotorId);
                        }
                    }
                }
            }
            
            //update activeRotorid array to contain data for active rotors.
            for(int i=0; i<G_WIDTH; ++i)
            {
                for(int j=0; j<G_HEIGHT; ++j)
                {
                    if(!isRotor[cyclicNow](i,j)) activeRotorId(i,j) = 0;
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
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio);
                    //display.rotor_inherit_putpixel(screen, *col, *(col+1), inheritedRotorId(*col, *(col+1)), RP_ratio);
                    
                }
            }
            //printing for rotors
            for(vector<int>::iterator col = rotorCoords[cyclicNow].begin(), col_end = rotorCoords[cyclicNow].end(); col != col_end; col+=4)
            {
                display.rotor_putpixel(screen, *col, *(col+1), ((double)*(col+2)/(double)RP));
                //display.rotor_id_putpixel(screen, *col, *(col+1), *(col+3), ((double)*(col+2))/RP);
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
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio);
                }
            }
            
            //printing loop for rotor data
            for(vector<int>::iterator col = rotorCoords[cyclicRwdNow].begin(), col_end = rotorCoords[cyclicRwdNow].end(); col != col_end; col+=4)
            {
                display.rotor_putpixel(screen, *col, *(col+1), ((double)*(col+2)/RP));
                //display.rotor_id_putpixel(screen, *col, *(col+1), *(col+3), ((double)*(col+2))/RP);
            }
            
            //logic update.
            if(logic.getIsStepping())logic.Pause();
            if(RWD_FRAME==FRAME-1)logic.GoLive();
        }
        
        //print patches
        patch1.print(screen);
        patch2.print(screen);
        
        //add data, print and reset histogram
        for(auto it=rotorIdDuration.begin(); it!=rotorIdDuration.end(); ++it) duration.add_point(*it);
        duration.print_axes(screen);
        duration.print_histogram(screen);
        duration.reset_frequency();

        
        if(1000/logic.getFrameRate() > SDL_GetTicks()-start) {SDL_Delay(1000/logic.getFrameRate()-(SDL_GetTicks()-start));}//buffer
        SDL_Flip(screen);//render
    
    }
    
    //exit SDL when complete
    SDL_Quit();
    
    return 0;
}





