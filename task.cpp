#include <algorithm>
#include <cstdlib>
#include <execution>
#include <iostream>
#include <vector>
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
  
  // CLI arguments handling block ===============
  if (argc < 2) { // Output how-to use the program error message
    cerr << "USAGE: " << argv[0] << " <number_of_degrees>\n";
    return 1;
  } else if (argc == 2) { // Only obtain the angle of rotation
    // read in command line argument as a float with error detection...
    cout << "Normal number of arguments" << endl;

  } else { // Only obtain the angle of rotation
    cerr << "Too many arguments!" << endl;
    cerr << "USAGE: " << argv[0] << " <number_of_degrees>\n";
    return 3;
  }
  const float deg{ strtof(argv[1], nullptr) };
  if (deg == HUGE_VALF)
  {
    cerr << "FATAL: Aborting since <number_of_degrees> is invalid.\n";
    return 2;
  }
  //=============================================
  

  auto radians = degrees_to_radians(REAL{deg});
  // create variables that own the data...
  auto r = make_rot2x2_matrix(radians);
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
  return 0;
}

