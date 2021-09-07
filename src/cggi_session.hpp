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
#ifndef cggi_session_hpp_20210901_224007_PDT
#define cggi_session_hpp_20210901_224007_PDT
namespace cggi {
/*****************************************************************************
 * Session variables ("get" method)
 ****************************************************************************/
using session_map_t = std::map<std::string, std::string>;
inline auto parse_session(const char* data, size_t sz) -> session_map_t
  {
    session_map_t result;
    enum class Mode { AddKey, Start, Name, Data } mode = Mode::Start;
    size_t idx = 0;
    std::string varname, vardata;
    auto add_key = [&]
      {
        result[varname]=vardata;
      };
    while(idx < sz)
    {
      auto ch = data[idx++];
      switch(mode)
      {
      case Mode::AddKey:
        add_key();
        // NO-BREAK
      case Mode::Start:
        mode = Mode::Start;
        varname.clear();
        vardata.clear();
        if(is_char_unreserved(ch))
        {
          varname += ch;
          mode = Mode::Name;
        }
        break;
      case Mode::Name:
        if(is_char_unreserved(ch))
        {
          varname += ch;
        }
        else if(ch == '=')
        {
          mode = Mode::Data;
        }
        else
        {
          mode = Mode::AddKey;
        }
        break;
      case Mode::Data:
        if(is_char_unreserved(ch) || ch == '%')
        {
          vardata += ch;
        }
        else
        {
          mode = Mode::AddKey;
        }
        break;
      }
    }
    if(!varname.empty())
    {
      add_key();
    }
    return result;
  }
inline auto post_query() -> const std::string&
  {
    static auto pd_ = []
      {
        auto content_length = getenv("CONTENT_LENGTH");
        auto data_sz = content_length == NULL? 0L : atol(content_length) + 1;
        std::string data(data_sz, 0);
        std::cin.get(data.data(), data_sz);
        auto read_result = fread( data.data(), data.size(), 1, stdin );
        return data;
      }();
    return pd_;
  }
inline auto session_post() -> const session_map_t&
  {
    static auto data_ = []
      { 
        return parse_session(post_query().data(), post_query().size());
      }();
    return data_;
  }

auto find(const session_map_t& map, const std::string& key) -> const std::string&
  {
    auto found = map.find(key);
    if(found != map.end())
    {
      return found->second;
    }
    else
    {
      static std::string empty_str;
      return empty_str;
    }
  }
auto Post(const std::string& key) -> const std::string&
  {
    return find(session_post(), key);
  }

inline auto get_query() -> const std::string&
  {
    static auto q_ = []
      {
        auto query_str = getenv("QUERY_STRING");
        return std::string { query_str == NULL? "" : query_str };
      }();
    return q_;
  }
inline auto session_get() -> const session_map_t&
  {
    static auto result_ = []
      {
        auto& data = get_query();
        return parse_session(data.data(), data.size());
      }();
    return result_;
  }
auto Get(const std::string& key) -> const std::string&
  {
    return find(session_get(), key);
  }

inline auto session_all() -> const session_map_t&
  {
    static auto data_ = []
      {
        session_map_t get  = session_get();
        session_map_t post = session_post();
        post.merge(get); 
        return post;
      }();
    return data_;
  }
auto Session(const std::string& key) -> const std::string&
  {
    return find(session_all(), key);
  }
} /* namespace cggi */
#endif//cggi_session_hpp_20210901_224007_PDT
