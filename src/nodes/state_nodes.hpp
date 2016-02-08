#ifndef SRC_NODES_STATE_NODES_HPP_
#define SRC_NODES_STATE_NODES_HPP_

#include <core/traits.hpp>
#include <ports/ports.hpp>
#include <nodes/base_node.hpp>

#include <utility>
#include <tuple>
#include <memory>
#include <cstddef>

namespace fc
{

/**
 * \brief merges all input states to one output state by given operation
 *
 * \tparam operation operation to apply to all inputs, returns desired output.
 *
 * example:
 * \code{.cpp}
 *	auto multiply = merge([](int a, int b){return a*b;});
 *	[](){ return 3;} >> multiply.in<0>();
 *	[](){ return 2;} >> multiply.in<1>();
 *	BOOST_CHECK_EQUAL(multiply(), 6);
 * \endcode
 */
template<class operation, class signature>
struct merge_node;

template<class operation, class result, class... args>
struct merge_node<operation, result (args...)> : public tree_base_node
{
	typedef std::tuple<args...> arguments;
	typedef std::tuple<state_sink<args> ...> in_ports_t;
	typedef result result_type;
	static constexpr auto nr_of_arguments = sizeof...(args);

	static_assert(nr_of_arguments > 0,
			"Tried to create merge_node with a function taking no arguments");

    explicit merge_node(operation o)
		: tree_base_node("merger")
  		, in_ports(state_sink<args>(this)...)
		, op(o)
	{}

	///calls all in ports, converts their results from tuple to varargs and calls operation
	result_type operator()()
	{
		return invoke_helper(in_ports, std::make_index_sequence<nr_of_arguments>{});
	}

	template<size_t i>
	auto& in() noexcept { return std::get<i>(in_ports); }

protected:
	in_ports_t in_ports;
	operation op;

private:
	///Helper function to get varargs index from nr of arguments by type deduction.
	template<class tuple, std::size_t... index>
	decltype(auto) invoke_helper(tuple&& tup, std::index_sequence<index...>)
	{
		return op(std::get<index>(std::forward<tuple>(tup)).get()...);
	}
};

///creats a merge node which applies the operation to all inputs and returns single state.
template<class parent_t, class operation>
auto make_merge(parent_t& parent, operation op)
{
	typedef merge_node
			<	operation,
				typename utils::function_traits<operation>::function_type
			> node_t;
	return parent.template make_child<node_t>(op);
}

} // namespace fc

#endif /* SRC_NODES_STATE_NODES_HPP_ */