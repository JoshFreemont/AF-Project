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
        edgeList.push_back(new std::vector<int>);
        isNode.push_back(new bool);
        *(isNode[i]) = false;
    }
}

void network::addEdge(int startNode, int endNode)
{
    edgeList[startNode]->push_back(endNode);
    return;
}

void network::addNode(int nodeIdValue)
{
    *(isNode[nodeIdValue]) = true;
    return;
}

std::vector<std::vector<int> * > network::getEdgeList()
{
    return edgeList;
}




