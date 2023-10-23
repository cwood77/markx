#include "../file/manager.hpp"
#include "../model/ast.hpp"
#include "../model/lang.hpp"
#include "../model/service.hpp"
#include "../pass_lib/punctuationProvider.hpp"
#include "../tcatlib/api.hpp"

namespace model {
namespace {

class language : public iOwnedService, public iLanguage, public pass::iPunctuationProvider {
public:
   virtual void release() { delete this; }
   virtual std::string desc() { return "markdown"; }
   virtual bool targetOnly() const { return false; }
   virtual void getMidwordPunctuation(std::set<std::string>& p) const
   {
      // limit this to only what I need for now

      // images
      p.insert("(");
      p.insert(")");
      p.insert("[");
      p.insert("]");

      // style
      p.insert("*");
      p.insert("_");

      // tables
      p.insert("|");

      // illegal HTML chars (really a target thing?)
      p.insert("&");
   }
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
      if(ext == "md")
         n.bindService<iLanguage>(*new language());
   }
};

tcatExposeSingletonTypeAs(nodeServiceProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
