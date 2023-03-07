#include "camera.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;

#define M_PI  3.1415926535897932

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR, double rndTheta) const {

  // TODO Assignment 7: Part 4
  // compute position and direction of ray from the input sensor sample coordinate.
  // Note: use rndR and rndTheta to uniformly sample a unit disk.

  double xCa = (x - 0.5) * 2 * tan((hFov/2)*M_PI/180.0);
  double yCa = (y - 0.5) * 2 * tan((vFov/2)*M_PI/180.0);

  Vector3D direction = Vector3D(xCa, yCa, -1);

  Vector3D pLens = Vector3D(lensRadius * sqrt(rndR) * cos(rndTheta), lensRadius * sqrt(rndR) * sin(rndTheta), 0);

  Vector3D pFocus = direction/direction.z * (-focalDistance);
  Vector3D pTarget = pFocus - pLens;
  
  pTarget.normalize();

  Vector3D pTarget_world = c2w*pTarget;
  Vector3D pLens_world = c2w*pLens + this->pos;

  Ray result = Ray(pLens_world, pTarget_world);
  result.min_t = nClip;
  result.max_t = fClip;

  return result;
}


} // namespace CGL
