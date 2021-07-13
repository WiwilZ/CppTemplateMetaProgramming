#include <chrono>
#include <iostream>


using namespace std;
using namespace std::chrono;


template<size_t N>
struct size_t_constant : std::integral_constant<size_t, N> {};

template<size_t N>
struct Fibonacci;

template<size_t N>
static constexpr size_t Fibonacci_v = Fibonacci<N>::value;

template<>
struct Fibonacci<0> : size_t_constant<0> {};

template<>
struct Fibonacci<1> : size_t_constant<1> {};

template<size_t N>
struct Fibonacci : size_t_constant<Fibonacci_v<N - 1> + Fibonacci_v<N - 2>> {};



template <size_t... Is>
static constexpr std::array<size_t, sizeof...(Is)> MakeFibonacciSequence(std::index_sequence<Is...>)
{
	return { Fibonacci_v<Is>... };
}

//64位整型最大能表示第200个Fibonacci数：17323038258947941269（20位数）
static constexpr auto FibonacciSequence = MakeFibonacciSequence(std::make_index_sequence<200>());



template <size_t I = 0>
static constexpr size_t FibonacciAt(size_t idx)
{
	if (I == idx)
		return Fibonacci_v<I>;
	return FibonacciAt<I + 1>(idx);
}

template <>
size_t FibonacciAt<201>(size_t)
{
	throw std::overflow_error("The result exceeds the maximum value that a integer can represent");
}



int main()
{
	size_t i = 200;
	
	cout << "The 200th Fibonacci number: " << FibonacciAt(i) << endl;

	for (auto i : FibonacciSequence)
		cout << i << endl;

	return 0;
}