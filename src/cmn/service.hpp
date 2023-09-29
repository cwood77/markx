#ifndef ___cmn_service___
#define ___cmn_service___

#include "../tcatlib/api.hpp"
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace cmn {

// instantiation of this guy is left to you, which controls the lifetime/scope

class serviceManager {
public:
   template<class T> void attach(T& intf, const std::string& id = "")
   {
      void*& pPtr = m_map[makeId<T>(id)];
      if(pPtr)
         throw std::runtime_error("duplicate attach");
      pPtr = &intf;
   }

   template<class T> void detach(T& intf, const std::string& id = "")
   {
      void*& pPtr = m_map[makeId<T>(id)];
      if(!pPtr)
         throw std::runtime_error("service not found");
      pPtr = NULL;
   }

   template<class T> T& demand(const std::string& id = "")
   {
      void *pPtr = m_map[makeId<T>(id)];
      if(!pPtr)
         throw std::runtime_error("service not found");
      return *reinterpret_cast<T*>(pPtr);
   }

private:
   template<class T> std::string makeId(const std::string& id)
   {
      return id + ":" + typeid(T).name();
   }

   std::map<std::string,void*> m_map;
};

template<class T>
class autoService {
public:
   autoService(serviceManager& man, T& svc, const std::string& id = "")
   : m_pSvc(&svc)
   , m_id(id)
   , m_pSMan(&man)
   {
      m_pSMan->attach<T>(*m_pSvc,m_id);
   }

   ~autoService()
   {
      m_pSMan->detach<T>(*m_pSvc,m_id);
   }

private:
   T *m_pSvc;
   std::string m_id;
   serviceManager *m_pSMan;
};

template<class T>
class autoTCatService {
public:
   autoTCatService(T& svc, const std::string& id = "")
   : m_pSvc(&svc)
   , m_id(id)
   {
      m_svcMan->attach<T>(*m_pSvc,m_id);
   }

   ~autoTCatService()
   {
      m_svcMan->detach<T>(*m_pSvc,m_id);
   }

private:
   T *m_pSvc;
   std::string m_id;
   tcat::typePtr<serviceManager> m_svcMan;
};

template<class T>
class lazyService {
public:
   lazyService(const std::string& id = "", serviceManager *pMan = NULL)
   : m_pSvc(NULL), m_id(id), m_pSMan(pMan) {}

   T *operator->()             { return &get(); }
   const T *operator->() const { return &get(); }

   T& get()             { fetchIf(); return *m_pSvc; }
   const T& get() const { fetchIf(); return *m_pSvc; }

private:
   void fetchIf() const
   {
      if(m_pSvc) return;

      tcat::typePtr<serviceManager> svcMan;
      serviceManager *pSm = m_pSMan ? m_pSMan : &*svcMan;
      m_pSvc = &pSm->demand<T>(m_id);
   }

   mutable T *m_pSvc;
   std::string m_id;
   mutable serviceManager *m_pSMan;
};

} // namespace cmn

#endif // ___cmn_service___
