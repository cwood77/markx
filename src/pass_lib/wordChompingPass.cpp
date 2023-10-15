#include "../model/textServices.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class recomp : public linePassBase {
public:
   explicit recomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   void runOnLine(model::text& l)
   {
      l.demandService<model::iPhrase>().combineAndDestroyAll();
   }
};

class decomp : public linePassBase {
public:
   explicit decomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   void runOnLine(model::text& n)
   {
      const char *pThumb = n.text.c_str();
      const char *pStart = pThumb;
      for(;*pThumb!=0;++pThumb)
      {
         if(*pThumb == ' ')
         {
            std::string word(pStart,pThumb-pStart);
            m_pLog->writeLnDebug("found word <%s>",word.c_str());
            n.addChild<model::text>().text = word;
            pStart = pThumb + 1;
         }
      }

      std::string word(pStart,pThumb-pStart);
      if(!word.empty())
      {
         m_pLog->writeLnDebug("final word <%s>",word.c_str());
         n.addChild<model::text>().text = word;
      }
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "word unchomping pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "word chomping pass"; }
   virtual state::type getInput() const { return state::kParagraphs; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kWords; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
