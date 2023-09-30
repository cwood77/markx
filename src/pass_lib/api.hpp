#ifndef ___passlib_api___
#define ___passlib_api___

#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../model/ast.hpp"
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
      pTree->forEachChild<model::file>([&](auto& f){ runOnFile(f); });
   }

protected:
   virtual void runOnFile(model::file& n) = 0;
};

} // namespace pass

#endif // ___passlib_api___
