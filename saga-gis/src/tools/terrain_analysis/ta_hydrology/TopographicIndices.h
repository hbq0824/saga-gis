
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 TopographicIndices.h                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__TopographicIndices_H
#define HEADER_INCLUDED__TopographicIndices_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTWI : public CSG_Tool_Grid
{
public:
	CTWI(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Topographic Indices" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CStream_Power : public CSG_Tool_Grid
{
public:
	CStream_Power(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Topographic Indices" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCIT : public CSG_Tool_Grid
{
public:
    CCIT(void);

    virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Topographic Indices" ));	}


protected:

    virtual bool			On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CLS_Factor : public CSG_Tool_Grid
{
public:
	CLS_Factor(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Topographic Indices" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

	int						m_Method, m_Stability;

	double					m_Erosivity;


	double					Get_LS			(double Slope, double SCA, bool bFeet);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTCI_Low : public CSG_Tool_Grid
{
public:
	CTCI_Low(void);

	virtual CSG_String		Get_MenuPath	(void)	{	return( _TL("Topographic Indices" ));	}


protected:

	virtual bool			On_Execute		(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__TopographicIndices_H
