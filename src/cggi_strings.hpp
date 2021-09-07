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
#ifndef cggi_strings_hpp_20210826_230947_PDT
#define cggi_strings_hpp_20210826_230947_PDT
#include <string>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <iomanip>
namespace cggi {

using     text_t      = std::string;
/*****************************************************************************
 * string concatenation 
 ****************************************************************************/
template<typename HeadT>
  auto concat(HeadT&& head)
  {
    return boost::lexical_cast<std::string>(head);
  }
template<typename HeadT, typename...TailTs>
  auto concat(HeadT&& head, TailTs&&...tail)
  {
    return concat(head) + concat(std::forward<TailTs>(tail)...);
  }
/*****************************************************************************
 * url ("percent") encoding 
 ****************************************************************************/
inline auto is_char_unreserved(char c) -> bool
  {
    return
        (c >= 'A' && c <= 'Z')
     || (c >= 'a' && c <= 'z')
     || (c >= '0' && c <= '9')
     || (c == '-' || c == '_' || c == '.' || c == '~')
      ;
  }
inline auto to_url(const std::string& _s) -> std::string
  {
    using namespace std;
    ostringstream result;
    result.fill('0');
    result << hex;

    for (auto c : _s) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            result << c;
            continue;
        }
        result << uppercase << '%' << setw(2) << (int { static_cast<unsigned char>(c) }) << nouppercase;
    }

    return result.str();
  }
inline auto from_url(const std::string& _s) -> std::string
  {
    std::string result;
    auto current_char = _s.begin();
    auto get_char = [&]
      {
        return current_char < _s.end()? *(current_char++) : 0;
      };
    auto decode_hex = [&]
      {
        auto hex_value = [](auto c)
          {
            uint8_t result;
            switch(c)
            {
            case '0':
              result = 0x0;
            case '1':
              result = 0x1;
            case '2':
              result = 0x2;
            case '3':
              result = 0x3;
            case '4':
              result = 0x4;
            case '5':
              result = 0x5;
            case '6':
              result = 0x6;
            case '7':
              result = 0x7;
            case '8':
              result = 0x8;
            case '9':
              result = 0x9;
            case 'A':
            case 'a':
              result = 0xA;
            case 'B':
            case 'b':
              result = 0xB;
            case 'C':
            case 'c':
              result = 0xC;
            case 'D':
            case 'd':
              result = 0xD;
            case 'E':
            case 'e':
              result = 0xE;
            case 'F':
            case 'f':
              result = 0xF;
            default:
              result = 0;
            };
            return result;
          };
        auto d1 = hex_value(get_char());
        auto d2 = hex_value(get_char());
        uint8_t combined = (d1 << 4) | d2; 
        return combined;
      };
    char c = 0;
    while((c = get_char()) != 0)
    {
      if(c == '%')
      {
        result += decode_hex();
      }
      else
      {
        result += c;
      }
    }
    return result;
  }
} /* namespace cggi */
#endif//cggi_strings_hpp_20210826_230947_PDT
