#include "../tcatlib/api.hpp"
#include "api.hpp"

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
   virtual std::string desc() const { return "renumber headers pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual bool isTransform() const { return true; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
