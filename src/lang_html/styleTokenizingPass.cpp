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
      std::list<model::style*> nodes;
      n.forEachDescendent<model::style>([&](auto& n){ nodes.push_back(&n); });

      for(auto *pN : nodes)
      {
         auto str = genHtml(pN->kind,pN->start);
         pN->replaceSelf<model::text>().text = str;
      }
   }

private:
   std::string genHtml(model::style::kinds k, bool start)
   {
      std::stringstream s;
      s << "<";
      if(!start)
         s << "/";
      switch(k)
      {
         case model::style::kBold:
            s << "b";
            break;
         case model::style::kItalic:
            s << "i";
            break;
         default:
            cmn::unimplemented(cdwHere).raise();
            break;
      }
      s << ">";
      return s.str();
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
   virtual state::type getInput() const { return state::kPrepTranslate; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "html style untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "html style tokenizing pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kPrepTranslate; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
