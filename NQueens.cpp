#include <iostream>


template <typename L, typename R>
struct concat;

template <typename L, typename R>
using concat_t = typename concat<L, R>::type;

template <size_t... L, size_t... R>
struct concat<std::index_sequence<L...>, std::index_sequence<R...>> {
	using type = std::index_sequence<L..., R...>;
};

template <size_t Row, size_t LD, size_t RD, size_t Mask, size_t Pos, typename Res>
struct helper;

template <size_t Row, size_t LD, size_t RD, size_t Mask, size_t Pos, typename Res>
using helper_t = typename helper<Row, LD, RD, Mask, Pos, Res>::type;

template <size_t Row, size_t LD, size_t RD, size_t Mask, typename Res>
struct solve {
	using type = helper_t<Row, LD, RD, Mask, ~(Row | LD | RD) & Mask, Res>;
};

template <size_t Row, size_t LD, size_t RD, size_t Mask, typename Res>
using solve_t = typename solve<Row, LD, RD, Mask, Res>::type;

template <size_t Row, size_t LD, size_t RD, typename Res>
struct solve<Row, LD, RD, Row, Res> {
	using type = Res;
};

template <size_t Row, size_t LD, size_t RD, size_t Mask, size_t Pos, typename Res>
struct helper {
	static constexpr size_t P = Pos & (~Pos + 1);
	using type = concat_t<solve_t<Row | P, ((LD | P) << 1), ((RD | P) >> 1), Mask, concat_t<Res, std::index_sequence<static_cast<size_t>(std::countr_zero(P) + 1)>>>, helper_t<Row, LD, RD, Mask, Pos - P, Res>>;
};

template <size_t Row, size_t LD, size_t RD, size_t Mask, typename Res>
struct helper<Row, LD, RD, Mask, 0, Res> {
	using type = std::index_sequence<>;
};



template <size_t N, size_t I, typename T>
struct display_impl {
	static void display() {}
};

template <size_t N, size_t I>
struct display_impl<N, I, std::index_sequence<>> {
	static void display() {}
};

template <size_t N, size_t I, size_t Head, size_t... Tail>
struct display_impl<N, I, std::index_sequence<Head, Tail...>> {
	static void display() {
		if constexpr (I % N == 0)
			std::cout << I / N + 1 << " (" << Head << ", ";
		else if constexpr (I % N == N - 1)
			std::cout << Head << ")" << std::endl;
		else
			std::cout << Head << ", ";
		display_impl<N, I + 1, std::index_sequence<Tail...>>::display();
	}
};

template <size_t N>
struct NQueens {
	static void display() {
		display_impl<N, 0, solve_t<0, 0, 0, (size_t{ 1 } << N) - 1, std::index_sequence<>>>::display();
	}
};



int main() {
	NQueens<8>::display();

	return 0;
}
