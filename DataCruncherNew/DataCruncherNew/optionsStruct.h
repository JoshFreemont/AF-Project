//
//  optionsStruct.h
//  DataCruncherNew
//
//  Created by josh freemont on 21/03/2014.
//  Copyright (c) 2014 com.example. All rights reserved.
//

#ifndef DataCruncherNew_optionsStruct_h
#define DataCruncherNew_optionsStruct_h
#include <string>

//Options Struct
struct optionsStruct
{
	std::string m_FileHeader;
	bool m_DETECTROTORS;
	bool m_COUNTEXCELLS;
	bool m_DISPLAYFULLEXCELLS;
	bool m_BIRTHPROBDIST;
	bool m_BIRTHEXPECTATION;
	bool m_STATICMODEL;
	bool m_JOINTMODEL;
	bool m_JOINT2MODEL;
	bool m_OUTPUTDEFECTLOC;
	bool m_DETECTCLEANBIRTH;
    
	double m_nuSTART;
	double m_nuMAX;
	double m_nuSTEP;
	double m_delta;
	double m_epsilon;
	double m_HOR;
	int m_repeatMAX;
	int m_MAXFRAME;

	//Patch options
	bool m_INITPATCH;
	bool m_STATICCPATCH;
	bool m_ABLATE;
	int m_PATCHRADIUS;
	double m_patchNuSTART;
	double m_patchNuMAX;
	double m_patchNuSTEP;
};

#endif
