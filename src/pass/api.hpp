#ifndef ___pass_api___
#define ___pass_api___

#include <functional>
#include <list>
#include <set>
#include <string>

namespace pass {

namespace state {
   enum state {
      kEmpty,
      kSingleBlock,
      kLines,
      kParagraphs,
      kWords,
      kTokens
   };
   typedef size_t type;
}

class iPass;

class iPassInfo {
public:
   virtual ~iPassInfo() {}
   virtual std::string guid() const { return desc(); }
   virtual std::string desc() const = 0;
   virtual state::type getInput() const = 0;
   virtual std::set<std::string> getDeps() const { return std::set<std::string>(); }
   virtual bool isTransform() const { return false; }
   virtual bool isStrictTransform() const { return false; }
   virtual iPass& create() const = 0;
};

class iDecompositionInfo : public iPassInfo {
public:
   virtual state::type getOutput() const = 0;
   virtual std::string inverseGuid() const = 0;
};

class iPass {
public:
   virtual ~iPass() {}
   virtual const iPassInfo& info() = 0;
   virtual void run() = 0;
};

class iPassSchedule {
public:
   virtual ~iPassSchedule() {}
   virtual void run() = 0;
};

class iPassCatalog;

class iPassManager {
public:
   virtual ~iPassManager() {}
   virtual void add(iPassInfo& p) = 0;
   virtual iPassSchedule& compile(iPassCatalog& c) = 0;
};

class iPassCatalog {
public:
   virtual ~iPassCatalog() {}
   virtual void dump(console::iLog& l) = 0;
   virtual void addAllTransforms(iPassManager& pm, bool strict) = 0;
   virtual std::list<iDecompositionInfo*> demand(state::type in, state::type out) = 0;
   virtual iPassInfo& demand(const std::string& guid) = 0;
};

} // namespace pass

#endif // ___pass_api___
