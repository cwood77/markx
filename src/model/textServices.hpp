#ifndef ___model_textServices___
#define ___model_textServices___

namespace model {

class node;

class iNumeric {
public:
   virtual bool isNumeric() const = 0;
};

class iPhrase {
public:
   virtual void reset() = 0;
   virtual void combine(node& n, bool destroy) = 0;

   virtual void combineAndDestroyAll() = 0;
};

class iSectionRef {
public:
   virtual void expand() = 0;
};

} // namespace model

#endif // ___model_textServices___
