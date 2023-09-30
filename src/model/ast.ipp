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

inline void node::reparent(node *pOther)
{
   if(m_pParent)
      m_pParent->detachChild(*this);
   if(pOther)
      pOther->_addChild(*this);
}

inline void node::detachChild(node& n)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &n)
      {
         m_children.erase(it);
         n.m_pParent = NULL;
         return;
      }
   }

   cmn::error(cdwHere,"can't find supposed child").raise();
}
