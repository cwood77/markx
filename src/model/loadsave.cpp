#include "../cmn/autoPtr.hpp"
#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "ast.hpp"
#include "loadsave.hpp"
#include "service.hpp"

namespace model {
namespace {

class loader : public iLoader, public iOwnedService {
public:
   explicit loader(file& n) : m_f(n) {}

   virtual bool tryLoad()
   {
      if(m_f.hasChildren())
         return true; // assume already loaded

      cmn::lazyService<console::iLog> pLog;
      auto *pPar = m_f.fetchService<iParser>();
      if(!pPar)
      {
         pLog->writeLnVerbose("no parser is available for file: %s",m_f.path.c_str());
         return false;
      }

      pLog->writeLnVerbose("loading file: %s",m_f.path.c_str());
      cmn::sizedAlloc block;
      {
         cmn::autoCFilePtr file(m_f.path,"r");
         block.realloc(file.calculateFileSizeFromHere());
         ::fread(block.ptr(),block.size(),1,file.fp);
      }
      pLog->writeLnVerbose("using parser %s",pPar->desc().c_str());
      pPar->parse(block.ptr(),m_f);

      return true;
   }

   virtual void release() { delete this; }

private:
   file& m_f;
};

class loaderProvider : public iNodeServiceProvider {
public:
   virtual void bindNodeIf(node& n)
   {
      file *pF = dynamic_cast<file*>(&n);
      if(!pF)
         return;

      n.bindService<iLoader>(*new loader(*pF));
   }
};

tcatExposeTypeAs(loaderProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
