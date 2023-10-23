#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"
#include "state.hpp"

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

class passInfo : public iTranslationInfo {
public:
   virtual std::string desc() const { return "HTML target pass"; }
   virtual state::type getInput() const { return state::kTranslate; }
   virtual iPass& create() const { return *new pass(*this); }
   virtual std::string destination() const { return "html"; }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
