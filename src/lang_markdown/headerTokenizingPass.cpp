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
      std::set<model::header*> s;
      n.forEachDescendent<model::header>([&](auto& h){ s.insert(&h); });

      for(auto *pH : s)
      {
         auto& num = pH->insertSibling<model::text>();
         num.text = pH->number;

         auto& text = num.insertSibling<model::text>();
         text.text = pH->text;

         std::string symbol(pH->level,'#');
         pH->replaceSelf<model::text>().text = symbol;

         if(num.text.empty())
            num.destroy();

         if(text.text.empty())
            text.destroy();
      }
   }
};

class decomp : public linePassBase {
public:
   explicit decomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   void runOnLine(model::text& l)
   {
      if(!l.hasChildren())
         return;

      auto& word = l.first().as<model::text>();
      m_pLog->writeLnDebug("checking word <%s>",word.text.c_str());

      bool found = false;
      for(size_t i=1;i<20&&!found;i++)
      {
         std::string symbol(i,'#');
         found = mapSymbol(symbol,i,word);
      }
   }

private:
   bool mapSymbol(const std::string& s, size_t level, model::text& n)
   {
      if(n.text == s)
      {
         m_pLog->writeLnDebug("found symbol %s",s.c_str());
         auto& h = n.replaceSelf<model::header>();
         h.level = level;
         expandHeader(h);
         return true;
      }
      return false;
   }

   void expandHeader(model::header& h)
   {
      bool foundNumbers = false;
      while(true)
      {
         auto *pNext = h.nextSibling();
         if(!pNext)
            break;

         auto& t = pNext->as<model::text>();
         bool handled = false;
         if(!foundNumbers)
         {
            foundNumbers = eatNumbers(h,t);
            handled = foundNumbers;
         }
         if(!handled)
            h.demandService<model::iPhrase>().combine(*pNext,/*destroy*/true);
      }

      m_pLog->writeLnVerbose("after expansion, header is <%lld>, <%s>",h.level,h.text.c_str());
   }

   bool eatNumbers(model::header& h, model::text& w)
   {
      if(!w.demandService<model::iNumeric>().isNumeric())
         return false;

      h.number = w.text;

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
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "markdown header untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "markdown header tokenizing pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTokens; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
