#include "../model/lang.hpp"
#include "../model/textServices.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"
#include <sstream>

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "html"; }

   virtual void runOnFile(model::file& n)
   {
      std::set<model::header*> s;
      n.forEachDescendent<model::header>([&](auto& h){ s.insert(&h); });

      for(auto *pH : s)
      {
         auto& g1 = pH->insertSibling<model::glue>();

         auto& num = g1.insertSibling<model::text>();
         num.text = pH->number;

         auto& text = num.insertSibling<model::text>();
         text.text = pH->text;

         auto& g2 = text.insertSibling<model::glue>();

         auto& term = g2.insertSibling<model::text>();
         term.text = createTag(*pH,/*start*/false);

         pH->replaceSelf<model::text>().text = createTag(*pH,/*start*/true);

         if(num.text.empty())
            num.destroy();

         if(text.text.empty())
            text.destroy();
      }
   }

private:
   std::string createTag(model::header& h, bool start)
   {
      std::stringstream stream;
      stream << "<" << (start ? "" : "/") << "h" << getLevel(h) << ">";
      return stream.str();
   }

   size_t getLevel(model::header& h)
   {
      return h.level > 3 ? 3 : h.level; // TODO expose this!
   }
};

class decomp : public filePassBase {
public:
   explicit decomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "html"; }

   void runOnFile(model::file& n)
   {
      cmn::unimplemented(cdwHere).raise();
   }
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "html header untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "html header tokenizing pass"; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTokens; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
