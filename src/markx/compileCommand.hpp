#ifndef ___markx_compileCommand___
#define ___markx_compileCommand___

#include "../console/arg.hpp"
#include <string>

namespace console { class iLog; };
namespace pass { class iPassCatalog; };
namespace pass { class iPassManager; };
namespace pass { class iPassSchedule; };

class compileCommand : public console::iCommand {
public:
   compileCommand() : oRecursive(false), oInFilePattern("*") {}

   bool oRecursive;
   std::string oInFilePath;
   std::string oInFilePattern;

   virtual void run(console::iLog& l);

protected:
   virtual pass::iPassSchedule& compile(pass::iPassManager& pm, pass::iPassCatalog& pc) = 0;
};

#endif // ___markx_compileCommand___
