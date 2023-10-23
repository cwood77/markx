#include "../file/manager.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"

namespace pass {
namespace {

class pass : public filePassBase {
public:
   explicit pass(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      tcat::typePtr<file::iFileManager> fMan;
      auto ext = fMan->getExtension(n.path);
      auto withoutExt = std::string(n.path.c_str(),n.path.length() - ext.length());
      n.path = withoutExt + "html";
      n.rebindServices();
   }
};

class passInfo : public iTranslationInfo {
public:
   virtual std::string desc() const { return "HTML target pass"; }
   virtual state::type getInput() const { return state::kTranslate; }
   virtual iPass& create() const { return *new pass(*this); }
   virtual std::string destination() const { return "html"; }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
