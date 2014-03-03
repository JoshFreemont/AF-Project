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
        edgeFrameCreateList.push_back(new std::vector<int>);
        
    }
    nodeFrameCreate.reserve(maxNodeInit);
    
}

void network::addEdge(int startNode, int endNode)
{
    (*(edgeList[startNode])).push_back(endNode);
    return;
}

//add frame of creation for child nodes to the edgeframecreatelist
void network::addEdgeFrame(int frame, int nodeId)
{
    (*(edgeFrameCreateList[nodeId])).push_back(frame);
    return;
}

void network::addNode(int nodeIdValue)
{
    *(isNode[nodeIdValue]) = true;
    return;
}

void network::addNodeFrame(int frame, int nodeId)
{
    nodeFrameCreate[nodeId]=frame;
    return;
}

std::vector<std::vector<int> * > network::getEdgeList()
{
    return edgeList;
}

std::vector<bool*> network::getIsNode()
{
    return isNode;
}

void network::outputEdgeList (std::ofstream& aStream)
{
    aStream<<"Start Node"<<"\t"<<"End Node"<<"\t"<<std::endl;
    for(int i=0; i<maxNode; ++i)
    {
        if(*(isNode[i]))
        {
            for(auto it = (*(edgeList[i])).begin(); it != (*(edgeList[i])).end(); ++it)
            {
                aStream<<i<<"\t"<<*it<<"\t"<<std::endl;
            }
        }
        
        else return;
    }
}

void network::outputTemporalEdgeList(std::ofstream& aStream)
{
    aStream<<"Start Node Creation Time"<<"\t"<<"End Node Creation Time"<<"\t"<<std::endl;
    int index = 0;
    for(auto nodeFrame = edgeFrameCreateList.begin(); nodeFrame != edgeFrameCreateList.end();  ++nodeFrame)
    {
        for(auto edgeFrame = (*(*nodeFrame)).begin(); edgeFrame != (*(*nodeFrame)).end(); ++edgeFrame)
        {
            aStream<<nodeFrameCreate[index]<<"\t"<<*edgeFrame<<"\t"<<std::endl;
        }
        
        ++index;
    }
    
    return;
}





