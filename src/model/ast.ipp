inline node::~node()
{
   for(auto *pChild : m_children)
      delete pChild;
}

inline void node::_addChild(node& n)
{
   m_children.push_back(&n);
   n.m_pParent = this;
}
