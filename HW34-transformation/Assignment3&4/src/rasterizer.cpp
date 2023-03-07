#include "rasterizer.h"
#include <iostream>

using namespace std;

namespace CGL {

  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height * sample_rate, Color::White);
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)


    sample_buffer[sample_rate * (y * width + x)] = c;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

    fill_pixel(sx, sy, color);
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

  // Check point in the triangele or not
  int line_test(float x, float y, float x0, float y0, float x1, float y1){
    return (-(x-x0)*(y1-y0) + (y-y0)*(x1-x0));
  }

  bool RasterizerImp::point_in_triangle(float x, float y, float x0, float y0, float x1, float y1, float x2, float y2){
    // // counter clock-wise
    bool counter_clock_wise = (line_test(x, y, x0, y0, x1, y1) >= 0) && (line_test(x, y, x1, y1, x2, y2) >= 0) && (line_test(x, y, x2, y2, x0, y0) >= 0);
    // clock-wise
    bool clock_wise = (line_test(x, y, x0, y0, x1, y1) <= 0) && (line_test(x, y, x1, y1, x2, y2) <= 0) && (line_test(x, y, x2, y2, x0, y0) <= 0);

    return clock_wise || counter_clock_wise;
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
    // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
    // find the rec boundary of the triangle
  
    // int x_min = min(min(x0, x1), x2), y_min = min(min(y0, y1), y2);
    // int x_max = max(max(x0, x1), x2), y_max = max(max(y0, y1), y2);


    // for(int yi = y_min; yi <= y_max; yi++){
    //   for(int xi = x_min; xi <= x_max; xi++){
    //     float x_sample = xi + 0.5, y_sample = yi + 0.5;
    //     if(point_in_triangle(x_sample, y_sample, x0, y0, x1, y1, x2, y2)){
    //       // Color col = color*(1/double(0.5));
    //       rasterize_point(x_sample, y_sample, color);
    //     }
    //   }
    // }
    // return;   
  
    // // TODO: Task 2: Update to implement super-sampled rasterization
    int x_min = min(min(x0, x1), x2), y_min = min(min(y0, y1), y2);
    int x_max = max(max(x0, x1), x2), y_max = max(max(y0, y1), y2);
    int edge_rate = sqrt(sample_rate);
    float edge_length = 1/edge_rate;
    
    for(int yi = y_min; yi <= y_max; yi++){
      for(int xi = x_min; xi <= x_max; xi++){
        // know focus on one pixel
        int count = 0;
        for (int yii = 0; yii < edge_rate; yii++){
          for (int xii = 0; xii < edge_rate; xii++){
            float x_sample = xi + (((float)xii + 0.5) / float(sample_rate));
            float y_sample = yi + (((float)yii + 0.5) / float(sample_rate));
            if(point_in_triangle(x_sample, y_sample, x0, y0, x1, y1, x2, y2)){
              sample_buffer[(width*yi + xi)*sample_rate + count] = color;
            }
            // col += color * (1/(double)sample_rate);
            
          }
          count++;
        }
        // if (col == standard_col){
        //   col = Color::White;
        // }
        // rasterize_point(xi, yi, col);
      }
    }
  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle
    int x_min = min(min(x0, x1), x2), y_min = min(min(y0, y1), y2);
    int x_max = max(max(x0, x1), x2), y_max = max(max(y0, y1), y2);
    int edge_rate = sqrt(sample_rate);
    float edge_length = 1/edge_rate;
    
    for(int yi = y_min; yi <= y_max; yi++){
      for(int xi = x_min; xi <= x_max; xi++){
        // know focus on one pixel
        int count = 0;
        for (int yii = 0; yii < edge_rate; yii++){
          for (int xii = 0; xii < edge_rate; xii++){
            float x_sample = xi + (((float)xii + 0.5) / float(sample_rate));
            float y_sample = yi + (((float)yii + 0.5) / float(sample_rate));
            if(point_in_triangle(x_sample, y_sample, x0, y0, x1, y1, x2, y2)){
              float para0, para1, para2;
              para0 = ((x1 - x_sample) * (y2 - y1) + (y_sample - y1) * (x2 - x1))/ ((x1 - x0) * (y2 - y1) + (y0 - y1) * (x2 - x1));
              para1 = ((x2 - x_sample) * (y0 - y2) + (y_sample - y2) * (x0 - x2))/ ((x2 - x1) * (y0 - y2) + (y1 - y2) * (x0 - x2));
              para2 = 1 - para1 - para0;
              sample_buffer[(width*yi + xi)*sample_rate + count] = para0* c0 + para1 * c1 + para2 * c2;
            }
          }
          count++;
        }
      }
    }



  }


  void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
    float x1, float y1, float u1, float v1,
    float x2, float y2, float u2, float v2,
    Texture& tex)
  {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    SampleParams params;
    params.lsm = lsm;
    params.psm = psm;

    int x_min = min(min(x0, x1), x2), y_min = min(min(y0, y1), y2);
    int x_max = max(max(x0, x1), x2), y_max = max(max(y0, y1), y2);
    int edge_rate = sqrt(sample_rate);
    float edge_length = 1/edge_rate;
    
    for(int yi = y_min; yi <= y_max; yi++){
      for(int xi = x_min; xi <= x_max; xi++){
        // know focus on one pixel
        int count = 0;
        for (int yii = 0; yii < edge_rate; yii++){
          for (int xii = 0; xii < edge_rate; xii++){
            float x_sample = xi + (((float)xii + 0.5) / float(sample_rate));
            float y_sample = yi + (((float)yii + 0.5) / float(sample_rate));
            if(point_in_triangle(x_sample, y_sample, x0, y0, x1, y1, x2, y2)){
              float para0, para1, para2, para0_up, para1_up, para2_up, para0_down, para1_down, para2_down;

              // calculate weight in the 3D space
              para0 = ((x1 - x_sample) * (y2 - y1) + (y_sample - y1) * (x2 - x1))/ ((x1 - x0) * (y2 - y1) + (y0 - y1) * (x2 - x1));
              para1 = ((x2 - x_sample) * (y0 - y2) + (y_sample - y2) * (x0 - x2))/ ((x2 - x1) * (y0 - y2) + (y1 - y2) * (x0 - x2));
              para2 = 1 - para1 - para0;

              para0_up = ((x1 - x_sample) * (y2 - y1) + (y_sample + 1 - y1) * (x2 - x1))/ ((x1 - x0) * (y2 - y1) + (y0 - y1) * (x2 - x1));
              para1_up = ((x2 - x_sample) * (y0 - y2) + (y_sample + 1 - y2) * (x0 - x2))/ ((x2 - x1) * (y0 - y2) + (y1 - y2) * (x0 - x2));
              para2_up = 1 - para1_up - para0_up;

              para0_down = ((x1 - (x_sample + 1)) * (y2 - y1) + (y_sample - y1) * (x2 - x1))/ ((x1 - x0) * (y2 - y1) + (y0 - y1) * (x2 - x1));
              para1_down = ((x2 - (x_sample + 1)) * (y0 - y2) + (y_sample - y2) * (x0 - x2))/ ((x2 - x1) * (y0 - y2) + (y1 - y2) * (x0 - x2));
              para2_down = 1 - para1_down - para0_down;
              Vector2D uv = para0 * Vector2D(u0, v0) + para1 * Vector2D(u1, v1) + para2 * Vector2D(u2, v2);
              Vector2D uv_up = para0_up * Vector2D(u0, v0) + para1_up * Vector2D(u1, v1) + para2_up * Vector2D(u2, v2);
              Vector2D uv_down = para0_down * Vector2D(u0, v0) + para1_down * Vector2D(u1, v1) + para2_down * Vector2D(u2, v2);
              // std::cout << uv[0] << std::endl;
              params.p_uv = uv;
              params.p_dx_uv = uv_down - uv;
              params.p_dy_uv = uv_up - uv;
              Color col;
              col = tex.sample(params);
              sample_buffer[(width*yi + xi)*sample_rate + count] = col;

            }
          }
          count++;
        }
      }
    }


    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle




  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support (check)

    this->sample_rate = rate;
    this->sample_buffer.resize(width * height * sample_rate, Color::White);
    // this->sample_buffer.resize(width * height, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support (check)

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;


    this->sample_buffer.resize(width * height * sample_rate, Color::White);
    // this->sample_buffer.resize(width * height, Color::White);
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialising, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support

    
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        int count = 0;
        Color col;
        for (int xii = 0; xii < sqrt(sample_rate); xii++){
          for (int yii = 0; yii < sqrt(sample_rate); yii++){
            col += (sample_buffer[(y * width + x) * sample_rate + count])*(float(1)/sample_rate); 
          }
          count++;
        }
        for (int k = 0; k < 3; ++k) {
          this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&col.r)[k] * 255;
        }
      }
    }

  }

  Rasterizer::~Rasterizer() { }


}// CGL
