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

class network
{
    int maxNode;
    std::vector<std::vector<int> * > nodeEdgeList;
    
public:
    network(const int maxNodeInit);//network with max node preassigned.
    void addEdge(int startNode, int endNode);
    std::vector<std::vector<int> * > getNodeEdgeList();
};


#endif /* defined(__AF__network__) */
