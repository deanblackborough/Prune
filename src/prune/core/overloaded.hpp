#pragma once

namespace prune {

  // Combines several lambdas into one callable with one operator() overload
  // per lambda, for use as the visitor passed to std::visit over a variant.
  template <class... Visitors> struct overloaded : Visitors... {
    using Visitors::operator()...;
  };
  template <class... Visitors>
  overloaded(Visitors...) -> overloaded<Visitors...>;

} // namespace prune
