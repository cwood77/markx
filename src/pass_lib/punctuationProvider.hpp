#ifndef ___passlib_punctuationProvider___
#define ___passlib_punctuationProvider___

#include <set>
#include <string>

namespace pass {

class iPunctuationProvider {
public:
   virtual void getMidwordPunctuation(std::set<std::string>& p) const = 0;
};

} // namespace pass

#endif // ___passlib_punctuationProvider___
