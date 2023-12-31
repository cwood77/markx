#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../pass/api.hpp"
#include "../tcatlib/api.hpp"
#include "compileCommand.hpp"
#include <memory>

namespace {

class command : public compileCommand {
public:
   command() : oStrict(false) {}

   bool oStrict;

protected:
   virtual pass::iPassSchedule& compile(pass::iPassManager& pm, pass::iPassCatalog& pc);
};

class myVerb : public console::globalVerb {
public:
   virtual void dumpDocs(console::iLog& l)
   {
      l.writeLnInfo("--update [-r] [-s] [in-path] [in-pattern]");
      l.writeLnInfo("   Update all known files types");
      l.writeLnInfo("     Use -s (strict) to skip Chris's custom transforms");
   }

protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<command>("--update"));

      v->addOption(
         *new console::boolOption("-r",offsetof(command,oRecursive)));
      v->addOption(
         *new console::boolOption("-s",offsetof(command,oStrict)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePath)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePattern)));

      return v.release();
   }
} gVerb;

pass::iPassSchedule& command::compile(pass::iPassManager& pm, pass::iPassCatalog& pc)
{
   pc.addAllTransforms(pm,oStrict);
   return pm.compileUpdate(pc);
}

} // anonymous namespace
