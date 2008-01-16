/**
 * \file makept.cpp
 *
 * \brief makept, another simple C++ driver for CGM
 *
 * This program acts as a simple driver for CGM.  It reads in a geometry,
 * performs imprints between all the bodies, merges them, and writes information
 * on the results.  It also performs pairwise intersections between the
 * bodies to check for overlaps.  Results are written to stardard output.
 *
 */
#include "config.h"
#include "CpuTimer.hpp"
#include "GeometryModifyTool.hpp"
#include "GeometryQueryTool.hpp"
#include "OCCQueryEngine.hpp"
#include "CubitUtil.hpp"
#include "CubitMessage.hpp"
#include "CubitDefines.h"
#include "RefEntity.hpp"
#include "Body.hpp"
#include "RefVolume.hpp"
#include "RefFace.hpp"
#include "RefEdge.hpp"
#include "RefVertex.hpp"
#include "CubitObserver.hpp"
#include "CastTo.hpp"
#include "OCCModifyEngine.hpp"
#include "AppUtil.hpp"
#include "RefEntityFactory.hpp"
#include "RefEdge.hpp"
#include "BodySM.hpp"
#include "OCCBody.hpp"
#include "OCCSurface.hpp"
#include "OCCCurve.hpp"

// forward declare some functions used and defined later
CubitStatus read_geometry(int, char **);
CubitStatus make_Point();
// macro for printing a separator line
#define PRINT_SEPARATOR   PRINT_INFO("=======================================\n");


// main program - initialize, then send to proper function
int main (int argc, char **argv)
{

  CubitObserver::init_static_observers();
    // Initialize the GeometryTool
  
  CGMApp::instance()->startup( argc, argv );
  OCCQueryEngine::instance();
  OCCModifyEngine::instance();

    // If there aren't any file arguments, print usage and exit
  //if (argc == 1) {
  //  PRINT_INFO("Usage: mergechk <geom_file> [<geom_file> ...]\n");
  //  exit(0);
  //}
  
  CubitStatus status = CUBIT_SUCCESS;



  //Do make point.
  status = make_Point();
  if (status == CUBIT_FAILURE) 
     PRINT_INFO("Operation Failed");

  int ret_val = ( CubitMessage::instance()->error_count() );
  if ( ret_val > 0 )
  {
    PRINT_ERROR("Errors found during Mergechk session.\n");
  }
  return ret_val;
  
}

/// attribs module: list, modify attributes in a give model or models
/// 
/// Arguments: file name(s) of geometry files in which to look
///
CubitStatus read_geometry(int num_files, char **argv) 
{
  CubitStatus status = CUBIT_SUCCESS;
  GeometryQueryTool *gti = GeometryQueryTool::instance();
  assert(gti);
  int i;
  
  PRINT_SEPARATOR;

  for (i = 0; i < num_files; i++) {
    status = gti->import_solid_model(argv[i], "OCC");
    if (status != CUBIT_SUCCESS) {
      PRINT_ERROR("Problems reading geometry file %s.\n", argv[i]);
    }
  }
  PRINT_SEPARATOR;

  return CUBIT_SUCCESS;
}

CubitStatus make_Point()
{
  GeometryQueryTool *gti = GeometryQueryTool::instance();
  GeometryModifyTool *gmti = GeometryModifyTool::instance();

  OCCQueryEngine::instance();
  OCCModifyEngine::instance();

  // Read in the geometry from files specified on the command line
  char *argv = "./66_shaver3.brep";
  CubitStatus status = read_geometry(1, &argv);
  if (status == CUBIT_FAILURE) exit(1);

  argv = "./62_shaver1.brep";
  status = read_geometry(1, &argv);
  if (status == CUBIT_FAILURE) exit(1);

  argv = "./72_shaver6.brep";
  status = read_geometry(1, &argv);
  if (status == CUBIT_FAILURE) exit(1);
  
  CubitVector vector1(10,10,10);
  CubitVector vector2(15,15,15);
  DLIList<RefEntity*> free_entities;
  gti->get_free_ref_entities(free_entities);
 
  for(int i = 1; i <= free_entities.size(); i++)
  {
     RefEntity * entity = free_entities.get_and_step();
     gti->translate((BasicTopologyEntity*)entity, i*vector1); 
  }

  // Read in the geometry from files specified on the command line
  gmti->make_RefVertex(vector1,5);
  gmti->make_RefVertex(vector2,5);

  CubitStatus rsl = CUBIT_SUCCESS;
  DLIList<RefEntity*> ref_entity_list;
  int num_ents_exported=0;
  const CubitString cubit_version="10.2";
  const char * filename = "point.occ";
  const char * filetype = "OCC";
  
  rsl = gti->export_solid_model(ref_entity_list, filename, filetype, 
                                 num_ents_exported, cubit_version);

 
  //check for vertex
  DLIList<Body*> bodies;
  gti->bodies(bodies);
  gti->get_free_ref_entities(free_entities);
 
  RefVertex* vertex1 = CAST_TO(free_entities.get_and_step(),RefVertex);
  RefVertex* vertex2 = CAST_TO(free_entities.get(),RefVertex);
  CubitBoolean is_equal = gti->
		about_spatially_equal(vertex1,vertex2);
  //vertex1,vertex2 are not spatially equal. 
  
  CubitVector vi, vii;
  double d;
  gti->entity_entity_distance(vertex1,vertex2,vi, vii,d);
  // distance (d) between vertex1,vertex2
 
  //check for body
  d = bodies.get()->measure(); 
  // first body's volume.

  vi = bodies.get()->center_point();
  //first body's bounding box's center point.
  
  CubitBox box = bodies.get()->bounding_box();
  //first body's bounding box.

  gti->entity_entity_distance(gti->get_first_ref_volume(), vertex2,vi, vii,d);
  //first body and vertex2 's distance(d).

  BodySM* body = bodies.get()->get_body_sm_ptr();
  CubitVector axis(10,0,0);
  gti->reflect(bodies, axis);
  vi = bodies.get()->center_point();
  // After reflection, only x value should change.

  gti->scale(bodies.get(),2);
  vi = bodies.get()->center_point();
  // After scale, center point moved by 2 times 

  gti->translate(bodies.get(),axis);
  vi = bodies.get()->center_point();
  // After parellel move, center point moved by x (10)

  gti->rotate(bodies.get(), axis, 3.14/6);
  vi = bodies.get()->center_point();
  // After rotation, center point changed in y and z value.

  OCCBody* occ_body = CAST_TO(body, OCCBody);
  occ_body->mass_properties(vi, d);
  //true center and volume, volume should be 8 times of the original one.

  vi = occ_body->get_bounding_box().center(); 
  // bounding box center.

  //check for surface
  DLIList<OCCSurface*> surfaces;

  CAST_TO(body, OCCBody)->get_all_surfaces(surfaces);
  OCCSurface* surface = surfaces.step_and_get();
  GeometryType type = surface->geometry_type();
  // CONE_SURFACE_TYPE

  box = surface->bounding_box();
  // bounding box

  vi = surface->center_point();
  // center point

  CubitVector normal;
  surface->get_point_normal(vi, normal);
  // surface normal at center point.

  CubitVector closest_location ;
  CubitVector unit_normal_ptr ;
  CubitVector curvature1_ptr ;
  CubitVector curvature2_ptr ; 

  surface->closest_point(vi, &closest_location, &unit_normal_ptr, 
                         &curvature1_ptr, &curvature2_ptr);
  // another way to get normal at center point, and found surface
  // projection location for vi, here curvature1_ptr, &curvature2_ptr
  // returns nothing.

  double curvature1, curvature2;
  surface->principal_curvatures(closest_location, curvature1, curvature2);
  // get principal curvatures at center point.

  double u = 0.5;      
  double v = -20000;
  vi = surface->position_from_u_v(u,v);
  // get location on surface for it's u,v

  CubitBoolean periodic = surface->is_periodic();
  // found if surface is periodic.

  double p = 0; //period
  periodic = surface->is_periodic_in_U(p); 
  // found if surface is periodic in U and its period.

  periodic = surface->is_periodic_in_V(p);
  // found if surface is periodic in V and its period.

  double lower, upper;
  surface->get_param_range_U(lower, upper);
  // get surface U direction boundaries. here it's (2.48, 3.91)

  surface->get_param_range_V(lower, upper);
  // get surface V direction boundaries. here it's (-20010,-19998)

  CubitBoolean closed = surface->is_closed_in_U();
  // check if surface is closed in U direction.

  closed = surface->is_closed_in_V();
  // check if surface is closed in V direction.

  CubitPointContainment pc = surface->point_containment(1,-20000);
  // this (u,v) location should be outside of the surface.

  CubitPointContainment pc2 = surface->point_containment(3,-20000);
  // this (u,v) location should be inside of the surface.

  //test for curve
  DLIList<OCCCurve*> curves;
  CAST_TO(body, OCCBody)->get_all_curves(curves);

  OCCCurve *curve = curves.step_and_get();
  type = curve->geometry_type();
  // ARC_CURVE_TYPE

  box = curve-> bounding_box();
  // bounding box

  d = curve->measure();
  // length of the curve.  

  curve->get_param_range(lower,upper);
  // paremeter range.

  d = curve->length_from_u(lower,upper);
  // double check whole curve length.

  d = curve->length_from_u(lower/2+upper/2, upper);
  // half curve length.

  periodic = curve->is_periodic(p);
  // if curve is periodic and its period (p). here it's not.

  curve->position_from_u(lower/2+upper/2, vi);
  // middle point.

  CubitVector c_point, tangent, center;
  double radius;
  curve->closest_point(vi, c_point);
  // Closed point on middle point.

  curve->get_tangent(vi, tangent);
  // tangent at middle point.

  curve->get_point_direction(c_point, tangent);
  // double check tangent

  curve->get_curvature(c_point, curvature1_ptr);
  // get mid-point curvature.
 
  curve->get_center_radius(center, radius);
  // center and radius for arc curves

  pc = curve->point_containment(c_point);
  // middle point should be on the curve.

  //delete all entities
  gti->delete_Body(bodies);

  for (int j = free_entities.size(); j--;)
    {
      gti->delete_RefEntity( free_entities.get_and_step());
    }

  return rsl;
}
