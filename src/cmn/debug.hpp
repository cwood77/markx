#ifndef ___cmn_debug___
#define ___cmn_debug___

#include <sstream>
#include <string>

namespace cmn {

class sourceLocation {
public:
   sourceLocation(const std::string& file, size_t line) : file(file), line(line) {}

   std::string toString() const
   {
      std::stringstream stream;
      stream << "[" << file << ":" << line << "]";
      return stream.str();
   }

   std::string file;
   size_t      line;
};

#define cdwHere cmn::sourceLocation(__FILE__,__LINE__)

} // namespace cmn

#endif // ___cmn_debug___
