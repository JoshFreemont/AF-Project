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
#include <map>
#include <utility>

class network
{

    std::map<int, std::vector<int> > edgeList;//stores a list of edges between nodes.
    std::map<int, bool> isNode;//stores if a node exists or not.
    std::map<int, int> nodeFrameCreate;//stores time when a node is created.
    std::map<int, std::vector<int> > edgeFrameCreateList;//stores frame when an edge is created
    std::map<int, std::pair<int, int> > nodePos;//stores x, y position of a node
    std::map<int, int> nodeCount;//stores count of how many instances of a node there are


public:
    network();
    network(const int maxNodeInit);//network with max node preassigned.
    void addEdge(int startNode, int endNode, int frame, int xDistance, int yDistance);
    void addNode(int nodeId, int x, int y);
    void reset();
    void FOutEdgeDistList (std::ofstream& aStream);
    void FOutGMLEdgeList (std::ofstream& aStream);
    void FOutGMLTreeEdgeList (std::ofstream& aStream);
    std::pair<int,int> getFirstEdgeXY();
};


#endif /* defined(__AF__network__) */
