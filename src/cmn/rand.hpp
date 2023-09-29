#ifndef ___cmn_rand___
#define ___cmn_rand___

#include <cstdlib>
#include <ctime>
#include <vector>

namespace rnd {

inline void seed() { ::srand(::time(NULL)); }

template<class T>
class die {
public:
   die& addSide(T x) { m_sides.push_back(x); return *this; }

   template<class C>
   die& addContainer(C& x)
   {
      for(auto it=x.begin();it!=x.end();++it)
         addSide(*it);
      return *this;
   }

   T roll() { return m_sides[::rand() % m_sides.size()]; }

private:
   std::vector<T> m_sides;
};

template<class T>
inline die<T>& addSides(die<T>& d, size_t n)
{
   for(size_t i=0;i<n;i++)
      d.addSide((T)(i+1));
   return d;
}

inline die<int> d2() { die<int> d; addSides(d,2); return d; }
inline die<int> d3() { die<int> d; addSides(d,3); return d; }
inline die<int> d6() { die<int> d; addSides(d,6); return d; }

} // namespace rnd

#endif // ___cmn_rand___
