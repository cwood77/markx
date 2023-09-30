#ifndef ___cmn_error___
#define ___cmn_error___

#include "../tcatlib/api.hpp"
#include "debug.hpp"
#include <list>
#include <sstream>
#include <stdexcept>

namespace cmn {

class iErrorContextAttachPoint {
public:
   virtual ~iErrorContextAttachPoint() {}
   virtual void describe(std::stringstream& error) = 0;
};

class iMasterErrorContext : public iErrorContextAttachPoint {
public:
   virtual void attach(iErrorContextAttachPoint& p) = 0;
   virtual void unattach(iErrorContextAttachPoint& p) = 0;
};

// you must provide a tcat singleton factory for this guy somewhere

class masterErrorContext : public iMasterErrorContext {
public:
   virtual void describe(std::stringstream& error)
   {
      for(auto *pPnt : m_points)
         pPnt->describe(error);
   }

   virtual void attach(iErrorContextAttachPoint& p)
   {
      m_points.push_front(&p);
   }

   virtual void unattach(iErrorContextAttachPoint& p)
   {
   }

private:
   std::list<iErrorContextAttachPoint*> m_points;
};

class iErrorContext {
public:
   virtual void describe(std::stringstream& error) = 0;
};

class errorContextAttachPoint : private iErrorContextAttachPoint {
public:
   static errorContextAttachPoint& get()
   {
      static errorContextAttachPoint self;
      return self;
   }

   void add(iErrorContext& e) { m_ctxt.push_front(&e); }
   void remove(iErrorContext& ) { m_ctxt.pop_front(); }

private:
   virtual void describe(std::stringstream& error)
   {
      for(auto *pCtxt : m_ctxt)
         pCtxt->describe(error);
   }

   errorContextAttachPoint()
   {
      tcat::typePtr<iMasterErrorContext> pMaster;
      pMaster->attach(*this);
   }

   std::list<iErrorContext*> m_ctxt;
};

template<class T>
class errorContext : public iErrorContext {
public:
   errorContext(const std::string& msg, const T& v)
   : m_msg(msg)
   , m_pValue(&v)
   {
      errorContextAttachPoint::get().add(*this);
   }

   ~errorContext()
   {
      errorContextAttachPoint::get().remove(*this);
   }

   virtual void describe(std::stringstream& error)
   {
      error << std::endl << "   " << m_msg << " <" << *m_pValue << ">";
   }

private:
   std::string m_msg;
   const T *m_pValue;
};

class error {
public:
   error(const sourceLocation& sl, const std::string& message)
   : m_loc(sl), m_msg(message) {}

   template<class T>
   error& with(const std::string& key, const T& v)
   {
      std::stringstream msg;
      msg << std::endl << "   with <" << key << "> = <" << v << ">";
      m_msg += msg.str();
      return *this;
   }

   double raise()
   {
      std::stringstream msg;
      msg << m_msg << std::endl << "   at " << m_loc.toString();

      tcat::typePtr<iMasterErrorContext> pMaster;
      pMaster->describe(msg);

      throw std::runtime_error(msg.str());
      return 3.14;
   }

private:
   sourceLocation m_loc;
   std::string m_msg;
};

class unimplemented : public error {
public:
   unimplemented(const sourceLocation& sl) : error(sl,"unimplemented") {}
};

class unexpected : public error {
public:
   unexpected(const sourceLocation& sl) : error(sl,"unexpected") {}
};

} // namespace cmn

#endif // ___cmn_error___
