#ifndef ___markx_finder___
#define ___markx_finder___

#include <set>
#include <string>

class finder {
public:
   finder(const std::string& pattern, bool recursive)
   : m_pattern(pattern), m_recursive(recursive) {}

   void find(const std::string& basePath, std::set<std::string>& paths);

private:
   std::string combine(const std::string& basePath, const std::string& leaf) const;

   std::string m_pattern;
   bool m_recursive;
};

#endif // ___markx_finder___
