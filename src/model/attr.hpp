#ifndef ___model_attr___
#define ___model_attr___

#include <map>

namespace model {

class attributable;

template<class T>
class attribute {
public:
   T& getOrCreateAttr(attributable& a);
   const T& getOrCreateAttr(attributable& a) const;

private:
   std::map<attributable*,T> m_values;
};

class attributable {
public:
   template<class T> T& operator[](attribute<T>& a)
   { return a.getOrCreateAttr(*this); }

   template<class T> const T& operator[](attribute<T>& a) const
   { return a.getOrCreateAttr(*this); }
};

} // namespace model

#endif // ___model_attr___
