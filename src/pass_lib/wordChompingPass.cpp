#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      n.forEachChild<model::text>([&](auto& t){ runOnText(t); });
   }

private:
   void runOnText(model::text& n)
   {
      const char *pThumb = n.text.c_str();
      const char *pStart = pThumb;
      for(;*pThumb!=0;++pThumb)
      {
         if(*pThumb == ' ' )
         {
            std::string word(pStart,pThumb-pStart);
            m_pLog->writeLnTemp("found word <%s>",word.c_str());
            n.addChild<model::text>().text = word;
            pStart = pThumb + 1;
         }
      }

      std::string word(pStart,pThumb-pStart);
      if(!word.empty())
      {
         m_pLog->writeLnTemp("final word <%s>",word.c_str());
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
   virtual state::type getInput() const { return state::kLines; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kWords; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
