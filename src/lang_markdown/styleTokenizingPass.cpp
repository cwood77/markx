#include "../model/lang.hpp"
#include "../model/textServices.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   virtual void runOnFile(model::file& n)
   {
      cmn::unimplemented(cdwHere).raise();
   }
};

class decomp : public wordPassBase {
public:
   explicit decomp(const iPassInfo& info) : wordPassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   void runOnWord(model::text& w)
   {
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kPrepTranslate; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "markdown style untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "markdown style tokenizing pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kPrepTranslate; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
