#include <iostream>
#include <array>


template <uint64_t N>
struct u64_constant : std::integral_constant<uint64_t, N> {};

template <uint64_t N>
struct Fibonacci;

template <uint64_t N>
static constexpr uint64_t Fibonacci_v = Fibonacci<N>::value;

template <>
struct Fibonacci<0> : u64_constant<0> {};

template <>
struct Fibonacci<1> : u64_constant<1> {};

template<uint64_t N>
struct Fibonacci : u64_constant<Fibonacci_v<N - 1> +Fibonacci_v<N - 2>> {};


static constexpr auto _94_index_seq = std::make_index_sequence<94>{};


template <uint64_t... Is>
static constexpr std::array<uint64_t, sizeof...(Is)> MakeFibonacciSequence(std::index_sequence<Is...>) {
	return { Fibonacci_v<Is>... };
}

//64位整型最大能表示第93个Fibonacci数：12200160415121876738（20位数）
static constexpr auto FibonacciSequence = MakeFibonacciSequence(_94_index_seq);



template <size_t I>
constexpr bool Get_res(size_t index, uint64_t& res) {
	if (I == index) {
		res = Fibonacci_v<I>;
		return true;
	}
	return false;
}

template <size_t... Is>
[[nodiscard]] constexpr uint64_t Make_res(std::index_sequence<Is...>, size_t index) {


	uint64_t res;
	if ((Get_res<Is>(index, res) || ...)) {
		return res;
	}
	throw std::overflow_error("The result exceeds the maximum value that a integer can represent");
}


constexpr uint64_t FibonacciAt(uint64_t index) {
	return Make_res(_94_index_seq, index);
}



int main() {
	std::cout << "The 93th Fibonacci number: " << FibonacciAt(93) << std::endl;

	for (auto&& e : FibonacciSequence) {
		std::cout << e << std::endl;
	}

	return 0;
}
