#ifndef ___model_services___
#define ___model_services___

#include "../cmn/error.hpp"
#include "../tcatlib/api.hpp"
#include <map>
#include <set>
#include <string>
#include <typeinfo>

namespace model {

class node;

class iOwnedService {
public:
   virtual ~iOwnedService() {}
   virtual void release() = 0;
};

class iNodeServiceProvider {
public:
   virtual ~iNodeServiceProvider() {}
   virtual void bindNodeIf(node& n) = 0;
};

class nodeServices {
public:
   virtual ~nodeServices()
   {
      for(auto *pSvc : m_owned)
         pSvc->release();
   }

   template<class T>
   void bindService(T& svc)
   {
      m_services[typeid(T).name()] = reinterpret_cast<void*>(&svc);
      auto *pOwned = dynamic_cast<iOwnedService*>(&svc);
      if(pOwned)
         m_owned.insert(pOwned);
   }

   template<class T>
   T *fetchService()
   {
      if(!m_bound)
      {
         tcat::typeSet<iNodeServiceProvider> provs;
         for(size_t i=0;i<provs.size();i++)
            provs[i]->bindNodeIf(dynamic_cast<node&>(*this));
         m_bound = true;
      }

      auto it = m_services.find(typeid(T).name());
      if(it == m_services.end())
         return NULL;
      else
         return reinterpret_cast<T*>(it->second);
   }

   template<class T>
   T& demandService()
   {
      T *pAns = fetchService<T>();
      if(!pAns)
         throw cmn::error(cdwHere,"service not available on node")
            .with("service name",typeid(T).name())
            .raise();
      return *pAns;
   }

protected:
   nodeServices() : m_bound(false) {}

private:
   bool m_bound;
   std::map<std::string,void*> m_services;
   std::set<iOwnedService*> m_owned;
};

} // namespace model

#endif // ___model_services___
