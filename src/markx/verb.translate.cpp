#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../model/ast.hpp"
#include "../model/lang.hpp"
#include "../pass/api.hpp"
#include "../tcatlib/api.hpp"
#include "compileCommand.hpp"
#include <memory>

namespace {

class command : public compileCommand {
public:
   std::string oDest;

protected:
   virtual void considerFile(console::iLog& l, model::file& f);
   virtual pass::iPassSchedule& compile(pass::iPassManager& pm, pass::iPassCatalog& pc);
};

class myVerb : public console::globalVerb {
public:
   virtual void dumpDocs(console::iLog& l)
   {
      l.writeLnInfo("--translate [-r] <dest> [in-path] [in-pattern]");
      l.writeLnInfo("   Translate all known files types to type <dest>");
   }

protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<command>("--translate"));

      v->addOption(
         *new console::boolOption("-r",offsetof(command,oRecursive)));
      v->addParameter(
         console::stringParameter::required(offsetof(command,oDest)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePath)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePattern)));

      return v.release();
   }
} gVerb;

void command::considerFile(console::iLog& l, model::file& f)
{
   auto *pL = f.fetchService<model::iLanguage>();
   if(pL && pL->desc() == oDest)
   {
      l.writeLnVerbose("identified language %s matches target language, so skipping",pL->desc().c_str());
      f.destroy();
   }
   else
   {
      compileCommand::considerFile(l,f);
   }
}

pass::iPassSchedule& command::compile(pass::iPassManager& pm, pass::iPassCatalog& pc)
{
   pc.translateTo(pm,oDest);
   return pm.compileUpdate(pc);
}

} // anonymous namespace
