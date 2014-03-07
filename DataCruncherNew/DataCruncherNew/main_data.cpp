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
#include "DataOutput.h"
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
    int FRAME=0;
    
    //other parameters
    MTRand drand(time(NULL));//seed rnd generator
    
    //AF matrix declarations
    array2D<double> inN (G_WIDTH,G_HEIGHT);
    array2D<double> inE (G_WIDTH,G_HEIGHT);
    array2D<double> inS (G_WIDTH,G_HEIGHT);
    array2D<double> inW (G_WIDTH,G_HEIGHT);
    array2D<int> state (G_WIDTH,G_HEIGHT);
    array2D<int> state_update (G_WIDTH,G_HEIGHT);
    array2D<int> exFrame (G_WIDTH,G_HEIGHT);
    array2D<int> exFrameNew (G_WIDTH,G_HEIGHT);
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
    
    //Declare filenamer, and file streams.
    FileNamer MyFileNamer;
    MyFileNamer.setFileHeader("test");
    ofstream rotorIDInheritNetwork, rotorExCountstream;
	ofstream rotorCountstream;
	ofstream rotorIDstream;
    
    //rotor variables
    array2D <int> rotorCellFrequency (G_WIDTH,G_HEIGHT,0);
    const int rotorLengthLimit = 2*RP;
    array2D <bool> isRotorInit (G_WIDTH,G_HEIGHT,false);
    vector <array2D <bool> > isRotor (MEMLIMIT, isRotorInit);//Stores whether a cell is a "rotor" cell or not
    unordered_map<int, bool> isRotorAlive;
    unordered_map<int, bool> isRotorAliveNew;
    int rotorCount=0;
    
    //rotor id variables
    array2D<int> activeRotorId (G_WIDTH, G_HEIGHT, -1);//stores all rotor ids for the state.
    array2D<int> inheritedRotorId (G_WIDTH, G_HEIGHT, 0);//stores all rotor ids for all cells, whether in a rotor or not.
    unordered_map<int, int> tempRotorIdFrequency;//stores temp frequency count of ids
    double rotorIdThresh = 0.01, tempRotorIdRatio;//threshold for rotor id inheritance. 1.0=100% of current rotor cells must have same id.
    int maxFreq;//counts the maximum frequency rotor id within rotor
    int tempRotorId;//creates a temp rotor id variable.
    int maxRotorId=-1;//global maximum rotor id counter.
    vector<rotorIDstruct> rotorIDdata;
    vector< pair <int,int> > rotorIdAverageCoords;
    
    //cycle data variables
    pair <int,int> tempCycleArray[rotorLengthLimit+1];//Temporary array to store values
    int cycleStart = 0;
    int cycleLength = 0;
    int tortoise, hare;
    bool isCycle;
    
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
    int maxIdNode = 10000;
    network rotorIdNetwork(maxIdNode);
    
    //cell coordinates
    int i;
    int j;
    int i_W;
    int i_E;
    int j_N;
    int j_S;
    
    //setup to look at rotor duration for different nu in order to measure dynamism.
    double nu;
    double nuSTART = 0.02;
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
        		MyFileNamer.IDFile(rotorIDstream, nu, repeat, rotorIdThresh);
                FOutRotorIDColumns(rotorIDstream);
                MyFileNamer.EdgeList(rotorIDInheritNetwork, nu, repeat, rotorIdThresh);
                MyFileNamer.RotorExCountFile(rotorExCountstream,nu,repeat,rotorIdThresh);
                MyFileNamer.RotorCountFile(rotorCountstream,nu,repeat,rotorIdThresh);
                
                //set seed each time
                drand.seed(time(NULL));
                
                //Reset everything
                for(int k=0; k<G_WIDTH; ++k)
                {
                    for(int l=0; l<G_HEIGHT; ++l)
                    {
                        inN(k,l)=nu;
                        inS(k,l)=nu;
                        inE(k,l)=HOR;
                        inW(k,l)=HOR;
                        state(k,l)=0;
                        state_update(k,l)=0;
                        exFrame(k,l)=9999999;
                        exFrameNew(k,l)=9999999;
                        isRotorInit(k,l) = false;
                        activeRotorId(k,l) = -1;
                        excitedBy(k,l) = make_pair(k,l);
                        rotorCellFrequency(k,l) = 0;
                    }
                }
                
                int cyclicNow=0;
                int cyclicOld=0;
                int cyclicBackRP=0;
                
                for (auto it=isRotor.begin();it!=isRotor.end();it++)
                {
                    
                    for(int k=0; k<G_WIDTH; ++k)
                    {
                        for(int l=0; l<G_HEIGHT; ++l)
                        {
                            (*it)(k,l)=false;
                        }
                    }
                }
                
                rotorIDdata.clear();
                tempRotorIdFrequency.clear();
                rotorIdAverageCoords.clear();
                isRotorAlive.clear();
                isRotorAliveNew.clear();
                maxRotorId=-1;
                
                exCoords = emptyCoords;
                rotorCoords = emptyCoords;
                
                rotorIdNetwork.reset();
                
                
                //Start simulation "frame loop"
                for(FRAME=0; FRAME<=MAXFRAME; FRAME++)
                {
                    //update frame and cyclic frame values
                    cyclicNow = FRAME%MEMLIMIT;
                    cyclicOld = (FRAME+MEMLIMIT-1)%MEMLIMIT;
                    cyclicBackRP = (FRAME+MEMLIMIT-1-RP)%MEMLIMIT;
                    rotorCount=0;
                    
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
                    for (auto col = exCoords[cyclicNow].begin(); col!= exCoords[cyclicNow].end(); col+=2)
                    {
                        
                        //First, fill up temp array to check for cycles
                        tempCycleArray[0] = excitedBy(*col,*(col+1));
                        
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
                        
                        //if no cycle (rotor),continue otherwise add to rotor counter.
                        if(!isCycle)continue;
                        else rotorCount++;
                        
                        //Rotor Id allocation
                        //fill rotor frequency map with previous rotor ids for rotor cells. Hence work out if rotor Id is
                        //+find average X and Y coordinates
                        double averageX = 0;
                        double averageY = 0;
                        int minY=199;
                        int maxY=0;
                        
                        for (int m=cycleStart, k=0;k<cycleLength;++k)
                        {
                            int i = tempCycleArray[m+k].first, j = tempCycleArray[m+k].second;
                            if(j>maxY)maxY = j;
                            if(j<minY)minY = j;
                            averageX += i;
                            
                            //fill tempRotorIdFrequency map.
                            tempRotorIdFrequency[activeRotorId(i,j)]++;
                        }
                        averageX /= (double)(cycleLength);
                        
                        //This checks if the loop is at the 0/200 boundary
                        if(minY<40 && maxY>160)
                        {
        					for (int m=cycleStart, k=0;k<cycleLength;++k)
                            {
        						int j = tempCycleArray[m+k].second;
                                if(j<40)averageY+=j+200;
        						else averageY+=j;
                            }
                            //If average is out of bounds, subtract 200
        					if(averageY>=G_HEIGHT)averageY -= 200;
        					averageY /= (double)(cycleLength);
                        }
                        else
                        {
                            for (int m=cycleStart, k=0;k<cycleLength;++k)
                            {
        						int j = tempCycleArray[m+k].second;
        						averageY+=j;
                            }
        					averageY /= (double)(cycleLength);
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
                        
                        //now check if tempRotorIdRatio > rotorIdThresh if so then inherit id and put all rotor data in memory.
                        // + create new id if all current ids are zeros.
                        tempRotorIdRatio=(double)maxFreq/cycleLength;
                        
                        //inherit if threshold fulfilled
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
                            //update RotorIdData[tempRotorId] struct, and isRotorAliveNew map.
                            updateRotorIdData(rotorIDdata[tempRotorId], cycleLength, averageX, averageY);
                            isRotorAliveNew[tempRotorId] = true;
                            
                            //do not add edge to rotorId network as no rotor is born.
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
                            
                            //add RotorIDdata struct + isRotorAlive = true for new_rotor = maxRotorId
                            rotorIDdata.push_back(rotorIDstruct(FRAME,cycleLength,averageX,averageY));
                            isRotorAliveNew[maxRotorId] = true;
                            
                            //Update rotorIdNetwork- add node at maxRotorId, and frame at which node is created.
                            rotorIdNetwork.addNode(maxRotorId);
                            rotorIdNetwork.addNodeFrame(FRAME, maxRotorId);
                            
                            //add edge between parentRotorId and inheritedRotorId if parentRotorId != 0 + if parent rotorId is alive
                            if(parentRotorId && isRotorAlive[parentRotorId])
                            {
                                rotorIdNetwork.addEdge(parentRotorId, maxRotorId);
                                rotorIdNetwork.addEdgeFrame(FRAME, parentRotorId);
                            }
                        }
                    }
                    
                    //fOutput FRAME vs. rotorCount
                    FOutFrameVsVar(rotorCountstream, FRAME, rotorCount);
                    
                    
                    //de-excitation process for refractory cells
                    //***function(exCoords, cyclicOld, cyclicBackRP, MEMLIMIT, stateupdate)
                    
                    for (int row = cyclicOld, rowEnd = cyclicBackRP; row != rowEnd; row=(row-1+MEMLIMIT)%MEMLIMIT)
                    {
                        for(vector<int>::iterator col = exCoords[row].begin(), col_end = exCoords[row].end(); col != col_end; col+=2)
                        {
                            --state_update(*col,*(col+1));
                        }
                    }
                    
                    //pacemaker
                    if(FRAME%SAP==0)pacemaker(state_update, exCoords[cyclicNow], exFrameNew, FRAME, RP, GRIDSIZE, excitedBy);
                    
                    //update loop variables.
                    state=state_update;
                    exFrame=exFrameNew;
                    isRotorAlive=isRotorAliveNew;
                    isRotorAliveNew.clear();
                    
                    //update activeRotorId array.
                    //***function(isRotor[cyclicNow], activeRotorId)
                    for(int i=0; i<G_WIDTH; ++i)
                    {
                        for(int j=0; j<G_HEIGHT; ++j)
                        {
                            if(!isRotor[cyclicNow](i,j)) activeRotorId(i,j) = -1;
                        }
                    }
                    
                }//end frame loop
                
                //fOutput Rotor + Network Data
                FOutRotorIdData(rotorIDstream, rotorIDdata);
                FOutRotorExCountData(rotorExCountstream, rotorCellFrequency);
                rotorIdNetwork.FOutGMLEdgeList(rotorIDInheritNetwork);
                
                //cOutput current progress.
                COutCurrentStatus(TotalIterations, iterationcount);
                iterationcount++;
                
            }//end repeat loop
            
        }//end threshold loop
        
    }//end nu loop

    return 0;
}







