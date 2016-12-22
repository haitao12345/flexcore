#ifndef SRC_GRAPH_GRAPH_HPP_
#define SRC_GRAPH_GRAPH_HPP_

#include <boost/uuid/uuid.hpp>
#include <flexcore/scheduler/parallelregion.hpp>

#include <map>
#include <set>

namespace fc
{
/**
 * \brief Contains all classes and functions to access
 * and read the abstract connection graph of a flexcore application.
 */
namespace graph
{

/**
 * \brief Contains the information carried by a node of the dataflow graph
 */
class graph_node_properties
{
public:
	typedef boost::uuids::uuid unique_id;

	explicit graph_node_properties(const std::string& name,
			parallel_region* region,
			unique_id id);

	explicit graph_node_properties(const std::string& name, parallel_region* region);

	explicit graph_node_properties(const std::string& name) : graph_node_properties(name, nullptr)
	{
	}

	const std::string& name() const { return human_readable_name; }
	std::string& name() { return human_readable_name; }
	unique_id get_id() const { return id; }
	parallel_region* region() const { return region_; }
private:
	std::string human_readable_name;
	unique_id id;
	parallel_region* region_;
};

/**
 * \brief Contains the information carried by a port of the dataflow graph
 */
class graph_port_properties
{
public:
	typedef boost::uuids::uuid unique_id;

	explicit graph_port_properties(std::string description, unique_id owning_node);

	bool operator<(const graph_port_properties&) const;

	const std::string& description() const { return description_; }
	unique_id owning_node() const { return owning_node_; }
	unique_id id() const { return id_; }

private:
	std::string description_;
	unique_id owning_node_;
	unique_id id_;
};

/**
 * \brief The abstract connection graph of a flexcore application.
 *
 * Contains all nodes which where declared with the additional information
 * and edges between these nodes.
 *
 * \invariant Number of vertices/nodes in dataflow_graph == vertex_map.size().
 */
class connection_graph
{
public:
	connection_graph();
	connection_graph(const connection_graph&) = delete;

	/// Adds a new Connection without ports to the graph.
	void add_connection(const graph_node_properties& source_node,
	                    const graph_node_properties& sink_node);

	void add_port(graph_port_properties port_info);

	const std::set<graph_port_properties>& ports() const;

	/// Prints current state of the abstract graph in graphviz format to stream.
	void print(std::ostream& stream);

	/// deleted the current graph
	void clear_graph();

	~connection_graph();

private:
	struct impl;
	std::unique_ptr<impl> pimpl;
};

}  // namespace graph
}  // namespace fc

#endif /* SRC_GRAPH_GRAPH_HPP_ */
