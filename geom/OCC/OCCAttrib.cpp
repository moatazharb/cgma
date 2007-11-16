#include "OCCAttrib.hpp"
#include "CubitString.hpp"
#include "CubitSimpleAttrib.hpp"
#include "CubitFileIOWrapper.hpp"

  // Constructor - copy from CubitSimpleAttrib
OCCAttrib::OCCAttrib( CubitSimpleAttrib* csa ) : listNext(0)
{
  int i;
  
    // reset all lists (if we didn't need to do this, 
    // csa could be const)
  csa->string_data_list()->reset();
  csa->int_data_list()->reset();
  csa->double_data_list()->reset();
  
    // save counts
  numStrings = csa->string_data_list()->size();
  numDoubles = csa->double_data_list()->size();
  numIntegers = csa->int_data_list()->size();
  
    // allocate arrays, but don't try to allocate zero-length arrays
  stringArray = numStrings ? new CubitString[numStrings] : NULL;
  doubleArray = numDoubles ? new double[numDoubles] : NULL;
  integerArray = numIntegers ? new int[numIntegers] : NULL;
  
    // copy data into arrays
  for( i = 0; i < numStrings; i++ )
    stringArray[i] = *csa->string_data_list()->next(i);
  for( i = 0; i < numIntegers; i++ )
    integerArray[i] = *csa->int_data_list()->next(i);
  for( i = 0; i < numDoubles; i++ )
    doubleArray[i] = *csa->double_data_list()->next(i);
}

  // Private constructor for use by restore(FILE*)
OCCAttrib::OCCAttrib( int string_count, CubitString strings[],
                          int double_count, double doubles[],
                          int int_count, int integers[] )
: stringArray(strings), doubleArray(doubles), integerArray(integers),
  numStrings(string_count),  numDoubles(double_count), numIntegers(int_count),
  listNext(0)
{}


  // Destructor -- free arrays
OCCAttrib::~OCCAttrib()
{
    // "delete"ing NULL pointers is okay.
  delete [] integerArray;
  delete [] doubleArray;
  delete [] stringArray;
}

  // Copy this into a new CubitSimpleAttrib
CubitSimpleAttrib* OCCAttrib::get_CSA() const
{
    // Set initial list size
  DLIList<CubitString*> string_list(numStrings);
  DLIList<int> int_list(numIntegers);
  DLIList<double> double_list(numDoubles);
  
    // Don't need to 'new' objects in DLIList because
    // CSA will make copies.  Just put addresses in list.
  int i;
  for( i = 0; i < numStrings; i++ )
    string_list.append( &stringArray[i] );
  for( i = 0; i < numIntegers; i++ )
    int_list.append( integerArray[i] );
  for( i = 0; i < numDoubles; i++ )
    double_list.append( doubleArray[i] );
  
  return new CubitSimpleAttrib( &string_list, &double_list, &int_list );
}

  // compare to a CubitSimpleAttrib
bool OCCAttrib::equals( CubitSimpleAttrib* csa ) const
{
  // compare counts
  csa->string_data_list();
  csa->string_data_list()->size();

  //int size1 = numIntegers;
  //int size2 = numDoubles;
  //int size3 = numStrings;
  
  if( csa->int_data_list()->size() != numIntegers ||
      csa->double_data_list()->size() != numDoubles ||
      csa->string_data_list()->size() != numStrings )
    return false;
  
    // compare strings first because most likely the
    // first string (the name) will differ.
  int i;
  csa->string_data_list()->reset();
  for( i = 0; i < numStrings; i++ )
    if( stringArray[i] != *csa->string_data_list()->next(i) )
      return false;

    // compare integers
  csa->int_data_list()->reset();
  for( i = 0; i < numIntegers; i++ )
    if( integerArray[i] != *csa->int_data_list()->next(i) )
      return false;

    // compare doubles
  csa->double_data_list()->reset();
  for( i = 0; i < numDoubles; i++ )
    if( doubleArray[i] != *csa->double_data_list()->next(i) )
      return false;

  return true;
}

  // write to a file at the current file offset
CubitStatus OCCAttrib::save(FILE *save_file) const
{
  if( save_file == NULL)
  {
    PRINT_ERROR("Problem saving MBG attributes: null FILE ptr\n");
    return CUBIT_FAILURE;
  }

  NCubitFile::CIOWrapper wrapper(save_file);

  // write a version number for the attribute data
  unsigned int Attrib_Version = 1;
  wrapper.Write(&Attrib_Version, 1);

  // write the number of strings, number of doubles, and number of integers
  int counts[3] = { numStrings, numDoubles, numIntegers };
  wrapper.Write(reinterpret_cast<unsigned int*>(counts), 3);

  // write the string data
  int i;
  for( i = 0; i < numStrings; i++ )
    wrapper.Write(stringArray[i].c_str());

  // write the doubles
  wrapper.Write(doubleArray, numDoubles);

  // write the integers
  wrapper.Write(reinterpret_cast<unsigned int*>(integerArray), numIntegers);
  
  return CUBIT_SUCCESS;
}


  // read from file starting at current file offset
OCCAttrib* OCCAttrib::restore(FILE *restore_file, unsigned int endian)
{
  if( restore_file == NULL )
    return NULL;

  NCubitFile::CIOWrapper wrapper(endian, restore_file );

  // write a version number for the attribute data
  unsigned int version;
  wrapper.Read(&version, 1);

  // haven't handled any version changes yet
  if( version != 1 )
  {
    PRINT_ERROR("Wrong OCCAttrib version : %u\n", version );
    return NULL;
  }

  // read the number of strings, number of doubles, and number of integers
  int counts[3];
  wrapper.Read(reinterpret_cast<unsigned int*>(counts), 3);
  int n_strings = counts[0];
  int n_doubles = counts[1];
  int n_ints = counts[2];
  
    // allocate arrays, but don't try to allocate zero-length array
  CubitString* strings = n_strings ? new CubitString[n_strings] : NULL;
  double *doubles = n_doubles ? new double[n_doubles] : NULL;
  int *ints = n_ints ? new int[n_ints] : NULL;
  
  // read the string data
  int i;
  for( i = 0; i < n_strings; i++ )
    strings[i] = CubitString(wrapper.Read());

  // write the doubles
  wrapper.Read(doubles, n_doubles);

  // write the integers
  wrapper.Read(reinterpret_cast<unsigned int*>(ints), n_ints);

  return new OCCAttrib(n_strings, strings, n_doubles, doubles, n_ints, ints);
}

