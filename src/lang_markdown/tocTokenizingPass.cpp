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
      n.forEachDescendent<model::toc>([](auto& t)
      {
         auto& p = t.template replaceSelf<model::text>();
         auto& l = p.template prependChild<model::text>();
         l.template addChild<model::text>().text = "Table";
         l.template addChild<model::text>().text = "of";
         l.template addChild<model::text>().text = "Contents";
      });
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   virtual void runOnFile(model::file& n)
   {
      n.forEachChild<model::text>([&](auto& p)
      {
         checkParagraph(p);
      });

      replace();
   }

private:
   void checkParagraph(model::text& t)
   {
      if(!t.hasChildren()) return;

      if(t.first().as<model::text>().text == "Table of Contents")
         m_tocs.insert(&t);
   }

   void replace()
   {
      for(auto *pPara : m_tocs)
         replace(*pPara);
   }

   void replace(model::text& p)
   {
      m_pLog->writeLnVerbose("found a TOC");
      p.first().as<model::text>().destroy(); // remove sentinel line
      p.replaceSelf<model::toc>();
   }

   std::set<model::text*> m_tocs;
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "markdown toc untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "markdown toc tokenizing pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTokens; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
