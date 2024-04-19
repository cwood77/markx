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
      std::list<model::toc*> tocs;
      n.forEachDescendent<model::toc>([&](auto& t){ tocs.push_back(&t); });
      if(tocs.size() == 0)
         return;

      std::list<model::header*> headers;
      n.forEachDescendent<model::header>([&](auto& h){ headers.push_back(&h); });

      for(auto *pToc : tocs)
      {
         // delete the old text
         while(pToc->hasChildren())
            pToc->first().destroy();

         // link headers
         for(auto *pHeader : headers)
            if(!pHeader->number.empty())
               pToc->entries.push_back(std::make_pair<>(pHeader->number,pHeader->text));
      }
   }
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "toc calculation pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual std::set<std::string> getDeps() const
   {
      std::set<std::string> deps;
      deps.insert("renumber headers pass");
      return deps;
   }
   virtual bool isTransform() const { return true; }
   virtual bool isStrictTransform() const { return false; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
