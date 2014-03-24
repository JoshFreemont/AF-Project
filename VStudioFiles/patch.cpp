//
//  patch.cpp
//  sdltest
//
//  Created by josh freemont on 11/02/2014.
//  Copyright (c) 2014 josh freemont. All rights reserved.
//

#include "patch.h"
#include "mtrand.h"
#include <ctime>

void Spatch::ablate()
{
    for (int i=0;i<toAblate.m_Width;i++)
    {
        for (int j=0;j<toAblate.m_Height;j++)
        {
            if(toAblate(i,j))
            {
                (*inNAddress)(i,j) = 0;
                (*inSAddress)(i,j) = 0;
                (*inEAddress)(i,j) = 0;
                (*inWAddress)(i,j) = 0;
            }
        }
    }
}

void Cpatch::ablate()
{
    for (int i=0;i<toAblate.m_Width;i++)
    {
        for (int j=0;j<toAblate.m_Height;j++)
        {
            if(toAblate(i,j))
            {
                (*inNAddress)(i,j) = 0;
                (*inSAddress)(i,j) = 0;
                (*inEAddress)(i,j) = 0;
                (*inWAddress)(i,j) = 0;
            }
        }
    }
}

Spatch::Spatch(const int &size_init, const int &x_init, const int &y_init, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW): m_xcentre(x_init), m_sidelength(size_init), m_ycentre(y_init), toAblate(array2D<bool>(inN.m_Width,inN.m_Height,false))
{
    inNAddress=&inN;
    inSAddress=&inS;
    inEAddress=&inE;
    inWAddress=&inW;
    int GRIDHEIGHT = inN.m_Height;
	int GRIDWIDTH = inN.m_Width;

    //mark patch areas
    for(int i=x_init-m_sidelength/2; i<x_init+m_sidelength/2; ++i)
    {
        for(int j=y_init-m_sidelength/2; j<y_init+m_sidelength/2; ++j)
        {
			toAblate((i+GRIDWIDTH)%GRIDWIDTH,(j+GRIDHEIGHT)%GRIDHEIGHT)=true;
        }
    }
}

Cpatch::Cpatch(const int &radius_init, const int &x_init, const int &y_init, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW): m_xcentre(x_init), m_radius(radius_init), m_ycentre(y_init), toAblate(array2D<bool>(inN.m_Width,inN.m_Height,false))
{
    inNAddress=&inN;
    inSAddress=&inS;
    inEAddress=&inE;
    inWAddress=&inW;
    int GRIDHEIGHT = inN.m_Height;
	int GRIDWIDTH = inN.m_Width;
    
    //mark patch areas
    for(int i=x_init-m_radius; i<x_init+m_radius; ++i)
    {
        for(int j=y_init-m_radius; j<y_init+m_radius; ++j)
        {
			int r2= i*i + j*j;
            if(r2<=(m_radius*m_radius))
            {
				toAblate((i+GRIDWIDTH)%GRIDWIDTH,
					(j+GRIDHEIGHT)%GRIDHEIGHT)=true;
            }
        }
    }
}


void SpatchSTATIC::setPatch()
{
	MTRand drand(time(NULL));

	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_delta < drand())
				{
				(*inEAddress)(i,j)=1.0-m_epsilon;
				(*inWAddress)(i,j)=1.0-m_epsilon;
				}
				else
				{
				(*inEAddress)(i,j)=1.0;
				(*inWAddress)(i,j)=1.0;
				}
			}
        }
    }
	int GRIDHEIGHT = inNAddress->m_Height;
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_nu < drand())
				{
				(*inNAddress)(i,j) = (*inWAddress)(i,j);
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =
					(*inWAddress)(i,(j+1)%GRIDHEIGHT);
				}
				else
				{
					(*inNAddress)(i,j) =0;
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =0;
				}
			}
        }
    }
}

void SpatchDYN::setPatch()
{
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				(*inNAddress)(i,j)=m_nu;
				(*inSAddress)(i,j)=m_nu;
				(*inEAddress)(i,j)=m_hor;
				(*inWAddress)(i,j)=m_hor;
			}
        }
    }
}

void SpatchJOINT::setPatch()
{
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				(*inEAddress)(i,j)=m_hor;
				(*inWAddress)(i,j)=m_hor;
			}
        }
    }

	MTRand drand(time(NULL));
	int GRIDHEIGHT = inNAddress->m_Height;
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_nu < drand())
				{
				(*inNAddress)(i,j) = (*inWAddress)(i,j);
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =
					(*inWAddress)(i,(j+1)%GRIDHEIGHT);
				}
				else
				{
					(*inNAddress)(i,j) =0;
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =0;
				}
			}
        }
    }
}

void CpatchSTATIC::setPatch()
{
	MTRand drand(time(NULL));

	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_delta < drand())
				{
				(*inEAddress)(i,j)=1.0-m_epsilon;
				(*inWAddress)(i,j)=1.0-m_epsilon;
				}
				else
				{
				(*inEAddress)(i,j)=1.0;
				(*inWAddress)(i,j)=1.0;
				}
			}
        }
    }
	int GRIDHEIGHT = inNAddress->m_Height;
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_nu < drand())
				{
				(*inNAddress)(i,j) = (*inWAddress)(i,j);
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =
					(*inWAddress)(i,(j+1)%GRIDHEIGHT);
				}
				else
				{
					(*inNAddress)(i,j) =0;
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =0;
				}
			}
        }
    }
}

void CpatchDYN::setPatch()
{
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				(*inNAddress)(i,j)=m_nu;
				(*inSAddress)(i,j)=m_nu;
				(*inEAddress)(i,j)=m_hor;
				(*inWAddress)(i,j)=m_hor;
			}
        }
    }
}

void CpatchJOINT::setPatch()
{
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				(*inEAddress)(i,j)=m_hor;
				(*inWAddress)(i,j)=m_hor;
			}
        }
    }

	MTRand drand(time(NULL));
	int GRIDHEIGHT = inNAddress->m_Height;
	for(int i=0; i<inNAddress->m_Width; ++i)
    {
        for(int j=0; j<inNAddress->m_Height; ++j)
        {
			if (toAblate(i,j))
			{
				if (m_nu < drand())
				{
				(*inNAddress)(i,j) = (*inWAddress)(i,j);
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =
					(*inWAddress)(i,(j+1)%GRIDHEIGHT);
				}
				else
				{
					(*inNAddress)(i,j) =0;
				(*inSAddress)(i,(j+1)%GRIDHEIGHT) =0;
				}
			}
        }
    }
}