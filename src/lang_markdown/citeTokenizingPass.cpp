#include "../model/lang.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"
#include <cstring>
#include <sstream>

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   virtual void runOnFile(model::file& n)
   {
      std::list<model::text*> nodes;
      n.forEachDescendent<model::cite>([&](auto& c)
      {
         nodes.push_back(&c.parent().template as<model::text>());
      });

      for(auto *pNode : nodes)
         combine(*pNode);
   }

private:
   void combine(model::text& w)
   {
      std::stringstream s;

      while(w.hasChildren())
      {
         auto& c = w.first().as<model::text>();

         if(c.is<model::cite>())
            s << "[" << c.text << "]";
         else
            s << c.text;

         c.destroy();
      }

      m_pLog->writeLnTemp("replacing node text <%s> with <%s>",w.text.c_str(),s.str().c_str());
      w.text = s.str();
   }
};

class decomp : public wordPassBase {
public:
   explicit decomp(const iPassInfo& info) : wordPassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   void runOnWord(model::text& w)
   {
      m_pLog->writeLnDebug("checking word <%s>",w.text.c_str());

      const char *pThumb = w.text.c_str();
      bool hasBrackets = ::strchr(pThumb,'[');
      if(!hasBrackets)
         return;
      m_pLog->writeLnDebug("hit!? -> <%s>",pThumb);

      const char *pStart = pThumb;
      bool inCite = false;
      for(;*pThumb!=0;++pThumb)
      {
         if(*pThumb == '[' && !inCite)
         {
            closeWord<model::text>(w,pStart,pThumb);
            inCite = true;
         }
         else if(*pThumb == ']' && inCite)
         {
            auto& c = closeWord<model::cite>(w,pStart,pThumb);
            inCite = false;
            m_pLog->writeLnVerbose("found citation <%s> in word <%s>",c.text.c_str(),w.text.c_str());
         }
      }

      closeWord<model::text>(w,pStart,pThumb);
   }

private:
   template<class T>
   T& closeWord(model::text& w, const char*& pStart, const char*& pThumb)
   {
      T& n = w.addChild<T>();
      n.text = std::string(pStart,pThumb-pStart);
      pStart = pThumb+1;
      return n;
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "markdown cite untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "markdown cite tokenizing pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTokens; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
