#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <sstream>

namespace pass {
namespace {

class pass : public filePassBase {
public:
   explicit pass(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
   }
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "toc calculation pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual std::set<std::string> getDeps() const
   {
      std::set<std::string> deps;
      deps.insert("renumber headers pass");
      return deps;
   }
   virtual bool isTransform() const { return true; }
   virtual bool isStrictTransform() const { return false; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass