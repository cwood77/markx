#ifndef ___model_lang___
#define ___model_lang___

#include <string>

namespace model {

class iLanguage {
public:
   virtual std::string desc() = 0;
};

} // namespace model

#endif // ___model_lang___
