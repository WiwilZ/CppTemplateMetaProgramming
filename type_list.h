#pragma once

#include <type_traits>


/****************************************helper****************************************/
template <typename _Ty>
struct identity
{
	using type = _Ty;
};


template <typename _Ty1>
struct _SameAs
{
	template <typename _Ty2>
	using _ = std::is_same<_Ty1, _Ty2>;
};


template <bool _Cur_val, typename _Next>
inline constexpr bool or_value = true;

template <typename _Next>
inline constexpr bool or_value<false, _Next> = _Next::value;


template <bool _Cur_val, typename _Next>
inline constexpr bool and_value = false;

template <typename _Next>
inline constexpr bool and_value<true, _Next> = _Next::value;


template <typename _Ty1, typename _Ty2>
struct ge_in_size : std::bool_constant<sizeof _Ty1 >= sizeof _Ty2> {};

template <typename _Ty1, typename _Ty2>
struct le_in_size : std::bool_constant<sizeof _Ty1 <= sizeof _Ty2> {};

template <typename _Ty>
void PrintType()
{
	std::cout << typeid(typename _Ty::type).name() << std::endl;
}
/****************************************helper****************************************/

/****************************************type_list definition****************************************/
template <typename... _Types>
struct type_list
{
	using type = type_list<_Types...>;
};
/****************************************type_list definition****************************************/

/****************************************is_type_list****************************************/
template <typename _Ty>
struct is_type_list : std::false_type {};

template <typename... _Types>
struct is_type_list<type_list<_Types...>> : std::true_type {};

template <typename _Ty>
inline constexpr bool is_type_list_v = is_type_list<_Ty>::value;

template <typename _Ty>
concept _type_list = is_type_list_v<_Ty>;
/****************************************is_type_list****************************************/

/****************************************any_type_list****************************************/
template <typename... _Types>
struct any_type_list : std::false_type {};

template <typename _First, typename... _Rest>
struct any_type_list<_First, _Rest...> : std::bool_constant<or_value<is_type_list_v<_First>, any_type_list<_Rest...>>> {};

template <typename... _Types>
inline constexpr bool any_type_list_v = any_type_list<_Types...>::value;
/****************************************any_type_list****************************************/

/****************************************all_type_list****************************************/
template <typename... _Types>
struct all_type_list : std::true_type {};

template <typename _First, typename... _Rest>
struct all_type_list<_First, _Rest...> : std::bool_constant<and_value<is_type_list_v<_First>, all_type_list<_Rest...>>> {};

template <typename... _Types>
inline constexpr bool all_type_list_v = all_type_list<_Types...>::value;
/****************************************all_type_list****************************************/

/****************************************empty****************************************/
template <_type_list _TyLst>
struct empty : std::false_type {};

template <>
struct empty<type_list<>> : std::true_type {};

template <_type_list _TyLst>
inline constexpr bool empty_v = empty<_TyLst>::value;
/****************************************empty****************************************/

/****************************************length****************************************/
template <_type_list _TyLst>
struct length;

template <typename... _Types>
struct length<type_list<_Types...>> : std::integral_constant<size_t, sizeof...(_Types)> {};

template <_type_list _TyLst>
static constexpr size_t length_v = length<_TyLst>::value;
/****************************************length****************************************/

/****************************************assert invalid****************************************/
template <_type_list _TyLst, size_t _Idx>
struct check_index_valid
{
	static_assert(_Idx < length_v<_TyLst>, "Index is out of range!");
};

template <_type_list _TyLst, size_t _Idx>
struct check_index_valid_
{
	static_assert(_Idx <= length_v<_TyLst>, "Index is out of range!");
};

template <_type_list _TyLst, size_t _Begin, size_t _End>
struct check_range_valid
{
	static_assert(_End <= length_v<_TyLst>, "Index is out of range!");
	static_assert(_Begin <= _End, "Begin must less than or equal to End");
};

template <_type_list _TyLst>
struct check_not_empty
{
	static_assert(!empty_v<_TyLst>, "empty type-list!");
};
/****************************************assert invalid****************************************/

/****************************************append****************************************/
template <_type_list _TyLst, typename _Ty>
struct append;

template <typename... _Types, typename _Ty>
struct append<type_list<_Types...>, _Ty> : type_list<_Types..., _Ty> {};

template <_type_list _TyLst, typename _Ty>
using append_t = typename append<_TyLst, _Ty>::type;
/****************************************append****************************************/

/****************************************prepend****************************************/
template <_type_list _TyLst, typename _Ty>
struct prepend;

template <typename... _Types, typename _Ty>
struct prepend<type_list<_Types...>, _Ty> : type_list<_Ty, _Types...> {};

template <_type_list _TyLst, typename _Ty>
using prepend_t = typename prepend<_TyLst, _Ty>::type;
/****************************************prepend****************************************/

/****************************************first****************************************/
template <_type_list _TyLst>
struct first : check_not_empty<_TyLst> {};

template <typename _First, typename... _Rest>
struct first<type_list<_First, _Rest...>> : identity<_First> {};

template <_type_list _TyLst>
using first_t = typename first<_TyLst>::type;
/****************************************first****************************************/

/****************************************pop_first****************************************/
template <_type_list _TyLst>
struct pop_first : check_not_empty<_TyLst> {};

template <typename _First, typename... _Rest>
struct pop_first<type_list<_First, _Rest...>> : type_list<_Rest...> {};

template <_type_list _TyLst>
using pop_first_t = typename pop_first<_TyLst>::type;
/****************************************pop_first****************************************/



/****************************************concat****************************************/
template <_type_list _TyLs1, _type_list _TyLs2, _type_list... _TyLsRest>
struct concat : concat<typename concat<_TyLs1, _TyLs2>::type, _TyLsRest...> {};

template <typename... _Types1, typename... _Types2>
struct concat<type_list<_Types1...>, type_list<_Types2...>> : type_list<_Types1..., _Types2...> {};

template <_type_list _TyLs1, _type_list _TyLs2, _type_list... _TyLsRest>
using concat_t = typename concat<_TyLs1, _TyLs2, _TyLsRest...>::type;
/****************************************concat****************************************/

/****************************************construct****************************************/
template <size_t _Num, typename _Ty>
struct construct : append<typename construct<_Num - 1, _Ty>::type, _Ty> {};

template <typename _Ty>
struct construct<0, _Ty> : type_list<> {};

template <size_t _Num, typename _Ty>
using construct_t = typename construct<_Num, _Ty>::type;
/****************************************construct****************************************/

/****************************************contain****************************************/
template <_type_list _TyLst, typename _Ty>
struct contain;

template <typename... _Types, typename _Ty>
struct contain<type_list<_Types...>, _Ty> : std::disjunction<std::is_same<_Ty, _Types>...> {};

template <_type_list _TyLst, typename U>
static constexpr bool contain_v = contain<_TyLst, U>::value;
/****************************************contain****************************************/

/****************************************locate****************************************/
template <_type_list _TyLst, typename _Ty>
struct locate
{
private:
	static constexpr int64_t tmp = locate_v<pop_first_t<_TyLst>, _Ty>;
public:
	static constexpr int64_t value = std::is_same_v<first_t<_TyLst>, _Ty> ? 0 : tmp == -1 ? -1 : tmp + 1;
};

template <typename _Ty>
struct locate<type_list<>, _Ty> : std::integral_constant<int64_t, -1> {};

template <_type_list _TyLst, typename _Ty>
static constexpr int64_t locate_v = locate<_TyLst, _Ty>::value;
/****************************************locate****************************************/

/****************************************get****************************************/
template <_type_list _TyLst, size_t _Idx>
struct get : check_index_valid<_TyLst, _Idx>, get<pop_first_t<_TyLst>, _Idx - 1> {};

template <_type_list _TyLst>
struct get<_TyLst, 0> : first<_TyLst> {};

template <_type_list _TyLst, size_t _Idx>
using get_t = typename get<_TyLst, _Idx>::type;

template <_type_list _TyLst>
using last_t = get_t<_TyLst, length_v<_TyLst> - 1>;
/****************************************get****************************************/

/****************************************insert****************************************/
template <_type_list _TyLst, size_t _Idx, typename _Ty>
struct insert : check_index_valid_<_TyLst, _Idx>, prepend<typename insert<pop_first_t<_TyLst>, _Idx - 1, _Ty>::type, first_t<_TyLst>> {};

template <_type_list _TyLst, typename _Ty>
struct insert<_TyLst, 0, _Ty> : prepend<_TyLst, _Ty> {};

template <_type_list _TyLst, size_t _Idx, typename _Ty>
using insert_t = typename insert<_TyLst, _Idx, _Ty>::type;
/****************************************insert****************************************/

/****************************************pop****************************************/
template <_type_list _TyLst, size_t _Idx>
struct pop : check_index_valid<_TyLst, _Idx>, prepend<typename pop<pop_first_t<_TyLst>, _Idx - 1>::type, first_t<_TyLst>> {};

template <_type_list _TyLst>
struct pop<_TyLst, 0> : pop_first<_TyLst> {};

template <_type_list _TyLst, size_t _Idx>
using pop_t = typename pop<_TyLst, _Idx>::type;

template <_type_list _TyLst>
using pop_last_t = pop_t<_TyLst, length_v<_TyLst> - 1>;
/****************************************pop****************************************/

/****************************************replace_by_index****************************************/
template <_type_list _TyLst, size_t _Idx, typename _NewType>
struct replace_by_index : check_index_valid<_TyLst, _Idx>, prepend<typename replace_by_index<pop_first_t<_TyLst>, _Idx - 1, _NewType>::type, first_t<_TyLst>> {};

template <_type_list _TyLst, typename _NewType>
struct replace_by_index<_TyLst, 0, _NewType> : prepend<pop_first_t<_TyLst>, _NewType> {};

template <_type_list _TyLst, size_t _Idx, typename _NewType>
using replace_by_index_t = typename replace_by_index<_TyLst, _Idx, _NewType>::type;

template <_type_list _TyLst, typename _NewType>
using replace_first_t = replace_by_index_t<_TyLst, 0, _NewType>;

template <_type_list _TyLst, typename _NewType>
using replace_last_t = replace_by_index_t<_TyLst, length_v<_TyLst> - 1, _NewType>;
/****************************************replace_by_index****************************************/

/****************************************slice****************************************/
template <_type_list _TyLst, size_t _Begin, size_t _End>
struct slice : check_range_valid<_TyLst, _Begin, _End>, slice<pop_first_t<_TyLst>, _Begin - 1, _End - 1> {};

template <_type_list _TyLst, size_t _End>
struct slice<_TyLst, 0, _End> : prepend<typename slice<pop_first_t<_TyLst>, 0, _End - 1>::type, first_t<_TyLst>> {};

template <_type_list _TyLst>
struct slice<_TyLst, 0, 1> : type_list<first_t<_TyLst>> {};

template <_type_list _TyLst>
struct slice<_TyLst, 0, 0> : type_list<> {};

template <_type_list _TyLst, size_t _Begin, size_t _End>
using slice_t = typename slice<_TyLst, _Begin, _End>::type;
/****************************************slice****************************************/

/****************************************erase****************************************/
template <_type_list _TyLst, size_t _Begin, size_t _End>
struct erase : check_range_valid<_TyLst, _Begin, _End>, prepend<typename erase<pop_first_t<_TyLst>, _Begin - 1, _End - 1>::type, first_t<_TyLst>> {};

template <_type_list _TyLst, size_t _End>
struct erase<_TyLst, 0, _End> : erase<pop_first_t<_TyLst>, 0, _End - 1> {};

template <_type_list _TyLst>
struct erase<_TyLst, 0, 1> : pop_first<_TyLst> {};

template <_type_list _TyLst>
struct erase<_TyLst, 0, 0> : _TyLst {};

template <_type_list _TyLst, size_t _Begin, size_t _End>
using erase_t = typename erase<_TyLst, _Begin, _End>::type;
/****************************************erase****************************************/

/****************************************reverse****************************************/
template <_type_list _TyLst, size_t _Begin = 0, size_t _End = length_v<_TyLst>>
struct reverse : check_range_valid<_TyLst, _Begin, _End>, prepend<typename reverse<pop_first_t<_TyLst>, _Begin - 1, _End - 1>::type, first_t<_TyLst>> {};

template <_type_list _TyLst, size_t _End>
struct reverse<_TyLst, 0, _End> : reverse<pop_first_t<insert_t<_TyLst, _End, first_t<_TyLst>>>, 0, _End - 1> {};

template <_type_list _TyLst>
struct reverse<_TyLst, 0, 1> : check_not_empty<_TyLst>, _TyLst {};

template <_type_list _TyLst>
struct reverse<_TyLst, 0, 0> : _TyLst {};

template <_type_list _TyLst, size_t _Begin = 0, size_t _End = length_v<_TyLst>>
using reverse_t = typename reverse<_TyLst, _Begin, _End>::type;
/****************************************reverse****************************************/

/****************************************flatten****************************************/
template <_type_list _TyLst>
struct flatten : prepend<typename flatten<pop_first_t<_TyLst>>::type, first_t<_TyLst>> {};

template <_type_list _First, typename... _Rest>
struct flatten<type_list<_First, _Rest...>> : flatten<concat_t<_First, type_list<_Rest...>>> {};

template <>
struct flatten<type_list<>> : type_list<> {};

template <_type_list _TyLst>
using flatten_t = typename flatten<_TyLst>::type;
/****************************************flatten****************************************/

/****************************************apply****************************************/
template <_type_list _TyLst, template <typename> class _Func>
struct apply : _TyLst {};

template <typename... _Types, template <typename> class _Func>
struct apply<type_list<_Types...>, _Func> : type_list<typename _Func<_Types>::type...> {};

template <_type_list _TyLst, template <typename> class _Func>
using apply_t = typename apply<_TyLst, _Func>::type;
/****************************************apply****************************************/

/****************************************filter****************************************/
template <_type_list _TyLst, template <typename> class _Func>
struct filter
{
private:
	using _Rest = typename filter<pop_first_t<_TyLst>, _Func>::value;
public:
	using type = std::conditional_t<_Func<first_t<_TyLst>>::value, prepend_t<_Rest, first_t<_TyLst>>, _Rest>;
};

template <template <typename> class _Func>
struct filter<type_list<>, _Func> : type_list<> {};

template <_type_list _TyLst, template <typename> class _Func>
using filter_t = typename filter<_TyLst, _Func>::type;
/****************************************filter****************************************/

/****************************************remove_if****************************************/
template <_type_list _TyLst, template <typename> class _Func, size_t _Count = 0>
struct remove_if
{
private:
	using _Satisfied = typename remove_if<pop_first_t<_TyLst>, _Func, _Count - 1>::type;
	using _Unsatisfied = prepend_t<typename remove_if<pop_first_t<_TyLst>, _Func, _Count>::type, first_t<_TyLst>>;
public:
	using type = std::conditional_t<_Func<first_t<_TyLst>>::value, _Satisfied, _Unsatisfied>;
};

template <_type_list _TyLst, template <typename> class _Func>
struct remove_if<_TyLst, _Func, 1>
{
private:
	using _Unsatisfied = prepend_t<typename remove_if<pop_first_t<_TyLst>, _Func, 1>::type, first_t<_TyLst>>;
public:
	using type = std::conditional_t<_Func<first_t<_TyLst>>::value, pop_first_t<_TyLst>, _Unsatisfied>;
};

template <template <typename> class _Func, size_t _Count>
struct remove_if<type_list<>, _Func, _Count> : type_list<> {};

template <_type_list _TyLst, template <typename> class _Func, size_t _Count = 0>
using remove_if_t = typename remove_if<_TyLst, _Func, _Count>::type;
/****************************************remove_if****************************************/

/****************************************remove****************************************/
template <_type_list _TyLst, typename _Ty, size_t _Count = 0>
struct remove_ : remove_if<_TyLst, _SameAs<_Ty>::template _, _Count> {};

template <_type_list _TyLst, typename _Ty, size_t _Count = 0>
using remove_t = typename remove_<_TyLst, _Ty, _Count>::type;
/****************************************remove****************************************/

/****************************************unique****************************************/
template <_type_list _TyLst>
struct unique : prepend<typename unique<remove_t<_TyLst, first_t<_TyLst>>>::type, first_t<_TyLst>> {};

template <>
struct unique<type_list<>> : type_list<> {};

template <_type_list _TyLst>
using unique_t = typename unique<_TyLst>::type;
/****************************************unique****************************************/

/****************************************replace_if****************************************/
template <_type_list _TyLst, template <typename> class _Func, typename _NewType, size_t _Count = 0>
struct replace_if
{
private:
	using _Satisfied = prepend_t<typename replace_if<pop_first_t<_TyLst>, _Func, _NewType, _Count - 1>::type, _NewType>;
	using _Unsatisfied = prepend_t<typename replace_if<pop_first_t<_TyLst>, _Func, _NewType, _Count>::type, first_t<_TyLst>>;
public:
	using type = std::conditional_t<_Func<first_t<_TyLst>>::value, _Satisfied, _Unsatisfied>;
};

template <_type_list _TyLst, template <typename> class _Func, typename _NewType>
struct replace_if<_TyLst, _Func, _NewType, 1>
{
private:
	using _Rest = prepend_t<pop_first_t<_TyLst>, _NewType>;
	using _Unsatisfied = prepend_t<typename replace_if<_Rest, _Func, _NewType, 1>::type, first_t<_TyLst>>;
public:
	using type = std::conditional_t<_Func<first_t<_TyLst>>::value, _Rest, _Unsatisfied>;
};

template <template <typename> class _Func, typename _NewType, size_t _Count>
struct replace_if<type_list<>, _Func, _NewType, _Count> : type_list<> {};

template <_type_list _TyLst, template <typename> class _Func, typename _NewType, size_t _Count = 0>
using replace_if_t = typename replace_if<_TyLst, _Func, _NewType, _Count>::type;
/****************************************replace_if****************************************/

/****************************************replace****************************************/
template <_type_list _TyLst, typename _Ty, typename _NewType, size_t _Count = 0>
struct replace : replace_if<_TyLst, _SameAs<_Ty>::template _, _NewType, _Count> {};

template <_type_list _TyLst, typename _Ty, typename _NewType, size_t _Count = 0>
using replace_t = typename replace<_TyLst, _Ty, _NewType, _Count>::type;
/****************************************replace****************************************/

/****************************************select_sort****************************************/
template <_type_list _TyLst, template <typename, typename> class _Compare>
struct _Select
{
private:
	using _Rest_t = typename _Select<pop_first_t<_TyLst>, _Compare>::type;
public:
	using type = std::conditional_t<_Compare<first_t<_TyLst>, _Rest_t>::value, first_t<_TyLst>, _Rest_t>;
};

template <typename _Ty, template <typename, typename> class _Compare>
struct _Select<type_list<_Ty>, _Compare> : identity<_Ty> {};

template <_type_list _TyLst, template <typename, typename> class _Compare>
struct select_sort
{
private:
	using _Selected = typename _Select<_TyLst, _Compare>::type;
	using _Rest = typename select_sort<pop_t<_TyLst, locate_v<_TyLst, _Selected>>, _Compare>::type;
public:
	using type = prepend_t<_Rest, _Selected>;
};

template <template <typename, typename> class _Compare>
struct select_sort<type_list<>, _Compare> : type_list<> {};

template <_type_list _TyLst, template <typename, typename> class _Compare>
using select_sort_t = typename select_sort<_TyLst, _Compare>::type;
/****************************************select_sort****************************************/

/****************************************bubble_sort****************************************/
template <_type_list _TyLst, template <typename, typename> class _Compare, size_t _Count>
struct _Bubble;

template <typename _First, typename _Second, typename... _Rest, template <typename, typename> class _Compare, size_t _Count>
struct _Bubble<type_list<_First, _Second, _Rest...>, _Compare, _Count>
{
private:
	using _Satisfied = prepend_t<typename _Bubble<type_list<_Second, _Rest...>, _Compare, _Count - 1>::type, _First>;
	using _Unsatisfied = prepend_t<typename _Bubble<type_list<_First, _Rest...>, _Compare, _Count - 1>::type, _Second>;
public:
	using type = std::conditional_t<_Compare<_First, _Second>::value, _Satisfied, _Unsatisfied>;
};

template <typename _First, typename _Second, typename... _Rest, template <typename, typename> class _Compare>
struct _Bubble<type_list<_First, _Second, _Rest...>, _Compare, 0>
{
private:
	using _Satisfied = type_list<_First, _Second, _Rest...>;
	using _Unsatisfied = type_list<_Second, _First, _Rest...>;
public:
	using type = std::conditional_t<_Compare<_First, _Second>::value, _Satisfied, _Unsatisfied>;
};

template <_type_list _TyLst, template <typename, typename> class _Compare, size_t _Count>
struct _Bubble_sort : _Bubble_sort<typename _Bubble<_TyLst, _Compare, _Count>::type, _Compare, _Count - 1> {};

template <_type_list _TyLst, template <typename, typename> class _Compare>
struct _Bubble_sort<_TyLst, _Compare, 0> : _Bubble<_TyLst, _Compare, 0> {};

template <_type_list _TyLst, template <typename, typename> class _Compare>
struct bubble_sort : _Bubble_sort<_TyLst, _Compare, length_v<_TyLst> - 2> {};

template <typename _Ty, template <typename, typename> class _Compare>
struct bubble_sort<type_list<_Ty>, _Compare> : type_list<_Ty> {};

template <template <typename, typename> class _Compare>
struct bubble_sort<type_list<>, _Compare> : type_list<> {};

template <_type_list _TyLst, template <typename, typename> class _Compare>
using bubble_sort_t = typename bubble_sort<_TyLst, _Compare>::type;
/****************************************bubble_sort****************************************/

template <_type_list _TyLs>
struct ascend_by_size : select_sort<_TyLs, le_in_size> {};

template <_type_list _TyLs>
using ascend_by_size_t = typename ascend_by_size<_TyLs>::type;

template <_type_list _TyLs>
struct descend_by_size : select_sort<_TyLs, ge_in_size> {};

template <_type_list _TyLs>
using descend_by_size_t = typename descend_by_size<_TyLs>::type;
