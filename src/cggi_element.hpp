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
#ifndef cggi_element_hpp_20210827_011457_PDT
#define cggi_element_hpp_20210827_011457_PDT
#include "cggi_strings.hpp"
#include <map>
#include <set>
#include <list>
#include <iostream>
namespace cggi {
class element; 
using     function_t  = std::function<void(element&)>;

/*****************************************************************************
 * attributes 
 ****************************************************************************/

using attribute = std::pair<text_t, text_t>;

auto  Class(const text_t& value) { return attribute { "class" , value }; }
auto    For(const text_t& value) { return attribute { "for"   , value }; }
auto   Href(const text_t& value) { return attribute { "href"  , value }; }
auto     Id(const text_t& value) { return attribute { "id"    , value }; }
auto Method(const text_t& value) { return attribute { "method", value }; }
auto   Name(const text_t& value) { return attribute { "name"  , value }; }
auto    Rel(const text_t& value) { return attribute { "rel"   , value }; }
auto    Src(const text_t& value) { return attribute { "src"   , value }; }
auto Source(const text_t& value) { return attribute { "src"   , value }; }
auto   Type(const text_t& value) { return attribute { "type"  , value }; }
auto  Value(const text_t& value) { return attribute { "value" , value }; }

auto nameid(const text_t& v) -> std::array<attribute, 2>
  {
    return std::array<attribute, 2>
    { attribute { Id(v)   }
    , attribute { Name(v) }
    };
  }
/*****************************************************************************
 * elements
 ****************************************************************************/
template<typename T, typename...>
class visitor;

class element 
{
private:
  class dictionary
  {
    using class_map = std::multimap<text_t, element*>;
    using id_map    = std::map<text_t, element*>;
    friend class element;
  public:
    static auto null_string() -> const std::string& 
      {
        static std::string ns;
        return ns;
      }

    auto add(element& e, text_t key, text_t value) -> void
      {
        if(key == "id")
        {
          add_id(e, value);
        }
        else if(key == "class")
        {
          add_class(e, value); 
        }
      }
    template<typename StringT>
    auto elements_in_class(StringT&& s) -> std::pair<class_map::iterator, class_map::iterator>
      {
        return classes().equal_range(s);
      }
    template<typename StringT>
    auto element_with_id(StringT&& _idname) -> element*
      {
        auto elem_iter = ids().find(_idname);
        element* result = nullptr;
        if(elem_iter != ids().end())
        {
          result = elem_iter->second;
        }
        return result;
      }
  private:

    auto add_id(element& e, text_t _value) -> void
      {
        ids()[_value] = &e;
      }
    auto add_class(element& e, text_t _classname) -> void
      {
        auto& map = classes();
        map.emplace(_classname, &e);
      }
    auto update_id(element& e, text_t _idname) -> void
      {
        remove_id(e, _idname);
        add_id(e, _idname);
      }
    auto get_class(element& _e) -> text_t
      {
        text_t result;
        auto found_iter = _e.attributes().find("class");
        if(found_iter != _e.attributes().end())
        {
          return found_iter->second;
        }
        return result;
      }
    auto get_id(element& _e) -> text_t
      {
        text_t result;
        auto found_iter = _e.attributes().find("id");
        if(found_iter != _e.attributes().end())
        {
          return found_iter->second;
        }
        return result;
      }
    auto update_id(element& _e) -> void
      {
        auto idname = get_id(_e);
        update_id(_e, idname);
      }
    auto remove_id(element& e, text_t _idname) -> void
      {
        ids().erase(_idname);
      }
    auto remove_element_from_class(element& e, text_t _classname) -> void 
      {
        // find all elements with the matching class
        auto range = classes().equal_range(_classname);
        for(auto iter = range.first; iter != range.second; ++iter)
        {
          // if the given element's entry was found, remove it 
          if(iter->second == &e)
          {
            classes().erase(iter);
          }
        }
      }
    auto update_class(element& e, text_t _classname) -> void 
      {
        remove_element_from_class(e, _classname);
        add_class(e, _classname);
      }
    auto update_class(element& e) -> void 
      {
        auto classname = get_class(e);
        update_class(e, classname);
      }
    static auto classes() -> class_map&
      {
        static class_map classes_;
        return classes_;
      }
    static auto ids() -> id_map&
      {
        static id_map ids_;
        return ids_;
      }
  };
public:
  using element_ptr     = std::unique_ptr<element>;
  using element_list_t  = std::list<element_ptr>;
  using attribute_map_t = std::map<text_t, text_t>;
  using value_t         = cggi::text_t;
  element() {}
  
  element(const element&) = delete;     // NON COPYABLE helps prevent circular references and other nasties;
                                        // makes it easier to do a bunch of constructor initialization, if
                                        // pain is what you desire; also makes it easer to maintain trees
                                        // if copying is needed, do it outside of the element tree
  element(element&& src) 
      : parent_     (std::move(src.parent_))
      , elements_   (std::move(src.elements_))
      , attributes_ (std::move(src.attributes_))
      , data_       (std::move(src.data_))
    {
    }


  template<typename...ArgTs>
    explicit element(ArgTs&&...args) noexcept
    {
      init_components(std::forward<ArgTs>(args)...);
    }

  virtual ~element() {}

  auto elements()   const -> const element_list_t& { return elements_; }
  auto attributes() const -> const attribute_map_t& { return attributes_; }

  template<typename StringT>
    auto set_id(StringT&& s) -> text_t&
    { 
      return set_attribute("id", s);
      dictionary().update_id(*this, s);
    }
  template<typename StringT>
    auto set_class(StringT&& s) -> text_t&
    {
      return set_attribute("class", s);
      dictionary().update_class(*this, s);
    }
  template<typename S1, typename S2>
    auto set_attribute(S1&& s1, S2&& s2) -> text_t& 
    {
      return attributes_[s1]=s2;
    }

  auto open_tag_prefix()  const -> value_t { return get_open_tag_prefix(); }
  auto open_tag_suffix()  const -> value_t { return get_open_tag_suffix(); }
  auto close_tag_prefix() const -> value_t { return get_close_tag_prefix(); }
  auto close_tag_suffix() const -> value_t { return get_close_tag_suffix(); }
  auto open_tag()         const -> value_t { return get_open_tag(); }
  auto close_tag()        const -> value_t { return get_close_tag(); }

  template<typename ElementT>
  auto append(std::unique_ptr<ElementT>&& _e) -> element_ptr&
    {
      auto& new_element = elements_.emplace_back(_e.release());
      new_element->parent_ = this;
      return new_element;
    }

  //template<typename ElementT>
  //auto append(std::unique_ptr<ElementT>&& e) -> element_ptr&
  //  {
  //    return elements_.emplace_back(e.release());
  //  }
  //auto append(element_ptr&& _e) -> element_ptr&
  //  {
  //    auto& new_element = elements_.emplace_back(_e.release());
  //    new_element->parent_ = this;
  //    dictionary().add(*new_element);
  //    return new_element;
  //  }
  template<typename ElementT>
  auto append(ElementT&& e) -> element_ptr&
    {
      using element_t = std::remove_reference_t<ElementT>;
      element_ptr new_element = std::make_unique<element_t>(std::move(e));
      auto& result = append(std::move(new_element));
      dictionary().update_id(*result);
      return result;
    }
  auto append() -> void
    {
    }
  template<size_t N>
  auto append(std::array<attribute, N>&& attr_array) -> attribute
    {
      attribute result;
      for(auto attr : attr_array)
      {
        result = append(std::move(attr));
      }
      return result;
    }
  auto append(attribute&& attr) -> attribute
    {
      auto result = attributes_.emplace(std::move(attr));
      if(result.second == false)
      {
        throw std::runtime_error("could not add attribute");
      }
      const attribute& new_attribute = *(result.first); 
      dictionary().add(*this, new_attribute.first, new_attribute.second);
      return new_attribute;
    }
  static auto dictionary()       ->       class dictionary& { return dictionary_; }
  auto append(const char* cstr) -> element&;
//  template<typename HeadT, typename...TailTs>
//    auto append(HeadT&& head, TailTs&&...tail) -> element_ptr&
//    {
//      auto& result = append(std::forward<HeadT>(head));
//      append(std::forward<TailTs>(tail)...);
//      return result;
//    }
  template<typename ElementT>
  auto operator<<(ElementT&& e) -> element&
    {
      append(std::forward<ElementT>(e));
      return *this;
    }
  auto data() const -> const std::string& { return data_; }
  template<typename StringT>
  auto data(StringT&& s) -> void
    {
      data_.assign(std::forward<StringT>(s));
    }
  
private:
  const element*    parent_ = nullptr;
  element_list_t    elements_;
  attribute_map_t   attributes_;
  std::string       data_;
  static class dictionary  dictionary_;
  template<typename HeadT>
  auto init_components(HeadT&& head) -> void
    {
      append(std::forward<HeadT>(head));
    }
  template<typename HeadT, typename...TailTs>
  auto init_components(HeadT&& head, TailTs&&...tail) -> void
    {
      init_components(std::forward<HeadT>(head));
      init_components(std::forward<TailTs>(tail)...);
    }
  virtual auto get_open_tag_prefix()  const -> value_t { return "<"; }
  virtual auto get_open_tag_suffix()  const -> value_t { return ">"; }
  virtual auto get_open_tag()         const -> value_t { return ""; }
  virtual auto get_close_tag()        const -> value_t { return get_open_tag(); }
  virtual auto get_close_tag_prefix() const -> value_t { return "</"; }
  virtual auto get_close_tag_suffix() const -> value_t { return ">"; }
};

template<typename StringT>
auto element_with_id(StringT&& _idname)
  {
    return element::dictionary().element_with_id(_idname);
  }
class element::dictionary  element::dictionary_;
class naked_element : public element
  {
  public:
    template<typename...ArgTs>
      naked_element(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag_prefix()  const -> value_t override { return ""; }
    virtual auto get_open_tag_suffix()  const -> value_t override { return ""; }
    virtual auto get_open_tag()         const -> value_t override { return ""; }
    virtual auto get_close_tag()        const -> value_t override { return ""; }
    virtual auto get_close_tag_prefix() const -> value_t override { return ""; }
    virtual auto get_close_tag_suffix() const -> value_t override { return ""; }
  };

class self_closing_element : public element
  {
  public:
    template<typename...ArgTs>
      self_closing_element(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag_suffix()  const -> value_t override { return "/>"; }
    virtual auto get_close_tag()        const -> value_t override { return ""; }
    virtual auto get_close_tag_prefix() const -> value_t override { return ""; }
    virtual auto get_close_tag_suffix() const -> value_t override { return ""; }
  };
class non_closing_element : public element
  {
  public:
    template<typename...ArgTs>
      non_closing_element(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_close_tag()        const -> value_t override { return ""; }
    virtual auto get_close_tag_prefix() const -> value_t override { return ""; }
    virtual auto get_close_tag_suffix() const -> value_t override { return ""; }
  };


class functional_element : public naked_element 
  {
  public:
    template<typename FnT>
      explicit functional_element(FnT&& _fn)
      {
        _fn(*this);
      }
  };
using Fn = functional_element;

class text_element : public naked_element
  {
  public:
    text_element(const char* s)
      {
        data(s);
      }
    template<typename StringT>
      text_element(StringT&& v)
      {
        using namespace std;
        using string_t = decay_t<StringT>;
        if constexpr(is_same_v<string_t, string> || is_same_v<string_t, char*>)
        {
          data(forward<StringT>(v));
        }
        else
        {
          data(boost::lexical_cast<string>(v));
        }
      }
  private:
    virtual auto get_open_tag_prefix()  const -> value_t override { return data(); }
  };
using T = text_element;

inline auto element::append(const char* cstr) -> element&
  {
    element_ptr new_element(std::make_unique<text_element>(cstr));
    return *append(std::move(new_element));
  }
//inline auto element::init_component(const char* cstr) -> void
//  {
//    element_ptr new_element(std::make_unique<text_element>(cstr));
//    append(std::move(new_element));
//  }

class link : public self_closing_element
  {
  public:
    template<typename...ArgTs>
      link(ArgTs&&...args) : self_closing_element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "link"; }
  };

class css_link : public link
  {
  public:
    template<typename StringT>
      css_link(StringT&& s) 
          : link(Rel("stylesheet"), Href(s))
        {}
  };

class doctype : public non_closing_element
  {
  public:
    template<typename...ArgTs>
    doctype(ArgTs&&...args)
        : non_closing_element(std::forward<ArgTs>(args)...)
      {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "!DOCTYPE html"; }
  };

class Body : public element 
  {
  public:
    template<typename...ArgTs>
      Body(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}

    auto operator()() -> Body& { return *this; }
  private:
    virtual auto get_open_tag()         const -> value_t override { return "body"; }

  };
class Div : public element 
  {
  public:
    template<typename...ArgTs>
      Div(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "div"; }
  };
class Form : public element 
  {
  public:
    template<typename...ArgTs>
      Form(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "form"; }
  };
class Head : public element 
  {
  public:
    template<typename...ArgTs>
      Head(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "head"; }
  };
class Html : public element 
  {
  public:
    template<typename...ArgTs>
      Html(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "html"; }
  };
class HttpHeader : public naked_element
  {
  };
class http_1_1 final : public HttpHeader
  {
  private:
    virtual auto get_open_tag() const -> value_t override
    {
      return "Content-type: text/html\n\n";
      //return "Content-Type: text/html;charset=us-ascii\n\n";
    }
  };
class Hyperlink : public element
  {
  public:
    template<typename...ArgTs>
      Hyperlink(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag() const -> value_t override { return "a"; }
  };
using A = Hyperlink;
class Image : public self_closing_element 
  {
  public:
    template<typename...ArgTs>
      Image(ArgTs&&...args) : self_closing_element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "img"; }
  };
using Img = Image;
class Input : public non_closing_element 
  {
  public:
    template<typename...ArgTs>
      Input(ArgTs&&...args) : non_closing_element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "input"; }
  };
class Label : public element 
  {
  public:
    template<typename...ArgTs>
      Label(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "label"; }
  };
class page_element : public naked_element 
  {
  public:
    page_element()
        : naked_element(http_1_1(), doctype())
        , html_(init_html())
        , head_(init_head())
        , body_(init_body())
      {}

    template<typename HeadT>
    explicit page_element(HeadT&& elem) : page_element()
      {
        body_->append(std::forward<HeadT>(elem));
      }
    template<typename HeadT, typename...TailTs>
    explicit page_element(HeadT&& head, TailTs&&...tail) : page_element(page_element(tail)...)
      {
        body_->append(std::forward<HeadT>(head));
      }
    
    auto html() const -> const element& { return *html_; }
    auto html()       ->       element& { return *html_; }

    auto head() const -> const element& { return *head_; }
    auto head()       ->       element& { return *head_; }

    auto body() const -> const element& { return *body_; }
    auto body()       ->       element& { return *body_; }
  private:
    element_ptr&  html_;
    element_ptr&  head_;
    element_ptr&  body_;

    explicit page_element(link&& elem) : page_element()
      {
        head_->append(std::move(elem));
      }
    auto init_html() -> element_ptr&
      {
        return append(Html {});
      }
    auto init_head() -> element_ptr&
      {
        return html_->append(Head {});
      }
    auto init_body() -> element_ptr&
      {
        return html_->append(Body {});
      }
  };
using Page = page_element;
class Paragraph : public element 
  {
  public:
    template<typename...ArgTs>
      Paragraph(ArgTs&&...args) : element(std::forward<ArgTs>(args)...) {}
  private:
    virtual auto get_open_tag()         const -> value_t override { return "p"; }
  };
using P = Paragraph;



/*****************************************************************************
 * end of file 
 ****************************************************************************/
} /* namespace cggi */
#endif//cggi_element_hpp_20210827_011457_PDT
