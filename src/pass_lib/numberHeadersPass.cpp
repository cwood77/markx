#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <sstream>

namespace pass {
namespace {

class numberer {
public:
   numberer() : m_ignoredLevels(0), m_currentIndent(0) {}

   void noNumbers(size_t level) { if(level > m_ignoredLevels) m_ignoredLevels = level; }

   std::string getNext(size_t _level)
   {
      auto level = _level - m_ignoredLevels;

      if(m_currentIndent > level)
         unindent(level);

      if(m_currentIndent == level)
         increment();

      if(m_currentIndent < level)
         indent(level);

      return format();
   }

private:
   void unindent(size_t level)
   {
      for(size_t i=m_currentIndent;i>level;i--)
         m_next.erase(i);
      m_currentIndent = level;
   }

   void indent(size_t level)
   {
      for(size_t i=m_currentIndent+1;i<=level;i++)
         if(m_next[i] == 0)
            m_next[i] = 1;
      m_currentIndent = level;
   }

   std::string format()
   {
      std::stringstream stream;
      for(size_t i=1;i<=m_currentIndent;i++)
         stream << m_next[i] << ".";
      return stream.str();
   }

   void increment()
   {
      m_next[m_currentIndent]++;
   }

   size_t m_ignoredLevels;
   size_t m_currentIndent;
   std::map<size_t,size_t> m_next;
};

class pass : public filePassBase {
public:
   explicit pass(const iPassInfo& info) : filePassBase(info) {}

protected:
   virtual void runOnFile(model::file& n)
   {
      numberer N;
      n.forEachDescendent<model::header>([&](auto& h)
      {
         if(h.number.empty())
            N.noNumbers(h.level);
         else
            h.number = N.getNext(h.level);
         m_pLog->writeLnVerbose("assigned number <%s> to header <%s>",
            h.number.c_str(),
            h.text.c_str());
      });
   }
};

class passInfo : public iPassInfo {
public:
   virtual std::string desc() const { return "renumber headers pass"; }
   virtual state::type getInput() const { return state::kTokens; }
   virtual bool isTransform() const { return true; }
   virtual bool isStrictTransform() const { return true; }
   virtual iPass& create() const { return *new pass(*this); }
};

tcatExposeSingletonTypeAs(passInfo,iPassInfo);

} // anonymous namespace
} // namespace pass
