#include "../model/textServices.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   void runOnFile(model::file& f)
   {
      std::list<model::sectionRef*> refs;
      f.forEachDescendent<model::sectionRef>([&](auto& s)
      {
         refs.push_back(&s);
      });

      for(auto *pRef : refs)
      {
         auto& n = pRef->insertSibling<model::text>();
         n.text = pRef->number;

         auto& t = n.insertSibling<model::text>();
         t.text = pRef->text;

         auto& x = pRef->replaceSelf<model::text>();
         x.text = "section";
      }
   }
};

class decomp : public linePassBase {
public:
   explicit decomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   void runOnLine(model::text& l)
   {
      if(!l.hasChildren())
         return;

      auto *pWord = &l.first();
      while(pWord)
      {
         auto *pText = pWord->asIf<model::text>();
         if(pText && pText->text == "section")
         {
            auto& s = pText->replaceSelf<model::sectionRef>();
            s.demandService<model::iSectionRef>().expand(/*wholeLine*/false);
            pWord = &s;
         }
         pWord = pWord->nextSibling();
      }
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "sectionRef unfinding pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "sectionRef finding pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTokens; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
