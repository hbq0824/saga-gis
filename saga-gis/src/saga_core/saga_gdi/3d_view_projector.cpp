
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 3d_view_projector.cpp                 //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "3d_view_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_3DView_Projector::CSG_3DView_Projector(void)
{
	Set_Center  (0.0, 0.0, 0.0);
	Set_Scaling (1.0, 1.0, 1.0);
	Set_Rotation(0.0, 0.0, 0.0);
	Set_Shift   (0.0, 0.0, 1500.0);
	Set_Screen  (100, 100);

	m_Scale		= 1.0;
	m_bCentral	= true;
	m_dCentral	= 1500.0;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Center(double x, double y, double z)
{
	m_Center.x	= x;
	m_Center.y	= y;
	m_Center.z	= z;
}

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Scale(double Scale)
{
	if( Scale > 0.0 )
	{
		m_Scale	= Scale;
	}
}

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Scaling(double x, double y, double z)
{
	m_Scaling.x	= x;
	m_Scaling.y	= y;
	m_Scaling.z	= z;
}

void CSG_3DView_Projector::Set_xScaling(double x)
{	m_Scaling.x = x;	}

void CSG_3DView_Projector::Set_yScaling(double y)
{	m_Scaling.y = y;	}

void CSG_3DView_Projector::Set_zScaling(double z)
{	m_Scaling.z = z;	}

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Rotation(double x, double y, double z)
{
	Set_xRotation(x);
	Set_yRotation(y);
	Set_zRotation(z);
}

void CSG_3DView_Projector::Set_xRotation(double x)
{	m_Rotate.x = x; m_Sin.x = sin(x - M_PI_180); m_Cos.x = cos(x - M_PI_180);	}

void CSG_3DView_Projector::Set_yRotation(double y)
{	m_Rotate.y = y; m_Sin.y = sin(y); m_Cos.y = cos(y);	}

void CSG_3DView_Projector::Set_zRotation(double z)
{	m_Rotate.z = z; m_Sin.z = sin(z); m_Cos.z = cos(z);	}

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Shift(double x, double y, double z)
{
	Set_xShift(x);
	Set_yShift(y);
	Set_zShift(z);
}

void CSG_3DView_Projector::Set_xShift(double x)
{	m_Shift.x = x;	}

void CSG_3DView_Projector::Set_yShift(double y)
{	m_Shift.y = y;	}

void CSG_3DView_Projector::Set_zShift(double z)
{	m_Shift.z = z;	}

//---------------------------------------------------------
void CSG_3DView_Projector::Set_Screen(int Width, int Height)
{
	m_Screen_NX	= Width;
	m_Screen_NY	= Height;
}

//---------------------------------------------------------
void CSG_3DView_Projector::do_Central(bool bOn)
{
	m_bCentral	= bOn;
}

void CSG_3DView_Projector::Set_Central_Distance(double Distance)
{
	m_dCentral	= Distance;
}

void CSG_3DView_Projector::Inc_Central_Distance(double Distance, bool bAdjustShift)
{
	m_dCentral	+= Distance;

	if( bAdjustShift )
	{
		m_Shift.z	+= Distance;
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Projector::Get_Projection(double &x, double &y, double &z)
{
	TSG_Point_Z	p;

	x	= m_Scale * (x - m_Center.x) * m_Scaling.x;
	y	= m_Scale * (y - m_Center.y) * m_Scaling.y;
	z	= m_Scale * (z - m_Center.z) * m_Scaling.z;

	double	a	= (m_Cos.y * z + m_Sin.y * (m_Sin.z * y + m_Cos.z * x));
	double	b	= (m_Cos.z * y - m_Sin.z * x);

	p.x	= m_Shift.x + (m_Cos.y * (m_Sin.z * y + m_Cos.z * x) - m_Sin.y * z);
	p.y	= m_Shift.y + (m_Sin.x * a + m_Cos.x * b);
	p.z	= m_Shift.z + (m_Cos.x * a - m_Sin.x * b);

	if( m_bCentral )
	{
		p.x	*= m_dCentral / p.z;
		p.y	*= m_dCentral / p.z;
	}
	else
	{
		p.x	*= m_dCentral / m_Shift.z;
		p.y	*= m_dCentral / m_Shift.z;
	}

	x	= p.x + m_Screen_NX / 2;
	y	= p.y + m_Screen_NY / 2;
	z	= p.z;
}

void CSG_3DView_Projector::Get_Projection(TSG_Point_Z &p)
{
	Get_Projection(p.x, p.y, p.z);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
