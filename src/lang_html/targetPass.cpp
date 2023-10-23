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

#if 0
      auto& outF = n.insertSibling(*new model::file(withoutExt + "html"));
      outF.pSource = &n;
      n.pTarget = &outF;

      m_pLog->writeLnInfo(
         "target file '%s' cooresponds to source file '%s'",
         outF.path.c_str(),
         n.path.c_str());
#else
      n.path = withoutExt + "html";
      n.rebindServices();
#endif
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
