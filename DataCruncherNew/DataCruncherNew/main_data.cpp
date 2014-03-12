#include "main_functions.h"
#include "mtrand.h"
#include "patch.h"
#include "array2D.h"
#include "filenamer.h"
#include "network.h"
#include <ctime>
#include <fstream>
#include <unordered_map>
#include <cmath>
#include "rotorIDstruct.h"
#include "analysisfunctions.h"
#include "rotorFunctions.h"
#include "DataOutput.h"
#include "histogram.h"
#include <algorithm>

using namespace std;

int main(int argc, char** argv)
{

//VARIABLE DECLARATION + INITIALIZATION//
    
    //AF parameters
    const int SAP=210;//Sinoatrial period is measured in frames- not in "SI time"- SI time=SAP/FPS
    const int RP=50;//Refractory peiod is measured in frames - not in "SI time".
    const double VER=0.1;//0.1477
    const double HOR=0.985;//0.97
    const int GRIDSIZE=200;
    const int G_HEIGHT=GRIDSIZE;
    const int G_WIDTH=GRIDSIZE;
    
    //frame and memory variables
    const int MEMLIMIT=RP+1;
    const int MAXFRAME=10000;
    int frame=0;
    int cyclicNow=0;
    int cyclicOld=0;
    int cyclicBackRP=0;
    
    //other parameters
    MTRand drand(time(NULL));//seed rnd generator
    
    //AF matrix declaration + initialization.
    array2D<double> inN (G_WIDTH, G_HEIGHT, VER);
    array2D<double> inE (G_WIDTH, G_HEIGHT, VER);
    array2D<double> inS (G_WIDTH, G_HEIGHT, HOR);
    array2D<double> inW (G_WIDTH, G_HEIGHT, HOR);
    array2D<int> state (G_WIDTH, G_HEIGHT, 0);
    array2D<int> state_update (G_WIDTH, G_HEIGHT, 0);
    array2D <pair <int,int> > excitedBy(G_WIDTH,G_HEIGHT); //Stores coords of cell which excited current cell
    for(int k=0; k<G_WIDTH; ++k)
    {
        for(int l=0; l<G_HEIGHT; ++l)
        {
            excitedBy(k,l) = make_pair(k,l);
        }
    }
    
    //Declare filenamer, and file streams.
    FileNamer MyFileNamer;
    MyFileNamer.setFileHeader("test1");
    ofstream rotorIdInherit_T;
    ofstream rotorIdInherit_S;
    ofstream rotorExCountstream;
	ofstream rotorCountstream;
	ofstream rotorIDstream;
    ofstream birthRateStream;
    ofstream deathRateStream;
    
    //rotor variables
    array2D <int> rotorCellFrequency (G_WIDTH,G_HEIGHT,0);
    const int rotorLengthLimit = 2*RP;
    array2D <bool> isRotorInit (G_WIDTH,G_HEIGHT,false);
    vector <array2D <bool> > isRotor (MEMLIMIT, isRotorInit);//Stores whether a cell is a "rotor" cell or not
    unordered_map<int, bool> isRotorAliveOld;
    unordered_map<int, bool> isRotorAliveNow;
    int rotorCount=0;
    
    //rotor id variables
    array2D<int> activeRotorId (G_WIDTH, G_HEIGHT, -1);//stores all rotor ids for the state.
    array2D<int> inheritedRotorId (G_WIDTH, G_HEIGHT, 0);//stores all rotor ids for all cells, whether in a rotor or not.
    unordered_map<int, int> tempRotorIdFrequency;//stores temp frequency count of ids
    double rotorIdThresh = 0.01, tempRotorIdRatio;//threshold for rotor id inheritance. 1.0=100% of current rotor cells must have same id.
    int maxFreq;//counts the maximum frequency rotor id within rotor
    int tempRotorId;//creates a temp rotor id variable.
    int maxRotorId=-1;//global maximum rotor id counter.
    vector<rotorIDstruct> rotorIdData;
    vector< pair <int,int> > rotorIdAverageCoords;
    
    //cycle data variables
    pair <int,int> tempCycleArray[rotorLengthLimit];//Temporary array to store values
    int cycleStart = 0;
    int cycleLength = 0;
    const int rotorHeightCutoff = 15;
    
	//Patches
    /*int size=40;
     double nuIn=0.08;
     Spatch patch1 (size, 50, 50, S_WIDTH, S_HEIGHT, GRIDSIZE, inN, inS, inE, inW, nuIn);
     Spatch patch2 (size, 150, 150, S_WIDTH, S_HEIGHT, GRIDSIZE, inN, inS, inE, inW, nuIn);*/
    
    //excited cell memory declaration + memory reservation
    vector<int> coords;
    coords.reserve(GRIDSIZE*GRIDSIZE);
    vector<vector<int> >exCoords(MEMLIMIT, coords);
    vector<vector<int> >rotorCoords(MEMLIMIT, coords);
    vector<vector<int> >emptyCoords(MEMLIMIT, coords);
    
    //rotorIdNetwork
    const int bucketSize = 25;
    int noBuckets = (GRIDSIZE/bucketSize)*(GRIDSIZE/bucketSize);
    int parentBucket, childBucket;//buckets for nodes of spatial network.
    vector<pair<int, int> > bucketPos(noBuckets);
    assignCoords(bucketPos, noBuckets, bucketSize);
    network rotorIdNetwork_T;
    network rotorIdNetwork_S;
    
    //cell coordinates
    int i;
    int j;
    int i_W;
    int i_E;
    int j_N;
    int j_S;
    
    //setup to look at rotor duration for different nu in order to measure dynamism.
    double nu;
    double nuSTART = 0.1;
    const double nuMAX = 0.4;
    const double nuSTEP = 0.02;
    const int repeatMAX = 10;
    int iterationcount = 1;
    const int TotalIterations = repeatMAX*3*((nuMAX-nuSTART)/nuSTEP+1);
    
    
//EXPERIMENTATION//
    //Start "nu loop"
    for (nu = nuSTART; nu < nuMAX; nu += nuSTEP)
    {
        
        //Start "threshold loop"
        for(rotorIdThresh = 0.01; rotorIdThresh <= 1.0; rotorIdThresh *= 10)
        {
            
            //Start simulation "repeat Loop"
            for(int repeat = 0; repeat < repeatMAX; ++repeat)
            {
                //name files and set column headings.
        		MyFileNamer.IDFile(rotorIDstream, nu, repeat, rotorIdThresh);
                MyFileNamer.EdgeList(rotorIdInherit_T, nu, repeat, rotorIdThresh, "Temporal");
                MyFileNamer.EdgeList(rotorIdInherit_S, nu, repeat, rotorIdThresh, "Spatial");
                MyFileNamer.RotorExCountFile(rotorExCountstream,nu,repeat,rotorIdThresh);
                MyFileNamer.RotorCountFile(rotorCountstream,nu,repeat,rotorIdThresh);
                MyFileNamer.HistoFile(birthRateStream, nu, repeat, rotorIdThresh, "BirthRate");
                MyFileNamer.HistoFile(deathRateStream, nu, repeat, rotorIdThresh, "DeathRate");
                FOutRotorIDColumns(rotorIDstream);
                
                //Reset everything
                inN.reset(nu);
                inS.reset(nu);
                inE.reset(HOR);
                inW.reset(HOR);
                state.reset(0);
                state_update.reset(0);
                isRotorInit.reset(false);
                activeRotorId.reset(-1);
                rotorCellFrequency.reset(0);
                
                for(int k=0; k<G_WIDTH; ++k)
                {
                    for(int l=0; l<G_HEIGHT; ++l)
                    {
                        excitedBy(k,l) = make_pair(k,l);
                    }
                }

                for (auto it=isRotor.begin();it!=isRotor.end();it++) (*it).reset(false);
                
                rotorIdData.clear();
                tempRotorIdFrequency.clear();
                rotorIdAverageCoords.clear();
                isRotorAliveOld.clear();
                isRotorAliveNow.clear();
                maxRotorId=-1;
                exCoords = emptyCoords;
                rotorCoords = emptyCoords;
                
                rotorIdNetwork_T.reset();
                rotorIdNetwork_S.reset();
                
                drand.seed(time(NULL));
                
                //Start simulation "frame loop"
                for(frame=0; frame<=MAXFRAME; frame++)
                {
                    //update frame and cyclic frame values
                    //***function(cyclicNow, cyclicOld, cyclicBackRP, frame, MEMLIMIT, RP)
                    cyclicNow = frame%MEMLIMIT;
                    cyclicOld = (frame+MEMLIMIT-1)%MEMLIMIT;
                    cyclicBackRP = (frame+MEMLIMIT-1-RP)%MEMLIMIT;
                    rotorCount=0;
                    
                    //free memory in exCoords and rotorCells arrays.
                    if(frame>MEMLIMIT-1)
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
                            update_arrays(state_update(i_W,j), exCoords[cyclicNow], RP, i_W, j);
                            excitedBy(i_W,j) = make_pair(i,j);
                            inheritedRotorId(i_W, j) = inheritedRotorId(i,j);
                        }
                        
                        //east cell checks and excitation
                        if(i_E<GRIDSIZE && state(i_E,j)==0 && state_update(i_E,j) != RP && inW(i_E,j)>drand())
                        {
                            update_arrays(state_update(i_E,j), exCoords[cyclicNow], RP, i_E, j);
                            excitedBy(i_E,j) = make_pair(i,j);
                            inheritedRotorId(i_E, j) = inheritedRotorId(i,j);
                        }
                        
                        
                        //north cell checks and excitation
                        if(state(i,j_N)==0 && state_update(i,j_N) != RP && inS(i,j_N)>drand())
                        {
                            update_arrays(state_update(i,j_N), exCoords[cyclicNow], RP, i, j_N);
                            excitedBy(i,j_N) = make_pair(i,j);
                            inheritedRotorId(i, j_N) = inheritedRotorId(i,j);
                        }
                        
                        //south cell checks and excitation
                        if(state(i,j_S)==0 && state_update(i,j_S) != RP && inN(i,j_S)>drand())
                        {
                            update_arrays(state_update(i,j_S), exCoords[cyclicNow], RP, i, j_S);
                            excitedBy(i,j_S) = make_pair(i,j);
                            inheritedRotorId(i, j_S) = inheritedRotorId(i,j);
                        }
                    }
                    
                    //check for rotors
                    for (auto col = exCoords[cyclicNow].begin(); col != exCoords[cyclicNow].end(); col+=2)
                    {
                        
                        //First, fill up temp array with path of excitation.
                        fillTempCycle(tempCycleArray, rotorLengthLimit, excitedBy, *col, *(col+1));
                        
                        //Check if already a rotor cell, and continue if so.
                        if (isRotor[cyclicNow](tempCycleArray[rotorLengthLimit-1].first,tempCycleArray[rotorLengthLimit-1].second)) continue;
                        if (abs(tempCycleArray[rotorLengthLimit-1].second - *(col+1)) > rotorHeightCutoff) continue;
                        
                        //if no cycle repeats-> no rotor -> continue to next iteration. Otherwise, there is rotor-> add to rotor counter.
                        if(returnIsCycleRepeat(rotorLengthLimit, RP, tempCycleArray, cycleLength, cycleStart))rotorCount++;
                        else continue;
                        
                        //Rotor Id allocation
                        //fill rotor frequency map with previous rotor ids + find average X and Y.
                        double avX = 0;
                        double avY = 0;
                        calcAvPos(cycleStart, cycleLength, tempCycleArray, tempRotorIdFrequency, avX, avY, rotorHeightCutoff, GRIDSIZE, activeRotorId);
                        
                        //now find max rotor id frequency and corresponding "tempRotorId"
                        calcMaxFreqId(tempRotorIdFrequency, tempRotorId, maxFreq);
                        tempRotorIdFrequency.clear();//clear rotor id frequency map after use.
                        
                        //now check if tempRotorIdRatio > rotorIdThresh if so then inherit id. Exclude tempRotorId=-1.
                        tempRotorIdRatio=(double)maxFreq/cycleLength;
                        if(tempRotorIdRatio >= rotorIdThresh && tempRotorId != -1)
                        {
                            //put data in memory, [0]=i, [1]=j, [2]=state, [3]=rotorid
                            for (int m=cycleStart, k=0;k<cycleLength;++k)
                            {
                                int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second, state = state_update(i,j)+1;
                                
                                updateRotorCoords(rotorCoords[cyclicNow], i, j, state, tempRotorId);
                                activeRotorId(i,j)=tempRotorId;
                                inheritedRotorId(i,j)=tempRotorId;
                                rotorCellFrequency(i,j)++;
                                isRotor[cyclicNow](i,j)=true;
                            }
                            
                            //update rotorIdData[tempRotorId] struct +isRotorAliveNow map - don't add edge/node- no rotor is born.
                            updateRotorIdData(rotorIdData[tempRotorId], cycleLength, avX, avY);
                            isRotorAliveNow[tempRotorId] = true;
                        }
                        
                        //otherwise new rotor is born.
                        else
                        {
                            //define parent RotorId = inheritedRotorId for excited cell.
                            int parentRotorId = inheritedRotorId(*col, *(col+1));
                            
                            //add to max rotor id counter and assign this new id to rotor.
                            maxRotorId++;
                            
                            //put data in memory, [0]=i, [1]=j, [2]=state, [3]=rotorid
                            for (int m=cycleStart, k=0;k<cycleLength;++k)
                            {
                                int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second, state = state_update(i,j)+1;
                                
                                updateRotorCoords(rotorCoords[cyclicNow], i, j, state, maxRotorId);
                                activeRotorId(i,j)=maxRotorId;
                                inheritedRotorId(i,j)=maxRotorId;
                                rotorCellFrequency(i,j)++;
                                isRotor[cyclicNow](i,j)=true;
                            }
                            
                            //add rotorIdData struct + isRotorAliveNow = true for new_rotor = maxRotorId
                            rotorIdData.push_back(rotorIDstruct(frame, cycleLength, avX, avY));
                            isRotorAliveNow[maxRotorId] = true;
                        
                            //add node at maxRotorId, and frame at which node is created.
                            rotorIdNetwork_T.addNode(maxRotorId, (int)(avX + 0.5), (int)(avY + 0.5));
                            rotorIdNetwork_T.addNodeFrame(frame, maxRotorId);
                            
                            //calculate rotor bucket (spatial node) and add to network, alongside bucket position.
                            childBucket = calcBucket(rotorIdData[maxRotorId].birthX, rotorIdData[maxRotorId].birthY, bucketSize, noBuckets);
                            rotorIdNetwork_S.addNode(childBucket, bucketPos[childBucket].first, bucketPos[childBucket].second);
                            
                            //add edge between parentRotorId and inheritedRotorId if parent rotorId is alive.
                            //note that rotorId=-1 is never registered as alive.
                            if(isRotorAliveOld[parentRotorId])
                            {
                                //temporal network
                                rotorIdNetwork_T.addEdge(parentRotorId, maxRotorId, frame);
                                rotorIdNetwork_T.addEdgeFrame(frame, parentRotorId);
                                
                                //spatial network
                                parentBucket = calcBucket(rotorIdData[parentRotorId].deathX, rotorIdData[parentRotorId].deathY, bucketSize, noBuckets);
                                rotorIdNetwork_S.addEdge(parentBucket, childBucket, frame);
                            }
                        }
                    }
                    
                    //de-excitation process for refractory cells
                    deExciteState(exCoords, cyclicOld, cyclicBackRP, MEMLIMIT, state_update);
                    
                    //pacemaker
                    if(frame%SAP==0)pacemaker(state_update, exCoords[cyclicNow], RP, GRIDSIZE, excitedBy);
                    
                    //update loop variables.
                    state=state_update;
                    isRotorAliveOld=isRotorAliveNow;
                    isRotorAliveNow.clear();
                    updateActiveRotorId(isRotor[cyclicNow], activeRotorId);
                    
                    //fOutput FRAME vs. rotorCount
                    FOutFrameVsVar(rotorCountstream, frame, rotorCount);
                    
                }//end frame loop
                
                //create and output histogram of birth rates wrt. time.
                histogram birthRateWTime(MAXFRAME/500);
                histogram deathRateWTime(MAXFRAME/500);
                birthRateWTime.addPoints(getBirthDataVect(rotorIdData));
                deathRateWTime.addPoints(getDeathDataVect(rotorIdData));
                birthRateWTime.printHist(birthRateStream);
                deathRateWTime.printHist(deathRateStream);
                birthRateWTime.resetFrequency();
                deathRateWTime.resetFrequency();
                
                //fOutput Rotor + Network Data
                FOutRotorIdData(rotorIDstream, rotorIdData);
                FOutRotorExCountData(rotorExCountstream, rotorCellFrequency);
                rotorIdNetwork_T.FOutGMLEdgeList(rotorIdInherit_T);
                rotorIdNetwork_S.FOutGMLEdgeList(rotorIdInherit_S);
                
                //cOutput current progress.
                COutCurrentStatus(TotalIterations, iterationcount);
                iterationcount++;
                
            }//end repeat loop
            
        }//end threshold loop
        
    }//end nu loop
    
    return 0;
}







