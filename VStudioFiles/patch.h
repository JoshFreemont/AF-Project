//
//  patch.h
//  sdltest
//
//  Created by josh freemont on 11/02/2014.
//  Modified by basil lim on 24/3/2014
//  Copyright (c) 2014 josh freemont. All rights reserved.
//

#ifndef __sdltest__patch__
#define __sdltest__patch__

#include <iostream>
#include "array2D.h"

//Base patch


//Base square patch
class Spatch
{
public:
    int m_sidelength;
    int m_xcentre;
    int m_ycentre;
    array2D<double>* inNAddress;
    array2D<double>* inSAddress;
    array2D<double>* inEAddress;
    array2D<double>* inWAddress;
    array2D<bool> toAblate;
    Spatch(const int &size_init, const int &x_init, const int &y_init, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW);
    void ablate();
    virtual void setPatch() =0;
};
//No need for deconstructor because no dynamic memory allocated


//Base circle patch
class Cpatch
{
public:
    int m_radius;
    int m_xcentre;
    int m_ycentre;
    array2D<double>* inNAddress;
    array2D<double>* inSAddress;
    array2D<double>* inEAddress;
    array2D<double>* inWAddress;
    array2D<bool> toAblate;
    Cpatch(const int &size_init, const int &x_init, const int &y_init, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW);
    void ablate();
    virtual void setPatch() =0;
};


//Square patch for dynamic model
class SpatchDYN: public Spatch
{
public:
    double m_nu;
    double m_hor;
	SpatchDYN(const int &size, const int &x, const int &y, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW, double inNu, double inHor): Spatch(size, x, y, inN, inS, inE, inW), m_nu(inNu), m_hor(inHor){}
	virtual void setPatch();
};


//Square patch static model
class SpatchSTATIC: public Spatch
{
public:

    double m_delta;
    double m_nu;
    double m_epsilon;
    SpatchSTATIC(const int &size, const int &x, const int &y, array2D<double> &inN, array2D<double> &inS, array2D<double> &inE, array2D<double> &inW, double inDelta, double inNu, double inEpsilon): Spatch(size, x, y, inN, inS, inE, inW), m_nu(inNu), m_delta(inDelta), m_epsilon(inEpsilon){}
	virtual void setPatch();
};

//Square patch joint model
class SpatchJOINT: public Spatch
{
public:
    double m_nu;
    double m_hor;
    SpatchJOINT(const int &size, const int &x, const int &y,
		array2D<double> &inN, array2D<double> &inS, array2D<double> &inE,
		array2D<double> &inW, double inNu, double inHor):
		Spatch(size, x, y, inN, inS, inE, inW), m_nu(inNu), m_hor(inHor){}
		virtual void setPatch();
};


//Circle patch dynamic model
class CpatchDYN: public Cpatch
{
public:
    double m_nu;
    double m_hor;
    CpatchDYN(const int &radius, const int &x, const int &y, array2D<double> &inN,
		array2D<double> &inS, array2D<double>& inE,
        array2D<double>& inW, double inNu, double inHor): Cpatch(radius, x, y, inN, 
		inS, inE, inW),
		m_nu(inNu), m_hor(inHor){}
	virtual void setPatch();
};


//Circle patch static model
class CpatchSTATIC: public Cpatch
{
public:
    double m_delta;
    double m_nu;
    double m_epsilon;
    CpatchSTATIC(const int &radius, const int &x, const int &y,
		array2D<double> &inN, array2D<double> &inS, array2D<double>& inE,
		array2D<double>& inW, double inDelta, double inNu, double inEpsilon):
	Cpatch(radius, x, y, inN, inS, inE, inW), m_nu(inNu), m_delta(inDelta), 
		m_epsilon(inEpsilon){}
	virtual void setPatch();
};

//Circle patch joint model
class CpatchJOINT: public Cpatch
{
public:
    double m_nu;
    double m_hor;
    CpatchJOINT(const int &radius, const int &x, const int &y,
		array2D<double> &inN, array2D<double> &inS, array2D<double>& inE,
		array2D<double>& inW, double inNu, double inHor):
	Cpatch(radius, x, y, inN, inS, inE, inW), m_nu(inNu), m_hor(inHor){}
	virtual void setPatch();
};


#endif