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
   }
};

class decomp : public wordPassBase {
public:
   explicit decomp(const iPassInfo& info) : wordPassBase(info) {}

protected:
   void runOnWord(model::text& n)
   {
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "punctuation unpeeling pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "punctuation peeling pass"; }
   virtual state::type getInput() const { return state::kParagraphs; }
   virtual std::set<std::string> getDeps() const
   {
      std::set<std::string> deps;
      deps.insert("word chomping pass");
      return deps;
   }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kWords; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
