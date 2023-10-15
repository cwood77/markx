#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../cmn/service.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../model/ast.hpp"
#include "../model/lang.hpp"
#include "../pass/api.hpp"
#include "../tcatlib/api.hpp"
#include "finder.hpp"
#include <memory>

namespace {

class command : public console::iCommand {
public:
   command() : oRecursive(false), oInFilePattern("*") {}

   bool oRecursive;
   std::string oInFilePath;
   std::string oInFilePattern;

   virtual void run(console::iLog& l);
};

class myVerb : public console::globalVerb {
public:
   virtual void dumpDocs(console::iLog& l)
   {
      l.writeLnInfo("--update [-r] [in-path] [in-pattern]");
      l.writeLnInfo("   Update all known files types");
   }

protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<command>("--update"));

      v->addOption(
         *new console::boolOption("-r",offsetof(command,oRecursive)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePath)));
      v->addParameter(
         console::stringParameter::optional(offsetof(command,oInFilePattern)));

      return v.release();
   }
} gVerb;

void command::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   l.writeLnDebug("loading config settings (optional)");
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kExeAdjacent,
      "config.sst",
      file::iFileManager::kReadOnly
   ));
   pFile->tie(l);
   l.configure(pFile->dict());

   l.writeLnVerbose("compiling services");
   tcat::typePtr<cmn::serviceManager> svcMan;
   cmn::autoService<console::iLog> _l(*svcMan,l);
   cmn::autoService<sst::dict> _c(*svcMan,pFile->dict(),"config");

   l.writeLnVerbose("arranging arguments");
   std::string basePath = oInFilePath;
   if(basePath.empty())
      basePath = fMan->calculatePath(file::iFileManager::kExeAdjacent,"");
   {
      l.writeLnDebug("recursive: %d",oRecursive?1:0);
      l.writeLnDebug("in: %s",oInFilePath.c_str());
      l.writeLnDebug("patt: %s",oInFilePattern.c_str());
      l.writeLnDebug("basePath: %s",basePath.c_str());
   }

   l.writeLnVerbose("discovering files");
   std::unique_ptr<model::rootNode> pNode(new model::rootNode());
   cmn::autoService<model::rootNode> _n(*svcMan,*pNode);
   {
      std::set<std::string> files;
      finder(oInFilePattern,oRecursive).find(basePath,files);
      for(auto f : files)
      {
         l.writeLnVerbose("considering file %s",f.c_str());
         console::autoIndent _i(l);
         auto& leaf = pNode->addChild(*new model::file(f));
         auto *pL = leaf.fetchService<model::iLanguage>();
         if(pL)
            l.writeLnVerbose("identified language %s",pL->desc().c_str());
         else
         {
            l.writeLnVerbose("no known language; ignoring");
            leaf.destroy();
         }
      }
   }

   l.writeLnVerbose("scheduling passes");
   std::unique_ptr<pass::iPassSchedule> pSched;
   {
      tcat::typePtr<pass::iPassManager> pm;
      tcat::typePtr<pass::iPassCatalog> pc;
      pc->addAllTransforms(*pm);
      pSched.reset(&pm->compile(*pc));
   }

   l.writeLnVerbose("running passes");
   pSched->run();

   l.writeLnVerbose("done");
}

} // anonymous namespace
