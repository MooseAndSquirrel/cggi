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
#include "cggi.hpp"
auto error_div(const std::exception& e) -> cggi::Div
  {
    using namespace cggi;
    return Div { Class("error"), T(e.what()) };
  }

template<typename StreamT>
auto send_error_document(StreamT&& os, const std::exception& e) -> void
  {
    using namespace cggi;
    using namespace std;
    os << http_1_1() << doctype() 
       << Html 
          { Head 
            { css_link { "/css/default.css" } }
          , Body { error_div(e) } }
       << endl;
  }



int main(int argc, char* argv[])
{
  using namespace std;
  using namespace cggi;
  
  try
  {
    auto document = Page {};
    document.head() << css_link { "/css/default.css" };
    document.body() <<
      Div 
        { Class ("container")
        , Div
          { Class ("column left")
          , Image { Source ("/images/ab21.png") }
          }
        , Div
          { Class ("column middle")
          , Div { Class ("title"   ) , T("AB21") }
          , Div { Class ("subtitle") , T("STANLEY") }
          }
        , Div
          { Class ("column right")
          , P { T("column-right") }
          }
        };
      cout << document;
  } 
  catch(const std::exception& e)
  {
    send_error_document(cout, e);
  }
  return 0;
}
auto junk()
  {
    using namespace cggi;
  }
