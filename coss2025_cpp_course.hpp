#ifndef COSS2025_CPP_COURSE_HPP_
#define COSS2025_CPP_COURSE_HPP_

//=============================================================================

#if __cplusplus < 202002L
#  error "Compile this code for (at least) C++20."
#endif

//=============================================================================

//
// C++20 introduced feature test macros (FTMs), e.g., 
//
//   https://en.cppreference.com/w/cpp/feature_test.html
// 
// which simplify dealing with knowing a specific C++ compiler implements
// a feature or not. This feature only involves macro definitions and
// comparing to __cplusplus which is always defined, e.g., see:
//
//   https://en.cppreference.com/w/cpp/preprocessor/replace#Predefined_macros
//
// The values used for FTMs are that of __cplusplus. If 
// the FTM is not defined or if that macro is less than __cplusplus, then
// that feature is not supported (with current compilation settings).
//
// #include <version> ensures all FTMs are defined regardless of
// which header files are used. (Header files also define relevant FTMs.
// The benefit in using #include <version> is that all FTMs are
// defined regardless of which header file was included.)
//
#include <version>

//
// More C++ header files this header requires...
//
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <istream>
#include <iterator>
#include <ostream>
#include <span>
#include <stack>
#include <string_view>
#include <tuple>
#include <vector>

//
// Demonstrate how to "quietly" include the correct #include header
// and, only if not successful, use #error to stop compilation.
//
// MDSPAN_NAMESPACE is defined to be used in code to refer to the correct
// namespace for mdspan.
//
// MDSPAN_USES_STD_MDSPAN is defined only if <mdspan> is included and
// __cpp_lib_mdspan is defined and supported. This was done because
// if std::mdspan is being used, then only operator[]() is overloaded
// and other implementations will typically support operator()() 
// instead (e.g., for backporting to older C++ standards). Defining
// this macro allows code to check if MDSPAN_USES_STD_MDSPAN is
// defined and if it is, use [] instead of () for element access.
//

#ifdef __has_include
#  if __has_include(<mdspan>) && \
     defined(__cpp_lib_mdspan) && __cpp_lib_mdspan >= 202207L && \
     defined(__cpp_lib_submdspan) && __cpp_lib_submdspan >= 202306L
#    include <mdspan>
#    define MDSPAN_NAMESPACE  ::std
#    define MDSPAN_USES_STD_MDSPAN
#  elif __has_include(<experimental/mdspan>)
#    include <experimental/mdspan>
#    ifdef MDSPAN_IMPL_STANDARD_NAMESPACE
       // Kokkos' mdspan is in namespace MDSPAN_IMPL_STANDARD_NAMESPACE...
#      define MDSPAN_NAMESPACE  :: MDSPAN_IMPL_STANDARD_NAMESPACE
#    else
#      define MDSPAN_NAMESPACE  ::std::experimental
#    endif
#  else
#    error "Fix code to properly #include <mdspan>."
#  endif
#endif

//
// Do the same with <linalg>...
//
// LINALG_NAMESPACE is defined to be used in code to refer to the correct
// linalg namespace.
//
#ifdef __has_include
#  if __has_include(<linalg>) && __cplusplus > 202311L /* Need C++26 */
#    include <linalg>
#    define LINALG_NAMESPACE  ::std::linalg
#  elif __has_include(<experimental/linalg>)
#    include <experimental/linalg>
#    if defined(MDSPAN_IMPL_STANDARD_NAMESPACE) && defined(MDSPAN_IMPL_PROPOSED_NAMESPACE)
       // Kokkos' linalg is in namespace MDSPAN_IMPL_STANDARD_NAMESPACE :: MDSPAN_IMPL_PROPOSED_NAMESPACE
#      define LINALG_NAMESPACE  :: MDSPAN_IMPL_STANDARD_NAMESPACE :: MDSPAN_IMPL_PROPOSED_NAMESPACE::linalg
#    else
#      define LINALG_NAMESPACE  ::std::experimental::linalg
#    endif
#  else
#    error "Fix code to properly #include <linalg>."
#  endif
#endif

//
// C++20 defined a number of mathematical constants using variable
// templates, e.g., see:
//
//   https://www.en.cppreference.com/w/cpp/numeric/constants.html
//
// The NVIDIA HPC SDK does not define such, however. This can be dealt
// with in code. This header deals with such as follows:
//   * a NUMBERS_NAMESPACE macro is defined just like MDSPAN_NAMESPACE
//     and LINALG_NAMESPACE macros were defined above
//   * If <numbers> isn't available, a numbers namespace is defined
//     along with pi_v and pi definitions so those can be used in this
//     header. (Obviously if more constants are needed these will
//     need to be defined somewhere.)
//   * The __cpp_variable_templates FTM is used to determine whether or
//     not the code is being compiled with C++14 variable template
//     support or not.
//
#ifdef __has_include
#  if __has_include(<numbers>)
#    include <numbers>
#  endif
#endif

#if defined(__cpp_lib_math_constants)
#  define NUMBERS_NAMESPACE  ::std::numbers
#else
#  if !defined(__cpp_variable_templates) || (__cplusplus < __cpp_variable_templates)
#    error "C++14 variable templates must be supported."
#  endif
#  include <cmath> // for std::atan() below

namespace coss2025 { namespace cpp_course { namespace numbers {

// define pi_v...
template <std::floating_point T>
inline constexpr T pi_v{T(4)*std::atan(T(1))};

// define pi...
inline constexpr double pi{pi_v<double>};

// other constants are left undefined here
}}}

#  define NUMBERS_NAMESPACE  ::coss2025::cpp_course::numbers
#endif

//=============================================================================

namespace coss2025 { 
namespace cpp_course {

//=============================================================================

//
// simple_mdarray<T,Extents...> is a very simple hack to easily represent a 
// fixed-sized multidimensional array using a std::vector that can be
// implicitly cast to an mdspan.
//
// NOTE: simple_mdarray is a struct so the class is easily used as an
//       aggregate.
//
template <typename T, std::size_t... Extents>
struct simple_mdarray
{
  using mdspan_type = 
    MDSPAN_NAMESPACE::mdspan<
      T,
      MDSPAN_NAMESPACE::extents<size_t,Extents...>
    >
  ;
  using const_mdspan_type = 
    MDSPAN_NAMESPACE::mdspan<
      T const,
      MDSPAN_NAMESPACE::extents<size_t,Extents...>
    >
  ;

  // allow one to obtain an mdspan for this simple_mdarray...
  constexpr mdspan_type mdspan()
    { return mdspan_type{ a_.data() }; }
  constexpr const_mdspan_type mdspan() const
    { return const_mdspan_type{ a_.data() }; }

  // allow implicitly obtaining an mdspan from this type...
  constexpr operator mdspan_type() { return this->mdspan(); }
  constexpr operator const_mdspan_type() const { return this->mdspan(); }

  // copy...
  constexpr simple_mdarray& copy(mdspan_type m)
  {
    auto this_m = mdspan();
    std::copy_n(m.data_handle(), this_m.size(), this_m.data_handle());
    return *this;
  }
  
  constexpr simple_mdarray& copy(const_mdspan_type m)
  {
    auto this_m = mdspan();
    std::copy_n(m.data_handle(), this_m.size(), this_m.data_handle());
    return *this;
  }

  std::array<T,(Extents * ... * 1)> a_;
};

//=============================================================================

//
// simple_mdvector<T,Extents...> is a very simple hack to easily represent a 
// fixed-sized multidimensional array using a std::vector that can be
// implicitly cast to an mdspan.
//
template <typename T, std::size_t... Extents>
class simple_mdvector
{
public:
  using mdspan_type = 
    MDSPAN_NAMESPACE::mdspan<
      T,
      MDSPAN_NAMESPACE::extents<size_t,Extents...>
    >
  ;
  using const_mdspan_type = 
    MDSPAN_NAMESPACE::mdspan<
      T const,
      MDSPAN_NAMESPACE::extents<size_t,Extents...>
    >
  ;

  constexpr simple_mdvector() : v_((Extents * ... * 1), T{}) { }

  // copy operations...
  constexpr simple_mdvector(simple_mdvector const&) = default;
  constexpr simple_mdvector& operator=(simple_mdvector const&) = default;

  // move operations...
  constexpr simple_mdvector(simple_mdvector&&) = default;
  constexpr simple_mdvector& operator=(simple_mdvector&&) = default;

  // allow one to obtain an mdspan for this simple_mdvector...
  constexpr mdspan_type mdspan()
    { return mdspan_type{ v_.data() }; }
  constexpr const_mdspan_type mdspan() const
    { return const_mdspan_type{ v_.data() }; }

  // allow implicitly obtaining an mdspan from this type...
  constexpr operator mdspan_type() { return this->mdspan(); }
  constexpr operator const_mdspan_type() const { return this->mdspan(); }
  
private:
  std::vector<T> v_;
};

//=============================================================================

// This function will convert from degrees to radians.
inline auto degrees_to_radians(std::floating_point auto angle)
{
  using real = std::remove_const_t<decltype(angle)>;

  return NUMBERS_NAMESPACE::pi_v<real> * angle / real{180};
}

//=============================================================================

// This function returns an "owning" 2x2 rotation matrix where theta is the
// angle of rotation in radians...
constexpr auto make_rot2x2_matrix(std::floating_point auto theta)
{
  using real = std::remove_const_t<decltype(theta)>;
  simple_mdarray<real,2,2> tmp;
  auto m = tmp.mdspan();
#ifdef MDSPAN_USES_STD_MDSPAN
  m[0,0] = m[1,1] = std::cos(theta);
  m[0,1] = -std::sin(theta);
  m[1,0] = std::sin(theta);
#else
  m(0,0) = m(1,1) = std::cos(theta);
  m(0,1) = -std::sin(theta);
  m(1,0) = std::sin(theta);
#endif
  return tmp;
}

//=============================================================================

template <typename T, std::size_t N>
class pointN
{
public:
  using value_type = std::remove_cvref_t<T>;

private:
  simple_mdarray<value_type,N> data_;
  
  using mdspan_type = typename decltype(data_)::mdspan_type;
  using const_mdspan_type = typename decltype(data_)::const_mdspan_type;

public:
  constexpr pointN()
  {
    data_.a_.fill(value_type{});
  }

  template <typename... Args>
  requires (std::convertible_to<Args,T> && ...)
  constexpr pointN(Args const&&... args) :
    data_{T{args}...}
  {
  }

  constexpr pointN(pointN const&) = default;
  constexpr pointN& operator=(pointN const&) = default;

  constexpr pointN(pointN&&) = default;
  constexpr pointN& operator=(pointN&&) = default;

  constexpr operator mdspan_type() { return data_.mdspan(); }
  constexpr operator const_mdspan_type() const { return data_.mdspan(); }

  constexpr mdspan_type mdspan() { return data_.mdspan(); }
  constexpr const_mdspan_type mdspan() const { return data_.mdspan(); }

  std::istream& read(std::istream& is)
  {
    // safely read the data into a temporary variable...
    std::array<value_type,N> tmp;

    // read in the data into tmp...
    auto pos{ tmp.begin() };
    auto end{ tmp.end() };
    auto count{ N };
    for (; is && count > 0 && pos != end; ++pos, --count)
      is >> *pos;

    // if not all of the data was read in, then fail the stream and don't
    // update the data...
    if (count == 0)
      data_.a_ = std::move(tmp);
    else
      is.setstate(std::ios_base::failbit);
    return is;
  }

  std::ostream& write(std::ostream& os) const
  {
    auto pos{ data_.a_.begin() };
    auto end{ data_.a_.end() };
    for (; os && pos != end; ++pos)
    {
      os << *pos;
      if (std::next(pos) != end)
        os << ' ';
    }
    return os;
  }
};


// Deduction guide...
template <typename... US>
requires 
  (sizeof...(US) > 0) && 
  std::floating_point<std::common_type_t<US...>>
pointN(US...) -> pointN<std::common_type_t<US...>,sizeof...(US)>;


// IOStreams related...
template <typename Ch, typename ChTr, typename T, std::size_t N>
inline std::basic_istream<Ch,ChTr>& operator>>(std::basic_istream<Ch,ChTr>& is, 
  coss2025::cpp_course::pointN<T,N>& p)
{ 
  p.read(is);
  return is;
}   

template <typename Ch, typename ChTr, typename T, std::size_t N>
std::basic_ostream<Ch,ChTr>& operator<<(std::basic_ostream<Ch,ChTr>& os, 
  coss2025::cpp_course::pointN<T,N> const& p)
{ 
  p.write(os);
  return os;
}

//=============================================================================

} // namespace cpp_course
} // namespace coss2025

//=============================================================================

#endif // #ifndef COSS2025_CPP_COURSE_HPP_
