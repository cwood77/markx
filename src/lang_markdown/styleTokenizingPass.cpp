#include "../model/lang.hpp"
#include "../pass_lib/api.hpp"
#include "../tcatlib/api.hpp"

namespace pass {
namespace {

class recomp : public filePassBase {
public:
   explicit recomp(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   virtual void runOnFile(model::file& n)
   {
      cmn::unimplemented(cdwHere).raise();
   }
};

class decomp : public wordPassBase {
public:
   explicit decomp(const iPassInfo& info) : wordPassBase(info) {}

protected:
   virtual std::string getReqLang() const { return "markdown"; }

   void runOnWord(model::text& w)
   {
      if(w.text == "*")
         swapNode(w,model::style::kBold);
      else if(w.text == "_")
         swapNode(w,model::style::kItalic);
   }

private:
   void swapNode(model::node& n, model::style::kinds k)
   {
      bool isStart = (m_currentStyles.find(k) == m_currentStyles.end());

      if(isStart)
         m_currentStyles.insert(k);
      else
         m_currentStyles.erase(k);

      auto& s = n.replaceSelf<model::style>();
      s.kind = k;
      s.start = isStart;
   }

   std::set<model::style::kinds> m_currentStyles;
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kTranslate; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "markdown style untokenizing pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "markdown style tokenizing pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual iPass& create() const { return *new decomp(*this); }
   virtual state::type getOutput() const { return state::kTranslate; }
   virtual std::string inverseGuid() const { return recompInfo::kDesc; }
};

tcatExposeSingletonTypeAs(decompInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
