#include "../cmn/autoPtr.hpp"
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
      m_pLog->writeLnVerbose("loading file: %s",n.path.c_str());
      cmn::sizedAlloc block;
      {
         cmn::autoCFilePtr file(n.path,"r");
         block.realloc(file.calculateFileSizeFromHere());
         ::fread(block.ptr(),block.size(),1,file.fp);
      }
      m_pLog->writeLnVerbose("read %lld byte(s)",block.size());

      n.addChild<model::text>().text = block.ptr();
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kSingleBlock; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "single block saving pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "single block loading pass"; }
   virtual state::type getInput() const { return state::kEmpty; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kSingleBlock; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
