//- Class: CubitVector
//- Description: This file defines the CubitVector class.
//- Owner: Greg Sjaardema
//- Checked by:

#include <math.h>
#include "CubitVector.hpp"

// Define PI and TWO_PI
#undef PI
#ifdef M_PI
const double PI = M_PI;
#else
const double PI = 3.14159265358979323846;
#endif
const double TWO_PI = 2.0 * PI;


CubitVector &CubitVector::length(const double new_length)
{
  double length = this->length();
  xVal *= new_length / length;
  yVal *= new_length / length;
  zVal *= new_length / length;
  return *this;
}


double CubitVector::distance_between_squared(const CubitVector& test_vector) const
{
  double x = xVal - test_vector.xVal;
  double y = yVal - test_vector.yVal;
  double z = zVal - test_vector.zVal;
  
  return(x*x + y*y + z*z);
}

double CubitVector::distance_between(const CubitVector& test_vector) const
{
  double x = xVal - test_vector.xVal;
  double y = yVal - test_vector.yVal;
  double z = zVal - test_vector.zVal;
  
  return(sqrt(x*x + y*y + z*z));
}

// double CubitVector::distance_between(const CubitVector& test_vector, RefEdge* test_edge)
// {
//   return( test_edge->get_arc_length(*this, test_vector) );
// }


void CubitVector::print_me()
{
  printf("X: %f\n",xVal);
  printf("Y: %f\n",yVal);
  printf("Z: %f\n",zVal);
  
}


double CubitVector::interior_angle(const CubitVector &otherVector) const
{
  double cosAngle = 0.0, angleRad = 0.0, len1, len2 = 0.0;
  
  if (((len1 = this->length()) > 0) && ((len2 = otherVector.length()) > 0))
    cosAngle = (*this % otherVector)/(len1 * len2);
  else
  {
    assert(len1 > 0);
    assert(len2 > 0);
  }
  
  if ((cosAngle > 1.0) && (cosAngle < 1.0001))
  {
    cosAngle = 1.0;
    angleRad = acos(cosAngle);
  }
  else if (cosAngle < -1.0 && cosAngle > -1.0001)
  {
    cosAngle = -1.0;
    angleRad = acos(cosAngle);
  }
  else if (cosAngle >= -1.0 && cosAngle <= 1.0)
    angleRad = acos(cosAngle);
  else
  {
    assert(cosAngle < 1.0001 && cosAngle > -1.0001);
  }
  
  return( (angleRad * 180.) / PI );
}


void CubitVector::xy_to_rtheta()
{
    //careful about overwriting
  double r_ = length();
  double theta_ = atan2( yVal, xVal );
  if (theta_ < 0.0) 
    theta_ += TWO_PI;
  
  r( r_ );
  theta( theta_ );
}

void CubitVector::rtheta_to_xy()
{
    //careful about overwriting
  double x_ =  r() * cos( theta() );
  double y_ =  r() * sin( theta() );
  
  x( x_ );
  y( y_ );
}

void CubitVector::rotate(double angle, double )
{
  xy_to_rtheta();
  theta() += angle;
  rtheta_to_xy();
}

void CubitVector::blow_out(double gamma, double rmin)
{
    // if gamma == 1, then 
    // map on a circle : r'^2 = sqrt( 1 - (1-r)^2 )
    // if gamma ==0, then map back to itself
    // in between, linearly interpolate
  xy_to_rtheta();
//  r() = sqrt( (2. - r()) * r() ) * gamma  + r() * (1-gamma);
  assert(gamma > 0.0);
    // the following limits should really be roundoff-based
  if (r() > rmin*1.001 && r() < 1.001) {
    r() = rmin + pow(r(), gamma) * (1.0 - rmin);
  }
  rtheta_to_xy();
}

void CubitVector::reflect_about_xaxis(double, double )
{
  yVal = -yVal;
}

void CubitVector::scale_angle(double gamma, double )
{
  const double r_factor = 0.3;
  const double theta_factor = 0.6;
  
  xy_to_rtheta();
  
    // if neary 2pi, treat as zero
    // some near zero stuff strays due to roundoff
  if (theta() > TWO_PI - 0.02)
    theta() = 0;
    // the above screws up on big sheets - need to overhaul at the sheet level
  
  if ( gamma < 1 )
  {
      //squeeze together points of short radius so that
      //long chords won't cross them
    theta() += (CUBIT_PI-theta())*(1-gamma)*theta_factor*(1-r());
    
      //push away from center of circle, again so long chords won't cross
    r( (r_factor + r()) / (1 + r_factor) );
    
      //scale angle by gamma
    theta() *= gamma;
  }
  else
  {
      //scale angle by gamma, making sure points nearly 2pi are treated as zero
    double new_theta = theta() * gamma;
    if ( new_theta < 2.5 * CUBIT_PI || r() < 0.2) 
      theta( new_theta );
  }
  rtheta_to_xy();
}

double CubitVector::vector_angle_quick(const CubitVector& vec1, 
			       	 const CubitVector& vec2)
{
  //- compute the angle between two vectors in the plane defined by this vector
  // build yAxis and xAxis such that xAxis is the projection of
  // vec1 onto the normal plane of this vector

  // NOTE: vec1 and vec2 are Vectors from the vertex of the angle along
  //       the two sides of the angle.
  //       The angle returned is the right-handed angle around this vector
  //       from vec1 to vec2.

  // NOTE: vector_angle_quick gives exactly the same answer as vector_angle below
  //       providing this vector is normalized.  It does so with two fewer
  //       cross-product evaluations and two fewer vector normalizations.
  //       This can be a substantial time savings if the function is called
  //       a significant number of times (e.g Hexer) ... (jrh 11/28/94)
  // NOTE: vector_angle() is much more robust. Do not use vector_angle_quick()
  //       unless you are very sure of the safety of your input vectors.

  CubitVector ry = (*this) * vec1;
  CubitVector rx = ry * (*this);

  double x = vec2 % rx;
  double y = vec2 % ry;

  double angle;
  assert(x != 0.0 || y != 0.0);

  angle = atan2(y, x);

  if (angle < 0.0)
  {
    angle += TWO_PI;
  }
  return angle;
}

CubitVector vectorRotate(const double angle,
                         const CubitVector &normalAxis,
                         const CubitVector &referenceAxis)
{
    // A new coordinate system is created with the xy plane corresponding
    // to the plane normal to the normal axis, and the x axis corresponding to
    // the projection of the reference axis onto the normal plane.  The normal
    // plane is the tangent plane at the root point.  A unit vector is
    // constructed along the local x axis and then rotated by the given
    // ccw angle to form the new point.  The new point, then is a unit
    // distance from the global origin in the tangent plane.
  
  double x, y;
  
    // project a unit distance from root along reference axis
  
  CubitVector yAxis = normalAxis * referenceAxis;
  CubitVector xAxis = yAxis * normalAxis;
  yAxis.normalize();
  xAxis.normalize();
  
  x = cos(angle);
  y = sin(angle);
  
  xAxis *= x;
  yAxis *= y;
  return CubitVector(xAxis + yAxis);
}

double CubitVector::vector_angle(const CubitVector &vector1,
                                 const CubitVector &vector2) const
{
    // This routine does not assume that any of the input vectors are of unit
    // length. This routine does not normalize the input vectors.
    // Special cases:
    //     If the normal vector is zero length:
    //         If a new one can be computed from vectors 1 & 2:
    //             the normal is replaced with the vector cross product
    //         else the two vectors are colinear and zero or 2PI is returned.
    //     If the normal is colinear with either (or both) vectors
    //         a new one is computed with the cross products
    //         (and checked again).
  
    // Check for zero length normal vector
  CubitVector normal = *this;
  double normal_lensq = normal.length_squared();
  double len_tol = 0.0000001;
  if( normal_lensq <= len_tol )
  {
      // null normal - make it the normal to the plane defined by vector1
      // and vector2. If still null, the vectors are colinear so check
      // for zero or 180 angle.
    normal = vector1 * vector2;
    normal_lensq = normal.length_squared();
    if( normal_lensq <= len_tol )
    {
      double cosine = vector1 % vector2;
      if( cosine > 0.0 ) return 0.0;
      else               return CUBIT_PI;
    }
  }
  
    //Trap for normal vector colinear to one of the other vectors. If so,
    //use a normal defined by the two vectors.
  double dot_tol = 0.985;
  double dot = vector1 % normal;
  if( dot * dot >= vector1.length_squared() * normal_lensq * dot_tol )
  {
    normal = vector1 * vector2;
    normal_lensq = normal.length_squared();
    
      //Still problems if all three vectors were colinear
    if( normal_lensq <= len_tol )
    {
      double cosine = vector1 % vector2;
      if( cosine >= 0.0 ) return 0.0;
      else                return CUBIT_PI;
    }
  }
  else
  {
      //The normal and vector1 are not colinear, now check for vector2
    dot = vector2 % normal;
    if( dot * dot >= vector2.length_squared() * normal_lensq * dot_tol )
    {
      normal = vector1 * vector2;
    }
  }
  
    // Assume a plane such that the normal vector is the plane's normal.
    // Create yAxis perpendicular to both the normal and vector1. yAxis is
    // now in the plane. Create xAxis as the perpendicular to both yAxis and
    // the normal. xAxis is in the plane and is the projection of vector1
    // into the plane.
  
  normal.normalize();
  CubitVector yAxis = normal;
  yAxis *= vector1;
  double y = vector2 % yAxis;
    //  yAxis memory slot will now be used for xAxis
  yAxis *= normal;
  double x = vector2 % yAxis;
  
  
    //  assert(x != 0.0 || y != 0.0);
  if( x == 0.0 && y == 0.0 )
  {
    return 0.0;
  }
  double angle = atan2(y, x);
  
  if (angle < 0.0)
  {
    angle += TWO_PI;
  }
  return angle;
}

CubitBoolean CubitVector::within_tolerance( const CubitVector &vectorPtr2,
                                            double tolerance) const
{
  return (( fabs (this->xVal - vectorPtr2.xVal) < tolerance) &&
          ( fabs (this->yVal - vectorPtr2.yVal) < tolerance) &&
          ( fabs (this->zVal - vectorPtr2.zVal) < tolerance)
         );
}

CubitBoolean CubitVector::within_scaled_tolerance(const CubitVector &v2, double tol) const
{
  if (tol < 0)
    tol = -tol;

  return (((fabs (xVal - v2.xVal) < tol) || ((xVal > 0 == v2.xVal > 0) && fabs(xVal) > tol && fabs(v2.xVal/xVal - 1) < tol)) &&
          ((fabs (yVal - v2.yVal) < tol) || ((yVal > 0 == v2.yVal > 0) && fabs(yVal) > tol && fabs(v2.yVal/yVal - 1) < tol)) &&
          ((fabs (zVal - v2.zVal) < tol) || ((zVal > 0 == v2.zVal > 0) && fabs(zVal) > tol && fabs(v2.zVal/zVal - 1) < tol))
         );
}

CubitBoolean CubitVector::about_equal(const CubitVector &w,
                                      const double relative_tolerance,
                                      const double absolute_tolerance ) const
{
  if ( absolute_tolerance == 0. && 
       relative_tolerance == 0. )
  {
    if ( xVal == w.xVal &&
         yVal == w.yVal &&
         zVal == w.zVal )
      return CUBIT_TRUE;
  }
  else 
  {
    const CubitVector diff = *this - w;
    const double diff_size = diff.length_squared();
    const double a_tol_size = absolute_tolerance * absolute_tolerance;
      // catches v == w
    if ( diff_size <= a_tol_size )
      return CUBIT_TRUE;
    if ( relative_tolerance > 0. )
    {
      const CubitVector sum = *this + w;
      const double sum_size = sum.length_squared();
      const double r_tol_size = relative_tolerance * relative_tolerance;
      if ( 4. * diff_size <= sum_size * r_tol_size )
           // Q: why this formula? 
           // A: because if v = 1,0,eps, w = 1,0,0, then
           // diff_size = eps^2
           // sum_size = about 4.
           // and function returns true if eps^2 <= tolerance^2
        return CUBIT_TRUE;
    }
  }
  return CUBIT_FALSE;
}


void CubitVector::orthogonal_vectors( CubitVector &vector2, 
                                      CubitVector &vector3 ) const
{
  double x[3];
  unsigned short i = 0;
  unsigned short imin = 0;
  double rmin = 1.0E20;
  unsigned short iperm1[3];
  unsigned short iperm2[3];
  unsigned short cont_flag = 1;
  double vec1[3], vec2[3];
  double rmag;
  
    // Copy the input vector and normalize it
  CubitVector vector1 = *this;
  vector1.normalize();
  
    // Initialize perm flags
  iperm1[0] = 1; iperm1[1] = 2; iperm1[2] = 0;
  iperm2[0] = 2; iperm2[1] = 0; iperm2[2] = 1;
  
    // Get into the array format we can work with
  vector1.get_xyz( vec1 );
  
  while (i<3 && cont_flag )
  {
    if (fabs(vec1[i]) < 1e-6)
    {
      vec2[i] = 1.0;
      vec2[iperm1[i]] = 0.0;
      vec2[iperm2[i]] = 0.0;
      cont_flag = 0;
    }
    
    if (fabs(vec1[i]) < rmin)
    {
      imin = i;
      rmin = fabs(vec1[i]);
    }
    ++i;
  }
  
  if (cont_flag)
  {
    x[imin] = 1.0;
    x[iperm1[imin]] = 0.0;
    x[iperm2[imin]] = 0.0;
    
      // Determine cross product
    vec2[0] = vec1[1] * x[2] - vec1[2] * x[1];
    vec2[1] = vec1[2] * x[0] - vec1[0] * x[2];
    vec2[2] = vec1[0] * x[1] - vec1[1] * x[0];
    
      // Unitize
    rmag = sqrt(vec2[0]*vec2[0] + vec2[1]*vec2[1] + vec2[2]*vec2[2]);
    vec2[0] /= rmag;
    vec2[1] /= rmag;
    vec2[2] /= rmag;
  }
  
    // Copy 1st orthogonal vector into CubitVector vector2
  vector2.set( vec2 );
  
    // Cross vectors to determine last orthogonal vector
  vector3 = vector1 * vector2;
  
  return;
}

//- Find next point from this point using a direction and distance
void CubitVector::next_point( const CubitVector &direction,
                              double distance, CubitVector& out_point ) const
{
  CubitVector my_direction = direction;
  my_direction.normalize();
  
    // Determine next point in space
  out_point.x( xVal + (distance * my_direction.xVal) );     
  out_point.y( yVal + (distance * my_direction.yVal) );     
  out_point.z( zVal + (distance * my_direction.zVal) ); 
  
  return;
}

//- Project this vector onto the plane specified by the input plane normal
void CubitVector::project_to_plane( const CubitVector &planenormal )
{
    // Cross the vector with the normal to get a vector on the plane
    CubitVector planevec = planenormal * (*this);

    // Cross the vector on the plane with the normal to get the 
    // projection of the vector on the plane
    *this = planevec * planenormal;
}

