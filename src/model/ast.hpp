#ifndef ___model_ast___
#define ___model_ast___

#include "../cmn/error.hpp"
#include "attr.hpp"
#include "service.hpp"
#include <functional>
#include <string>
#include <vector>

namespace model {

class node : public attributable, public nodeServices {
public:
   virtual ~node();

   // ------------------------- tree edits

   void destroy() { reparent(NULL); delete this; }

   template<class T> T& addChild() { T *pN = new T(); return addChild<T>(*pN); }
   template<class T> T& addChild(T& n) { _addChild(n); return n; }
   void _addChild(node& n);

   void reparent(node *pOther);

   void replaceChild(node& old, node *pNoob);

   template<class T> T& replaceSelf() { T *pN = new T(); return replaceSelf<T>(*pN); }
   template<class T> T& replaceSelf(T& n) { _replaceSelf(n); return n; }
   void _replaceSelf(node& noob);

   // ------------------------- child access

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

   template<class T>
   void forEachDescendent(std::function<void(T&)> f)
   {
      for(auto *pChild : m_children)
      {
         auto *pLeaf = dynamic_cast<T*>(pChild);
         if(pLeaf)
            f(*pLeaf);
         pChild->forEachDescendent<T>(f);
      }
   }

   node& first()
   {
      if(m_children.size() == 0)
         throw cmn::error(cdwHere,"wrong number of children")
            .with("actual size",m_children.size())
            .raise();
      return **m_children.begin();
   }

   node& single()
   {
      if(m_children.size() != 1)
         throw cmn::error(cdwHere,"wrong number of children")
            .with("actual size",m_children.size())
            .raise();
      return **m_children.begin();
   }

   bool hasChildren() const { return m_children.size() > 0; }

   node *next(node& n);

   node *nextSibling();

   // ------------------------- type casts

   template<class T>
   bool is(){ return dynamic_cast<T*>(this) != NULL; }

   template<class T>
   T *asIf(){ return dynamic_cast<T*>(this); }

   template<class T>
   T& as(){ return *asIf<T>(); }

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

class header : public node {
public:
   header() : level(0) {}

   size_t level;
   std::string number;
   std::string text;
};

#include "ast.ipp"

} // namespace model

#endif // ___model_ast___
