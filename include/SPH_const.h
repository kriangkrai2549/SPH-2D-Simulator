#ifndef SPHCONST_H
#define SPHCONST_H


struct SPHConst{
  float h;
  float h2;
  float h4;
  float den;
  float h6;
  float vis_lap;
  float h7;
  float inv_h7;
  float h8;
  float h9;
  float poly;
  float poly_diff;  
  float poly_lap;
  float spiky_diff;
  SPHConst(const float& smooth_radius){
    h = smooth_radius;
    h2 = h * h;
    h4 = h2 * h2;
    h6 = h4 * h2;
    h7 = h6 * h;
    h8 = h7 * h;
    h9 = h8 * h;
    vis_lap = 12.73 / h6;
    den = 1.2732f / h4;
    inv_h7 = 1 / h7;
    poly = 1.2732f / h8;
    poly_lap = (315.0f * 6.0f) / (64.0f * M_PI * h9);
    spiky_diff = -45.0f / (M_PI * h6);
    poly_diff = -5.7296f / h7;
  }
};

#endif



