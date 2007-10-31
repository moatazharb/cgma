//-------------------------------------------------------------------------
// Filename      : OCCBody.cpp
//
// Purpose       : 
//
// Special Notes :
//
// Creator       : David White
//
// Creation Date : 7/18/00
//
//-------------------------------------------------------------------------

// ********** BEGIN STANDARD INCLUDES      **********
#include <assert.h>
// ********** END STANDARD INCLUDES        **********
#include "config.h"

// ********** BEGIN CUBIT INCLUDES         **********
#include "CubitDefines.h"
#include "CubitString.hpp"
#include "CubitPoint.hpp"
#include "CastTo.hpp"
#include "BodySM.hpp"
#include "Body.hpp"
#include "OCCBody.hpp"
#include "CubitSimpleAttrib.hpp"
#include "OCCQueryEngine.hpp"
#include "DLIList.hpp"
#include "FacetEvalTool.hpp"
#include "Surface.hpp"
#include "OCCSurface.hpp"
#include "CubitTransformMatrix.hpp"
#include "OCCPoint.hpp"
#include "OCCCurve.hpp"
#include "OCCCoEdge.hpp"
#include "OCCLoop.hpp"
#include "OCCShell.hpp"
#include "OCCLump.hpp"
#include "CubitFacetEdge.hpp"
#include "OCCModifyEngine.hpp"
#include "OCCAttrib.hpp"

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include "BRepBuilderAPI_Transform.hxx"
#include "gp_Ax1.hxx"
#include "gp_Ax2.hxx"
#include "Bnd_Box.hxx"
#include "BRepBndLib.hxx"

//-------------------------------------------------------------------------
// Purpose       : A constructor with a list of lumps that are attached.
//
// Special Notes :
//
//-------------------------------------------------------------------------
OCCBody::OCCBody(TopoDS_Shape *theShape)
{
  myTopoDSShape = theShape;
  update_bounding_box();
}

OCCBody::~OCCBody() 
{
    //Not sure what to do..
}

GeometryQueryEngine* OCCBody::get_geometry_query_engine() const
{
  return OCCQueryEngine::instance();
}

void OCCBody::append_simple_attribute_virt(CubitSimpleAttrib *csa)
  { attribSet.append_attribute(csa); }
  
void OCCBody::remove_simple_attribute_virt(CubitSimpleAttrib *csa)
  { attribSet.remove_attribute(csa); }

void OCCBody::remove_all_simple_attribute_virt()
  { attribSet.remove_all_attributes(); }
  
CubitStatus OCCBody::get_simple_attribute(DLIList<CubitSimpleAttrib*>& csa_list)
  { return attribSet.get_attributes(csa_list); }

CubitStatus OCCBody::get_simple_attribute( const CubitString& name,
                                          DLIList<CubitSimpleAttrib*>& csa_list )
  { return attribSet.get_attributes( name, csa_list ); }

CubitStatus OCCBody::save_attribs( FILE *file_ptr )
  { return attribSet.save_attributes( file_ptr); }

CubitStatus OCCBody::restore_attribs( FILE *file_ptr, unsigned int endian )
  { return attribSet.restore_attributes( file_ptr, endian ); }



//----------------------------------------------------------------
// Function: copy
// Description: create a new copy of the body.
// Author: sjowen
//----------------------------------------------------------------
BodySM* OCCBody::copy()
{
  return (BodySM*)NULL;
}
    
//----------------------------------------------------------------
// Function: move
// Description: translate the body and its child entities
//
// Author: Jane Hu
//----------------------------------------------------------------
CubitStatus OCCBody::move(double dx, double dy, double dz)
{
  gp_Vec aVec(dx, dy, dz);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(aVec);

  BRepBuilderAPI_Transform aBRepTrsf(*myTopoDSShape, aTrsf);
  update_bounding_box();
  return CUBIT_SUCCESS;
}


//----------------------------------------------------------------
// Function: rotate
// Description: rotate the body and its child entities
//
// Author: Jane Hu
//----------------------------------------------------------------
CubitStatus OCCBody::rotate( double x, double y, double z, 
                               double angle )//in radians
{
  gp_Pnt aOrigin(0,0,0);
  gp_Dir aDir(x, y, z);
  gp_Ax1 anAxis(aOrigin, aDir);

  //a is angular value of rotation in radians
  gp_Trsf aTrsf;
  aTrsf.SetRotation(anAxis, angle);

  BRepBuilderAPI_Transform aBRepTrsf(*myTopoDSShape, aTrsf);
  update_bounding_box();
  return CUBIT_SUCCESS;
}

//----------------------------------------------------------------
// Function: scale
// Description: scale the body and its child entities
//              use a constant scale factor
//
// Author: Jane Hu
//----------------------------------------------------------------
CubitStatus OCCBody::scale(double scale_factor )
{
  gp_Trsf aTrsf;
  aTrsf.SetScaleFactor(scale_factor);

  BRepBuilderAPI_Transform aBRepTrsf(*myTopoDSShape, aTrsf);
  update_bounding_box();
  return CUBIT_SUCCESS;  
}

//----------------------------------------------------------------
// Function: scale
// Description: scale the body and its child entities
//
// Author: 
//----------------------------------------------------------------
CubitStatus OCCBody::scale(double scale_factor_x,
                             double scale_factor_y,
                             double scale_factor_z )
{
  PRINT_ERROR("non-uniform scaling is not performed on OCC bodies");
  return CUBIT_FAILURE;
}

//----------------------------------------------------------------
// Function: restore
// Description: restore the body and its child entities
//              to its original coordinates using the inverse
//              transformation matrix
//
// Author: sjowen
//----------------------------------------------------------------
CubitStatus OCCBody::restore()
{
  // invert the transformation matrix and apply to entities 
  // (assumes an orthogonal matrix (ie. no shear or non-uniform scaling)

  CubitTransformMatrix inverse_mat;
  inverse_mat = myTransforms.inverse();

  CubitStatus stat = transform( inverse_mat, CUBIT_TRUE );

  if (stat == CUBIT_SUCCESS)
    myTransforms.set_to_identity();

  return stat;
}

//----------------------------------------------------------------
// Function: reflect
// Description: reflect the body about a exis
//
// Author: Jane Hu
//----------------------------------------------------------------
CubitStatus OCCBody::reflect( double reflect_axis_x,
                                double reflect_axis_y,
                                double reflect_axis_z )
{
  gp_Pnt aOrigin(0,0,0);
  gp_Dir aDir(reflect_axis_x, reflect_axis_y,reflect_axis_z); 
  gp_Ax2 anAx2(aOrigin, aDir);

  gp_Trsf aTrsf;
  aTrsf.SetMirror(anAx2);

  BRepBuilderAPI_Transform aBRepTrsf(*myTopoDSShape, aTrsf); 
  update_bounding_box();
  return CUBIT_SUCCESS;
}

//----------------------------------------------------------------
// Function: update_bounding_box
// Description: calculate for bounding box of this OCCBody
//
// Author: janehu
//----------------------------------------------------------------
void OCCBody::update_bounding_box() 
{
  Bnd_Box box;
  const TopoDS_Shape shape=*myTopoDSShape;
  //calculate the bounding box
  BRepBndLib::Add(shape, box);
  double min[3], max[3];
  
  //get values
  box.Get(min[0], min[1], min[2], max[0], max[1], max[2]);

  //update boundingbox.
  CubitBox cBox(min, max);
  boundingbox = cBox;
}

//----------------------------------------------------------------
// Function: get_bounding_box
// Description: get the  bounding box of this OCCBody
//
// Author: janehu
//----------------------------------------------------------------
CubitBox OCCBody::get_bounding_box()
{
  return boundingbox ;
}

//----------------------------------------------------------------
// Function: transform
// Description: transform the body based on a transformation matrix
//              main function for applying transformations to 
//              facet-based bodies
//
// Author: sjowen
//----------------------------------------------------------------
CubitStatus OCCBody::transform( CubitTransformMatrix &tfmat, 
                                  CubitBoolean is_rotation )
{
  return CUBIT_SUCCESS;
}

CubitStatus OCCBody::get_transforms( CubitTransformMatrix &tfm ) 
{
  tfm = myTransforms;
  return CUBIT_SUCCESS;
}

CubitStatus OCCBody::set_transforms( CubitTransformMatrix tfm ) 
{
  myTransforms = tfm;
  return CUBIT_SUCCESS;
}

int OCCBody::validate(const CubitString &, DLIList <TopologyEntity*>&)
{
  PRINT_ERROR("This option is not available for mesh defined geometry.\n");
  return 0;
}

void OCCBody::get_parents_virt( DLIList<TopologyBridge*>& ) 
  {}
  
void OCCBody::get_children_virt( DLIList<TopologyBridge*>& lumps )
{
  TopTools_IndexedMapOfShape M;
  TopExp::MapShapes(*myTopoDSShape, TopAbs_SOLID, M);
  int ii;
  for (ii=1; ii<=M.Extent(); ii++) {
	  TopologyBridge *lump = OCCQueryEngine::occ_to_cgm(M(ii));
	  lumps.append_unique(lump);
  }
}

//-------------------------------------------------------------------------
// Purpose       : Tear down topology
//
// Special Notes : 
//
// Creator       : Jason Kraftcheck
//
// Creation Date : 09/29/03
//-------------------------------------------------------------------------
void OCCBody::disconnect_all_lumps()
{
  myLumps.reset();
  for (int i = myLumps.size(); i--; )
  {
    Lump* sm_ptr = myLumps.get_and_step();
    OCCLump* lump = dynamic_cast<OCCLump*>(sm_ptr);
    if (lump)
    {
      assert(lump->get_body() == this);
      lump->remove_body();
    }
  }
  myLumps.clean_out();
}

//-------------------------------------------------------------------------
// Purpose       : Find centroid
//
// Special Notes : 
//
// Creator       : Jason Kraftcheck
//
// Creation Date : 05/10/04
//-------------------------------------------------------------------------
CubitStatus OCCBody::mass_properties( CubitVector& centroid, 
                                        double& volume )
{
  centroid.set( 0.0, 0.0, 0.0 );
  volume = 0.0;
  
  DLIList<OCCLump*> lumps (myLumps.size());
  CAST_LIST( myLumps, lumps, OCCLump );
  assert( myLumps.size() == lumps.size() );
  for (int i = lumps.size(); i--; )
  {
    CubitVector cent;
    double vol;
    if (CUBIT_SUCCESS != lumps.get_and_step()->mass_properties(cent,vol))
      return CUBIT_FAILURE;
    centroid += vol*cent;
    volume += vol;
  }
  if (volume > CUBIT_RESABS)
  {
    centroid /= volume;
  }
  else
  {
    centroid.set( 0.0, 0.0, 0.0 );
    volume = 0.0;
  }

  return CUBIT_SUCCESS;
}

//-------------------------------------------------------------------------
// Purpose       : Used to be OCCQueryEngine::is_point_in_body
//
// Special Notes : 
//
// Creator       : Jason Kraftcheck
//
// Creation Date : 05/10/04
//-------------------------------------------------------------------------
CubitPointContainment OCCBody::point_containment( const CubitVector &point )
{
  CubitPointContainment pc_value; 
  OCCLump *facet_lump;

  int i;
  for(i=myLumps.size(); i--;)
  {
    facet_lump = dynamic_cast<OCCLump*>(myLumps.get_and_step()); 
    pc_value = facet_lump->point_containment( point );
    if( pc_value == CUBIT_PNT_INSIDE )
      return CUBIT_PNT_INSIDE;
    else if( pc_value == CUBIT_PNT_BOUNDARY )
      return CUBIT_PNT_BOUNDARY;
  }

  return CUBIT_PNT_OUTSIDE;
}


