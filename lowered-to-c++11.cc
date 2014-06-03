#include <iostream>

template<int N> struct Int { };

struct Identity {
  template<typename T> T operator()(const T& t) const { return t; }
};

template<int N> struct Church;

template<> struct Church<0> {
  template<typename F> Identity operator()(const F&) const { return Identity(); }
};

template<int N, typename F>
struct Church_ {
  Church_(const F& f): f_(f) {}
  template<typename X> auto operator()(const X& x) const -> decltype(std::declval<F>()(Church_<N-1,F>(std::declval<F>())(x))) {
    return f_(Church_<N-1,F>(f_)(x));
  }
  F f_;
};

template<typename F>
struct Church_<0, F> {
  Church_(const F&) {}
  template<typename X> X operator()(const X& x) const { return x; }
};

template<int N> struct Church {
  template<typename F> Church_<N,F> operator()(const F& f) const {
    return Church_<N,F>(f);
  }
};

template<int N>
Church<N> church(Int<N>) {
  return Church<N>();
}

template<typename C>
int unchurch(const C& c) {
  return c([](int x) { return x + 1; })(0);
}

template<typename M, typename N, typename F>
struct Plus_ {
  Plus_(const M& m, const N& n, const F& f) : m_(m), n_(n), f_(f) {}
  template<typename X> auto operator()(const X& x) const -> decltype(std::declval<M>()(std::declval<F>())(std::declval<N>()(std::declval<F>())(x))) {
    return m_(f_)(n_(f_)(x));
  }
  M m_;
  N n_;
  F f_;
};

template<typename M, typename N>
struct Plus {
  Plus(const M& m, const N& n) : m_(m), n_(n) {}
  template<typename F> Plus_<M,N,F> operator()(const F& f) const {
    return Plus_<M,N,F>(m_, n_, f);
  }
  M m_;
  N n_;
};

template<typename M, typename N>
Plus<M,N> plus(const M& m, const N& n) {
  return Plus<M,N>(m, n);
}

template<typename M, typename N>
struct Mult {
  Mult(const M& m, const N& n) : m_(m), n_(n) {}
  template<typename F> auto operator()(const F& f) const -> decltype(std::declval<M>()(std::declval<N>()(f))) {
    return m_(n_(f));
  }
  M m_;
  N n_;
};

template<typename M, typename N>
Mult<M,N> mult(const M& m, const N& n) {
  return Mult<M,N>(m, n);
}

template<typename M, typename N>
auto exp(const M& m, const N& n) -> decltype(n(m)) {
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
