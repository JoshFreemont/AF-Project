//
//  network.cpp
//  AF
//
//  Created by josh freemont on 02/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#include "network.h"

network::network(const int maxNodeInit)
{
    maxNode=maxNodeInit;
    for(int i=0; i<maxNodeInit; ++i)
    {
        nodeEdgeList.push_back(new std::vector<int>);
    }
}

void network::addEdge(int startNode, int endNode)
{
    nodeEdgeList[startNode]->push_back(endNode);
    return;
}

std::vector<std::vector<int> * > network::getNodeEdgeList()
{
    return nodeEdgeList;
}




