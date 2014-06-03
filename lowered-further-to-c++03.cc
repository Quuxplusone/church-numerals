#include <iostream>

template<typename FuncType, typename ArgType>
struct ResultType {
  typedef typename FuncType::template ResultTypeWhenCalledWith<ArgType>::type type;
};

template<typename R, typename A>
struct ResultType<R(*)(A), A> {
  typedef R type;
};

template<int N> struct Int { };

struct Identity {
  template<typename T> T operator()(const T& t) const { return t; }

  template<typename T> struct ResultTypeWhenCalledWith { typedef T type; };
};

template<int N> struct Church;

template<> struct Church<0> {
  template<typename F> Identity operator()(const F&) const { return Identity(); }

  template<typename F> struct ResultTypeWhenCalledWith { typedef Identity type; };
};

template<int N, typename F>
struct Church_ {
  Church_(const F& f): f_(f) {}
  template<typename X> typename ResultType<Church_, X>::type operator()(const X& x) const {
    return f_(Church_<N-1,F>(f_)(x));
  }
  F f_;

  template<typename X> struct ResultTypeWhenCalledWith {
    typedef typename ResultType<Church_<N-1,F>, X>::type CFX;
    typedef typename ResultType<F, CFX>::type type;
  };
};

template<typename F>
struct Church_<0, F> {
  Church_(const F&) {}
  template<typename X> X operator()(const X& x) const { return x; }

  template<typename X> struct ResultTypeWhenCalledWith { typedef X type; };
};

template<int N> struct Church {
  template<typename F> Church_<N,F> operator()(const F& f) const {
    return Church_<N,F>(f);
  }

  template<typename F> struct ResultTypeWhenCalledWith { typedef Church_<N,F> type; };
};

template<int N>
Church<N> church(Int<N>) {
  return Church<N>();
}

static int add1(int x) { return x+1; }

template<typename C>
int unchurch(const C& c) {
  return c(&add1)(0);
}

template<typename M, typename N, typename F>
struct Plus_ {
  Plus_(const M& m, const N& n, const F& f) : m_(m), n_(n), f_(f) {}
  template<typename X> typename ResultType<Plus_,X>::type operator()(const X& x) const {
    return m_(f_)(n_(f_)(x));
  }
  M m_;
  N n_;
  F f_;

  template<typename X> struct ResultTypeWhenCalledWith {
    typedef typename ResultType<M, F>::type MF;
    typedef typename ResultType<N, F>::type NF;
    typedef typename ResultType<NF, X>::type NFX;
    typedef typename ResultType<MF, NFX>::type type;
  };
};

template<typename M, typename N>
struct Plus {
  Plus(const M& m, const N& n) : m_(m), n_(n) {}
  template<typename F> Plus_<M,N,F> operator()(const F& f) const {
    return Plus_<M,N,F>(m_, n_, f);
  }
  M m_;
  N n_;

  template<typename F> struct ResultTypeWhenCalledWith { typedef Plus_<M,N,F> type; };
};

template<typename M, typename N>
Plus<M,N> plus(const M& m, const N& n) {
  return Plus<M,N>(m, n);
}

template<typename M, typename N>
struct Mult {
  Mult(const M& m, const N& n) : m_(m), n_(n) {}
  template<typename F> typename ResultType<Mult,F>::type operator()(const F& f) const {
    return m_(n_(f));
  }
  M m_;
  N n_;

  template<typename F> struct ResultTypeWhenCalledWith {
    typedef typename ResultType<N,F>::type NF;
    typedef typename ResultType<M,NF>::type type;
  };
};

template<typename M, typename N>
Mult<M,N> mult(const M& m, const N& n) {
  return Mult<M,N>(m, n);
}

template<typename M, typename N>
typename ResultType<N,M>::type exp(const M& m, const N& n) {
  return n(m);
}

int main() {
  Church<41> c41 = church(Int<41>());
  Church<1> c1 = church(Int<1>());
  std::cout << unchurch(plus(c41, c1)) << "\n";
  Church<6> c6 = church(Int<6>());
  Church<7> c7 = church(Int<7>());
  std::cout << unchurch(mult(c6, c7)) << "\n";
  Church<2> c2 = church(Int<2>());
  Church<9> c9 = church(Int<9>());
  std::cout << unchurch(exp(c2, c9)) << "\n";
  return 0;
}
