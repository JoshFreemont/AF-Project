//
//  network.cpp
//  AF
//
//  Created by josh freemont on 02/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#include "network.h"

//empty constructor.
network::network()
{}

//constructor for initializing with a certain no. nodes
network::network(const int maxNodeInit)
{
    for(int i=0; i<maxNodeInit; ++i)
    {
        isNode[i] = true;
    }
}

void network::reset()
{
    edgeList.clear();
    isNode.clear();
    edgeFrameCreateList.clear();
    nodeFrameCreate.clear();
}

void network::reset(const int maxNodeInit)
{
    edgeList.clear();
    isNode.clear();
    edgeFrameCreateList.clear();
    nodeFrameCreate.clear();
    
    for(int i=0; i<maxNodeInit; ++i)
    {
        isNode[i] = true;
    }
}
void network::addEdge(int startNode, int endNode, int frame)
{
    edgeList[startNode].push_back(endNode);
    edgeList[startNode].push_back(frame);
    return;
}

//add frame of creation for child nodes to the edgeframecreatelist
void network::addEdgeFrame(int frame, int nodeId)
{
    edgeFrameCreateList[nodeId].push_back(frame);
    return;
}

void network::addNode(int nodeIdValue)
{
    isNode[nodeIdValue] = true;
    return;
}

void network::addNodeFrame(int frame, int nodeId)
{
    nodeFrameCreate[nodeId]=frame;
    return;
}

void network::FOutEdgeList (std::ofstream& aStream)
{
    aStream<<"Start Node"<<"\t"<<"End Node"<<std::endl;
    for(auto node = edgeList.begin(); node != edgeList.end(); node++)
    {
            for(auto it = node->second.begin(); it != node->second.end(); ++it)
            {
                aStream<< node->first << "\t" << *it << std::endl;
            }
    }
}

void network::FOutGMLEdgeList (std::ofstream& aStream)
{
    aStream<<"graph\n[\n";
    for(auto node = isNode.begin(); node != isNode.end(); ++node)
    {
        if(node->second)
        {
            aStream<< "\tnode\n\t[\n\tid " << node->first << std::endl;
            aStream<< "\tlabel \"" << node->first << "\"" << std::endl;
            aStream<< "\tframe " << nodeFrameCreate[node->first] << "\n\t]\n";
        }
        
        else continue;
    }

    for(auto node = edgeList.begin(); node!=edgeList.end(); ++node)
    {
        for(auto it = node->second.begin(); it != node->second.end(); it+=2)
        {
            aStream << "\tedge\n\t[\n\tsource " << node->first << std::endl;
            aStream << "\ttarget " << *it << std::endl;
            aStream << "\tframe "<< *(it+1) << "\n\t]\n";
        }
    }
    
    aStream << "]";
    return;
}

void network::FOutTemporalEdgeList(std::ofstream& aStream)
{
    aStream<<"Start Node Creation Time"<<"\t"<<"End Node Creation Time"<<std::endl;
    int index = 0;
    for(auto nodeFrame = edgeFrameCreateList.begin(); nodeFrame != edgeFrameCreateList.end();  ++nodeFrame)
    {
        for(auto edgeFrame = (*nodeFrame).second.begin(); edgeFrame != (*nodeFrame).second.end(); ++edgeFrame)
        {
            aStream << nodeFrameCreate[index] << "\t" << *edgeFrame << std::endl;
        }
        ++index;
    }
    return;
}





