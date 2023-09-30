#include "../cmn/error.hpp"
#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <map>
#include <memory>
#include <set>

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

   void schedule(iPassInfo& p)
   {
      m_pLog->writeLnVerbose("scheduling %s",p.desc().c_str());
      m_passes.push_back(&p.create());
   }

private:
   std::vector<iPass*> m_passes;
   cmn::lazyService<console::iLog> m_pLog;
};

class stateManager {
public:
   stateManager(iPassCatalog& c, passSchedule& s)
   : m_catalog(c)
   , m_sched(s)
   , m_currentState(state::kEmpty)
   {}

   void decomposeTo(state::type dest)
   {
      if(dest < m_currentState)
         cmn::unimplemented(cdwHere).raise();

      for(;m_currentState!=dest;m_currentState++)
      {
         auto decomps = m_catalog.demand(m_currentState,m_currentState+1);
         for(auto *pD : decomps)
         {
            m_sched.schedule(*pD);
            m_inversions.push_front(pD->inverseGuid());
         }
      }
   }

   void recompose()
   {
      for(auto& guid : m_inversions)
      {
         auto& r = m_catalog.demand(guid);
         m_sched.schedule(r);
      }
   }

private:
   iPassCatalog& m_catalog;
   passSchedule& m_sched;
   state::type m_currentState;
   std::list<std::string> m_inversions;
};

class passManager : public iPassManager {
public:
   virtual void add(iPassInfo& p)
   {
      m_pLog->writeLnVerbose("seeding pass %s",p.desc().c_str());
      m_pInfos[p.getInput()].insert(&p);
   }

   virtual iPassSchedule& compile(iPassCatalog& c)
   {
      std::unique_ptr<passSchedule> pSched(new passSchedule());
      stateManager sm(c,*pSched);

      for(auto it=m_pInfos.begin();it!=m_pInfos.end();++it)
      {
         sm.decomposeTo(it->first);
         for(auto jit=it->second.begin();jit!=it->second.end();++jit)
            pSched->schedule(**jit);
      }

      sm.recompose();

      return *pSched.release();
   }

private:
   std::map<state::type,std::set<iPassInfo*> > m_pInfos;
   cmn::lazyService<console::iLog> m_pLog;
};

tcatExposeTypeAs(passManager,iPassManager);

class passCatalog : public iPassCatalog {
public:
   virtual void addAllTransforms(iPassManager& pm)
   {
      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
         if(infos[i]->isTransform())
            pm.add(*infos[i]);
   }

   virtual std::list<iDecompositionInfo*> demand(state::type in, state::type out)
   {
      std::list<iDecompositionInfo*> ans;

      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
      {
         auto *pD = dynamic_cast<iDecompositionInfo*>(infos[i]);
         if(pD && pD->getInput() == in && pD->getOutput() == out)
            ans.push_back(pD);
      }

      if(ans.size())
         return ans;

      throw cmn::error(cdwHere,"no decomposition found matching requirements")
         .with("in",in)
         .with("out",out)
         .raise();
   }

   virtual iPassInfo& demand(const std::string& guid)
   {
      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
         if(infos[i]->guid() == guid)
            return *infos[i];

      throw cmn::error(cdwHere,"no pass with guid")
         .with("guid",guid)
         .raise();
   }
};

tcatExposeTypeAs(passCatalog,iPassCatalog);

} // namespace pass
