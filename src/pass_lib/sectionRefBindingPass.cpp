#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class pass : public filePassBase {
public:
   explicit pass(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      std::list<model::sectionRef*> refs;
      n.forEachDescendent<model::sectionRef>([&](auto& r){ refs.push_back(&r); });
      if(refs.size() == 0)
         return;

      std::map<std::string,std::set<model::header*> > table;
      n.forEachDescendent<model::header>([&](auto& h){ table[h.text].insert(&h); });

      for(auto it=refs.begin();it!=refs.end();++it)
      {
         auto& ans = table[(*it)->text];
         if(ans.size() == 0)
            cmn::error(cdwHere,"referenced section cannot be found")
               .with("section",(*it)->text)
               .raise();
         if(ans.size() > 1)
            cmn::error(cdwHere,"referenced section is multiply-defined")
               .with("section",(*it)->text)
               .raise();
         (*it)->number = (*ans.begin())->number;
      }
   }
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "sectionRef binding pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual std::set<std::string> getDeps() const
   {
      std::set<std::string> rval;
      rval.insert("renumber headers pass");
      return rval;
   }
   virtual bool isTransform() const { return true; }
   virtual bool isStrictTransform() const { return false; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
