#include "../tcatlib/api.hpp"
#include "ast.hpp"
#include "service.hpp"
#include "textServices.hpp"

namespace model {
namespace {

class phrase : public iOwnedService, public iPhrase {
public:
   explicit phrase(text& t) : m_node(t), m_bGlue(false) {}

   virtual void release() { delete this; }

   virtual void reset() { m_node.text = ""; m_bGlue = false; }

   virtual void combine(node& n, bool destroy)
   {
      auto& t = n.as<text>();

      if(n.is<glue>())
         m_bGlue = true;
      else
      {
         if(t.text.empty())
            addText(" ");
         else
         {
            addSpaceIfNeedsIt();
            addText(t.text);
         }
      }

      if(destroy)
         t.destroy();
   }

   virtual void combineAndDestroyAll()
   {
      reset();
      while(m_node.hasChildren())
         combine(m_node.first(),/*destroy*/true);
   }

private:
   void addSpaceIfNeedsIt()
   {
      if(m_node.text.length() == 0)
         return;
      if(m_node.text.c_str()[m_node.text.length()-1] != ' ')
         addText(" ");
   }

   void addText(const std::string& t)
   {
      if(m_bGlue && t == " ")
      {
         // ignore it, because that's that glue means
         m_bGlue = false;
         return;
      }
      m_node.text += t;
   }

   text& m_node;
   bool m_bGlue;
};

class nodeServiceProvider : public iNodeServiceProvider {
public:
   virtual void bindNodeIf(node& n)
   {
      auto *pText = n.asIf<text>();
      if(!pText)
         return;

      n.bindService<iPhrase>(*new phrase(*pText));
   }
};

tcatExposeSingletonTypeAs(nodeServiceProvider,iNodeServiceProvider);

} // anonymous namespace
} // namespace model
