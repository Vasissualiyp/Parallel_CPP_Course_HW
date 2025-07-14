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
    return 2;
  }
  // argc == 2 case
  const float deg{ strtof(argv[1], nullptr) };
  if (deg == HUGE_VALF)
  {
    cerr << "FATAL: Aborting since <number_of_degrees> is invalid.\n";
    return 3;
  }
  //=============================================
  

  // Declare rotation matrix
  auto radians = degrees_to_radians(REAL{deg});
  auto r = make_rot2x2_matrix(radians);

  // Declare and read the size of numbers' vector
  vector<pointN<REAL,2>> v;
  long long n;
  cin >> n;
  cout << n << endl;
  if (cin.fail()) {
    cout << "Unable to read n (number of points)" << endl;
    return 4;
  }
  v.reserve(n);

  // Read in the numbers
  REAL read_value;
  for (int i = 0; i < n; ++i) {
    cin >> read_value;
    v.push_back(read_value);
  }
  cout << v << endl;

  auto p  = pointN(3.F,2.F);
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

