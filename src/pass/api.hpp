#ifndef ___pass_api___
#define ___pass_api___

#include <string>

namespace pass {

class iPass;

class iPassInfo {
public:
   virtual ~iPassInfo() {}
   virtual std::string desc() const = 0;
   virtual iPass& create() const = 0;
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

class iPassManager {
public:
   virtual ~iPassManager() {}
   virtual void add(iPass& p) = 0;
   virtual iPassSchedule& compile() = 0;
};

class iPassCatalog {
public:
   virtual ~iPassCatalog() {}
   virtual void addAll(iPassManager& pm) = 0;
};

} // namespace pass

#endif // ___pass_api___
