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
#ifndef cggi_visitor_hpp_20210904_023122_PDT
#define cggi_visitor_hpp_20210904_023122_PDT
#include <functional>
#include <iostream>
#include <memory>

namespace cggi {
  
template<typename...>
class Visitor;

template<typename ObjT>
class Visitor<ObjT>
{
  template<typename...>
    friend class Visitor;
  static std::function<void(const ObjT&)> do_visit_;
public:
  ~Visitor() 
    { 
      do_visit_ = [](const auto&) {}; 
    }
  Visitor() 
    { 
      do_visit_ = [this](const auto& _obj) 
          { 
            this->do_visit(_obj); 
          };
    }
  Visitor(const std::function<void(const ObjT&)>& _fn) 
    {
      do_visit_ = _fn;
    }

  auto visit(const ObjT& _obj) -> void { do_visit(_obj); }
  
  template<typename AnyT>
    auto visit(const AnyT& _obj) -> void { Visitor<AnyT>::do_visit_(_obj); }
private:
  virtual auto do_visit(const ObjT&) -> void = 0;
};
template<typename ObjT>
std::function<void(const ObjT&)> Visitor<ObjT>::do_visit_;

template<typename...Ts>
class Visitor : public Visitor<Ts>...
{
public:
  Visitor() : Visitor<Ts>([this](const auto& _obj) { visit(_obj); })... {}

  template<typename AnyT>
    auto visit(const AnyT& _obj) -> void
    {
      Visitor<AnyT>::visit(_obj);
    }
};

} /* namespace cggi */
#endif//cggi_visitor_hpp_20210904_023122_PDT
