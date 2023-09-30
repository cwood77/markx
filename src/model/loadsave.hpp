#ifndef ___model_loadsave_hpp
#define ___model_loadsave_hpp

#include <string>

namespace model {

class iLoader {
public:
   virtual ~iLoader() {}
   virtual bool tryLoad() = 0;
};

class iSaver {
public:
   virtual ~iSaver() {}
   virtual void save() = 0;
};

class file;

class iParser {
public:
   virtual std::string desc() const = 0;
   virtual void parse(const std::string& contents, file& f) const = 0;
};

class iFormatter {
public:
   virtual std::string desc() const = 0;
   virtual void format(file& f, std::string& contents) const = 0;
};

} // namespace model

#endif // ___model_loadsave_hpp
