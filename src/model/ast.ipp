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
      m_pParent->replaceChild(*this,NULL);
   if(pOther)
      pOther->_addChild(*this);
}

inline void node::replaceChild(node& old, node *pNoob)
{
   if(pNoob)
      pNoob->reparent(NULL);

   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &old)
      {
         if(pNoob)
         {
            *it = pNoob;
            pNoob->m_pParent = this;
         }
         else
            m_children.erase(it);
         old.m_pParent = NULL;
         return;
      }
   }

   cmn::error(cdwHere,"can't find supposed child").raise();
}

inline void node::_replaceSelf(node& noob)
{
   if(!m_pParent)
      cmn::error(cdwHere,"can't replace myself when I have no parent")
         .raise();
   m_pParent->replaceChild(*this,&noob);
   destroy();
}

inline node *node::next(node& n)
{
   for(auto it=m_children.begin();it!=m_children.end();++it)
   {
      if(*it == &n)
      {
         ++it;
         if(it == m_children.end())
            return NULL; // I am the last child
         else
            return *it;
      }
   }

   throw cmn::error(cdwHere,"can't find supposed child").raise();
}

inline node *node::nextSibling()
{
   if(!m_pParent)
      cmn::error(cdwHere,"can't get siblings if I have no parent")
         .raise();
   return m_pParent->next(*this);
}
