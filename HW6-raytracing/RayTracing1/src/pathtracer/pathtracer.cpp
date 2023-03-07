#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out(0., 0., 0.);

  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading

  for(int target=0; target<num_samples; target++){
    Vector3D wi;
    double pdf;
    Vector3D sample = isect.bsdf->sample_f(w_out, &wi, &pdf); 

    Ray next_ray(hit_p, o2w*wi, 1);
    next_ray.min_t = EPS_F;
    

    Intersection i;
    if(bvh->intersect(next_ray, &i)){
      if(cos_theta(wi)>0){
        L_out += i.bsdf->get_emission() * sample * cos_theta(wi) / pdf;
      }
    }
  }
  L_out = L_out/num_samples;
  return L_out;
}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in
  // a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
  Vector3D L_out;

  // loop through all the lights
  // int count = 0;
  for (auto L_target = scene->lights.begin(); L_target != scene->lights.end(); L_target++){
    // count++;
    Vector3D wi;
    Vector3D L_sample;
    double distToLight;
    double pdf;
    // start sample, I only sample once per light
    Vector3D sample = (*L_target)->sample_L(hit_p, &wi, &distToLight, &pdf);
    Ray next_ray(hit_p, wi, 1);
    next_ray.min_t = EPS_F;
    next_ray.max_t = (double)distToLight - EPS_F;

    Intersection ii;
    if(!bvh->intersect(next_ray, &ii)){
      if(dot(wi, isect.n)>0){
        L_sample += sample * isect.bsdf->f(w_out, wi) * dot(wi, isect.n) / pdf;
      }
    }
    if ((*L_target)->is_delta_light()) {
      break;
    }
    L_out += L_sample;
  }
  return L_out;
}

Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light

  return isect.bsdf->get_emission();
}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`

  if (direct_hemisphere_sample) {
      return estimate_direct_lighting_hemisphere(r, isect);
  }
  else {
      return estimate_direct_lighting_importance(r, isect);
  }
  // return Vector3D(1.0);
  // return estimate_direct_lighting_hemisphere(r, isect);

}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Vector3D L_out;
  int num_samples = scene->lights.size()* ns_area_light;
  // Direct illumination on recursed object
  L_out += one_bounce_radiance(r, isect);

  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.

  double cpdf = 0.7;
  // random decision with recursion
  Vector3D wi;
  double pdf;

  Vector3D sample = isect.bsdf->sample_f(w_out, &wi, &pdf);
  Ray next_ray(hit_p, o2w * wi, 1);
  next_ray.depth = r.depth - 1;
  next_ray.min_t = EPS_F;

  if (coin_flip(cpdf)){
    Intersection ii;
    if(bvh->intersect(next_ray, &ii)){
      if (next_ray.depth > 1 && cos_theta(wi) > 0) {
          Vector3D sample_more = at_least_one_bounce_radiance(next_ray, ii);
          L_out += sample_more * sample * cos_theta(wi) / pdf / cpdf;
      }
    }
  }
  return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect)){
    return L_out;
  }

  // L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

  // TODO (Part 3): Return the direct illumination.

  L_out = zero_bounce_radiance(r, isect);
  L_out = L_out + at_least_one_bounce_radiance(r, isect);

  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct

  return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa;          // total samples to evaluate
  Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel
  
  //Start sampling
  if(num_samples == 1){
    Vector2D target;
    target.x = (origin.x + 0.5)/sampleBuffer.w;
    target.y = (origin.y + 0.5)/sampleBuffer.h;

    Ray my_ray = camera->generate_ray(target.x , target.y);
    my_ray.depth = max_ray_depth;
    Vector3D target_Color = est_radiance_global_illumination(my_ray);
    sampleBuffer.update_pixel(target_Color, x, y);
    sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;
  }else{
    Vector3D sum_Color = Vector3D(0,0,0);
    double s1 = 0.;
    double s2 = 0.;
    double mju = 0.;
    double sigma_square = 0.;
    double iTest = 0.;
    int count = 0;
    int sample_count = 0;
    int n=0;
    for(int i=0; i<num_samples; i++){
      Vector2D target = origin + gridSampler->get_sample();
      target.x = target.x / sampleBuffer.w;
      target.y = target.y / sampleBuffer.h;

      Ray my_ray = camera->generate_ray(target.x , target.y);
      my_ray.depth = max_ray_depth;

      Vector3D target_Color = est_radiance_global_illumination(my_ray);
      sum_Color += target_Color;
      s1 += target_Color.illum();
      s2 += target_Color.illum() * target_Color.illum();
      count++;
      sample_count++;

      // // judge batchsize
      if(count == samplesPerBatch){
        count = 0;
        mju = s1/double(i+1);
        sigma_square = (1./double(i))*(s2 - (s1*s1)/double(i+1));
        iTest = 1.96 * sqrt(sigma_square)/sqrt(i+1);
        if (iTest <= maxTolerance*mju) {
          // cout<<"Here!!!"<<endl;
          break;
        }
      }
    }
    sampleBuffer.update_pixel(sum_Color/sample_count, x, y);
    sampleCountBuffer[x + y * sampleBuffer.w] = sample_count;
  }
  // sampleBuffer.update_pixel(Vector3D(0.2, 1.0, 0.8), x, y);
  
}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL
