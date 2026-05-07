#include<iostream>
#include<vector>
#include<cstdint>
#include<cmath>
#include<algorithm>
#include <unordered_set>

#include<spatialhashgrid.h>


using namespace std;



SpatialHashGrid::SpatialHashGrid(int _grid_x, int _grid_y,float grid_size_x,float grid_size_y,uint32_t particle_number, float smooth_radius) {
      grid_x  = _grid_x;
      grid_y  = _grid_y;
      this->grid_size_x = grid_size_x;
      this->grid_size_y = grid_size_y;
      max_x = grid_x * grid_size_x;
      max_y = grid_y * grid_size_y;

      hash_size = grid_x * grid_y;
      table_size = particle_number;   
      h = smooth_radius;

      
      start_cell.resize(hash_size); 
      end_cell.resize(hash_size); 
      hash_particles.reserve(table_size);
};


uint32_t SpatialHashGrid::compute_hash(Vec2 vec){
      const uint32_t P1 = 73856093, P2 = 19349663;
      int ix = std::min((int)vec.x,grid_x-1);
      int iy = std::min((int)vec.y,grid_y-1);

      int term_1 = (ix * P1);
      int term_2 = (iy * P2);
      int result = (term_1 ^ term_2); 
      uint32_t hash = result % hash_size; 

      return hash;
};


int SpatialHashGrid::build(const vector<Particle>& particles){
      if( particles.size() == 0){return 1;}

      // Reset all cell lookup arrays to -1 (empty)
      fill(end_cell.begin(),end_cell.end(),-1);
      fill(start_cell.begin(),start_cell.end(),-1);

      //Create hash_particle(hash_number,index);
      hash_particles.resize(particles.size());
      for(int i = 0;i < particles.size();i++){
        const Particle& parti  = particles[i];
        int ix = (int)(parti.pos.x / grid_size_x);
        int iy = (int)(parti.pos.y / grid_size_y);
        uint32_t hash_number = compute_hash({(float)ix,(float)iy});        
        hash_particles[i] = {hash_number, i};
      }
      
      // Map each particle to a HashEntry { hash, index } based on its grid cell
      sort(hash_particles.begin(),hash_particles.end(),[](const HashEntry& a,const HashEntry& b){
        return a.hash < b.hash;
      });

      // Build start_cell / end_cell index ranges into the sorted hash_particles array
      uint32_t pre_hash = hash_particles[0].hash;
      for(int i = 0 ;i < hash_particles.size();i++){
        uint32_t cur_hash = hash_particles[i].hash;
        if(i == 0 || cur_hash != pre_hash){
          start_cell[cur_hash] = i;
          if(i!=0){end_cell[pre_hash] = i;}
        }
        pre_hash = cur_hash;
      }
      end_cell[pre_hash] = hash_particles.size();
      return 0;
    }







void SpatialHashGrid::get_neighbor_index(Vec2 pos, const vector<Particle>& all_particle,vector<int>& result,float smooth_radius) {
    result.clear();
    int ix = (int)floor(pos.x / grid_size_x);
    int iy = (int)floor(pos.y / grid_size_y);
    std::unordered_set<int> visited_hash;
    for (int x = ix - 1; x <= ix + 1; x++) {
      for (int y = iy - 1; y <= iy + 1; y++) {
        if (x < 0 || x >= grid_x || y < 0 || y >= grid_y) continue;
        
        uint32_t hash = compute_hash({ (float)x, (float)y });
        if(visited_hash.count(hash)){continue;}
        visited_hash.insert(hash);

        int start = start_cell[hash];
        int end = end_cell[hash];
        
        if (start == -1) continue;
        
        for (int i = start; i < end; i++) {
          int cur_parti_index = hash_particles[i].particle_index;
          Vec2 cur_parti_pos = all_particle[cur_parti_index].pos;
          
          if (is_near(pos, cur_parti_pos,smooth_radius) && (pos.x != cur_parti_pos.x || pos.y != cur_parti_pos.y)) {
                    result.push_back(cur_parti_index);
                }
            }
        } 
    }
}

bool SpatialHashGrid::is_near(Vec2 vec1,Vec2 vec2,float smooth_radius){
      float epsilon = 1e-6f;
      float dx = vec1.x - vec2.x;
      float dy = vec1.y - vec2.y;
      float len2 = dx*dx + dy*dy;
      float h2 = smooth_radius*smooth_radius;
      bool t = len2 < h2;
      return t;
}

void SpatialHashGrid::sh(){
      for(int i = 0 ; i < hash_particles.size();i++){
         HashEntry entry =  hash_particles[i];
         cout << "Hash: "<< entry.hash<<" Particle_index: " << entry.particle_index <<endl;
      }
      cout << "Start_cell[" ;
      for(int i = 0 ;i < start_cell.size();i++){
       cout << "," <<start_cell[i];
      }
      cout <<"]"<<endl;
      cout << "End_cell[" ;
      for(int i = 0 ;i < end_cell.size();i++){
       cout << "," <<end_cell[i];
      }
      cout <<"]"<<endl;

}




