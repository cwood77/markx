#include "../file/manager.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"
#include "state.hpp"

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      m_pLog->writeLnInfo("   %s is noop",info().desc().c_str());
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

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

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTranslate; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "HTML untranslation pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "HTML translation pass"; }
   virtual state::type getInput() const { return state::kPrepTranslate; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTranslate; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
