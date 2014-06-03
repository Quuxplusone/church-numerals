#include <iostream>

template<int N> struct Int { };

auto church(Int<0>) {
  return [](auto f) {
    return [=](auto x) { return x; };
  };
}

template<int N>
auto church(Int<N>) {
  return [](auto f) {
    return [=](auto x) {
      return f(church(Int<N-1>())(f)(x));
    };
  };
}

template<typename C>
auto unchurch(const C& c) {
  return c([=](auto x) { return x + 1; })(0);
}

template<typename M, typename N>
auto plus(const M& m, const N& n) {
  return [=](auto f) {
    return [=](auto x) {
      return m(f)(n(f)(x));
    };
  };
}

template<typename M, typename N>
auto mult(const M& m, const N& n) {
  return [=](auto f) {
    return n(m(f));
  };
}

template<typename M, typename N>
auto exp(const M& m, const N& n) {
  return n(m);
}

int main() {
  auto c41 = church(Int<41>());
  auto c1 = church(Int<1>());
  std::cout << unchurch(plus(c41, c1)) << "\n";
  auto c6 = church(Int<6>());
  auto c7 = church(Int<7>());
  std::cout << unchurch(mult(c6, c7)) << "\n";
  auto c2 = church(Int<2>());
  auto c9 = church(Int<9>());
  std::cout << unchurch(exp(c2, c9)) << "\n";
  return 0;
}
