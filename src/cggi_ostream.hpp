//%start-copyright%
/*
\copyright
    CPPG, a C++ cgi library.
    Copyright (C) 2021 mooseandsquirrel@anotherwheel.com 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 2 of the License, and no other 
    version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
//%copyright_end%
#ifndef cggi_ostream_hpp_20210827_011333_PDT
#define cggi_ostream_hpp_20210827_011333_PDT
#include "cggi.hpp"
namespace cggi {
/*****************************************************************************
 * ostream 
 ****************************************************************************/

class element_visitor
{
public:
  
};

auto operator<<(std::ostream& os, const attribute& attr) -> std::ostream&
  {
    os << attr.first << "=\"" << attr.second << "\"";
    return os;
  }
auto operator<<(std::ostream& os, const element& e) -> std::ostream&
  {
    // send '<tag-name'
    os << e.open_tag_prefix() << e.open_tag();
    // send attribute_list, space-separated
    for(const auto& attr : e.attributes())
    {
      os << " " << attr; 
    }
    os << e.open_tag_suffix();
    for(const auto& elem : e.elements())
    {
      os << *elem;
    }
    // send close tag
    os << e.close_tag_prefix() << e.close_tag() << e.close_tag_suffix();
    return os;
  }
} /* namespace cggi */
#endif//cggi_ostream_hpp_20210827_011333_PDT
