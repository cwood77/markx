#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class pass : public filePassBase {
public:
   explicit pass(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& f)
   {
      f.forEachChild<model::node>([&](auto& p)
      {
         p.template forEachChild<model::text>([&](auto& l)
         {
            handleLine(l);
         });
      });
   }

private:
   void handleLine(model::text& l)
   {
      if(!l.hasChildren())
         return;

      auto *pWord = &l.first();
      int i = 1;

      bool sawNonSpaces = false;
      while(pWord)
      {
         auto& word = pWord->as<model::text>();
         bool kill = (sawNonSpaces && word.text.empty());

         if(!kill && !sawNonSpaces)
            sawNonSpaces = !word.text.empty();

         pWord = pWord->nextSibling();
         if(kill)
         {
            m_pLog->writeLnVerbose("cutting redundant whitespace--word %d--from line <%s>",i,l.text.c_str());
            word.destroy();
         }
         else
            i++;
      }
   }
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "space removing pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual bool isTransform() const { return true; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
