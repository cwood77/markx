#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../model/ast.hpp"
#include "../model/loadsave.hpp"
#include "../pass/api.hpp"
#include "../tcatlib/api.hpp"

namespace pass {
namespace {

class pass : public iPass {
public:
   explicit pass(const iPassInfo& info) : m_info(info) {}
   virtual const iPassInfo& info() { return m_info; }

   virtual void run()
   {
      cmn::lazyService<model::rootNode> pTree;
      pTree->forEachChild<model::file>([&](auto& f){ run(f); });
   }

private:
   void run(model::file& n)
   {
      if(n.demandService<model::iLoader>().tryLoad())
         return;
   }

   const iPassInfo& m_info;
   cmn::lazyService<console::iLog> m_pLog;
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "renumber headers pass"; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
