#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <memory>

namespace pass {

class passSchedule : public iPassSchedule {
public:
   virtual ~passSchedule()
   {
      for(auto *p : m_passes)
         delete p;
   }

   virtual void run()
   {
      for(auto *p : m_passes)
      {
         m_pLog->writeLnVerbose("running %s",p->info().desc().c_str());
         p->run();
      }
   }

   std::vector<iPass*> m_passes;

private:
   cmn::lazyService<console::iLog> m_pLog;
};

class passManager : public iPassManager {
public:
   passManager()
   : m_pSched(new passSchedule())
   {
   }

   virtual void add(iPass& p)
   {
      m_pLog->writeLnVerbose("scheduling pass %s",p.info().desc().c_str());
      m_pSched->m_passes.push_back(&p);
   }

   virtual iPassSchedule& compile()
   {
      iPassSchedule *p = m_pSched.release();
      m_pSched.reset(new passSchedule());
      return *p;
   }

private:
   std::unique_ptr<passSchedule> m_pSched;
   cmn::lazyService<console::iLog> m_pLog;
};

tcatExposeTypeAs(passManager,iPassManager);

class passCatalog : public iPassCatalog {
public:
   virtual void addAll(iPassManager& pm)
   {
      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
         pm.add(infos[i]->create());
   }
};

tcatExposeTypeAs(passCatalog,iPassCatalog);

} // namespace pass
