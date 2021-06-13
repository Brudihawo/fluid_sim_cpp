#pragma once
#include "container.h"
#include <vector>
#include <string>

class SimulationDomain {
  protected:
    const int N_SCALAR_FIELDS;
    std::vector<std::vector<double>> fields;
    std::vector<std::vector<double>> old;
    
    const long NX = -1;
    const long NY = -1;
    const double DELTA = -1;
    const double DELTA_T = -1;
    const long N_TIMESTEPS = -1;

    BoundaryType b_type[2]; // TODO: Find a way for this to be const

    std::vector<std::string> field_descriptors;
    std::vector<std::pair<double, double>> field_value_limits;

    long idx(long x, long y);

  private:
    void init();
    
  public:
    SimulationDomain(SimParams& p, int n_scalar_fields);
    virtual ~SimulationDomain();

    virtual bool timestep(long t) = 0;
    inline DomainData get_data() {return {fields, field_descriptors, field_value_limits, N_SCALAR_FIELDS, NX, NY};} 

    double ddx(std::vector<double>& s, long  x, long  y);
    double ddy(std::vector<double>& s, long  x, long  y);
    double d2dx(std::vector<double>& s, long  x, long  y);
    double d2dy(std::vector<double>& s, long  x, long  y);
    long idx(long x, long y, int xoff, int yoff);

    void add_noise(int field_index, double sigma);
    void smooth_gaussian(int field_index, int n_iterations);

    //void read_infile(std::string filename);

    bool set_value(int field_id, long x, long y, double value);
    bool set_value_shape(Shape shape, int field_id, long x0, long x1, long y0, long y1, double value);
};
