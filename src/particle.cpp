#include<iostream>
#include <iomanip>
#include"particle.h"

std::ostream& operator<<(std::ostream &os, const Particle &p) {
    os << std::fixed << std::setprecision(4);
    
    os << "Pos: "   << std::setw(5) << p.pos << " | "
       << "Force: "   << std::setw(5) << p.force << " | "
       << "Velo: "  << std::setw(5) << p.velocity << " | "
       << "Den: "   << std::setw(4)  << p.density << " | "
       << "Pres: "  << std::setw(4)  << p.pressure << " | " ;
    return os;
}

