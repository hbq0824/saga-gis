
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     io_shapes_dxf                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     dxf_import.cpp                    //
//                                                       //
//                 Copyright (C) 2007 by                 //
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
#include "dxf_import.h"

#ifdef SYSTEM_DXFLIB
#include <dxflib/dl_dxf.h>
#else
#include "./dxflib/dl_dxf.h"
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define ADD_RESULT(list, result)	if( result->is_Valid() && result->Get_Count() > 0 ) Parameters(list)->asList()->Add_Item(result); else delete(result);


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	TBL_LAYERS_NAME		= 0,
	TBL_LAYERS_FLAGS
};

enum
{
	TBL_BLOCKS_NAME		= 0,
	TBL_BLOCKS_FLAGS,
	TBL_BLOCKS_X,
	TBL_BLOCKS_Y,
	TBL_BLOCKS_Z
};

enum
{
	TBL_POINTS_LAYER	= 0,
	TBL_POINTS_Z
};

enum
{
	TBL_LINES_LAYER		= 0,
	TBL_LINES_Z1,
	TBL_LINES_Z2
};

enum
{
	TBL_POLYOBJ_LAYER	= 0,
	TBL_POLYOBJ_FLAGS
};

enum
{
	TBL_TRIANGLE_LAYER	= 0,
	TBL_TRIANGLE_THICK,
	TBL_TRIANGLE_Z1,
	TBL_TRIANGLE_Z2,
	TBL_TRIANGLE_Z3
};

enum
{
	TBL_TEXT_LAYER		= 0,
	TBL_TEXT_Z,
	TBL_TEXT_TEXT,
	TBL_TEXT_HEIGHT,
	TBL_TEXT_ANGLE,
	TBL_TEXT_APX,
	TBL_TEXT_APY,
	TBL_TEXT_APZ,
	TBL_TEXT_SCALE,
	TBL_TEXT_HJUST,
	TBL_TEXT_VJUST,
	TBL_TEXT_STYLE,
	TBL_TEXT_FLAGS
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDXF_Import::CDXF_Import(void)
{
	Set_Name		(_TL("Import DXF Files"));

	Set_Author		("O.Conrad (c) 2007");

	Set_Description	(_TW(
		"This tool imports DXF files using the free \"dxflib\" library. Get more information "
	));
	
	Add_Reference(
		"http://www.ribbonsoft.com/dxflib.html", SG_T("RibbonSoft")
	);

	//-----------------------------------------------------
	Parameters.Add_Shapes_List("",
		"SHAPES"	, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table_List("",
		"TABLES"	, _TL("Tables"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format("DXF %s (*.dxf)|*.dxf|%s|*.*",
			_TL("Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Choice("",
		"FILTER"	, _TL("Import Filter"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("all entities"),
			_TL("only entities with layer definition"),
			_TL("only entities without layer definition")
		), 1
	);

	Parameters.Add_Double("",
		"DCIRCLE"	, _TL("Circle Point Distance [Degree]"),
		_TL(""),
		5., 0.01, true, 45., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDXF_Import::On_Execute(void)
{
	CSG_String	File(Parameters("FILE")->asString());

	if( !SG_File_Exists(File) )
	{
		Error_Fmt("%s [%s]", _TL("file does not exist"), File.c_str());

		return( false );
	}

	Parameters("TABLES")->asTableList ()->Del_Items();
	Parameters("SHAPES")->asShapesList()->Del_Items();

	m_Filter = Parameters("FILTER" )->asInt();
	m_dArc   = Parameters("DCIRCLE")->asDouble() * M_DEG_TO_RAD;

	//-----------------------------------------------------
	CSG_String	Name(SG_File_Get_Name(File, false));

	m_pLayers    = SG_Create_Table();
	m_pLayers    ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Layers"   ));
	m_pLayers    ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pLayers    ->Add_Field("FLAGS"    , SG_DATATYPE_Int);

	m_pBlocks    = SG_Create_Table();
	m_pBlocks    ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Blocks"   ));
	m_pBlocks    ->Add_Field("BLOCK"    , SG_DATATYPE_String);
	m_pBlocks    ->Add_Field("FLAGS"    , SG_DATATYPE_Int   );
	m_pBlocks    ->Add_Field("X"        , SG_DATATYPE_Double);
	m_pBlocks    ->Add_Field("Y"        , SG_DATATYPE_Double);
	m_pBlocks    ->Add_Field("Z"        , SG_DATATYPE_Double);

	m_pPoints    = SG_Create_Shapes(SHAPE_TYPE_Point);
	m_pPoints    ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Points"   ));
	m_pPoints    ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pPoints    ->Add_Field("Z"        , SG_DATATYPE_Double);

	m_pLines     = SG_Create_Shapes(SHAPE_TYPE_Line);
	m_pLines     ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Lines"    ));
	m_pLines     ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pLines     ->Add_Field("Z1"	    , SG_DATATYPE_Double);
	m_pLines     ->Add_Field("Z2"       , SG_DATATYPE_Double);

	m_pPolyLines = SG_Create_Shapes(SHAPE_TYPE_Line);
	m_pPolyLines ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Polylines"));
	m_pPolyLines ->Add_Field("LAYER"	, SG_DATATYPE_String);
	m_pPolyLines ->Add_Field("FLAGS"	, SG_DATATYPE_Int   );

	m_pPolygons  = SG_Create_Shapes(SHAPE_TYPE_Polygon);
	m_pPolygons  ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Polygons" ));
	m_pPolygons  ->Add_Field("LAYER"	, SG_DATATYPE_String);
	m_pPolygons  ->Add_Field("FLAGS"	, SG_DATATYPE_Int);

	m_pCircles   = SG_Create_Shapes(SHAPE_TYPE_Line);
	m_pCircles   ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Circles"  ));
	m_pCircles   ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pCircles   ->Add_Field("FLAGS"    , SG_DATATYPE_Int   );

	m_pTriangles = SG_Create_Shapes(SHAPE_TYPE_Polygon);
	m_pTriangles ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Triangles"));
	m_pTriangles ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pTriangles ->Add_Field("THICK"    , SG_DATATYPE_Int   );
	m_pTriangles ->Add_Field("Z1"       , SG_DATATYPE_Double);
	m_pTriangles ->Add_Field("Z2"       , SG_DATATYPE_Double);
	m_pTriangles ->Add_Field("Z3"       , SG_DATATYPE_Double);

	m_pText      = SG_Create_Shapes(SHAPE_TYPE_Point);
	m_pText      ->Fmt_Name("%s [%s]"   , Name.c_str(), _TL("Text"     ));
	m_pText      ->Add_Field("LAYER"    , SG_DATATYPE_String);
	m_pText      ->Add_Field("Z"        , SG_DATATYPE_Double);
	m_pText      ->Add_Field("TEXT"     , SG_DATATYPE_String);
	m_pText      ->Add_Field("HEIGHT"   , SG_DATATYPE_Int   );
	m_pText      ->Add_Field("ANGLE"    , SG_DATATYPE_Double);
	m_pText      ->Add_Field("APX"      , SG_DATATYPE_Double);
	m_pText      ->Add_Field("APY"      , SG_DATATYPE_Double);
	m_pText      ->Add_Field("APZ"      , SG_DATATYPE_Double);
	m_pText      ->Add_Field("SCALE"    , SG_DATATYPE_Double);
	m_pText      ->Add_Field("HJUST"    , SG_DATATYPE_Int   );
	m_pText      ->Add_Field("VJUST"    , SG_DATATYPE_Int   );
	m_pText      ->Add_Field("STYLE"    , SG_DATATYPE_String);
	m_pText      ->Add_Field("FLAGS"    , SG_DATATYPE_Int   );

	//-----------------------------------------------------
	m_Offset.x	= 0.;
	m_Offset.y	= 0.;
	m_Offset.z	= 0.;

	m_pPolyLine	= NULL;

	DL_Dxf	*pDXF	= new DL_Dxf();

	pDXF->in(File.b_str(), this);

	delete(pDXF);

	//-----------------------------------------------------
	ADD_RESULT("TABLES", m_pLayers   );
	ADD_RESULT("TABLES", m_pBlocks   );
	ADD_RESULT("SHAPES", m_pPoints   );
	ADD_RESULT("SHAPES", m_pLines    );
	ADD_RESULT("SHAPES", m_pPolyLines);
	ADD_RESULT("SHAPES", m_pPolygons );
	ADD_RESULT("SHAPES", m_pCircles  );
	ADD_RESULT("SHAPES", m_pTriangles);
	ADD_RESULT("SHAPES", m_pText     );

	return( Parameters("SHAPES")->asShapesList()->Get_Item_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CDXF_Import::Check_Process(void)
{
	static int	iProcess	= 0;

	if( (iProcess++) % 100 == 0 )
	{
		Process_Get_Okay();
	}
}

//---------------------------------------------------------
inline bool CDXF_Import::Check_Layer(const CSG_String &Name)
{
	Check_Process();

	switch( m_Filter )
	{
	case  1: return( Name.Cmp("0") != 0 );
	case  2: return( Name.Cmp("0") == 0 );
	default: return( true );
	}
}

//---------------------------------------------------------
inline void CDXF_Import::Add_Arc_Point(CSG_Shape *pShape, double cx, double cy, double d, double theta)
{
	pShape->Add_Point(
		cx + d * cos(theta),
		cy + d * sin(theta)
	);
}

//---------------------------------------------------------
void CDXF_Import::Add_Arc(CSG_Shape *pShape, double cx, double cy, double d, double alpha, double beta)
{
	double	theta;

	alpha	*= M_DEG_TO_RAD;
	beta	*= M_DEG_TO_RAD;

	if( alpha > beta )
	{
		beta	+= M_PI_360;
	}

	for(theta=alpha; theta<beta; theta+=m_dArc)
	{
		Add_Arc_Point(pShape, cx, cy, d, theta);
	}

	Add_Arc_Point(pShape, cx, cy, d, beta);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CDXF_Import::addLayer(const DL_LayerData &data)
{
	CSG_Table_Record	*pRecord	= m_pLayers->Add_Record();

	pRecord->Set_Value(TBL_LAYERS_NAME , CSG_String(data.name.c_str()));
	pRecord->Set_Value(TBL_LAYERS_FLAGS, data.flags);
}

//---------------------------------------------------------
void CDXF_Import::addBlock(const DL_BlockData &data)
{
	CSG_Table_Record	*pRecord	= m_pBlocks->Add_Record();

	pRecord->Set_Value(TBL_BLOCKS_NAME , CSG_String(data.name.c_str()));
	pRecord->Set_Value(TBL_BLOCKS_FLAGS, data.flags);
	pRecord->Set_Value(TBL_BLOCKS_X    , data.bpx);
	pRecord->Set_Value(TBL_BLOCKS_Y    , data.bpy);
	pRecord->Set_Value(TBL_BLOCKS_Z    , data.bpz);
}

//---------------------------------------------------------
void CDXF_Import::endBlock(void)
{
}

//---------------------------------------------------------
void CDXF_Import::addPoint(const DL_PointData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pPoint	= m_pPoints->Add_Shape();

	pPoint->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);

	pPoint->Set_Value(TBL_POINTS_LAYER, CSG_String(attributes.getLayer().c_str()));
	pPoint->Set_Value(TBL_POINTS_Z    , m_Offset.z + data.z);
}

//---------------------------------------------------------
void CDXF_Import::addLine(const DL_LineData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	CSG_Shape	*pLine	= m_pLines->Add_Shape();

	pLine->Add_Point(m_Offset.x + data.x1, m_Offset.y + data.y1);
	pLine->Add_Point(m_Offset.x + data.x2, m_Offset.y + data.y2);

	pLine->Set_Value(TBL_LINES_LAYER, CSG_String(attributes.getLayer().c_str()));
	pLine->Set_Value(TBL_LINES_Z1   , m_Offset.z + data.z1);
	pLine->Set_Value(TBL_LINES_Z2   , m_Offset.z + data.z2);
}

//---------------------------------------------------------
void CDXF_Import::addPolyline(const DL_PolylineData &data)
{
	if( !Check_Layer(attributes.getLayer().c_str()) )
		return;

	switch( data.flags )
	{
	default: m_pPolyLine = m_pPolyLines->Add_Shape(); break;
	case  1: m_pPolyLine = m_pPolygons ->Add_Shape(); break;
	}

	m_pPolyLine->Set_Value(TBL_POLYOBJ_LAYER, CSG_String(attributes.getLayer().c_str()));
}

//---------------------------------------------------------
void CDXF_Import::addVertex(const DL_VertexData &data)
{
	if( m_pPolyLine )
	{
		m_pPolyLine->Add_Point(m_Offset.x + data.x, m_Offset.y + data.y);	// data.z, data.bulge
	}
}

//---------------------------------------------------------
void CDXF_Import::endSequence(void)
{
	if( m_pPolyLine )
	{
		if( !m_pPolyLine->is_Valid() )
		{
			((CSG_Shapes *)m_pPolyLine->Get_Table())->Del_Shape(m_pPolyLine);
		}

		m_pPolyLine	= NULL;
	}
}

//---------------------------------------------------------
void CDXF_Import::addCircle(const DL_CircleData &data)
{
	if( Check_Layer(attributes.getLayer().c_str()) )
	{
		CSG_Shape	*pCircle	= m_pCircles->Add_Shape();

		Add_Arc(pCircle, data.cx, data.cy, data.radius, 0., 360.);
	}
}

//---------------------------------------------------------
void CDXF_Import::addArc(const DL_ArcData &data)
{
	if( Check_Layer(attributes.getLayer().c_str()) )
	{
		CSG_Shape	*pArc	= m_pPolyLine ? m_pPolyLine : m_pPolyLines->Add_Shape();

		Add_Arc(pArc, data.cx, data.cy, data.radius, data.angle1, data.angle2);

		if( pArc != m_pPolyLine )
		{
			pArc->Set_Value(TBL_POLYOBJ_LAYER, CSG_String(attributes.getLayer().c_str()));
		}
	}
}

//---------------------------------------------------------
void CDXF_Import::add3dFace(const DL_3dFaceData &data)
{
	if( Check_Layer(attributes.getLayer().c_str()) )
	{
		CSG_Shape	*pTriangle	= m_pTriangles->Add_Shape();

		for(int i=0; i<3; i++)
		{
			pTriangle->Add_Point(m_Offset.x + data.x[i], m_Offset.y + data.y[i]);
		}

		pTriangle->Set_Value(TBL_TRIANGLE_LAYER, CSG_String(attributes.getLayer().c_str()));
		pTriangle->Set_Value(TBL_TRIANGLE_THICK, data.thickness);
		pTriangle->Set_Value(TBL_TRIANGLE_Z1   , m_Offset.z + data.z[0]);
		pTriangle->Set_Value(TBL_TRIANGLE_Z2   , m_Offset.z + data.z[1]);
		pTriangle->Set_Value(TBL_TRIANGLE_Z3   , m_Offset.z + data.z[2]);
	}
}

//---------------------------------------------------------
void CDXF_Import::addText(const DL_TextData &data)
{
	if( Check_Layer(attributes.getLayer().c_str()) )
	{
		CSG_Shape	*pText	= m_pText->Add_Shape();

		pText->Add_Point(m_Offset.x + data.ipx, m_Offset.y + data.ipy);

		pText->Set_Value(TBL_TEXT_LAYER , CSG_String(attributes.getLayer().c_str()));
		pText->Set_Value(TBL_TEXT_Z     , m_Offset.z + data.ipz);
		pText->Set_Value(TBL_TEXT_TEXT  , CSG_String(data.text.c_str()));
		pText->Set_Value(TBL_TEXT_HEIGHT, data.height);
		pText->Set_Value(TBL_TEXT_ANGLE	, data.angle * M_RAD_TO_DEG);
		pText->Set_Value(TBL_TEXT_APX   , m_Offset.z + data.apx);
		pText->Set_Value(TBL_TEXT_APY   , m_Offset.z + data.apy);
		pText->Set_Value(TBL_TEXT_APZ   , m_Offset.z + data.apz);
		pText->Set_Value(TBL_TEXT_SCALE , data.xScaleFactor);
		pText->Set_Value(TBL_TEXT_HJUST , data.hJustification);
		pText->Set_Value(TBL_TEXT_VJUST , data.vJustification);
		pText->Set_Value(TBL_TEXT_STYLE , CSG_String(data.style.c_str()));
		pText->Set_Value(TBL_TEXT_SCALE , data.textGenerationFlags);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
