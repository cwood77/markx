#ifndef ___model_ast___
#define ___model_ast___

#include "attr.hpp"
#include "service.hpp"
#include <functional>
#include <string>
#include <vector>

namespace model {

class node : public attributable, public nodeServices {
public:
   virtual ~node();

   template<class T> T& addChild() { T *pN = new T(); return addChild<T>(*pN); }
   template<class T> T& addChild(T& n) { _addChild(n); return n; }
   void _addChild(node& n);

   template<class T>
   void forEachChild(std::function<void(T&)> f)
   {
      for(auto *pChild : m_children)
      {
         auto *pLeaf = dynamic_cast<T*>(pChild);
         if(pLeaf)
            f(*pLeaf);
      }
   }

   bool hasChildren() const { return m_children.size() > 0; }

   template<class T>
   bool is(){ return dynamic_cast<T*>(this) != NULL; }

   template<class T>
   T& as(){ return *dynamic_cast<T*>(this); }

protected:
   node() : m_pParent(NULL) {}

private:
   std::vector<node*> m_children;
   node *m_pParent;
};

class rootNode : public node {
};

class file : public node {
public:
   explicit file(const std::string& path) : path(path) {}

   std::string path;
};

class text : public node {
public:
   std::string text;
};

#include "ast.ipp"

} // namespace model

#endif // ___model_ast___
