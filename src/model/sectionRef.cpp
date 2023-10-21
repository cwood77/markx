#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "ast.hpp"
#include "service.hpp"
#include "textServices.hpp"

namespace model {
namespace {

class sectionRef : public iOwnedService, public iSectionRef {
public:
   explicit sectionRef(sectionBase& s) : m_node(s) {}

   virtual void release() { delete this; }

   virtual void expand()
   {
      bool foundNumbers = false;
      while(true)
      {
         auto *pNext = m_node.nextSibling();
         if(!pNext)
            break;

         auto& t = pNext->as<model::text>();
         bool handled = false;
         if(!foundNumbers)
         {
            foundNumbers = eatNumbers(m_node,t);
            handled = foundNumbers;
         }
         if(!handled)
            m_node.demandService<model::iPhrase>().combine(*pNext,/*destroy*/true);
      }

      m_pLog->writeLnVerbose("after expansion, sectionBase is <%s>",m_node.text.c_str());
   }

private:
   bool eatNumbers(model::sectionBase& s, model::text& w)
   {
      if(!w.demandService<model::iNumeric>().isNumeric())
         return false;

      s.number = w.text;

      eatNumericPeriodIf(w);

      w.destroy();
      return true;
   }

   void eatNumericPeriodIf(model::text& w)
   {
      auto *pG = w.nextSibling()->asIf<model::glue>();
      if(!pG)
         return;

      auto& p = pG->nextSibling()->as<model::text>();
      if(p.text == ".")
      {
         p.destroy();
         pG->destroy();
      }
      else
         cmn::error(cdwHere,"found glue but don't understand next node?")
            .raise();
   }

   sectionBase& m_node;
   cmn::lazyService<console::iLog> m_pLog;
};

class nodeServiceProvider : public iNodeServiceProvider {
public:
   virtual void bindNodeIf(node& n)
   {
      auto *pSec = n.asIf<sectionBase>();
      if(!pSec)
         return;

      n.bindService<iSectionRef>(*new sectionRef(*pSec));
   }
};

tcatExposeSingletonTypeAs(nodeServiceProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
