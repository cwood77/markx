#include "../cmn/error.hpp"
#include "../cmn/service.hpp"
#include "../cmn/win32.hpp"
#include "../console/log.hpp"
#include "finder.hpp"

void finder::find(const std::string& basePath, std::set<std::string>& paths)
{
   cmn::lazyService<console::iLog> pLog;

   WIN32_FIND_DATA fData;
   cmn::autoFindHandle hFind;
   hFind.h = ::FindFirstFileA(combine(basePath,m_pattern).c_str(),&fData);
   if(hFind.h == INVALID_HANDLE_VALUE)
      cmn::error(cdwHere,"bad path in search")
         .with("basePath",basePath)
         .raise();

   do
   {
      if(std::string(".") == fData.cFileName)
         continue;
      if(std::string("..") == fData.cFileName)
         continue;

      std::string fullPath = combine(basePath,fData.cFileName);
      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         if(m_recursive)
         {
            pLog->writeLnVerbose("found folder %s",fullPath.c_str());
            console::autoIndent _i(*pLog);
            find(fullPath,paths);
         }
      }
      else
      {
         pLog->writeLnVerbose("found file %s",fullPath.c_str());
         paths.insert(fullPath);
      }

   } while(::FindNextFileA(hFind.h,&fData));
}

std::string finder::combine(const std::string& basePath, const std::string& leaf) const
{
   std::string ans = basePath;
   if(ans.c_str()[ans.length()-1] != '\\')
      ans += "\\";
   ans += leaf;
   return ans;
}
