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

class network
{
    int maxNode;
    std::vector<std::vector<int> * > edgeList;
    std::vector<bool*> isNode;
    std::vector<int> nodeFrameCreate;
    std::vector<std::vector<int> * > edgeFrameCreateList;
    
    
public:
    network(const int maxNodeInit);//network with max node preassigned.
    void addEdge(int startNode, int endNode);
    void addEdgeFrame(int frame, int nodeId);
    void addNode(int nodeId);
    void addNodeFrame(int frame, int nodeId);
    std::vector<std::vector<int> * > getEdgeList();
    std::vector<bool*> getIsNode();
    void outputEdgeList (std::ofstream& aStream);
    void outputTemporalEdgeList (std::ofstream& aStream);
};


#endif /* defined(__AF__network__) */
