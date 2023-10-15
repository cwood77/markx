#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class pass : public linePassBase {
public:
   explicit pass(const iPassInfo& info) : linePassBase(info) {}

protected:
   virtual void runOnLine(model::text& l)
   {
      if(!l.hasChildren())
         return;

      auto *pWord = &l.first();

      bool sawNonSpaces = false;
      while(pWord)
      {
         bool kill = false;

         auto& t = pWord->as<model::text>();
         if(!pWord->is<model::glue>())
         {
            kill = (sawNonSpaces && t.text.empty());
            if(!kill && !sawNonSpaces)
               sawNonSpaces = !t.text.empty();
         }

         pWord = pWord->nextSibling();
         if(kill)
         {
            m_pLog->writeLnVerbose("cutting redundant whitespace from line <%s>",l.text.c_str());
            t.destroy();
         }
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
