#include "../tcatlib/api.hpp"
#include "ast.hpp"
#include "service.hpp"
#include "textServices.hpp"

namespace model {
namespace {

class numeric : public iOwnedService, public iNumeric {
public:
   explicit numeric(text& t) : m_node(t) {}

   virtual void release() { delete this; }

   virtual bool isNumeric() const
   {
      const char *pThumb = m_node.text.c_str();
      for(;*pThumb!=0;pThumb++)
         if(!::isdigit(*pThumb) && *pThumb!='.')
            return false;
      return true;
   }

private:
   text& m_node;
};

class nodeServiceProvider : public iNodeServiceProvider {
public:
   virtual void bindNodeIf(node& n)
   {
      auto *pText = n.asIf<text>();
      if(!pText)
         return;

      n.bindService<iNumeric>(*new numeric(*pText));
   }
};

tcatExposeSingletonTypeAs(nodeServiceProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
