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
      model::text scratch;

      bool first = true;
      while(n.hasChildren())
      {
         if(!first)
            scratch.addChild<model::text>(); // blank line terminates paragraphs
         first = false;
         auto& p = n.first();
         while(p.hasChildren())
         {
            auto& l = p.first().as<model::text>();
            l.reparent(&scratch);
         }
         p.destroy();
      }

      while(scratch.hasChildren())
         scratch.first().reparent(&n);
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      model::text scratch;
      model::text *pPara = NULL;

      while(n.hasChildren())
      {
         auto& l = n.first().as<model::text>();
         if(l.text.empty())
         {
            // blank lines terminate paragraphs
            pPara = NULL;
            l.destroy();
         }
         else
         {
            // open a new paragraph if
            if(pPara == NULL)
            {
               m_pLog->writeLnVerbose(
                  "starting new paragraph for line <%s>",l.text.c_str());
               pPara = &scratch.addChild<model::text>();
            }
            l.reparent(pPara);
         }
      }

      if(n.hasChildren())
         cmn::unexpected(cdwHere).raise();

      while(scratch.hasChildren())
         scratch.first().reparent(&n);
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kParagraphs; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "paragraph unclumping pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "paragraph clumping pass"; }
   virtual state::type getInput() const { return state::kLines; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kParagraphs; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
