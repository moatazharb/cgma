/**
 * Copyright 2006 Sandia Corporation.  Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Coroporation, the U.S. Government
 * retains certain rights in this software.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 */
/**
 * \file testgeom.cpp
 *
 * \brief testgeom, a unit test for the TSTT geometry interface
 *
 */
#include "iGeom.h"
#include <iostream>
#include <set>
#include <vector>
#include <iterator>

#define CHECK( STR ) if (err != iBase_SUCCESS) return print_error( STR, err, geom, __FILE__, __LINE__ )

static bool print_error( const char* desc, 
                         int err,
                         iGeom_Instance geom,
                         const char* file,
                         int line )
{
  char buffer[1024];
  int err2 = err;
  iGeom_getDescription( geom, buffer, &err2, sizeof(buffer) );
  buffer[sizeof(buffer)-1] = '\0';
  
  std::cerr << "ERROR: " << desc << std::endl
            << "  Error code: " << err << std::endl
            << "  Error desc: " << buffer << std::endl
            << "  At        : " << file << ':' << line << std::endl
            ;
  
  return false; // must always return false or CHECK macro will break
}

typedef iBase_TagHandle TagHandle;
typedef iBase_EntityHandle GentityHandle;
typedef iBase_EntitySetHandle GentitysetHandle;

/* Frees allocated arrays for us */
template <typename T> class SimpleArray
{
  private:
    T* arr;
    int arrSize;
    int arrAllocated;
     
  public:
    SimpleArray()             : arr(0)         , arrSize(0), arrAllocated(0) {}
    SimpleArray( unsigned s ) : arr( new T[s] ), arrSize(s), arrAllocated(s) {}
    ~SimpleArray() { delete [] arr; }

    T**  ptr()            { return &arr; }
    int& size()           { return arrSize; }
    int  size()     const { return arrSize; }
    int& capacity()       { return arrAllocated; }
    int  capacity() const { return arrAllocated; }
    
    typedef T* iterator;
    typedef const T* const_iterator;
    iterator       begin()       { return arr; }
    const_iterator begin() const { return arr; }
    iterator         end()       { return arr + arrSize; }
    const_iterator   end() const { return arr + arrSize; }
    
    
    T& operator[]( unsigned idx )       { return arr[idx]; }
    T  operator[]( unsigned idx ) const { return arr[idx]; }
};

#define ARRAY_INOUT( A ) A.ptr(), &A.capacity(), &A.size()
#define ARRAY_IN( A ) &A[0], A.size()

bool gLoad_test(const std::string filename, iGeom_Instance);

bool tags_test(iGeom_Instance geom);
bool tag_get_set_test(iGeom_Instance geom);
bool tag_info_test(iGeom_Instance geom);
bool gentityset_test(iGeom_Instance geom, bool /*multiset*/, bool /*ordered*/);
bool topology_adjacencies_test(iGeom_Instance geom);
bool construct_test(iGeom_Instance geom);
bool primitives_test(iGeom_Instance geom);
bool transforms_test(iGeom_Instance geom);
bool booleans_test(iGeom_Instance geom);

void handle_error_code(const bool result,
                       int &number_failed,
                       int &/*number_not_implemented*/,
                       int &number_successful)
{
  if (result) {
    std::cout << "Success";
    number_successful++;
  }
  else {
    std::cout << "Failure";    
    number_failed++;
  }
}

int main( int argc, char *argv[] )
{
    // Check command line arg
  std::string filename = "testgeom.sat";
  
  if (argc == 2) 
    filename = argv[1];
  else if (argc != 1) {
    std::cerr << "Usage: " << argv[0] << " [geom_filename]" << std::endl;
    return 1;
  }

  bool result;
  int number_tests = 0;
  int number_tests_successful = 0;
  int number_tests_not_implemented = 0;
  int number_tests_failed = 0;

    // initialize the Mesh
  int err;
  iGeom_Instance geom;
  iGeom_newGeom( 0, &geom, &err, 0 );

    // Print out Header information
  std::cout << "\n\nITAPS GEOMETRY INTERFACE TEST PROGRAM:\n\n";

    // gLoad test
  std::cout << "   gLoad: ";
  result = gLoad_test(filename, geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // tags test
  std::cout << "   tags: ";
  result = tags_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // gentitysets test
  std::cout << "   gentity sets: ";
  result = gentityset_test(geom, false, false);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // gentitysets test
  std::cout << "   topology adjacencies: ";
  result = topology_adjacencies_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // construct test
  std::cout << "   construct: ";
  result = construct_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // primitives test
  std::cout << "   primitives: ";
  result = primitives_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // transforms test
  std::cout << "   transforms: ";
  result = transforms_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // booleans test
  std::cout << "   booleans: ";
  result = booleans_test(geom);
  handle_error_code(result, number_tests_failed,
                    number_tests_not_implemented,
                    number_tests_successful);
  number_tests++;
  std::cout << "\n";

    // summary

  std::cout << "\nTSTT TEST SUMMARY: \n"
            << "   Number Tests:           " << number_tests << "\n"
            << "   Number Successful:      " << number_tests_successful << "\n"
            << "   Number Not Implemented: " << number_tests_not_implemented << "\n"
            << "   Number Failed:          " << number_tests_failed 
            << "\n\n" << std::endl;
  
  return 0;
}

/*!
  @test 
  Load Mesh
  @li Load a mesh file
*/
bool gLoad_test(const std::string filename, iGeom_Instance geom)
{
  int err;
  iGeom_load( geom, &filename[0], 0, &err, filename.length(), 0 );
  CHECK( "ERROR : can not load a geometry" );
  
  iBase_EntityHandle root_set;
  iGeom_getRootSet( geom, &root_set, &err );
  CHECK( "ERROR : getRootSet failed!" );
  
    // print out the number of entities
  std::cout << "Model contents: " << std::endl;
  const char *gtype[] = {"vertices: ", "edges: ", "faces: ", "regions: "};
  for (int i = 0; i <= 3; ++i) {
    int count;
    iGeom_getNumOfType( geom, root_set, i, &count, &err );
    CHECK( "Error: problem getting entities after gLoad." );
    std::cout << gtype[i] << count << std::endl;
  }

  return true;
}

/*!
  @test 
  Test tag creating, reading, writing, deleting
  @li Load a mesh file
*/
bool tags_test(iGeom_Instance geom)
{
  bool success = true;

  success = tag_info_test(geom);
  if (!success) return success;
  
  success = tag_get_set_test(geom);
  if (!success) return success;

  return true;
}

bool tag_info_test(iGeom_Instance geom) 
{
  int err;
  
  iBase_EntityHandle root_set;
  iGeom_getRootSet( geom, &root_set, &err );
  CHECK( "ERROR : getRootSet failed!" );

    // create an arbitrary tag, size 4
  iBase_TagHandle this_tag, tmp_handle;
  std::string tag_name("tag_info tag"), tmp_name;
  iGeom_createTag( geom, &tag_name[0], 4, iBase_BYTES, &this_tag, &err, tag_name.length() );
  CHECK( "ERROR : can not create a tag." );

    // get information on the tag
  
  char name_buffer[256];
  iGeom_getTagName( geom, this_tag, name_buffer, &err, sizeof(name_buffer) );
  CHECK( "ERROR : Couldn't get tag name." );
  if (tag_name != name_buffer) {
    std::cerr << "ERROR: getTagName returned '" << name_buffer 
              << "' for tag created as '" << tag_name << "'" << std::endl;
    return false;
  }
  
  
  iGeom_getTagHandle( geom, &tag_name[0], &tmp_handle, &err, tag_name.length() );
  CHECK( "ERROR : Couldn't get tag handle." );
  if (tmp_handle != this_tag) {
    std::cerr << "ERROR: getTagHandle didn't return consistent result." << std::endl;
    return false;
  }  

  int tag_size;
  iGeom_getTagSizeBytes( geom, this_tag, &tag_size, &err );
  CHECK( "ERROR : Couldn't get tag size." );
  if (tag_size != 4) {
    std::cerr << "ERROR: getTagSizeBytes: expected 4, got " << tag_size << std::endl;
    return false;
  }

  iGeom_getTagSizeValues( geom, this_tag, &tag_size, &err );
  CHECK( "ERROR : Couldn't get tag size." );
  if (tag_size != 4) {
    std::cerr << "ERROR: getTagSizeValues: expected 4, got " << tag_size << std::endl;
    return false;
  }

  int tag_type;
  iGeom_getTagType( geom, this_tag, &tag_type, &err );
  CHECK( "ERROR : Couldn't get tag type." );
  if (tag_type != iBase_BYTES) {
    std::cerr << "ERROR: getTagType: expected " << iBase_BYTES 
              << ", got " << tag_type << std::endl;
    return false;
  }
  
  iGeom_destroyTag( geom, this_tag, true, &err );
  CHECK( "ERROR : Couldn't delete a tag." );

    // print information about all the tags in the model

  std::set<iBase_TagHandle> tags;
  SimpleArray<iBase_EntityHandle> entities;
  iGeom_getEntities( geom, root_set, iBase_ALL_TYPES, 
                     ARRAY_INOUT(entities),  &err );
  CHECK( "getEntities( ..., iBase_ALL_TYPES, ... ) failed." );
  for (int i = 0; i < entities.size(); ++i) {
    SimpleArray<iBase_TagHandle> tag_arr;
    iGeom_getAllTags( geom, entities[i], ARRAY_INOUT(tag_arr), &err);
    CHECK( "getAllTags failed." );
    std::copy( tag_arr.begin(), tag_arr.end(), std::inserter( tags, tags.begin() ) );
  }
  
  std::cout << "Tags defined on model: ";
  bool first = true;
  for (std::set<iBase_TagHandle>::iterator sit = tags.begin(); sit != tags.end(); ++sit) {
    iGeom_getTagName( geom, *sit, name_buffer, &err, sizeof(name_buffer) );
    name_buffer[sizeof(name_buffer)-1] = '\0'; // mnake sure of NUL termination
    CHECK( "getTagName failed." );
    
    if (!first) std::cout << ", ";
    std::cout << name_buffer;
    first = false;
  }
  if (first) std::cout << "<none>";
  std::cout << std::endl;

  return true;
}

bool tag_get_set_test(iGeom_Instance geom) 
{
  int err;
  
    // create an arbitrary tag, size 4
  iBase_TagHandle this_tag;
  std::string tag_name("tag_get_set tag");
  iGeom_createTag( geom, &tag_name[0], sizeof(int), iBase_BYTES, &this_tag, &err, tag_name.length() );
  CHECK( "ERROR : can not create a tag for get_set test." );
  
  iBase_EntityHandle root_set;
  iGeom_getRootSet( geom, &root_set, &err );
  CHECK( "ERROR : getRootSet failed!" );
  
    // set this tag to an integer on each entity; keep track of total sum
  int sum = 0, num = 0, dim;
  for (dim = 0; dim <= 3; dim++) {
    SimpleArray<iBase_EntityHandle> gentity_handles;
    iGeom_getEntities( geom, root_set, dim, ARRAY_INOUT( gentity_handles ), &err );
    int num_ents = gentity_handles.size();
    std::vector<int> tag_vals( num_ents );
    for (int i = 0; i < num_ents; ++i) {
      tag_vals[i] = num;
      sum += num;
      ++num;
    }
    
    iGeom_setArrData( geom, ARRAY_IN( gentity_handles ),
                      this_tag, 
                      (char*)&tag_vals[0], tag_vals.size()*sizeof(int),
                      &err );
    CHECK( "ERROR : can't set tag on entities" );
  }
  
    // check tag values for entities now
  int get_sum = 0;
  for (dim = 0; dim <= 3; dim++) {
    SimpleArray<iBase_EntityHandle> gentity_handles;
    iGeom_getEntities( geom, root_set, dim, ARRAY_INOUT( gentity_handles ), &err );
    int num_ents = gentity_handles.size();
    
    SimpleArray<char> tag_vals;
    iGeom_getArrData( geom, ARRAY_IN( gentity_handles ), this_tag, 
                      ARRAY_INOUT( tag_vals ), &err );
    CHECK( "ERROR : can't get tag on entities" );
    
    int* tag_ptr = (int*)(&tag_vals[0]);
    for (int i = 0; i < num_ents; ++i)
      get_sum += tag_ptr[i];
  }
  
  if (get_sum != sum) {
    std::cerr << "ERROR: getData didn't return consistent results." << std::endl;
    return false;
  }
  
  iGeom_destroyTag( geom, this_tag, true, &err );
  CHECK( "ERROR : couldn't delete tag." );

  return true;
}

/*!
  @test
  TSTT gentity sets test (just implemented parts for now)
  @li Check gentity sets
*/
bool gentityset_test(iGeom_Instance geom, bool /*multiset*/, bool /*ordered*/)
{
  int num_type = 4;
  int num_all_gentities_super = 0;
  iBase_EntitySetHandle ges_array[num_type];
  int number_array[num_type];
  int ent_type = iBase_VERTEX;

  int err;
  iBase_EntityHandle root_set;
  iGeom_getRootSet( geom, &root_set, &err );
  CHECK( "ERROR : getRootSet failed!" );
  
    // get the number of sets in the whole model
  int all_sets = 0;
  iGeom_getNumEntSets( geom, root_set, 0, &all_sets, &err );
  CHECK( "Problem getting the number of all gentity sets in whole model." );

    // add gentities to entitysets by type
  for (; ent_type < num_type; ent_type++) {
      // initialize the entityset
    iGeom_createEntSet( geom, true, &ges_array[ent_type], &err );
    CHECK( "Problem creating entityset." );

      // get entities by type in total "mesh"
    SimpleArray<iBase_EntityHandle> gentities;
    iGeom_getEntities( geom, root_set, ent_type, ARRAY_INOUT(gentities), &err );
    CHECK( "Failed to get gentities by type in gentityset_test." );
    
      // add gentities into gentity set
    iGeom_addEntArrToSet( geom, ARRAY_IN( gentities ), &ges_array[ent_type], &err );
    CHECK( "Failed to add gentities in entityset_test." );
    
      // Check to make sure entity set really has correct number of entities in it
    iGeom_getNumOfType( geom, ges_array[ent_type], ent_type, &number_array[ent_type], &err );
    CHECK( "Failed to get number of gentities by type in entityset_test." );

      // compare the number of entities by type
    int num_type_gentity = gentities.size();

    if (number_array[ent_type] != num_type_gentity)
    {
      std::cerr << "Number of gentities by type is not correct"
                << std::endl;
      return false;
    }

      // add to number of all entities in super set
    num_all_gentities_super += num_type_gentity;
  }

    // make a super set having all entitysets
  iBase_EntitySetHandle super_set;
  iGeom_createEntSet( geom, true, &super_set, &err );
  CHECK( "Failed to create a super set in gentityset_test." );

  for (int i = 0; i < num_type; i++) {
    iGeom_addEntSet( geom, ges_array[i], &super_set, &err );
    CHECK( "Failed to create a super set in gentityset_test." );
  }

    //----------TEST BOOLEAN OPERATIONS----------------//

  iBase_EntitySetHandle temp_ges1;
  iGeom_createEntSet( geom, true, &temp_ges1, &err );
  CHECK( "Failed to create a super set in gentityset_test." );

    // Subtract
    // add all EDGEs and FACEs to temp_es1
    // get all EDGE entities
  SimpleArray<iBase_EntityHandle> gedges, gfaces, temp_gentities1;
  iGeom_getEntities( geom, ges_array[iBase_EDGE], iBase_EDGE, ARRAY_INOUT(gedges), &err );
  CHECK( "Failed to get gedge gentities in gentityset_test." );

    // add EDGEs to ges1
  iGeom_addEntArrToSet( geom, ARRAY_IN(gedges), &temp_ges1, &err );
  CHECK( "Failed to add gedge gentities in gentityset_test." );

    // get all FACE gentities
  iGeom_getEntities( geom, ges_array[iBase_FACE], iBase_FACE, ARRAY_INOUT(gfaces), &err );
  CHECK( "Failed to get gface gentities in gentityset_test." );

    // add FACEs to es1
  iGeom_addEntArrToSet( geom, ARRAY_IN(gfaces), &temp_ges1, &err );
  CHECK( "Failed to add gface gentities in gentityset_test." );

    // subtract EDGEs
  iGeom_subtract( geom, temp_ges1, ges_array[iBase_EDGE], &temp_ges1, &err );
  CHECK( "Failed to subtract gentitysets in gentityset_test." );
  
  iGeom_getEntities( geom, temp_ges1, iBase_FACE, ARRAY_INOUT(temp_gentities1), &err );
  CHECK( "Failed to get gface gentities in gentityset_test." );

  if (gfaces.size() != temp_gentities1.size()) {
    std::cerr << "Number of entitysets after subtraction not correct \
             in gentityset_test." << std::endl;
    return false;
  }

    // check there's nothing but gfaces in temp_ges1
  int num_gents;
  iGeom_getNumOfType( geom, temp_ges1, iBase_EDGE, &num_gents, &err );
  CHECK( "Failed to get dimensions of gentities in gentityset_test." );
  if (0 != num_gents) {
    std::cerr << "Subtraction failed to remove all edges" << std::endl;
    return false;
  }

    //------------Intersect------------
    //

    // clean out the temp_ges1
  iGeom_rmvEntArrFromSet( geom, ARRAY_IN(gfaces), &temp_ges1, &err );
  CHECK( "Failed to remove gface gentities in gentityset_test." );

    // check if it is really cleaned out
  iGeom_getNumOfType( geom, temp_ges1, iBase_FACE, &num_gents, &err );
  CHECK( "Failed to get number of gentities by type in gentityset_test." );

  if (num_gents != 0) {
    std::cerr << "failed to remove correctly." << std::endl;
    return false;
  }
  
    // add EDGEs to temp ges1
  iGeom_addEntArrToSet( geom, ARRAY_IN(gedges), &temp_ges1, &err );
  CHECK( "Failed to add gedge gentities in gentityset_test." );

    // add FACEs to temp ges1
  iGeom_addEntArrToSet( geom, ARRAY_IN(gfaces), &temp_ges1, &err );
  CHECK( "Failed to add gface gentities in gentityset_test." );

    // intersect temp_ges1 with gedges set 
    // temp_ges1 entityset is altered
  iGeom_intersect( geom, temp_ges1, ges_array[iBase_EDGE], &temp_ges1, &err );
  CHECK( "Failed to intersect in gentityset_test." );
  
    // try to get FACEs, but there should be nothing but EDGE
  iGeom_getNumOfType( geom, temp_ges1, iBase_FACE, &num_gents, &err );
  CHECK( "Failed to get gface gentities in gentityset_test." );

  if (num_gents != 0) {
    std::cerr << "wrong number of gfaces." << std::endl;
    return false;
  }


    //-------------Unite--------------

    // get all regions
  iBase_EntitySetHandle temp_ges2;
  SimpleArray<iBase_EntityHandle> gregions;

  iGeom_createEntSet( geom, true, &temp_ges2, &err );
  CHECK( "Failed to create a temp gentityset in gentityset_test." );
  
  iGeom_getEntities( geom, ges_array[iBase_REGION], iBase_REGION, ARRAY_INOUT(gregions), &err );
  CHECK( "Failed to get gregion gentities in gentityset_test." );
  
    // add REGIONs to temp es2
  iGeom_addEntArrToSet( geom, ARRAY_IN(gregions), &temp_ges2, &err );
  CHECK( "Failed to add gregion gentities in gentityset_test." );

    // unite temp_ges1 and temp_ges2
    // temp_ges1 gentityset is altered
  iGeom_unite( geom, temp_ges1, temp_ges2, &temp_ges1, &err );
  CHECK( "Failed to unite in gentityset_test." );

    // perform the check
  iGeom_getNumOfType( geom, temp_ges1, iBase_REGION, &num_gents, &err );
  CHECK( "Failed to get number of gregion gentities by type in gentityset_test." );
  
  if (num_gents != number_array[iBase_REGION]) {
    std::cerr << "different number of gregions in gentityset_test." << std::endl;
    return false;
  }


    //--------Test parent/child stuff in entiysets-----------

    // Add 2 sets as children to another
  iBase_EntitySetHandle parent_child;
  iGeom_createEntSet( geom, true, &parent_child, &err );
  CHECK( "Problem creating gentityset in gentityset_test." );

  iGeom_addPrntChld( geom, &ges_array[iBase_VERTEX], &parent_child, &err );
  CHECK( "Problem add parent in gentityset_test." );

    // check if parent is really added
  SimpleArray<iBase_EntitySetHandle> parents;
  iGeom_getPrnts( geom, parent_child, 1, ARRAY_INOUT(parents), &err );
  CHECK( "Problem getting parents in gentityset_test." );

  if (parents.size() != 1) {
    std::cerr << "number of parents is not correct in gentityset_test."
              << std::endl;
    return false;
  }

    // add parent and child
  //sidl::array<void*> parent_child_array = sidl::array<void*>::create1d(1);
  //int num_parent_child_array;
  //sidl::array<void*> temp_gedge_array = sidl::array<void*>::create1d(1);
  //int num_temp_gedge_array;
  //parent_child_array.set(0, parent_child);
  //temp_gedge_array.set(0, ges_array[TSTTG::EntityType_EDGE]);
  iGeom_addPrntChld( geom, &ges_array[iBase_EDGE], &parent_child, &err );
  CHECK( "Problem adding parent and child in gentityset_test." );

  //sidl::array<void*> temp_gface_array = sidl::array<void*>::create1d(1);
  //int num_temp_gface_array;
  //temp_gface_array.set(0, ges_array[TSTTG::EntityType_FACE]);
  iGeom_addPrntChld( geom, &parent_child, &ges_array[iBase_FACE], &err );
  CHECK( "Problem adding parent and child in gentityset_test." );

    // add child
  iGeom_addPrntChld( geom, &parent_child, &ges_array[iBase_REGION], &err );
  CHECK( "Problem adding child in gentityset_test." );

    // get the number of parent gentitysets
  num_gents = -1;
  iGeom_getNumPrnt( geom, parent_child, 1, &num_gents, &err );
  CHECK( "Problem getting number of parents in gentityset_test." );

  if (num_gents != 2) {
    std::cerr << "number of parents is not correct in gentityset_test."
              << std::endl;
    return false;
  }

    // get the number of child gentitysets
  num_gents = -1;
  iGeom_getNumChld( geom, parent_child, 1, &num_gents, &err );
  CHECK( "Problem getting number of children in gentityset_test." );

  if (num_gents != 2) {
    std::cerr << "number of children is not correct in gentityset_test."
              << std::endl;
    return false;
  }

  SimpleArray<iBase_EntitySetHandle> children;
  iGeom_getChldn( geom, parent_child, 1, ARRAY_INOUT(children), &err );
  CHECK( "Problem getting children in gentityset_test." );

  if (children.size() != 2) {
    std::cerr << "number of children is not correct in gentityset_test."
              << std::endl;
    return false;
  }

    // remove children
  iGeom_rmvPrntChld( geom, &parent_child, &ges_array[iBase_FACE], &err );
  CHECK( "Problem removing parent child in gentityset_test." );

    // get the number of child gentitysets
  iGeom_getNumChld( geom, parent_child, 1, &num_gents, &err );
  CHECK( "Problem getting number of children in gentityset_test." );

  if (num_gents != 1) {
    std::cerr << "number of children is not correct in gentityset_test."
              << std::endl;
    return false;
  }

    // parent_child and ges_array[TSTTG::EntityType_EDGE] should be related
  int result = 0;
  iGeom_isChildOf( geom, ges_array[iBase_EDGE], parent_child, &result, &err );
  CHECK( "Problem checking relation in gentityset_test." );
  if (!result) {
    std::cerr << "parent_child and ges_array[TSTTG::EntityType_EDGE] should be related" << std::endl;
    return false;
  }

    // ges_array[TSTTG::EntityType_FACE] and ges_array[TSTTG::REGION] are not related
  result = 2;
  iGeom_isChildOf( geom, ges_array[iBase_FACE], ges_array[iBase_REGION], &result, &err );
  if (result) {
    std::cerr << "ges_array[TSTTG::REGION] and ges_array[TSTTG::EntityType_FACE] should not be related" << std::endl;
    return false;
  }
  

    //--------test modify and query functions-----------------------------
  
    // check the number of gentity sets in whole mesh
  SimpleArray<iBase_EntitySetHandle> gentity_sets;
  iGeom_getEntSets( geom, root_set, 1, ARRAY_INOUT( gentity_sets ), &err );
  CHECK( "Problem to get all gentity sets in mesh." );
  
  if (gentity_sets.size() != all_sets + 8) {
    std::cerr << "the number of gentity sets in whole mesh should be 8 times of num_iter."
              << std::endl;
    return false;
  }

    // get all gentity sets in super set
  SimpleArray<iBase_EntitySetHandle> ges_array1;
  iGeom_getEntSets( geom, super_set, 1, ARRAY_INOUT( ges_array1 ), &err );
  CHECK( "Problem to get gentity sets in super set." );

    // get the number of gentity sets in super set
  int num_super;
  iGeom_getNumEntSets( geom, super_set, 1, &num_super, &err );
  CHECK( "Problem to get the number of all gentity sets in super set." );
  
    // the number of gentity sets in super set should be same
  if (num_super != ges_array1.size()) {
    std::cerr << "the number of gentity sets in super set should be same." << std::endl;
    return false;
  }

    // get all entities in super set
  SimpleArray<iBase_EntityHandle> all_gentities;
  iGeom_getEntSets( geom, super_set, 1, ARRAY_INOUT( all_gentities ), &err );
  CHECK( "Problem to get all gentities in super set." );
  
    // compare the number of all gentities in super set
  // HJK : num_hops is not implemented
  //if (num_all_gentities_super != ARRAY_SIZE(all_gentities)) {
  //std::cerr << "number of all gentities in super set should be same." << std::endl;
  //success = false;
  //}

    // test add, remove and get all entitiy sets using super set
    // check GetAllGentitysets works recursively and dosen't return
    // multi sets
  for (int k = 0; k < num_super; k++) {
      // add gentity sets of super set to each gentity set of super set
      // make multiple child super sets
    iBase_EntitySetHandle ges_k = ges_array1[k];

    for (int a = 0; a < ges_array1.size(); a++) {
      iGeom_addEntSet( geom, ges_array1[a], &ges_k, &err );
      CHECK( "Problem to add entity set." );
    }
    
      // add super set to each entity set
    //    sidl::array<GentitysetHandle> superset_array
    //= sidl::array<GentitysetHandle>::create1d(1);
    //superset_array.set(0, super_set);
    //int num_superset_array;
    
    iGeom_addEntSet( geom, super_set, &ges_k, &err );
    CHECK( "Problem to add super set to gentitysets." );

      // add one gentity sets multiple times
    // HJK: ??? how to deal this case?
    //sidl::array<GentitysetHandle> temp_array1
    //= sidl::array<GentitysetHandle>::create1d(1);
    //int num_temp_array1;
    //temp_array1.set(0, temp_ges1);

    //for (int l = 0; l < 3; l++) {
    iGeom_addEntSet( geom, temp_ges1, &ges_k, &err );
    CHECK( "Problem to add temp set to gentitysets." );
      //}
  }

  return true;
}
  
/*!
@test
TSTTG topology adjacencies Test
@li Check topology information
@li Check adjacency
*/
// make each topological entity vectors, check their topology
// types, get interior and exterior faces of model
bool topology_adjacencies_test(iGeom_Instance geom)
{
  int i, err;
  iBase_EntityHandle root_set;
  iGeom_getRootSet( geom, &root_set, &err );
  CHECK( "ERROR : getRootSet failed!" );

  int top = iBase_VERTEX;
  int num_test_top = iBase_ALL_TYPES;
  std::vector< std::vector<iBase_EntityHandle> > gentity_vectors(num_test_top);

  // fill the vectors of each topology entities
  // like lines vector, polygon vector, triangle vector,
  // quadrilateral, polyhedrron, tet, hex, prism, pyramid,
  // septahedron vectors
  for (i = top; i < num_test_top; i++) {
    SimpleArray<iBase_EntityHandle> gentities;
    iGeom_getEntities( geom, root_set, i, ARRAY_INOUT( gentities ), &err );
    CHECK("Failed to get gentities in adjacencies_test.");
  
    gentity_vectors[i].resize( gentities.size() );
    std::copy( gentities.begin(), gentities.end(), gentity_vectors[i].begin() );
  }

  // check number of entities for each topology
  for (i = top; i < num_test_top; i++) {
    int num_tops = 0;
    iGeom_getNumOfType( geom, root_set, i, &num_tops, &err );
    CHECK( "Failed to get number of gentities in adjacencies_test." );
    
    if (static_cast<int>(gentity_vectors[i].size()) != num_tops) {
      std::cerr << "Number of gentities doesn't agree with number returned for dimension " 
                << i << std::endl;
      return false;
    }
  }

  // check adjacencies in both directions
  std::vector<iBase_EntityHandle>::iterator vit;
  for (i = iBase_REGION; i >= iBase_VERTEX; i--) {
    for (vit = gentity_vectors[i].begin(); vit != gentity_vectors[i].end(); vit++) {
      iBase_EntityHandle this_gent = *vit;

        // check downward adjacencies
      for (int j = iBase_VERTEX; j < i; j++) {

        SimpleArray<iBase_EntityHandle> lower_ents;
        iGeom_getEntAdj( geom, this_gent, j, ARRAY_INOUT(lower_ents), &err );
        CHECK( "Bi-directional adjacencies test failed." );

          // for each of them, make sure they are adjacent to the upward ones
        int num_lower = lower_ents.size();
        for (int k = 0; k < num_lower; k++) {
          SimpleArray<iBase_EntityHandle> upper_ents;
          iGeom_getEntAdj( geom, lower_ents[k], i, ARRAY_INOUT(upper_ents), &err );
          CHECK( "Bi-directional adjacencies test failed." );
          if (std::find(upper_ents.begin(),upper_ents.end(), this_gent) ==
              upper_ents.end()) {
            std::cerr << "Didn't find lower-upper adjacency which was supposed to be there, dims = "
                 << i << ", " << j << std::endl;
            return false;
          }
        }
      }
    }
  }

  return true;
}

/*!
@test
TSTTG construct Test
@li Check construction of geometry
*/
bool construct_test(iGeom_Instance geom)
{
  int err;
  iBase_EntityHandle new_body = 0;

    // construct a cylinder, sweep it about an axis, and delete the result
  iBase_EntityHandle cyl = 0;
  iGeom_createCylinder( geom, 1.0, 1.0, 0.0, &cyl, &err );
    // Is the minor radius really supposed to be zero??? - JK
  CHECK( "Creating cylinder failed." );
  
    // move it onto the y axis
  iGeom_moveEnt( geom, &cyl, 0.0, 1.0, -0.5, &err );
  CHECK( "Problems moving surface." );

      // get the surface with max z
  iBase_EntityHandle max_surf = 0;
  SimpleArray<iBase_EntityHandle> surfs;
  iGeom_getEntAdj( geom, cyl, iBase_FACE, ARRAY_INOUT(surfs), &err );
  CHECK( "Problems getting max surf for rotation." );
  
  SimpleArray<double> max_corn, min_corn;
  int so = iBase_INTERLEAVED;
  iGeom_getArrBoundBox( geom, ARRAY_IN(surfs), &so, 
                        ARRAY_INOUT( min_corn ),
                        ARRAY_INOUT( max_corn ),
                        &err );
  CHECK( "Problems getting max surf for rotation." );
  for (int i = 0; i < surfs.size(); ++i) {
    if (max_corn[3*i+2] >= 0.0 && min_corn[3*i+2] >= 0.0) {
      max_surf = surfs[i];
      break;
    }
  }  
  
  if (0 == max_surf) {
    std::cerr << "Couldn't find max surf for rotation." << std::endl;
    return false;
  }
  
    // sweep it around the x axis
  iGeom_sweepEntAboutAxis( geom, max_surf, 360.0, 1.0, 0.0, 0.0, &new_body, &err );
  CHECK( "Problems sweeping surface about axis." );
  
    // now delete
  iGeom_deleteEnt( geom, new_body, &err );
  CHECK( "Problems deleting cylinder or swept surface body." );
  
    // if we got here, we were successful
  return true;
}

static bool compare_box( const double* expected_min,
                         const double* expected_max,
                         const double* actual_min,
                         const double* actual_max )
{
  bool same = true;
  for (int i = 0; i < 3; ++i)
    if (expected_min[i] != actual_min[i] || expected_max[i] != actual_max[i])
      same = false;
  
  return same;
}

bool primitives_test(iGeom_Instance geom) 
{
  int err;
  SimpleArray<iBase_EntityHandle> prims(3);
  iBase_EntityHandle prim;
  
  iGeom_createBrick( geom, 1.0, 2.0, 3.0, &prim, &err );
  CHECK( "createBrick failed." );
  prims[0] = prim;
  
  iGeom_createCylinder( geom, 1.0, 4.0, 2.0, &prim, &err );
  CHECK( "createCylinder failed." );
  prims[1] = prim;
  
  iGeom_createTorus( geom, 2.0, 1.0, &prim, &err );
  CHECK( "createTorus failed." );
  prims[2] = prim;
  
    // verify the bounding boxes
  SimpleArray<double> max_corn, min_corn;
  int so = iBase_INTERLEAVED;
  iGeom_getArrBoundBox( geom, ARRAY_IN(prims), &so, ARRAY_INOUT(min_corn), ARRAY_INOUT(max_corn), &err );

  double preset_min_corn[] = 
      // min brick corner xyz
    {-0.5, -1.0, -1.5, 
      // min cyl corner xyz
     -4.0, -2.0, -0.5,
      // min torus corner xyz
     -3.0, -3.0, -1.0
    };
  
  double preset_max_corn[] = 
      // max brick corner xyz
    {0.5, 1.0, 1.5, 
      // max cyl corner xyz
     4.0, 2.0, 0.5,
      // max torus corner xyz
     3.0, 3.0, 1.0
    };
  
  if (!compare_box( preset_min_corn, preset_max_corn,
                    &min_corn[0], &max_corn[0] )) {
    std::cerr << "Box check failed for brick" << std::endl;
    return false;
  }
  
  if (!compare_box( preset_min_corn+3, preset_max_corn+3,
                    &min_corn[3], &max_corn[3] )) {
    std::cerr << "Box check failed for cylinder" << std::endl;
    return false;
  }
  
  if (!compare_box( preset_min_corn+6, preset_max_corn+6,
                    &min_corn[6], &max_corn[6] )) {
    std::cerr << "Box check failed for torus" << std::endl;
    return false;
  }
  
    // must have worked; delete the entities then return
  for (int i = 0; i < 3; ++i) {
    iGeom_deleteEnt( geom, prims[i], &err );
    CHECK( "Problems deleting primitive after boolean check." );
  }
  
  return true;
}
  
bool transforms_test(iGeom_Instance geom) 
{
  int err;
  
    // construct a brick
  iBase_EntityHandle brick = 0;
  iGeom_createBrick( geom, 1.0, 2.0, 3.0, &brick, &err );
  CHECK( "Problems creating brick for transforms test." );
  
    // move it, then test bounding box
  iGeom_moveEnt( geom, &brick, 0.5, 1.0, 1.5, &err );
  CHECK( "Problems moving brick for transforms test." );
  
  double bb_min[3], bb_max[3];
  iGeom_getEntBoundBox( geom, brick, bb_min, bb_min+1, bb_min+2, bb_max, bb_max+1, bb_max+2, &err );
  CHECK( "Problems getting bounding box after move." );

  if (bb_min[0] != 0.0 || bb_min[1] != 0.0 || bb_min[2] != 0.0 ||
      bb_max[0] != 1.0 || bb_max[1] != 2.0 || bb_max[2] != 3.0) {
    std::cerr << "Wrong bounding box after move." << std::endl;
    return false;
  }
  
    // now rotate it about +x, then test bounding box
  iGeom_rotateEnt( geom, &brick, 90, 1.0, 0.0, 0.0, &err );
  CHECK( "Problems rotating brick for transforms test." );
  
  iGeom_getEntBoundBox( geom, brick, bb_min, bb_min+1, bb_min+2, bb_max, bb_max+1, bb_max+2, &err );
  CHECK( "Problems getting bounding box after rotate." );

  if (bb_min[0] != 0.0 || bb_min[1] != -3.0 || bb_min[2] != 0.0 ||
      bb_max[0] != 1.0 || bb_max[1] < -1.0e-6 || 
      bb_max[1] > 1.0e-6 || bb_max[2] != 2.0) {
    std::cerr << "Wrong bounding box after rotate." << std::endl;
    return false;
  }
  
    // now reflect through y plane; should recover original bb
  iGeom_reflectEnt( geom, &brick, 0.0, 1.0, 0.0, &err );
  CHECK( "Problems reflecting brick for transforms test." );
  
  iGeom_getEntBoundBox( geom, brick, bb_min, bb_min+1, bb_min+2, bb_max, bb_max+1, bb_max+2, &err );
  CHECK( "Problems getting bounding box after reflect." );
  
  if (bb_min[0] != 0.0 || bb_min[1] < -1.0e-6 
      || bb_min[1] > 1.0e-6 ||
      bb_min[2] < -1.0e-6 || bb_min[2] > 1.0e-6 ||
      bb_max[0] != 1.0 || bb_max[1] != 3.0 || bb_max[2] != 2.0) {
    std::cerr << "Wrong bounding box after reflect." << std::endl;
    return false;
  }

    // must have worked; delete the entities then return
  iGeom_deleteEnt( geom, brick, &err );
  CHECK( "Problems deleting brick after transforms check." );
  return true;
}


bool booleans_test(iGeom_Instance geom) 
{
  int err;

    // construct a brick size 1, and a cylinder rad 0.25 height 2
  iBase_EntityHandle brick = 0, cyl = 0;
  iGeom_createBrick( geom, 1.0, 0.0, 0.0, &brick, &err );
  CHECK( "Problems creating brick for booleans test." );
  iGeom_createCylinder( geom, 1.0, 0.25, 0.0, &cyl, &err );
  CHECK( "Problems creating cylinder for booleans test." );

    // subtract the cylinder from the brick
  iBase_EntityHandle subtract_result = 0;
  iGeom_subtractEnts( geom, brick, cyl, &subtract_result, &err );
  CHECK( "Problems subtracting for booleans subtract test." );

    // section the brick
  iBase_EntityHandle section_result = 0;
  iGeom_sectionEnt( geom, &subtract_result, 1.0, 0.0, 0.0, 0.25, true, &section_result, &err );
  CHECK( "Problems sectioning for booleans section test." );

    // unite the section result with a new cylinder
  iGeom_createCylinder( geom, 1.0, 0.25, 0.0, &cyl, &err );
  CHECK( "Problems creating cylinder for unite test." );
  iBase_EntityHandle unite_results;
  iBase_EntityHandle unite_input[] = { section_result, cyl };
  iGeom_uniteEnts( geom, unite_input, 2, &unite_results, &err );
  CHECK( "Problems uniting for booleans unite test." );
  
  iGeom_deleteEnt( geom, unite_results, &err );
  CHECK( "Problems deleting for booleans unite test." );
  return true;
}