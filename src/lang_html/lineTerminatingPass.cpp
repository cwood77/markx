#include "../model/lang.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"
#include <sstream>

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "html"; }

   virtual void runOnFile(model::file& n)
   {
      n.forEachChild<model::text>([](auto& l){ l.text += "<br/>"; });
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "html"; }

   virtual void runOnFile(model::file& n)
   {
      cmn::unimplemented(cdwHere).raise();
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kParagraphs; }
   virtual std::set<std::string> getDeps() const
   {
      std::set<std::string> rval;
      rval.insert("paragraph unclumping pass");
      return rval;
   }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "html line unterminating pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "html line terminating pass"; }
   virtual state::type getInput() const { return state::kLines; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kParagraphs; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
