//
//  network.h
//  AF
//
//  Created by josh freemont on 02/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#ifndef __AF__network__
#define __AF__network__

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

class network
{

    std::unordered_map<int, std::vector<int> > edgeList;
    std::unordered_map<int, bool> isNode;
    std::unordered_map<int, int> nodeFrameCreate;
    std::unordered_map<int, std::vector<int> > edgeFrameCreateList;


public:
    network();
    network(const int maxNodeInit);//network with max node preassigned.
    void addEdge(int startNode, int endNode, int frame);
    void addEdgeFrame(int frame, int nodeId);
    void addNode(int nodeId);
    void reset();
    void reset(const int maxNodeInit);
    void addNodeFrame(int frame, int nodeId);
    void FOutEdgeList (std::ofstream& aStream);
    void FOutTemporalEdgeList (std::ofstream& aStream);
    void FOutGMLEdgeList (std::ofstream& aStream);
    
};


#endif /* defined(__AF__network__) */
