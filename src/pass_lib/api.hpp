#ifndef ___passlib_api___
#define ___passlib_api___

#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../model/ast.hpp"
#include "../model/lang.hpp"
#include "../pass/api.hpp"

namespace pass {

class passBase : public iPass {
public:
   explicit passBase(const iPassInfo& info) : m_info(info) {}
   virtual const iPassInfo& info() { return m_info; }

protected:
   cmn::lazyService<console::iLog> m_pLog;

private:
   const iPassInfo& m_info;
};

class filePassBase : public passBase {
public:
   explicit filePassBase(const iPassInfo& info) : passBase(info) {}

   virtual void run()
   {
      cmn::lazyService<model::rootNode> pTree;
      pTree->forEachChild<model::file>([&](auto& f){ _runOnFile(f); });
   }

protected:
   virtual std::string getReqLang() const { return ""; } // empty means no requirement

   virtual void runOnFile(model::file& n) = 0;

private:
   void _runOnFile(model::file& n)
   {
      if(isAlienLanguage(n))
         return;
      runOnFile(n);
   }

   bool isAlienLanguage(model::file& n)
   {
      auto l = getReqLang();
      if(!l.empty() && n.demandService<model::iLanguage>().desc() != l)
      {
         m_pLog->writeLnVerbose("skipping file in alien language");
         return true;
      }
      return false;
   }
};

class linePassBase : public filePassBase {
public:
   explicit linePassBase(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      n.forEachChild<model::text>([&](auto& p)
      {
         p.template forEachChild<model::text>([&](auto& l)
         {
            runOnLine(l);
         });
      });
   }

   virtual void runOnLine(model::text& n) = 0;
};

} // namespace pass

#endif // ___passlib_api___
