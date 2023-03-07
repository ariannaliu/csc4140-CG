#include "bvh.h"

#include "CGL/CGL.h"
#include "triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL {
namespace SceneObjects {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  primitives = std::vector<Primitive *>(_primitives);
  root = construct_bvh(primitives.begin(), primitives.end(), max_leaf_size);
}

BVHAccel::~BVHAccel() {
  if (root)
    delete root;
  primitives.clear();
}

BBox BVHAccel::get_bbox() const { return root->bb; }

void BVHAccel::draw(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->draw(c, alpha);
    }
  } else {
    draw(node->l, c, alpha);
    draw(node->r, c, alpha);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color &c, float alpha) const {
  if (node->isLeaf()) {
    for (auto p = node->start; p != node->end; p++) {
      (*p)->drawOutline(c, alpha);
    }
  } else {
    drawOutline(node->l, c, alpha);
    drawOutline(node->r, c, alpha);
  }
}

BVHNode *BVHAccel::construct_bvh(std::vector<Primitive *>::iterator start,
                                 std::vector<Primitive *>::iterator end,
                                 size_t max_leaf_size) {

  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  // bbox is the largest box
  BBox bbox;
  Vector3D judge_center(0.,0.,0.);
  int num_objects = end - start;
  for (auto p = start; p != end; p++) {
    BBox bb = (*p)->get_bbox();
    bbox.expand(bb);
    judge_center += (*p)->get_bbox().centroid();
  }
  judge_center /= (double)num_objects;

  BVHNode *node = new BVHNode(bbox);

  // return situation
  if(num_objects <= max_leaf_size){
    node->start = start;
    node->end = end;
    node->l = NULL;
    node->r = NULL;
    return node;
  }else{
    // which means we will split 
    // 1. Find the larget axis to split
    int axis = bbox.extent[0] < bbox.extent[1] ? 1 : 0;
    axis = bbox.extent[axis] < bbox.extent[2] ? 2 : axis;

    double axis_center = judge_center[axis];
    vector<Primitive*> left_objects;
    vector<Primitive*> right_objects;

    for(auto p = start; p != end; p++){
      if((*p)->get_bbox().centroid()[axis] <= axis_center){
        left_objects.push_back(*p);
      }else{
        right_objects.push_back(*p);
      }
    }

    if (left_objects.size()==0 || right_objects.size()==0){
      return node;
    }

    auto mid = start;
    auto mid_r = start;
    for (int i = 0; i < num_objects; i++){
      if (i < left_objects.size()){
        *mid = left_objects[i];
        mid++;
      }
      else{
        *mid_r = right_objects[i - left_objects.size()];
      }
      mid_r++;
    }
    node->l = construct_bvh(start, mid, max_leaf_size);
    node->r = construct_bvh(mid, end, max_leaf_size);

  }

}

bool BVHAccel::has_intersection(const Ray &ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.

  double t0, t1;
  if(node->bb.intersect(ray, t0, t1)){
    // ray has intersection with this node
    if(node->isLeaf()){
      for (auto p = node->start; p != node->end; p++){
        if((*p)->has_intersection(ray)){
          return true;
        }else{
          return has_intersection(ray, node->l) || has_intersection(ray, node->r);
        }
      }
    }
  }else{
    return false;
  }
}

bool BVHAccel::intersect(const Ray &ray, Intersection *i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.

  // double t0, t1;
  // total_isects++;
  if(node->bb.intersect(ray, ray.min_t, ray.max_t)){
    if(!(node->isLeaf())){
      // this node is not a leaf node
      bool ll = intersect(ray, i, node->l);
      bool rr = intersect(ray, i, node->r);
      // return intersect(ray, i, node->l) || intersect(ray, i, node->r);
      return ll||rr;
    }else{
      // this is a leaf node
      bool hit = false;
      for (auto p = node->start; p != node->end; p++){
        total_isects++;
        hit = (*p)->intersect(ray, i) || hit;
      }
      return hit;
    }
  }else{
    return false;
  }
}

} // namespace SceneObjects
} // namespace CGL
