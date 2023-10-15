#include "../tcatlib/api.hpp"
#include "api.hpp"

namespace pass {
namespace {

class recomp : public linePassBase {
public:
   explicit recomp(const iPassInfo& info) : linePassBase(info) {}

protected:
   void runOnLine(model::text& l)
   {
      std::list<model::glue*> glue;
      l.forEachChild<model::glue>([&](auto& g){ glue.push_back(&g); });

      for(auto *pG : glue)
      {
         auto *pLeft = pG->prevSibling();
         auto *pRight = pG->nextSibling();
         if(!pLeft || !pRight)
            cmn::error(cdwHere,"How is it possible to have glue without left and right?")
               .raise();

         pG->destroy();

         pLeft->as<model::text>().text += pRight->as<model::text>().text;
         pRight->destroy();

         m_pLog->writeLnTemp("resolved glue -> <%s>",pLeft->as<model::text>().text.c_str());
      }
   }
};

class decomp : public linePassBase {
public:
   explicit decomp(const iPassInfo& info) : linePassBase(info)
   {
      // build set of punctuation to peel
      m_punt.insert('.');
      m_punt.insert('?');
      m_punt.insert('!');
   }

protected:
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
      if(w.text.length() == 0)
         return;
      if(w.text.length() == 1)
         return;

      auto c = w.text.c_str()[w.text.length()-1];
      if(m_punt.find(c) == m_punt.end())
         return;

      m_pLog->writeLnTemp("peeling punct off word <%s>",w.text.c_str());

      // edit word
      w.text = std::string(w.text.c_str(),w.text.length()-1);

      // add glue
      auto& g = w.insertSibling<model::glue>();

      // add punct
      auto& p = g.insertSibling<model::text>();
      p.text = std::string(1,c);
   }

   std::set<char> m_punt;
};

class recompInfo : public iPassInfo {
public:
   static const char *kDesc;

   virtual std::string desc() const { return kDesc; }
   virtual state::type getInput() const { return state::kWords; }
   virtual iPass& create() const { return *new recomp(*this); }
};

const char *recompInfo::kDesc = "punctuation unpeeling pass";

tcatExposeSingletonTypeAs(recompInfo,iPassInfo);

class decompInfo : public iDecompositionInfo {
public:
   virtual std::string desc() const { return "punctuation peeling pass"; }
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
