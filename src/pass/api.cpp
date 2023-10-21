#include "../cmn/error.hpp"
#include "../cmn/misc.hpp"
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
         console::autoIndent _(*m_pLog);
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

class passScheduler {
public:
   explicit passScheduler(passSchedule& s) : m_sched(s) {}

   void addToBatch(iPassInfo& p)
   {
      m_batchGuids.insert(p.guid());
      m_batch.push_back(&p);
   }

   void scheduleBatch() { scheduleBatch([](auto&){}); }

   void scheduleBatch(std::function<void(iPassInfo&)> onSched)
   {
      cmn::runawayLoopCheck loop;

      while(m_batch.size())
      {
         loop.sanityCheck();
         for(auto it=m_batch.begin();it!=m_batch.end();++it)
         {
            if(satisfied(**it))
            {
               onSched(**it);
               schedule(**it);
               m_batch.erase(it);
               break; // restart
            }
            else
               m_pLog->writeLnDebug("deferring schedule of %s because of dep",(*it)->desc().c_str());
         }
      }
   }

private:
   bool satisfied(iPassInfo& p)
   {
      bool satisfied = true;
      auto deps = p.getDeps();
      for(auto dep : deps)
      {
         if(m_scheduled.find(dep) != m_scheduled.end()) // satisfied
            continue;
         if(m_batchGuids.find(dep) != m_batchGuids.end()) // not satisfied, but satisfiable
            satisfied = false;
         else
            cmn::error(cdwHere,"pass dependencies are unsatisifable")
               .with("pass desc",p.desc())
               .with("unsatisfiable guid",dep)
               .raise();
      }
      return satisfied;
   }

   void schedule(iPassInfo& p)
   {
      m_sched.schedule(p);
      m_scheduled.insert(p.guid());
   }

   passSchedule& m_sched;
   std::set<std::string> m_scheduled;
   std::set<std::string> m_batchGuids;
   std::list<iPassInfo*> m_batch;
   cmn::lazyService<console::iLog> m_pLog;
};

class stateManager {
public:
   stateManager(iPassCatalog& c, passScheduler& s)
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
            m_sched.addToBatch(*pD);
         m_sched.scheduleBatch([&](auto &d)
         {
            m_inversions.push_front(dynamic_cast<iDecompositionInfo&>(d).inverseGuid());
         });
      }
   }

   void recompose()
   {
      for(auto& guid : m_inversions)
      {
         auto& r = m_catalog.demand(guid);
         m_sched.addToBatch(r);
      }
      m_sched.scheduleBatch();
   }

private:
   iPassCatalog& m_catalog;
   passScheduler& m_sched;
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
      passScheduler scheduler(*pSched);
      stateManager sm(c,scheduler);

      for(auto it=m_pInfos.begin();it!=m_pInfos.end();++it)
      {
         sm.decomposeTo(it->first);
         for(auto jit=it->second.begin();jit!=it->second.end();++jit)
            scheduler.addToBatch(**jit);
         scheduler.scheduleBatch();
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
   virtual void dump(console::iLog& l)
   {
      {
      console::autoIndent _i(l);
      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
      {
         std::stringstream msg;
         msg << infos[i]->desc();
         if(infos[i]->isStrictTransform())
            msg << " [T]";
         else if(infos[i]->isTransform())
            msg << " [T*]";
         if(dynamic_cast<iDecompositionInfo*>(infos[i]))
            msg << " [D]";
         l.writeLnInfo(msg.str());
      }
      }
      l.writeLnInfo("T  = strict transform pass");
      l.writeLnInfo("T* = non-strict transform pass");
      l.writeLnInfo("D  = decomposition pass");
   }

   virtual void addAllTransforms(iPassManager& pm, bool strict)
   {
      tcat::typeSet<iPassInfo> infos;
      for(size_t i=0;i<infos.size();i++)
         if(
            (strict && infos[i]->isStrictTransform()) ||
            (!strict && infos[i]->isTransform())
         )
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
