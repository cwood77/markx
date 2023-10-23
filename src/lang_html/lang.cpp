#include "../file/manager.hpp"
#include "../model/ast.hpp"
#include "../model/lang.hpp"
#include "../model/service.hpp"
#include "../tcatlib/api.hpp"

namespace model {
namespace {

class language : public iOwnedService, public iLanguage {
public:
   virtual void release() { delete this; }
   virtual std::string desc() { return "HTML"; }
};

class nodeServiceProvider : public iNodeServiceProvider {
public:
   virtual void bindNodeIf(node& n)
   {
      auto *pFile = n.asIf<file>();
      if(!pFile)
         return;

      tcat::typePtr<::file::iFileManager> fMan;
      auto ext = fMan->getExtension(pFile->path);
      if(ext == "html")
         n.bindService<iLanguage>(*new language());
   }
};

tcatExposeSingletonTypeAs(nodeServiceProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
