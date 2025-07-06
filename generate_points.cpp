#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>

using REAL = float;

struct point2d { REAL x, y; };

inline std::ostream& operator<<(std::ostream& os, point2d const& p)
{
  os << p.x << ' ' << p.y;
  return os;
}

int main(int argc, char *argv[])
{
  using namespace std;

  if (argc <= 1)
  {
    cerr << "USAGE: " << argv[0] << " <number_of_2d_points>\n";
    return 1;
  }

  errno = 0;
  auto const n{ strtoll(argv[1], nullptr, 10) };

  if (n < 1 || errno == ERANGE)
  {
    cerr << "FATAL: Aborting since <number_of_2d_points> is invalid.\n";
    return 2;
  }

  random_device rd;
  default_random_engine re{rd()};
  uniform_real_distribution<REAL> ud{ REAL{-10'000}, REAL{10'000} };

  // output the necessarily digits of precision...
  cout.precision(numeric_limits<REAL>::max_digits10);

  // output the number of numbers...
  cout << n << '\n';

  // product n random points to std::cout...
  ranges::generate_n(
    ostream_iterator<point2d>(cout, " "),
    n,
    [&]() { return point2d{ ud(re), ud(re) }; }
  );
  cout << '\n';
}
