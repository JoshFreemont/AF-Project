#include "main_functions.h"
#include "event_logic.h"
#include "histogram.h"
#include "state.h"
#include "mtrand.h"
#include <ctime>
#include <cmath>
#include "graph.h"

#define PI_ 3.1416
#define E_ 2.7183f

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
    //SURFACE PARAMETERS
	const int FPS = 50;//initial frame rate
    const int ZOOM=4;
    const int S_WIDTH=1250;
    const int S_HEIGHT=600;
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    const double VER=0.1;
    const double HOR=0.985;
    const int GRIDSIZE=600;
    const int G_HEIGHT=GRIDSIZE;
    const int G_WIDTH=GRIDSIZE;
    const int MEMLIMIT=RP+1;
    const int MAXFRAME=1000000;
    int FRAME=0;
    int cyclicNow=0;
    int cyclicOld=0;
    int cyclicBackRP=0;
    MTRand drand(time(NULL));
    Uint32 start;

    //DEFINE AFIB MATRICIES
    int state [G_WIDTH][G_HEIGHT];
    int state_update [G_WIDTH][G_HEIGHT];
    double inN [G_WIDTH][G_HEIGHT];
    double inS [G_WIDTH][G_HEIGHT];
    double inE [G_WIDTH][G_HEIGHT];
    double inW [G_WIDTH][G_HEIGHT];

//-----VECTOR FOR ABLATION COORDS-----//
    vector <int> ablationCoords;
//------------------------------------//

//Initialising the "normal" cells
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            state[k][l]=0;
            state_update[k][l]=0;
            inN[k][l]=0.25;
            inS[k][l]=0.25;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
        }
    }

    //Circular patch of fibrillating stuff
    int radius = 50;
    int xcentre = 300;
    int ycentre = 300;
    for(int k=xcentre-radius; k<xcentre+radius; ++k)
    {
        for(int l=ycentre-radius; l<xcentre+radius; ++l)
        {
            if (1)
            {
            inN[k][l]=VER;
            inS[k][l]=VER;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            ablationCoords.push_back(k);
            ablationCoords.push_back(l);
            }
        }
    }

    //Circular patch of fibrillating stuff
    radius = 30;
    xcentre = 100;
    ycentre = 100;
    for(int k=xcentre-radius; k<xcentre+radius; ++k)
    {
        for(int l=ycentre-radius; l<xcentre+radius; ++l)
        {
            if ((k-xcentre)*(k-xcentre)+(l-ycentre)*(l-ycentre)<=radius*radius)
            {
            inN[k][l]=VER;
            inS[k][l]=VER;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            ablationCoords.push_back(k);
            ablationCoords.push_back(l);
            }
        }
    }


    /*Gaussian Patch
    double sigma = 125;
    double preconstant = 1/(sqrt(2*PI_)*sigma);
    for(int k=100; k<500; ++k)
    {
        for(int l=100; l<500; ++l)
        {
            inN[k][l]=0.6-0.5*pow(E_,-(((k-300)*(k-300)+(l-300)*(l-300))/(2*sigma*sigma)));
            inS[k][l]=0.6-0.5*pow(E_,-(((k-300)*(k-300)+(l-300)*(l-300))/(2*sigma*sigma)));
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            ablationCoords.push_back(k);
            ablationCoords.push_back(l);
        }
    }*/

    /*//Initialise kishan style
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {


            inN[k][l]=0;
            inS[k][l]=0;
            inE[k][l]=1;
            inW[k][l]=1;

        }
    }

    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            if (0.3 > drand())
            {
            inE[k][l]=0.95;
            inW[k][l]=0.95;
            }
        }
    }


    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            if (0.075 > drand())
            {
            inN[k][l]=inE[k][l];
            inS[k][(l-1+G_HEIGHT)%G_HEIGHT]=inE[k][(l-1+G_HEIGHT)%G_HEIGHT];
            }
        }
    }*/

    vector<vector<int> > exFrame (GRIDSIZE, vector<int>(GRIDSIZE, 9999999));
    vector<vector<int> > exFrameNew = exFrame;
    vector<vector<int> > discId (GRIDSIZE, vector<int> (GRIDSIZE, 0));

    //MEMORY OF EXCITED CELLS
    vector<int> coords;
    coords.reserve(GRIDSIZE*GRIDSIZE);
    vector<vector<int> >exCoords(MEMLIMIT, coords);

    //CELL COORDINATES
    int i;
    int j;
    int i_W;
    int i_E;
    int j_N;
    int j_S;

    //MEASURES OF FIBRILLATION
    int exCount=0;
    int discCount=0;
    int discCountNow;
    int h_DiscCountNow;
    int v_DiscCountNow;
    int LOWDISC=0;
    int HIGHDISC=SAP;
    int disc=0;
    int maxDiscId=0;

    //DISPLAY
    int x_ofset = 50, y_ofset = 20;
    int x_origin = GRIDSIZE+x_ofset;
    int x_length = S_WIDTH-2*x_ofset-GRIDSIZE;
    int y_origin = S_HEIGHT-y_ofset;
    int y_length = S_HEIGHT-2*y_ofset;
    histogram histo (x_origin, y_origin, x_length , y_length, HIGHDISC, 500);
    state_display display(0, GRIDSIZE, GRIDSIZE, histo.getIsBinSelectArray(), histo.getIsAllSelect());


    //INITIALIZE SCREEN
    SDL_Init(SDL_INIT_EVERYTHING);//initialize sdl
	SDL_Surface *screen;//declare screen (sdl surface)
	screen = SDL_SetVideoMode(S_WIDTH, S_HEIGHT, 32, SDL_SWSURFACE);

    //EVENT PARAMETERS
    general_logic logic(FPS);

    //INITIALIZE KISHAN'S METHOD
    /*double defect_density = 0.3;
     double vertical_connections = 0.2;
     double firing_prob = 0.95;
     initialise_Kishan(inN,inE,inS,inW,GRIDSIZE,defect_density,vertical_connections,firing_prob);*/

    //-------------------------MAIN LOOP START-------------------------------//

    while(logic.getIsRunning())
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            histo.handle_event(event);
            logic.handle_event(event);
            display.handle_event(event);

        }

        //ABLATION (Ablate with 'a')
        if (logic.getIsAblate())
        {
            for (auto currentAblate= ablationCoords.begin();
                currentAblate != ablationCoords.end(); currentAblate++)
                {
                    int xCoord = *currentAblate;
                    currentAblate++;
                    int yCoord = *currentAblate;
                    state[xCoord][yCoord]=0;
                    state_update[xCoord][yCoord]=0;
                    inN[xCoord][yCoord]=0.0;
                    inS[xCoord][yCoord]=0.0;
                    inE[xCoord][yCoord]=0.0;
                    inW[xCoord][yCoord]=0.0;
                    //Search for cells with coords in ablated areas
                    //and make them unexcited

                    for (int m=0; m<MEMLIMIT; m++)
                        {
                    //If coords found in exCoord vector, erase
                        for (auto ablateSearch = exCoords[m].begin();
                            ablateSearch!= exCoords[m].end();ablateSearch+= 2)
                            {
                            if (*ablateSearch == xCoord && *(ablateSearch+1) == yCoord)
                                {
                                exCoords[m].erase(ablateSearch, (ablateSearch+2));
                                break;
                                }
                            }
                        }
                }
            logic.resetIsAblate();
        }

        //DEFIBRILLATION (Clear everything with 'f')
        if (logic.getIsDefib())
        {
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    state[k][l]=0;
                    state_update[k][l]=0;
                    for (int m=0; m<MEMLIMIT; m++)
                        {exCoords[m].erase(exCoords[m].begin(),
                                                   exCoords[m].end());}
                }
            }
            logic.resetIsDefib();
        }


    //---------------------------AFIB LOGIC START----------------------------//
        if(!logic.getIsPaused())//if not paused then carry out loop.
        {
            if(logic.getIsClearScreen())SDL_FillRect(screen, NULL, 0);//Clear Screen
            start = SDL_GetTicks();//Start Timer
            cyclicNow = FRAME%MEMLIMIT;
            cyclicOld = (FRAME+MEMLIMIT-1)%MEMLIMIT;
            cyclicBackRP = (FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
            exCount=0;
            discCountNow=0;
            h_DiscCountNow=0;
            v_DiscCountNow=0;

            //FREE MEMORY IN "exCoords" ARRAY
            if(FRAME>MEMLIMIT-1){exCoords[cyclicNow].erase(exCoords[cyclicNow].begin(), exCoords[cyclicNow].end());}


            //"EXCITED SCAN" ALGORITHM
            for(vector<int>::iterator it = exCoords[cyclicOld].begin(),
                it_end = exCoords[cyclicOld].end(); it != it_end; it+=2)
            {
                i= *it;
                j= *(it+1);
                j_N=(j-1+GRIDSIZE)%GRIDSIZE;
                i_E=i+1;
                j_S=(j+1)%GRIDSIZE;
                i_W=i-1;

                //WEST CELL
                if(i_W>=0 && state[i_W][j]==0 && state_update[i_W][j] != RP && inE[i_W][j]>drand())
                {
                    update_arrays(state_update[i_W][j], exCoords[cyclicNow], exFrameNew[i_W][j], FRAME, RP, i_W, j);
                    disc = returnDisc(exFrame[i][j], exFrame[i_W][j], LOWDISC, HIGHDISC);
                    display.state_putpixel(screen, i_W, j, 1.0, RP);
                    ++exCount;

                    if(disc)
                    {
                        update_discId(maxDiscId, discId[i][j], discId[i_W][j]);
                        display.disc_putpixel(screen, i_W, j, disc, discId[i_W][j]);
                        histo.add_point(disc);
                        ++h_DiscCountNow;
                        ++discCountNow;
                        ++discCount;
                    }
                }

                //EAST CELL
                if(i_E<GRIDSIZE && state[i_E][j]==0 && state_update[i_E][j] != RP && inW[i_E][j]>drand())
                {
                    update_arrays(state_update[i_E][j], exCoords[cyclicNow], exFrameNew[i_E][j], FRAME, RP, i_E, j);
                    disc = returnDisc(exFrame[i][j], exFrame[i_E][j], LOWDISC, HIGHDISC);
                    display.state_putpixel(screen, i_E, j, 1.0, RP);
                    ++exCount;

                    if(disc)
                    {
                        update_discId(maxDiscId, discId[i][j], discId[i_E][j]);
                        display.disc_putpixel(screen, i_E, j, disc, discId[i_E][j]);
                        histo.add_point(disc);
                        ++h_DiscCountNow;
                        ++discCountNow;
                        ++discCount;
                    }
                }

                //NORTH CELL
                if(state[i][j_N]==0 && state_update[i][j_N] != RP && inS[i][j_N]>drand())
                {
                    update_arrays(state_update[i][j_N], exCoords[cyclicNow], exFrameNew[i][j_N], FRAME, RP, i, j_N);
                    disc = returnDisc(exFrame[i][j], exFrame[i][j_N], LOWDISC, HIGHDISC);
                    display.state_putpixel(screen, i, j_N, 1.0, RP);
                    ++exCount;

                    if(disc)
                    {
                        update_discId(maxDiscId, discId[i][j], discId[i][j_N]);
                        display.disc_putpixel(screen, i, j_N, disc, discId[i][j_N]);
                        histo.add_point(disc);
                        ++h_DiscCountNow;
                        ++discCountNow;
                        ++discCount;
                    }
                }

                //SOUTH CELL
                if(state[i][j_S]==0 && state_update[i][j_S] != RP && inN[i][j_S]>drand())
                {
                    update_arrays(state_update[i][j_S], exCoords[cyclicNow], exFrameNew[i][j_S], FRAME, RP, i, j_S);
                    disc = returnDisc(exFrame[i][j], exFrame[i][j_S], LOWDISC, HIGHDISC);
                    display.state_putpixel(screen, i, j_S, 1.0, RP);
                    ++exCount;

                    if(disc)
                    {
                        update_discId(maxDiscId, discId[i][j], discId[i][j_S]);
                        display.disc_putpixel(screen, i, j_S, disc, discId[i][j_S]);
                        histo.add_point(disc);
                        ++h_DiscCountNow;
                        ++discCountNow;
                        ++discCount;
                    }
                }
            }


            //EXCITED CELL PRINTING PROCEDURE + STATE DE-EXCITATION PROCESS.//
            for (int row = cyclicOld, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(); col != exCoords[row].end(); col+=2)
                {
                    state_update[*col][*(col+1)]-=1;
                    display.state_putpixel(screen, *col, *(col+1), RP_ratio, RP);
                }
            }


            //PACEMAKER
            if(FRAME%SAP==0)
            {
                pacemaker(screen, state_update[0], exCoords[cyclicNow], exFrameNew[0], FRAME, RP, GRIDSIZE, display);

            }

            //HISTOGRAM
            histo.print_histogram(screen);
            histo.print_axes(screen);
            //histo.reset_frequency();


            //UPDATE LOOP VARIABLES
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    state[k][l]=state_update[k][l];
                }
            }
            exFrame=exFrameNew;
            ++FRAME;

            //GENERAL LOGIC CHECK
            if(logic.getIsStepping())logic.Pause();//framestep logic
            if(FRAME==MAXFRAME)logic.Stop();//stop at MAXFRAME
        }

    //BUFFER
    if(1000/logic.getFrameRate() > SDL_GetTicks()-start) {SDL_Delay(1000/logic.getFrameRate()-(SDL_GetTicks()-start));}
    //RENDER
    SDL_Flip(screen);
    }

SDL_Quit();
return 0;
}



//if(display.getIsStateView()){state_putpixel(screen, i_W, j, 1.0, display.get_x(), display.get_y(), GRIDSIZE, RP, ZOOM);}

//if(display.getIsDiscView() && (histo.getIsBinSelectArray()[histo.getbin(disc)] || histo.getIsAllSelect())){discontinuity_putpixel(screen, i_W, j, display.get_x(), display.get_y(), GRIDSIZE, ZOOM, disc, discId[i_W][j]);}
//pacemaker_central(screen, state_update[GRIDSIZE/2][GRIDSIZE/2], exCoords[cyclicNow], exFrameNew[GRIDSIZE/2][GRIDSIZE/2], frame, RP, display.get_x(), display.get_y(), GRIDSIZE, ZOOM, display.getIsStateView());

