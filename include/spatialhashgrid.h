#ifndef SPATIALHASHGRID_H
#define SPATIALHASHGRID_H

#include<cstdint>
#include<vector>
#include"particle.h"

struct HashEntry {uint32_t hash; int particle_index;};


class SpatialHashGrid{
  private:
    int grid_x;
    int grid_y;
    float grid_size_x;
    float grid_size_y;
    float h;
    float max_x;
    float max_y;
    uint32_t table_size;
    uint32_t hash_size;
    std::vector<int> end_cell;
    std::vector<int> start_cell;
    std::vector<HashEntry> hash_particles;
  public:
    SpatialHashGrid(int _grid_x, int _grid_y,float grid_size_x,float grid_size_y,uint32_t particle_number, float smooth_radius);
    
    //Create a hash for specific cell
    uint32_t compute_hash(Vec2 vec);

    //Create a looking table,Store particle index in specific bucket
    int build(const std::vector<Particle>& particles);

    //Find who is near by
    void get_neighbor_index(Vec2 pos,const std::vector<Particle>&all_particle,std::vector<int>& result,float smooth_radius);

    //Compare two position
    bool is_near(Vec2 vec1,Vec2 vec2,float smooth_radius);

    //Show hash_paritcle,end_cell,start_cell
    void sh();
};

#endif