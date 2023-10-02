#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../pass/api.hpp"
#include "../tcatlib/api.hpp"

namespace {

console::helpVerbs gHelp([](console::iLog& l)
{
   l.writeLnInfo("");
   l.writeLnInfo("a generic text compiler for mark-down/mark-up languages");
   l.writeLnInfo("");
   l.writeLnInfo("passes");
   tcat::typePtr<pass::iPassCatalog>()->dump(l);
   l.writeLnInfo("");
   l.writeLnInfo("verbs and switches");
});

} // anonymous namespace
