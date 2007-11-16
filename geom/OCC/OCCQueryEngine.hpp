//-------------------------------------------------------------------------
// Filename      : OCCQueryEngine.hpp
//
// Purpose       : Facet geometry engine.
//
//                 This class is implemented as a Singleton pattern. Only
//                 one instance is created and it is accessed through the 
//                 {instance()} static member function.
//
// Special Notes :
//
// Creator       : David R. White
//
// Creation Date : 6/29/00
//
//-------------------------------------------------------------------------

#ifndef OCC_GEOMETRY_ENGINE_HPP
#define OCC_GEOMETRY_ENGINE_HPP

// ********** BEGIN STANDARD INCLUDES         **********

#include <typeinfo>
#if !defined(NT) && !defined(CANT_USE_STD)
using std::type_info;
#endif

// ********** END STANDARD INCLUDES           **********

// ********** BEGIN CUBIT INCLUDES            **********
#include "config.h"
#include "CubitFileIOWrapper.hpp"
#include "GeometryQueryEngine.hpp"

#include <map>
// ********** END CUBIT INCLUDES              **********

// ********** BEGIN FORWARD DECLARATIONS
class TopologyEntity;
class TopologyBridge;
class RefEntity;
class Body;
class Shell;
class ShellSM;
class Loop;
class Chain;
class CoEdgeSM;
class LoopSM;
class RefVolume;
class RefFace;
class RefEdge;
class RefVertex;
class Point;
class Curve;
class Surface;
class Lump;
class BodySM;

class GeometryEntity;
class BodySM;
class Lump;
class ShellSM;
class Surface;
class LoopSM;
class Curve;
class CoEdgeSM;
class Point;
class TopologyEntity;
class CubitBox;
class CubitString;

class OtherSolidModelingEntity;
class OCCLump;
class OCCShell;
class OCCLoop;
class OCCSurface;
class OCCBody;
class OCCCoEdge;
class OCCCurve;
class OCCPoint;
 
class TopTools_DataMapOfShapeInteger;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Solid;
class TopoDS_Shell;
// ********** END FORWARD DECLARATIONS        **********

// ********** BEGIN MACRO DEFINITIONS         **********
// ********** END MACRO DEFINITIONS           **********

// ********** BEGIN ENUM DEFINITIONS          **********

// ********** END ENUM DEFINITIONS            **********

class OCCQueryEngine : public GeometryQueryEngine
{
public:
// ********** BEGIN FRIEND DECLARATIONS        **********
  friend class OCCSurface;
  
// ********** END FRIEND DECLARATIONS        **********

//HEADER- Constructor and Destructor
  
  static OCCQueryEngine* instance();
    //- Singleton pattern
    //- Controlled access and creation of the sole instance of this class.

  virtual ~OCCQueryEngine();
  
  const char* modeler_type()
     { return "OCC"; }

  TopoDS_Shape *get_TopoDS_Shape_of_entity( TopologyBridge * entity);
  int get_major_version();

  int get_minor_version();

  int get_subminor_version();

  CubitString get_engine_version_string();
  
//HEADER- RTTI and safe casting functions.
  
  virtual const type_info& entity_type_info() const ;
    //R- The geometric modeler type
    //- This function returns the type of the geometric modeler.
  
  virtual CubitBoolean is_solid_modeler_type() const 
    {return CUBIT_FALSE;}
    //R CubitBoolean
    //R- This  is not a solid modeling engine.
//HEADER- Functions for importing and exporting solid models.

  virtual CubitStatus get_graphics( Surface* surface_ptr,
                                          int& number_triangles,
                                          int& number_points,
                                          int& number_facets,
                                          GMem* gMem,
                                          unsigned short normal_tolerance = 15,
                                          double distance_tolerance = 0,
                                          double longest_edge = 0) const;
  virtual CubitStatus get_graphics( Curve* curve_ptr,
                                    int& num_points,
                                    GMem* gMem = NULL,
                                    double tolerance = 0.0 ) const;
    //R CubitStatus
    //R- CUBIT_SUCCESS/CUBIT_FAILURE
    //I ref_edge_ptr
    //I- The RefEdge for which hoops facetting information will be
    //I- gathered.
    //O numSteps
    //O- The number of points in resulting polyline.
    //O gMem
    //O- The storage place for edges, involved in facetting.
    //I tolerance
    //I- The tolerance deviation used when facetting the curve (optional
    //I- and currently IGNORED by this engine).
    //- This function gathers and outputs ACIS edge information for
    //- hoops involved in facetting a RefEdge.  If all goes well,
    //- CUBIT_SUCCESS is returned.  Otherwise, CUBIT_FAILURE is
    //- returned.
 
  virtual CubitStatus get_isoparametric_points(Surface* ,
                                               int&, int&,
                                               GMem*&) const;
  
  virtual CubitStatus get_u_isoparametric_points(Surface* ref_face_ptr,
                                                 double v, int& n,
                                                 GMem *&gMem) const;
  
  virtual CubitStatus get_v_isoparametric_points(Surface* ref_face_ptr,
                                                 double u, int&n,
                                                 GMem *&gMem) const;
  
  virtual CubitStatus transform_vec_position( 
    CubitVector const& ,
    BodySM *,
    CubitVector & ) const;
  
  virtual CubitStatus get_intersections( Curve*, CubitVector& point1,
                                         CubitVector&,
                                         DLIList<CubitVector*>& ,
                                         CubitBoolean,
                                         CubitBoolean );

  virtual CubitStatus get_intersections( Curve* , Curve* ,
                                         DLIList<CubitVector*>& ,
                                         CubitBoolean,
                                         CubitBoolean );
  virtual CubitStatus get_intersections( Curve* ref_edge, Surface* ref_face,
                                        DLIList<CubitVector*>& intersection_list,
                                        CubitBoolean bounded = CUBIT_FALSE );

  virtual CubitStatus entity_extrema( DLIList<GeometryEntity*> &ref_entity_list, 
                                      const CubitVector *dir1, 
                                      const CubitVector *dir2,
                                      const CubitVector *dir3, 
                                      CubitVector &extrema,
                                      GeometryEntity *&extrema_entity_ptr );
  //- Gets the extrema position along the first given direction. If there 
  //- is more than one extrema position, the other directions will be used 
  //- to determine a unique position.  Directions 2 and 3 can be NULL.
  //- Entities supported include bodies, volumes, surfaces, curves and
  //- vertices.  The entity the extrema is found on is also returned.

  virtual CubitStatus entity_entity_distance( GeometryEntity *ref_entity_ptr1,
                                              GeometryEntity *ref_entity_ptr2,
                                              CubitVector &pos1, CubitVector &pos2,
                                              double &distance );
  //- Gets the minimum distance between two entities and the closest positions 
  //- on those entities. Supports vertices, curves, surfaces, volumes and bodies.

  virtual CubitStatus export_solid_model( DLIList<TopologyBridge*>& bridge_list,
                                          const char* file_name,
                                          const char* file_type,
                                          const CubitString &cubit_version,
                                          const char* logfile_name = NULL );

  virtual CubitStatus save_temp_geom_file( DLIList<TopologyBridge*>& ref_entity_list,
                                          const char *file_name,
                                          const CubitString &cubit_version,
                                          CubitString &created_file,
                                          CubitString &created_file_type ); 

 virtual CubitStatus import_temp_geom_file(FILE* file_ptr,
                                      const char* file_name,
                                      const char* file_type,
                                      DLIList<TopologyBridge*> &bridge_list );

 virtual CubitStatus import_solid_model(const char* file_name,
                                         const char* file_type,
                                         DLIList<TopologyBridge*>& imported_entities,
                                         CubitBoolean print_results = CUBIT_TRUE,
                                         const char* logfile_name = NULL,
                                         CubitBoolean heal_step = CUBIT_TRUE,
                                         CubitBoolean import_bodies = CUBIT_TRUE,
                                         CubitBoolean import_surfaces = CUBIT_TRUE,
                                         CubitBoolean import_curves = CUBIT_TRUE,
                                         CubitBoolean import_vertices = CUBIT_TRUE,
                                         CubitBoolean free_surfaces = CUBIT_TRUE );
private:
  CubitStatus import_solid_model(FILE *file_ptr,
                                 const char* /*file_type*/,
                                 DLIList<TopologyBridge*> &imported_entities,
                                 CubitBoolean print_results = CUBIT_TRUE,
                                 const char* logfile_name = NULL,
                                 CubitBoolean heal_step = CUBIT_TRUE,
                                 CubitBoolean import_bodies = CUBIT_TRUE,
                                 CubitBoolean import_surfaces = CUBIT_TRUE,
                                 CubitBoolean import_curves = CUBIT_TRUE,
                                 CubitBoolean import_vertices = CUBIT_TRUE,
                                 CubitBoolean free_surfaces = CUBIT_TRUE);
public:
  virtual void delete_solid_model_entities(DLIList<BodySM*>& body_list) const;
    //- Deletes all solid model entities associated with the Bodies in 
    //- the input list. 
      
  virtual CubitStatus delete_solid_model_entities( BodySM* body_ptr ) const;
  virtual CubitStatus delete_solid_model_entities(Surface* surf_ptr ) const;
  virtual CubitStatus delete_solid_model_entities( Curve* curve_ptr ) const;
  virtual CubitStatus delete_solid_model_entities( Point* point_ptr ) const;

  virtual CubitStatus fire_ray(BodySM *body,
                               const CubitVector &ray_point,
                               const CubitVector &unit,
                               DLIList<double>& ray_params,
                               DLIList<GeometryEntity*> *entity_list = NULL) const;
    //- fire a ray at the specified body, returning the entities hit and
    //- the parameters along the ray; return CUBIT_FAILURE if error

  virtual double get_sme_resabs_tolerance() const; // Gets solid modeler's resolution absolute tolerance
  virtual double set_sme_resabs_tolerance( double new_resabs );

  virtual CubitStatus set_int_option( const char* opt_name, int val );
  virtual CubitStatus set_dbl_option( const char* opt_name, double val );
  virtual CubitStatus set_str_option( const char* opt_name, const char* val );
    //- Set solid modeler options

  CubitStatus ensure_is_ascii_stl_file(FILE * fp, CubitBoolean &is_ascii);
  //- returns true in is_ascii if fp points to an ascii stl file

  CubitStatus create_super_bounding_box(
                                DLIList<BodySM*>& body_list,
                                CubitBox& super_box );

  CubitStatus restore_transform( BodySM* body );

  CubitStatus translate( BodySM* body, const CubitVector& offset );
  CubitStatus rotate   ( BodySM* body, const CubitVector& axis, double angle );
  CubitStatus scale    ( BodySM* body, double factor );
  CubitStatus scale    ( BodySM* body, const CubitVector& factors );
  CubitStatus reflect  ( BodySM* body, const CubitVector& axis );

  CubitStatus translate( GeometryEntity* ent, const CubitVector& offset );
  CubitStatus rotate   ( GeometryEntity* ent, const CubitVector& axis, double degrees );
  CubitStatus scale    ( GeometryEntity* ent, double factor );
  CubitStatus scale    ( GeometryEntity* ent, const CubitVector& factors );
  CubitStatus reflect  ( GeometryEntity* ent, const CubitVector& axis );

  CubitStatus get_connected_patch( DLIList<OCCSurface*>& remaining_surfs,
                                   DLIList<OCCSurface*>& output_patch );
  virtual CubitBoolean bodies_overlap (BodySM *body_ptr_1,
                                       BodySM *body_ptr_2 ) const;
  //R CubitBoolean
  //R- CUBIT_TRUE if the two bodies overlap, CUBIT_FALSE if they don't
  //R- overlap.  If the bodies are touching the function
  //R- should return CUBIT_FALSE.
  //I body_ptr_1, body_ptr_2
  //I- The two body pointers that are being tested for overlap.
  //-  The function uses the intersect call to test if the bodies
  //-  are overlaping.  The full intersect Boolean is needed to see if
  //-  the bodies actually overlap and don't just touch.

  static TopologyBridge* occ_to_cgm(TopoDS_Shape shape);
  
  virtual CubitBoolean volumes_overlap (Lump *lump1, Lump *lump2 ) const ;

  DLIList<TopologyBridge*> populate_topology_bridge(TopoDS_Shape aShape);
  BodySM* populate_topology_bridge(TopoDS_Solid aShape);
  Surface* populate_topology_bridge(TopoDS_Face aShape);
  Curve* populate_topology_bridge(TopoDS_Edge aShape);
  Point* populate_topology_bridge(TopoDS_Vertex aShape);

protected:
  
  OCCQueryEngine();
  
private:

  OCCLoop* populate_topology_bridge(TopoDS_Wire aShape);  
  OCCShell* populate_topology_bridge(TopoDS_Shell aShape);

  CubitStatus write_topology( const char* file_name, 
                              DLIList<OCCBody*> &facet_bodies,
                              DLIList<OCCSurface*> &facet_surfaces,
                              DLIList<OCCCurve*> &facet_curves,
                              DLIList<OCCPoint*> &facet_points );


  static TopTools_DataMapOfShapeInteger* OCCMap;
  static std::map<int, TopologyBridge*>* OccToCGM;
  static TopTools_DataMapOfShapeInteger* OCCMapr;
  static DLIList<TopologyBridge*>* CGMList;
  static int iTotalTBCreated ;
  static OCCQueryEngine* instance_;
  static CubitBoolean PRINT_RESULTS;
    //- static pointer to unique instance of this class

  static const int OCCQE_MAJOR_VERSION;
  static const int OCCQE_MINOR_VERSION;
  static const int OCCQE_SUBMINOR_VERSION;

};

// ********** BEGIN INLINE FUNCTIONS          **********
// ********** END INLINE FUNCTIONS            **********

// ********** BEGIN FRIEND FUNCTIONS          **********
// ********** END FRIEND FUNCTIONS            **********

// ********** BEGIN EXTERN FUNCTIONS          **********
// ********** END EXTERN FUNCTIONS            **********

// ********** BEGIN HELPER CLASS DECLARATIONS **********
// ********** END HELPER CLASS DECLARATIONS   **********

#endif