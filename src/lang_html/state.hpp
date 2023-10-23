#include "../pass/api.hpp"

namespace pass {

namespace state {
   enum htmlState {
      kTranslate = _kNext, // switch language from markdown to html
      kHtmlParticles,      // individual atoms, like <b>
      kHtmlComplete        // target pass state
   };
}

} // namespace pass
