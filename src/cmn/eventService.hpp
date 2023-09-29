#ifndef ___cmn_eventService___
#define ___cmn_eventService___

#include "../cmn/service.hpp"
#include "../tcatlib/api.hpp"
#include <functional>
#include <set>

namespace cmn {

template<class T>
class eventService {
public:
   virtual ~eventService() {}
   void installObserver(T& obs) { m_obs.insert(&obs); }
   void uninstallObserver(T& obs) { m_obs.erase(&obs); }

protected:
   void raise(std::function<void(T&)> f)
   {
      std::set<T*> copy = m_obs;
      for(auto *pObs : copy)
         f(*pObs);
   }

private:
   std::set<T*> m_obs;
};

template<class T>
class autoEventObserver {
public:
   explicit autoEventObserver(T& obs) : m_obs(obs)
   {
      tcat::typePtr<cmn::serviceManager> svcMan;
      svcMan->demand<typename T::eventService>().installObserver(m_obs);
   }

   ~autoEventObserver()
   {
      tcat::typePtr<cmn::serviceManager> svcMan;
      svcMan->demand<typename T::eventService>().uninstallObserver(m_obs);
   }

private:
   T& m_obs;
};

} // namespace cmn

#endif // ___cmn_eventService___
