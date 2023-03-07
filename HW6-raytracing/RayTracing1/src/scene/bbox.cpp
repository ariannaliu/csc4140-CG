#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  double t0_temp = -INF_D;
  double t1_temp = INF_D;

  for (int axis=0; axis<3; axis++){
    double t_min = ((min[axis] - r.o[axis])/r.d[axis]) <= ((max[axis] - r.o[axis])/r.d[axis]) ? ((min[axis] - r.o[axis])/r.d[axis]):((max[axis] - r.o[axis])/r.d[axis]);
    double t_max = ((min[axis] - r.o[axis])/r.d[axis]) >= ((max[axis] - r.o[axis])/r.d[axis]) ? ((min[axis] - r.o[axis])/r.d[axis]):((max[axis] - r.o[axis])/r.d[axis]);
    t0_temp = std::max(t0_temp, t_min);
    t1_temp = std::min(t1_temp, t_max);
  }
  // if((t0 <= t1) && (std::max(t0, r.min_t) <= std::min(t1, r.max_t))){
  //   return true;
  // }else{
  //   return false;
  // }
  if ((t0_temp <= t1_temp) && ((t1 - t0_temp) >= 0 && (t1_temp - t0) >= 0)){
    return true;
  }else return false;
}

void BBox::draw(Color c, float alpha) const {

  glColor4f(c.r, c.g, c.b, alpha);

  // top
  glBegin(GL_LINE_STRIP);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
  glEnd();

  // bottom
  glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glEnd();

  // side
  glBegin(GL_LINES);
  glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
  glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
