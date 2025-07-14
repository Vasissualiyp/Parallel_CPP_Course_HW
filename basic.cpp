#include <cstdlib>
#include <iostream>
#include "coss2025_cpp_course.hpp"

int main(int argc, char *argv[])
{
  // "convenient" namespace directives...
  using namespace std;
  using namespace std::experimental;
  using namespace coss2025::cpp_course;
  using LINALG_NAMESPACE::matrix_vector_product;
  
  // define REAL type alias to float...
  using REAL = float;
  
  if (argc == 2)
  {
    // read in command line argument as a float with error detection...
    float const deg{ strtof(argv[1], nullptr) };
    if (deg == HUGE_VALF)
    {
      cerr << "FATAL: Aborting since <number_of_degrees> is invalid.\n";
      return 2;
    }
  }

  // create variables that own the data...
  auto r = make_rot2x2_matrix(degrees_to_radians(REAL{90}));
  auto p = pointN(3.F,2.F);
  auto fp = pointN<REAL,2>{};

  // create mdspans for those we want to exist longer than a single 
  // expression...
  auto pm = p.mdspan();
  auto fpm = fp.mdspan();

  // compute fp = r*p to rotate p about the origin...
  matrix_vector_product(r.mdspan(), pm, fpm);

  // output results...
  cout << "original point: (" << pm(0) << ',' << pm(1) << ")\n";
  cout << "final point: (" << fpm(0) << ',' << fpm(1) << ")\n";
}

