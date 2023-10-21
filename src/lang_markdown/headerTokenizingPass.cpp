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
         h.demandService<model::iSectionRef>().expand();
         return true;
      }
      return false;
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
