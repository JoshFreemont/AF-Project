#include "main_functions.h"
#include "mtrand.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <cmath>
#include "filenamer.h"
#include <fstream>

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
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    double VER=0.2;
    const double HOR=0.985; //0.985 by default
    const int GRIDSIZE=200;
    const int G_HEIGHT=GRIDSIZE;
    const int G_WIDTH=GRIDSIZE;
    const int MEMLIMIT=RP+1;
    int FRAME=0;
    int cyclicNow=0;
    int cyclicOld=0;
    int cyclicBackRP=0;
    bool isAblate = false;
    bool isReset = false;

    //MEASUREMENT VARIABLES
    FileNamer myFileNamer, myFileNamer2;
    myFileNamer.setFileHeader("patchisosceles30");
    myFileNamer2.setFileHeader("ablatedisosceles30");

    //SEED
    auto currentseed = time(NULL);
    MTRand drand(currentseed);

    //DEFINE AFIB MATRICIES
    int state [G_WIDTH][G_HEIGHT];
    int state_update [G_WIDTH][G_HEIGHT];
    double inN [G_WIDTH][G_HEIGHT];
    double inS [G_WIDTH][G_HEIGHT];
    double inE [G_WIDTH][G_HEIGHT];
    double inW [G_WIDTH][G_HEIGHT];

    auto inNstart = vector <vector<double> > (G_WIDTH, vector<double> (G_HEIGHT, 0));
    auto inSstart = vector <vector<double> > (G_WIDTH, vector<double> (G_HEIGHT, 0));
    auto inEstart = vector <vector<double> > (G_WIDTH, vector<double> (G_HEIGHT, 0));
    auto inWstart = vector <vector<double> > (G_WIDTH, vector<double> (G_HEIGHT, 0));

    //ROTOR IDENTIFICATION
    bool rotorCells[G_WIDTH][G_HEIGHT]; //Stores whether a cell is a "rotor" cell or not
    pair <int,int> excitedBy[G_WIDTH][G_HEIGHT]; //Stores coords of cell which excited current cell
    int rotorLengthLimit = 2*RP;
    pair <int,int> tempCycleArray[rotorLengthLimit];//Temporary array to store values
                                        //to detect rotors
    int cycleStart = 0;
    int cycleLength = 0;
    int tortoise, hare;
    bool isIdentifyRotors = false;

    vector <int> cycleLengthCount (rotorLengthLimit+1, 0);
    vector <int> cycleLengthCount2 (rotorLengthLimit+1, 0);
    vector <int> cycleLengthTotalCount (rotorLengthLimit+1, 0);
    vector <int> cycleLengthTotalCount2 (rotorLengthLimit+1, 0);

    //ROTOR COUNT MATRICES
    int currentRotorCount[G_WIDTH][G_HEIGHT]; //For current iteration
    int currentRotorCount2[G_WIDTH][G_HEIGHT]; //For current iteration (for ablated version)
    int totalRotorCount[G_WIDTH][G_HEIGHT]; //For whole nu value
    int totalRotorCount2[G_WIDTH][G_HEIGHT]; //For whole nu value (for ablated version)


        for (int k=0;k<G_WIDTH;k++)
    {
        for (int l=0;l<G_HEIGHT;l++)
        {
            totalRotorCount[k][l] = 0;
            totalRotorCount2[k][l] = 0;
        }
    }

    //Just output streams to output files
    ofstream excited_list_stream; //Prints out the number of excited cells in each stream
    ofstream patchStatsStream; //Prints out the number of frames in AF for a given nu fpr each iteration
    ofstream patchMasterStream; //Contains time in AF for each nu value
    ofstream ablatedStatsStream; //Prints out the number of frames in AF for a given nu fpr each iteration for ablated
    ofstream ablatedMasterStream; //Contains time in AF for each nu value
    ofstream patchGridStream; //Contains rotor count as a grid
    ofstream patchAvgGridStream;
    ofstream ablatedGridStream;
    ofstream ablatedAvgGridStream;


//-----VECTOR FOR ABLATION COORDS-----//
    vector <int> ablationCoords;
//------------------------------------//


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
    int LOWDISC=0;
    int HIGHDISC=SAP;
    int disc=0;
    int maxDiscId=0;
    int AFdiscCount = 0;
    int frames_in_AF = 0;
    int frames_in_AFd = 0;
    int AF_threshold = 300; //Number of excited cells to count as excited.
    int AFdiscThreshold = 40;

    //---------------------SET ITERATION PARAMETERS--------------------------//
    const double nu_min = 0.0; //This is nu INSIDE the patches
    const double nu_max = 0.30;
    const int nu_steps = 15;//Number of steps to use between nu_min and nu_max
    const double VER_max = 0.40;//This is nu OUTSIDE the patches
    const double VER_min = 0.20;
    const int VER_steps = 2;
    const int MAXFRAME=100000;

    double nu_spacing = (nu_max-nu_min)/nu_steps;
    double VER_spacing = (VER_max-VER_min)/VER_steps;
    double nu;

    double time_in_AF;//Not needed any more
    int AF_or_not; //Counts the number of fibrillating cases for given nu
    int AF_or_notd; //Counts the number of fibrillating cases for given nu
    int AF_or_not2; //Counts the number of fibrillating cases for given nu
    int AF_or_not2d; //Counts the number of fibrillating cases for given nu


    const int iterations = 10;   //Number of iterations for each given nu values

    int counter = 1;

    //-----------VERTICAL COUNT START----------------------//
    for (int VER_int = 0;VER_int <=VER_steps ;VER_int++)
    {

    VER = VER_min + VER_int*VER_spacing;

    myFileNamer.RenameFile(patchMasterStream, VER);
    myFileNamer2.RenameFile(ablatedMasterStream, VER);
    //String to create filenames

    ostringstream convert; //Number to string conversion string

    patchMasterStream << "Patch NuInside" << "\t" << "Time in AF" << "\t"
    << "Fibrillating cases" << "\tStd Dev of time in AF"
    << "\tStd Err of time in AF" << "\t";
//    << "Time in AF(D)" << "\t"
//    << "Fibrillating cases(D)" << "\tStd Dev of time in AF(D)"
//    << "\tStd Err of time in AF(D)" << "\t";

            for (int k=RP;k<rotorLengthLimit;k++)
        {
            patchMasterStream << "Length " << k << "\t";
        }
        patchMasterStream << endl;

    ablatedMasterStream << "Ablated NuInside" << "\t" << "Time in AF" << "\t"
    << "Fibrillating cases" << "\tStd Dev of time in AF"
    << "\tStd Err of time in AF" << "\t";
//    << "Time in AF(D)" << "\t"
//    << "Fibrillating cases(D)" << "\tStd Dev of time in AF(D)"
//    << "\tStd Err of time in AF(D)" << "\t";

    for (int k=RP;k<rotorLengthLimit;k++)
        {
            ablatedMasterStream << "Length " << k << "\t";
        }
        ablatedMasterStream << endl;


    //-------------------------MAIN LOOP START-------------------------------//
    for(int nu_int=0;nu_int<=nu_steps;nu_int++) //Iterate through different nu values
    {

    for (int k=0;k<G_WIDTH;k++)
    {
        for (int l=0;l<G_HEIGHT;l++)
        {
            excitedBy[k][l] = make_pair(k,l);
            totalRotorCount[k][l] = 0;
        }
    }

    nu = nu_min + nu_int*nu_spacing; //This is the value of nu we are considering

    AF_or_not = 0;
    AF_or_notd = 0;
    AF_or_not2 = 0;
    AF_or_not2d = 0;

    myFileNamer.RenameFile(patchStatsStream, "stats", nu, VER);
    myFileNamer2.RenameFile(ablatedStatsStream, "stats", nu, VER);
    myFileNamer.RenameFile(patchAvgGridStream, "avgGrid", nu, VER);
    myFileNamer2.RenameFile(ablatedAvgGridStream, "avgGrid", nu, VER);

    patchStatsStream << "Iteration" << "\t" << "No. of Frames in AF" << "\t"
    << "Time in AF" << "\t" <<
    //"Frames in AF(D)" << "\t" << "Time in AF(D)" <<
    "\t" << "Horizontal firing prob" << "\t" << "VER outside" <<"\t" <<"NuInside\t";
    for (int k=RP;k<rotorLengthLimit;k++)
        {
            patchStatsStream << "Length " << k << "\t";
        }
        patchStatsStream << endl;

    ablatedStatsStream << "Iteration" << "\t" << "No. of Frames in AF" << "\t"
    << "Time in AF" <<
    //"\t" << "Frames in AF(D)" << "\t" << "Time in AF(D)" <<
    "\t" << "Horizontal firing prob" << "\t" << "VER outside" <<"\t" <<"NuInside\t";
    for (int k=RP;k<rotorLengthLimit;k++)
        {
            ablatedStatsStream << "Length " << k << "\t";
        }
        ablatedStatsStream << endl;


    vector<double> AF_times = vector<double>(iterations, 0.0);
    vector<double> AF_times2 = vector<double>(iterations, 0.0);
    vector<double> AF_timesd = vector<double>(iterations, 0.0);
    vector<double> AF_times2d = vector<double>(iterations, 0.0);

    //Reset cycleLengthCounts
    cycleLengthTotalCount = vector <int> (rotorLengthLimit+1, 0);
    cycleLengthTotalCount2 = vector <int> (rotorLengthLimit+1, 0);

    double AF_times_avg;
    double AF_times_std_dev;

    for (int l=0;l<G_HEIGHT;l++)
        {
            for(int k=0;k<G_WIDTH;k++)
            {
                totalRotorCount[k][l] = 0;
                totalRotorCount2[k][l] = 0;
            }
        }

//-----------ITERATIONS START-----------------------//
    for(int it=0;it<iterations;it++) //Iterate over a given nu value many times
{

    myFileNamer.RenameFile(excited_list_stream, "frames", nu, it, VER);
    myFileNamer.RenameFile(patchGridStream, "grid", nu, it, VER);

    //RE-SEED AND EVERYTHING
    currentseed = time(NULL);
    drand.seed(currentseed);
    ablationCoords.clear();

    for (int k=0;k<G_WIDTH;k++)
    {
        for (int l=0;l<G_HEIGHT;l++)
        {
            excitedBy[k][l] = make_pair(k,l);
            currentRotorCount[k][l] = 0;
        }
    }

    //Initialising the "normal" cells
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            state[k][l]=0;
            state_update[k][l]=0;
            inN[k][l]=VER;
            inS[k][l]=VER;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            for (int m=0; m<MEMLIMIT; m++)
                        {exCoords[m].erase(exCoords[m].begin(),
                                                   exCoords[m].end());}
        }
    }

    //Circular patch of fibrillating stuff
    int radius = 15;
    int xcentre = 150;
    int ycentre = 150;
    for(int k=xcentre-radius; k<xcentre+radius; ++k)
    {
        for(int l=ycentre-radius; l<xcentre+radius; ++l)
        {
 //           if ((k-xcentre)*(k-xcentre)+(l-ycentre)*(l-ycentre)<=radius*radius)
        if(l-ycentre>=0.5*(k-xcentre)-7.5 && l-ycentre<=-0.5*(k-xcentre)+7.5
           && k-xcentre>=-15)//Triangle bounds 30 up, 30 across
            {
            inN[k][l]=nu;
            inS[k][l]=nu;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            ablationCoords.push_back(k);
            ablationCoords.push_back(l);
            }
        }
    }

    //Circular patch of fibrillating stuff
    radius = 15;
    xcentre = 50;
    ycentre = 50;
    for(int k=xcentre-radius; k<xcentre+radius; ++k)
    {
        for(int l=ycentre-radius; l<xcentre+radius; ++l)
        {
//            if ((k-xcentre)*(k-xcentre)+(l-ycentre)*(l-ycentre)<=radius*radius)
        if(l-ycentre>=0.5*(k-xcentre)-7.5 && l-ycentre<=-0.5*(k-xcentre)+7.5
           && k-xcentre>=-15)//Triangle bounds 30 up, 30 across
            {
            inN[k][l]=nu;
            inS[k][l]=nu;
            inE[k][l]=HOR;
            inW[k][l]=HOR;
            ablationCoords.push_back(k);
            ablationCoords.push_back(l);
            }
        }
    }

    //SAVE INITIAL STATE TO RESET
    for (int k=0; k<G_WIDTH; ++k)
    {
        for (int l=0; l<G_HEIGHT; ++l)
        {
            inNstart[k][l] = inN[k][l];
            inSstart[k][l] = inS[k][l];
            inEstart[k][l] = inE[k][l];
            inWstart[k][l] = inW[k][l];
        }
    }

    exFrame = vector<vector<int> > (GRIDSIZE, vector<int>(GRIDSIZE, 9999999));
    exFrameNew = exFrame;

    frames_in_AF = 0;
    frames_in_AFd = 0;

    cycleLengthCount = vector <int> (rotorLengthLimit+1, 0);
    cycleLengthCount2 = vector <int> (rotorLengthLimit+1, 0);

    excited_list_stream << currentseed << "\n";


    //---------------------------PATCH START----------------------------//
    isIdentifyRotors=false;

    for (FRAME=0; FRAME<MAXFRAME; FRAME++)
    {

            cyclicNow = FRAME%MEMLIMIT;
            cyclicOld = (FRAME+MEMLIMIT-1)%MEMLIMIT;
            cyclicBackRP = (FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
            exCount=0;
//            AFdiscCount=0;

            //FREE MEMORY IN "exCoords" ARRAY
            if(FRAME>MEMLIMIT-1){exCoords[cyclicNow].erase(exCoords[cyclicNow].begin(), exCoords[cyclicNow].end());}

            //CLEAR ROTOR CELLS MEMORY
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    rotorCells[k][l]=false;
                }
            }

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
//                    disc = returnDisc(exFrame[i][j], exFrame[i_W][j], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i_W][j] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
 //                       ++AFdiscCount;
 //                   }
                }

                //EAST CELL
                if(i_E<GRIDSIZE && state[i_E][j]==0 && state_update[i_E][j] != RP && inW[i_E][j]>drand())
                {
                    update_arrays(state_update[i_E][j], exCoords[cyclicNow], exFrameNew[i_E][j], FRAME, RP, i_E, j);
//                    disc = returnDisc(exFrame[i][j], exFrame[i_E][j], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i_E][j] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }

                //NORTH CELL
                if(state[i][j_N]==0 && state_update[i][j_N] != RP && inS[i][j_N]>drand())
                {
                    update_arrays(state_update[i][j_N], exCoords[cyclicNow], exFrameNew[i][j_N], FRAME, RP, i, j_N);
//                    disc = returnDisc(exFrame[i][j], exFrame[i][j_N], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i][j_N] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
 //                   }
                }

                //SOUTH CELL
                if(state[i][j_S]==0 && state_update[i][j_S] != RP && inN[i][j_S]>drand())
                {
                    update_arrays(state_update[i][j_S], exCoords[cyclicNow], exFrameNew[i][j_S], FRAME, RP, i, j_S);
//                    disc = returnDisc(exFrame[i][j], exFrame[i][j_S], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i][j_S] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }
            }

            //CHECK FOR ROTORS
            if (isIdentifyRotors)
            {
            for (auto col = exCoords[cyclicNow].begin(); col!= exCoords[cyclicNow].end(); col+=2)
            {
                //First, fill up temp array to check for cycles
                tempCycleArray[0] = excitedBy[*col][*(col+1)];

                for (int k=1;k<rotorLengthLimit;k++)
                {
                    tempCycleArray[k]=excitedBy[tempCycleArray[k-1].first][tempCycleArray[k-1].second];
                }

                //Check if cell is already identified as being a rotor cell
                //continue to next iteration of loop if yes
                if (rotorCells[tempCycleArray[rotorLengthLimit-1].first][tempCycleArray[rotorLengthLimit-1].second])
                    continue;

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

                if (cycleLength>2)
                {
                    for (int m=cycleStart, k=0;k<cycleLength;k++)
                    {
                        rotorCells[tempCycleArray[m+k].first][tempCycleArray[m+k].second] = true;
                        currentRotorCount[tempCycleArray[m+k].first][tempCycleArray[m+k].second]++;
                    }
                    cycleLengthCount[cycleLength]++;
                }
                else
                    continue;

            }

            }


            //STATE DE-EXCITATION PROCESS.//
            for (int row = cyclicOld, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(); col != exCoords[row].end(); col+=2)
                {
                    state_update[*col][*(col+1)]-=1;
                }
            }


            //PACEMAKER
            if(FRAME%SAP==0)
            {
                pacemaker(state_update[0], exCoords[cyclicNow], exFrameNew[0], FRAME, RP, GRIDSIZE, excitedBy[0]);

            }


            //UPDATE LOOP VARIABLES
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    state[k][l]=state_update[k][l];
                }
            }
            exFrame=exFrameNew;

            excited_list_stream << exCount
            //<< "\t"<< AFdiscCount
            << "\n";

            if (exCount >= AF_threshold)
            {
                frames_in_AF++;
                isIdentifyRotors = true;
            }

//            if (AFdiscCount >= AFdiscThreshold)
//            {
//                frames_in_AFd++;
//            }

    }

    //-----------PATCH END---------------//
cout << counter << " complete out of " << iterations*(nu_steps+1)*2*(VER_steps+1) << endl;
counter++;

AF_times[it]=((double)frames_in_AF)/((double)MAXFRAME);
//AF_timesd[it]=((double)frames_in_AFd)/((double)MAXFRAME);

patchStatsStream << it << "\t" << frames_in_AF << "\t" << AF_times[it]
//        << "\t" << frames_in_AFd
//        << "\t" << AF_timesd[it]
        <<"\t" << HOR << "\t" << VER << "\t" << nu <<"\t";

        for (int k=RP;k<rotorLengthLimit;k++)
        {
            patchStatsStream << cycleLengthCount[k] << "\t";
            cycleLengthTotalCount[k]+=cycleLengthCount[k];
        }
        patchStatsStream << endl;

if (frames_in_AF>0)
{
    AF_or_not++;
}

//if (frames_in_AFd>0)
//{
//    AF_or_notd++;
//}

for (int l=0;l<G_HEIGHT;l++)
{
    for(int k=0;k<G_WIDTH;k++)
    {
        totalRotorCount[k][l]+=currentRotorCount[k][l];
        patchGridStream << currentRotorCount[k][l] << "\t";
    }
    patchGridStream << "\n";
}

//RESET EVERYTHING WITH INITIAL SEED (Clear everything with '0')
        if (1)
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
                    inN[k][l] = inNstart[k][l];
                    inS[k][l] = inSstart[k][l];
                    inE[k][l] = inEstart[k][l];
                    inW[k][l] = inWstart[k][l];
                }
            }
            drand.seed(currentseed);
        }

    //Reset everything
        //ABLATION (Ablate with 'a')
        if (1)
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
        }

    //Reset Rotor Grids
    for (int k=0;k<G_WIDTH;k++)
    {
        for (int l=0;l<G_HEIGHT;l++)
        {
            excitedBy[k][l] = make_pair(k,l);
            currentRotorCount2[k][l] = 0;
        }
    }

    myFileNamer2.RenameFile(excited_list_stream, "frames", nu, it, VER);
    myFileNamer2.RenameFile(ablatedGridStream, "grid", nu, it, VER);

    frames_in_AF = 0;
    frames_in_AFd = 0;

    exFrame = vector<vector<int> > (GRIDSIZE, vector<int>(GRIDSIZE, 9999999));
    exFrameNew = exFrame;

    excited_list_stream << currentseed << "\n";

    //-----------------------START ABLATED ITERATION----------------------------//

    isIdentifyRotors=false;
    for (FRAME=0; FRAME<MAXFRAME; FRAME++)
    {

            cyclicNow = FRAME%MEMLIMIT;
            cyclicOld = (FRAME+MEMLIMIT-1)%MEMLIMIT;
            cyclicBackRP = (FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
            exCount=0;
//            AFdiscCount=0;

            //FREE MEMORY IN "exCoords" ARRAY
            if(FRAME>MEMLIMIT-1){exCoords[cyclicNow].erase(exCoords[cyclicNow].begin(), exCoords[cyclicNow].end());}

            //CLEAR ROTOR CELLS MEMORY
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    rotorCells[k][l]=false;
                }
            }

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
//                    disc = returnDisc(exFrame[i][j], exFrame[i_W][j], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i_W][j] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }

                //EAST CELL
                if(i_E<GRIDSIZE && state[i_E][j]==0 && state_update[i_E][j] != RP && inW[i_E][j]>drand())
                {
                    update_arrays(state_update[i_E][j], exCoords[cyclicNow], exFrameNew[i_E][j], FRAME, RP, i_E, j);
//                    disc = returnDisc(exFrame[i][j], exFrame[i_E][j], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i_E][j] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }

                //NORTH CELL
                if(state[i][j_N]==0 && state_update[i][j_N] != RP && inS[i][j_N]>drand())
                {
                    update_arrays(state_update[i][j_N], exCoords[cyclicNow], exFrameNew[i][j_N], FRAME, RP, i, j_N);
//                    disc = returnDisc(exFrame[i][j], exFrame[i][j_N], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i][j_N] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }

                //SOUTH CELL
                if(state[i][j_S]==0 && state_update[i][j_S] != RP && inN[i][j_S]>drand())
                {
                    update_arrays(state_update[i][j_S], exCoords[cyclicNow], exFrameNew[i][j_S], FRAME, RP, i, j_S);
//                    disc = returnDisc(exFrame[i][j], exFrame[i][j_S], LOWDISC, HIGHDISC);
                    ++exCount;
                    excitedBy[i][j_S] = make_pair(i,j);

//                    if(disc <= 2*RP && disc >= RP)
//                    {
//                        ++AFdiscCount;
//                    }
                }
            }

            //CHECK FOR ROTORS
            if (isIdentifyRotors)
            {
            for (auto col = exCoords[cyclicNow].begin(); col!= exCoords[cyclicNow].end(); col+=2)
            {
                //First, fill up temp array to check for cycles
                tempCycleArray[0] = excitedBy[*col][*(col+1)];

                for (int k=1;k<rotorLengthLimit;k++)
                {
                    tempCycleArray[k]=excitedBy[tempCycleArray[k-1].first][tempCycleArray[k-1].second];
                }

                //Check if cell is already identified as being a rotor cell
                //continue to next iteration of loop if yes
                if (rotorCells[tempCycleArray[rotorLengthLimit-1].first][tempCycleArray[rotorLengthLimit-1].second])
                    continue;

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

                if (cycleLength>2)
                {
                    for (int m=cycleStart, k=0;k<cycleLength;k++)
                    {
                        rotorCells[tempCycleArray[m+k].first][tempCycleArray[m+k].second] = true;
                        currentRotorCount2[tempCycleArray[m+k].first][tempCycleArray[m+k].second]++;

                    }
                    cycleLengthCount2[cycleLength]++;
                }
                else
                    continue;

            }

            }


            //STATE DE-EXCITATION PROCESS.//
            for (int row = cyclicOld, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
            {
                double RP_ratio=(double)(RP-(FRAME+MEMLIMIT-row)%MEMLIMIT)/RP;
                for(vector<int>::iterator col = exCoords[row].begin(); col != exCoords[row].end(); col+=2)
                {
                    state_update[*col][*(col+1)]-=1;
                }
            }


            //PACEMAKER
            if(FRAME%SAP==0)
            {
                pacemaker(state_update[0], exCoords[cyclicNow], exFrameNew[0], FRAME, RP, GRIDSIZE, excitedBy[0]);
            }


            //UPDATE LOOP VARIABLES
            for(int k=0; k<G_WIDTH; ++k)
            {
                for(int l=0; l<G_HEIGHT; ++l)
                {
                    state[k][l]=state_update[k][l];
                }
            }
            exFrame=exFrameNew;

            excited_list_stream << exCount
            //<< "\t" << AFdiscCount
            << "\n";

            if (exCount >= AF_threshold)
            {
                frames_in_AF++;
                isIdentifyRotors=true; //If in AF, start identifying rotors
            }

//            if (AFdiscCount >= AFdiscThreshold)
//            {
//                frames_in_AFd++;
//            }

    }

//-----------END ABLATED LOOP------------//

            for (int l=0;l<G_HEIGHT;l++)
        {
            for(int k=0;k<G_WIDTH;k++)
            {
                totalRotorCount2[k][l]+=currentRotorCount2[k][l];
                ablatedGridStream << currentRotorCount2[k][l] << "\t";
            }
            ablatedGridStream << "\n";
        }

        cout << counter << " complete out of " << iterations*(nu_steps+1)*2*(VER_steps+1) << endl;
        counter++;

        AF_times2[it]=((double)frames_in_AF)/((double)MAXFRAME);
//        AF_times2d[it]=((double)frames_in_AFd)/((double)MAXFRAME);

        ablatedStatsStream << it << "\t" << frames_in_AF << "\t" << AF_times2[it] <<"\t"
//        << frames_in_AFd << "\t"
//        << AF_times2d[it] <<"\t"
        << HOR << "\t" << VER << "\t" << nu <<"\t";

        for (int k=RP;k<rotorLengthLimit;k++)
        {
            ablatedStatsStream << cycleLengthCount2[k] << "\t";
            cycleLengthTotalCount2[k]+=cycleLengthCount2[k];
        }
        ablatedStatsStream << endl;

        if (frames_in_AF>0)
        {
            AF_or_not2++;
        }

        if (frames_in_AFd>0)
        {
            AF_or_not2d++;
        }
}

//-----------------END ITERATION--------------//

//--------UPDATE AVG ROTOR FILES-------------//
for (int l=0;l<G_HEIGHT;l++)
        {
            for(int k=0;k<G_WIDTH;k++)
            {
                patchAvgGridStream << totalRotorCount[k][l]/(double)iterations << "\t";
                ablatedAvgGridStream << totalRotorCount2[k][l]/(double)iterations << "\t";
            }
            patchAvgGridStream << "\n";
            ablatedAvgGridStream << "\n";
        }

//---------UPDATE MASTER FILES----------------//
AF_times_avg = avg(AF_times,iterations);
AF_times_std_dev = std_dev(AF_times,iterations,AF_times_avg);

patchMasterStream << nu << "\t" << AF_times_avg << "\t"
<< ((double)AF_or_not)/((double)iterations) << "\t" << AF_times_std_dev
<< "\t" << AF_times_std_dev/sqrt((double)iterations) << "\t";

//AF_times_avg = avg(AF_timesd,iterations);
//AF_times_std_dev = std_dev(AF_timesd,iterations,AF_times_avg);

//patchMasterStream << AF_times_avg << "\t" << ((double)AF_or_notd)/((double)iterations)
//<< "\t" << AF_times_std_dev << "\t" << AF_times_std_dev/sqrt((double)iterations) << "\t";

for (int k=RP;k<rotorLengthLimit;k++)
        {
            patchMasterStream << cycleLengthTotalCount[k]/(double)iterations << "\t";
        }
        patchMasterStream << endl;

AF_times_avg = avg(AF_times2,iterations);
AF_times_std_dev = std_dev(AF_times2,iterations,AF_times_avg);

ablatedMasterStream << nu << "\t" << AF_times_avg << "\t"
<< ((double)AF_or_not2)/((double)iterations) << "\t" << AF_times_std_dev
<< "\t" << AF_times_std_dev/sqrt((double)iterations) << "\t";

//AF_times_avg = avg(AF_times2d,iterations);
//AF_times_std_dev = std_dev(AF_times2d,iterations,AF_times_avg);

//ablatedMasterStream << AF_times_avg << "\t"
//<< ((double)AF_or_not2d)/((double)iterations) << "\t"
//<< AF_times_std_dev << "\t"
//<< AF_times_std_dev/sqrt((double)iterations) << "\t";

for (int k=RP;k<rotorLengthLimit;k++)
        {
            ablatedMasterStream << cycleLengthTotalCount2[k]/(double)iterations << "\t";
        }
        ablatedMasterStream << endl;

patchMasterStream.flush(); //Contains time in AF for each nu value
ablatedMasterStream.flush();

}

excited_list_stream.flush(); //Prints out the number of excited cells in each stream
patchStatsStream.flush(); //Prints out the number of frames in AF for a given nu fpr each iteration
ablatedStatsStream.flush(); //Prints out the number of frames in AF for a given nu fpr each iteration
    }

    cout << "Finished" << endl;
    cin.get();
	return 0;
}



//if(display.getIsStateView()){state_putpixel(screen, i_W, j, 1.0, display.get_x(), display.get_y(), GRIDSIZE, RP, ZOOM);}

//if(display.getIsDiscView() && (histo.getIsBinSelectArray()[histo.getbin(disc)] || histo.getIsAllSelect())){discontinuity_putpixel(screen, i_W, j, display.get_x(), display.get_y(), GRIDSIZE, ZOOM, disc, discId[i_W][j]);}
//pacemaker_central(screen, state_update[GRIDSIZE/2][GRIDSIZE/2], exCoords[cyclicNow], exFrameNew[GRIDSIZE/2][GRIDSIZE/2], frame, RP, display.get_x(), display.get_y(), GRIDSIZE, ZOOM, display.getIsStateView());

