#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "punctuationProvider.hpp"
#include <cstring>

namespace pass {
namespace {

class recomp : public linePassBase {
public:
   explicit recomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   void runOnLine(model::text& l)
   {
      // do nothing, because unpeeling pass will handle it
   }
};

class decomp : public linePassBase {
public:
   explicit decomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      m_punct.clear();
      auto *pL = n.fetchService<model::iLanguage>();
      if(!pL)
         return;
      auto *pPp = dynamic_cast<iPunctuationProvider*>(pL);
      if(!pPp)
         return;
      pPp->getMidwordPunctuation(m_punct);

      linePassBase::runOnFile(n);
   }

   void runOnLine(model::text& n)
   {
      if(!n.hasChildren())
         return;

      auto *pN = &n.first();
      while(pN)
      {
         auto& t = pN->as<model::text>();
         checkWord(t);
         pN = pN->nextSibling();
      }
   }

private:
   void checkWord(model::text& w)
   {
      for(auto& punct : m_punct)
      {
         if(w.text == punct)
            continue; // don't resplit infinitely

         auto *pThumb = ::strstr(w.text.c_str(),punct.c_str());
         if(!pThumb)
            continue;

         splitWord(w,pThumb,punct);
      }
   }

   void splitWord(model::text& w, const char *pThumb, const std::string& punct)
   {
      std::string w1(w.text.c_str(),pThumb-w.text.c_str());
      std::string w3 = pThumb+punct.length();

      m_pLog->writeLnTemp("splitting word <%s> => {<%s><%s><%s>}",
         w.text.c_str(),
         w1.c_str(),
         punct.c_str(),
         w3.c_str()
      );

      encodeWords(
         w,
         w1,
         punct,
         w3
      );
   }

   void encodeWords(model::text& w, const std::string& w1, const std::string& w2, const std::string& w3)
   {
      if(w1.empty())
         updateNodes(w,w2,w3);
      else if(w3.empty())
         updateNodes(w,w1,w2);
      else
         updateNodes(w,w1,w2,w3);
   }

   model::node& updateNodes(model::text& n, const std::string& w1, const std::string& w2)
   {
      // edit word
      n.text = w1;

      // add glue+punct
      auto& w = n
         .insertSibling<model::glue>()
            .insertSibling<model::text>();
      w.text = w2;

      return w;
   }

   void updateNodes(model::text& n, const std::string& w1, const std::string& w2, const std::string& w3)
   {
      // add glue+rest
      auto& w = updateNodes(n,w1,w2);

      w
         .insertSibling<model::glue>()
         .insertSibling<model::text>().text = w3;
   }

   std::set<std::string> m_punct;
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "punctuation combining pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "punctuation splitting pass"; }
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
