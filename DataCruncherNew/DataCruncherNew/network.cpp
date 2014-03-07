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
        edgeList.push_back(std::vector<int>(0));
        isNode.push_back(false);
        edgeFrameCreateList.push_back(std::vector<int>(0));
    }
    nodeFrameCreate.reserve(maxNodeInit);

}

void network::reset()
{
    edgeList.clear();
    isNode.clear();
    edgeFrameCreateList.clear();
    nodeFrameCreate.clear();
    for(int i=0; i<maxNode; ++i)
    {
        edgeList.push_back(std::vector<int>(0));
        isNode.push_back(false);
        edgeFrameCreateList.push_back(std::vector<int>(0));
    }
}

void network::addEdge(int startNode, int endNode)
{
    ((edgeList[startNode])).push_back(endNode);
    return;
}

//add frame of creation for child nodes to the edgeframecreatelist
void network::addEdgeFrame(int frame, int nodeId)
{
    ((edgeFrameCreateList[nodeId])).push_back(frame);
    return;
}

void network::addNode(int nodeIdValue)
{
    (isNode[nodeIdValue]) = true;
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
    for(int i=0; i<maxNode; ++i)
    {
        if((isNode[i]))
        {
            for(auto it = ((edgeList[i])).begin(); it != ((edgeList[i])).end(); ++it)
            {
                aStream<<i<<"\t"<<*it<<std::endl;
            }
        }

        else return;
    }
}

void network::FOutGMLEdgeList (std::ofstream& aStream)
{
    aStream<<"graph\n[\n";
    for(int i=0; i<maxNode; ++i)
    {
        if((isNode[i]))
        {

            aStream<<"\tnode\n\t[\n\tid "<<i<<std::endl;
            aStream<<"\tlabel \"" << i << "\"\n\t]\n";
        }
        else break;
    }

    for(int i=0; i<maxNode; ++i)
    {
        if((isNode[i]))
        {
            for(auto it = ((edgeList[i])).begin(); it != ((edgeList[i])).end(); ++it)
            {
                aStream<<"\tedge\n\t[\n\tsource "<<i<<std::endl;
                aStream<<"\ttarget " << *it << "\n\t]\n";
            }
        }

        else
        {
            aStream << "]";
            return;
        }
    }
}

void network::FOutTemporalEdgeList(std::ofstream& aStream)
{
    aStream<<"Start Node Creation Time"<<"\t"<<"End Node Creation Time"<<std::endl;
    int index = 0;
    for(auto nodeFrame = edgeFrameCreateList.begin(); nodeFrame != edgeFrameCreateList.end();  ++nodeFrame)
    {
        for(auto edgeFrame = ((*nodeFrame)).begin(); edgeFrame != ((*nodeFrame)).end(); ++edgeFrame)
        {
            aStream<<nodeFrameCreate[index]<<"\t"<<*edgeFrame<<std::endl;
        }

        ++index;
    }

    return;
}





