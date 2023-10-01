#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <cstring>

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      std::stringstream stream;

      while(n.hasChildren())
      {
         auto& l = n.first().as<model::text>();
         stream << l.text << std::endl;
         l.destroy();
      }

      n.addChild<model::text>().text = stream.str();
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      auto& block = n.single().as<model::text>();

      const char *pThumb = block.text.c_str();
      const char *pStart = pThumb;
      for(;*pThumb!=0;++pThumb)
      {
         size_t s = 0;
         if(*pThumb == '\n')
            s = 1;
         else if(::strncmp(pThumb,"\r\n",2)==0)
            s = 2;

         if(s)
         {
            std::string line(pStart,pThumb-pStart);
            m_pLog->writeLnTemp("found line <%s>",line.c_str());
            n.addChild<model::text>().text = line;
            pStart = pThumb + s;
            pThumb += (s-1);
         }
      }

      std::string line(pStart,pThumb-pStart);
      if(!line.empty())
      {
         m_pLog->writeLnTemp("final line <%s>",line.c_str());
         n.addChild<model::text>().text = line;
      }

      block.destroy();
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kLines; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "line unchomping pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "line chomping pass"; }
   virtual state::type getInput() const { return state::kSingleBlock; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kLines; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
