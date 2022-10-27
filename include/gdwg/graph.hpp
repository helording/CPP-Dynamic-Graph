#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <algorithm>
#include <concepts>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

// for testing genericness of graph
template<typename T>
auto operator<<(std::ostream& os, const std::vector<T>& collection) noexcept -> std::ostream& {
	auto it = collection.begin();

	os << "[";
	while (it != collection.end()) {
		os << " " << *it;
		++it;
	}
	os << "]" << std::endl;
	return os;
}

namespace gdwg {

	template<typename T, typename P>
	class PointerComparator {
	public:
		using is_transparent = void;

		bool operator()(const T& left, const T& right) const {
			return (*left < *right);
		}

		bool operator()(const T& left, P right) const {
			return (*left < right);
		}

		bool operator()(P left, const T& right) const {
			return (left < *right);
		}
	};

	template<typename T, typename P>
	class PairPointersComparator {
	public:
		using is_transparent = void;

		bool operator()(const std::pair<T, T> left, const std::pair<T, T> right) const {
			return (*(left.first) == *(right.first)) ? *(left.second) < *(right.second)
			                                         : *(left.first) < *(right.first);
		}

		bool operator()(const std::pair<T, T> left, std::pair<P, P> right) const {
			return (*(left.first) == (right.first)) ? *(left.second) < (right.second)
			                                        : *(left.first) < (right.first);
		}

		bool operator()(std::pair<P, P> left, const std::pair<T, T> right) const {
			return ((left.first) == *(right.first)) ? (left.second) < *(right.second)
			                                        : (left.first) < *(right.first);
		}

		bool operator()(const std::pair<T, T> left, P right) const {
			return *(left.first) < right;
		}

		bool operator()(P left, const std::pair<T, T> right) const {
			return left < *(right.first);
		}
	};

	template<typename N, typename E>
	class graph {
	public:
		struct value_type {
			value_type() = default;
			value_type(N f, N t, E w)
			: from(f)
			, to(t)
			, weight(w){};
			N from;
			N to;
			E weight;
			[[nodiscard]] auto operator==(value_type const& other) const noexcept -> bool = default;

			friend auto operator<<(std::ostream& os, value_type const& v) noexcept -> std::ostream& {
				os << "(" << v.from << " " << v.to << " " << v.weight << ")";
				return os;
			}
		};

	private:
		class iterator {
			using outer_iter =
			   typename std::map<std::pair<std::shared_ptr<N>, std::shared_ptr<N>>,
			                     std::set<std::shared_ptr<E>, PointerComparator<std::shared_ptr<E>, E>>,
			                     PairPointersComparator<std::shared_ptr<N>, N>>::const_iterator;
			using inner_iter =
			   typename std::set<std::shared_ptr<E>, PointerComparator<std::shared_ptr<E>, E>>::const_iterator;

		public:
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator() = default;
			iterator(outer_iter o_iter = {},
			         inner_iter i_iter = {},
			         outer_iter o_rend = {},
			         outer_iter o_end = {})
			: outer_iter_{o_iter}
			, inner_iter_{i_iter}
			, outer_iter_rend_{o_rend}
			, outer_iter_end_{o_end} {}

			auto operator*() -> reference {
				auto res = value_type();
				res.from = *(*outer_iter_).first.first;
				res.to = *(*outer_iter_).first.second;
				res.weight = *(*inner_iter_);
				return res;
			}

			auto operator++() -> iterator& {
				if (++inner_iter_ == outer_iter_->second.cend()) {
					++outer_iter_;
					inner_iter_ = (outer_iter_ == outer_iter_end_) ? inner_iter{}
					                                               : outer_iter_->second.cbegin();
				}
				return *this;
			}

			auto operator++(int) -> iterator {
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			auto operator--() -> iterator& {
				if ((inner_iter_ == inner_iter{} && outer_iter_ == outer_iter_end_)
				    || (inner_iter_ == outer_iter_->second.cbegin()))
				{
					if (outer_iter_-- == outer_iter_rend_) {
						inner_iter_ = inner_iter{};
						outer_iter_ = outer_iter_rend_;
					}
					else {
						auto iter = outer_iter_->second.cend();
						inner_iter_ = --iter;
					}
				}
				else {
					--inner_iter_;
				}

				return *this;
			}

			auto operator--(int) -> iterator {
				iterator tmp = *this;
				--(*this);
				return tmp;
			}

			auto operator==(iterator const& other) const noexcept -> bool = default;

		private:
			outer_iter outer_iter_;
			inner_iter inner_iter_;
			outer_iter outer_iter_rend_;
			outer_iter outer_iter_end_;

			friend class graph;

			auto get_inner() const noexcept -> inner_iter {
				return this->inner_iter_;
			};
			auto get_outer() const noexcept -> outer_iter {
				return this->outer_iter_;
			};
		};

	public:
		using iter = iterator;
		using reverse_iterator = std::reverse_iterator<iter>;

		graph() noexcept = default;
		graph(std::initializer_list<N> il);
		template<typename InputIt>
		graph(InputIt first, InputIt last);
		graph(graph const& other);
		graph(graph&& other) noexcept;
		auto operator=(graph const& other) -> graph&;
		auto operator=(graph&& other) noexcept -> graph&;

		[[nodiscard]] auto operator==(graph const& other) const noexcept -> bool;

		[[nodiscard]] auto is_node(N const& value) const noexcept -> bool;
		[[nodiscard]] auto empty() const noexcept -> bool;
		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool;
		[[nodiscard]] auto nodes() const noexcept -> std::vector<N>;
		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E>;
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const noexcept -> iterator;
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N>;

		auto insert_node(N const& value) -> bool;
		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool;
		auto replace_node(N const& old_data, N const& new_data) -> bool;
		auto merge_replace_node(N const& old_data, N const& new_data) -> void;
		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool;
		auto erase_node(N const& value) noexcept -> bool;
		auto erase_edge(iterator i) noexcept -> iterator;
		auto erase_edge(iterator i, iterator s) noexcept -> iterator;
		auto clear() noexcept -> void;

		[[nodiscard]] auto begin() const -> iter {
			return edges_.cbegin() == edges_.cend() ? end()
			                                        : iter{edges_.cbegin(),
			                                               edges_.cbegin()->second.begin(),
			                                               edges_.cbegin(),
			                                               edges_.cend()};
		}

		[[nodiscard]] auto end() const -> iter {
			return iter{edges_.cend(), {}, edges_.cbegin(), edges_.cend()};
		}

		friend auto operator<<(std::ostream& os, graph<N, E> const& g) noexcept -> std::ostream& {
			std::for_each(g.nodes_.begin(), g.nodes_.end(), [&](const auto& node) {
				os << *node << " (\n";
				print_edges(*node, os, g);
				os << ")\n";
			});

			return os;
		}

		friend auto print_edges(N node, std::ostream& os, graph<N, E> const& g) noexcept
		   -> std::ostream& {
			auto edge_it = std::find_if(g.begin(), g.end(), [&](auto e) { return ((e.from == node)); });
			if (edge_it != g.end()) {
				while (edge_it != g.end() && (*edge_it).from == node) {
					os << "  " << (*edge_it).to << " | " << (*edge_it).weight << "\n";
					++edge_it;
				}
			}
			return os;
		}

	private:
		using nodes_type = std::set<std::shared_ptr<N>, PointerComparator<std::shared_ptr<N>, N>>;
		using edges_type =
		   std::map<std::pair<std::shared_ptr<N>, std::shared_ptr<N>>,
		            std::set<std::shared_ptr<E>, PointerComparator<std::shared_ptr<E>, E>>,
		            PairPointersComparator<std::shared_ptr<N>, N>>;
		using weights_type = std::set<std::shared_ptr<E>, PointerComparator<std::shared_ptr<E>, E>>;

		nodes_type nodes_;
		edges_type edges_;

		auto swap(graph& other) noexcept -> void;
		[[nodiscard]] auto get_node_ptr(N const& value) const noexcept -> std::shared_ptr<N>;
		[[nodiscard]] auto find_weight(E const& weight) const noexcept -> std::shared_ptr<E>;
		auto extract_edges(N const& value) noexcept -> edges_type;

		[[nodiscard]] auto get_iterator(typename edges_type::const_iterator o_it,
		                                typename weights_type::const_iterator i_it) const noexcept
		   -> iter {
			return iter{o_it, i_it, edges_.cbegin(), edges_.cend()};
		}
	};

	template<typename N, typename E>
	graph<N, E>::graph(std::initializer_list<N> il)
	: graph(il.begin(), il.end()){};

	template<typename N, typename E>
	template<typename InputIt>
	graph<N, E>::graph(InputIt first, InputIt last) {
		std::transform(first, last, std::inserter(nodes_, nodes_.end()), [](auto& node) {
			return std::make_shared<N>(node);
		});
	};

	template<typename N, typename E>
	graph<N, E>::graph(graph const& other) {
		std::transform(other.nodes_.begin(),
		               other.nodes_.end(),
		               std::inserter(this->nodes_, this->nodes_.end()),
		               [&](auto& node) { return std::make_shared<N>(*(node)); });

		std::for_each(other.edges_.begin(), other.edges_.end(), [&](auto& pair) {
			std::for_each(pair.second.begin(), pair.second.end(), [&](auto& weight) {
				this->insert_edge(*(pair.first.first), *(pair.first.second), *weight);
			});
		});
	}

	template<typename N, typename E>
	graph<N, E>::graph(graph&& other) noexcept
	: nodes_(std::exchange(other.nodes_, nodes_type{}))
	, edges_(std::exchange(other.edges_, edges_type{})) {}

	template<typename N, typename E>
	void graph<N, E>::swap(graph& other) noexcept {
		std::swap(this->nodes_, other.nodes_);
		std::swap(this->edges_, other.edges_);
	}

	template<typename N, typename E>
	auto graph<N, E>::operator=(graph const& other) -> graph& {
		graph(other).swap(*this);
		return *this;
	}

	template<typename N, typename E>
	auto graph<N, E>::operator=(graph&& other) noexcept -> graph& {
		this->swap(other);

		other.nodes_ = nodes_type{};
		other.edges_ = edges_type{};
		return *this;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::operator==(graph const& other) const noexcept -> bool {
		if (this->nodes_.size() != other.nodes_.size())
			return false;
		if (this->edges_.size() != other.edges_.size())
			return false;

		auto other_nodes_iter = other.nodes_.begin();
		if (std::all_of(this->nodes_.begin(),
		                this->nodes_.end(),
		                [&](auto& node) { return *node == *(*other_nodes_iter++); })
		    == false)
			return false;

		auto o_iter = other.begin();
		if (std::all_of(this->begin(), this->end(), [&](auto e) { return e == *o_iter++; }) == true) {
			return (o_iter == other.end()) ? true : false;
		}
		else {
			return false;
		}
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::is_node(N const& value) const noexcept -> bool {
		return (this->nodes_.find(value) != this->nodes_.end()) ? true : false;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::get_node_ptr(N const& value) const noexcept -> std::shared_ptr<N> {
		return *(this->nodes_.find(value));
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::empty() const noexcept -> bool {
		return nodes_.empty();
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::is_connected(N const& src, N const& dst) const -> bool {
		if ((is_node(src) == false) || (is_node(dst) == false)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
			                         "don't exist in the graph");
		}

		return (edges_.find(std::pair{src, dst}) != this->edges_.end()) ? true : false;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::nodes() const noexcept -> std::vector<N> {
		auto ret = std::vector<N>();
		std::transform(this->nodes_.begin(),
		               this->nodes_.end(),
		               std::back_inserter(ret),
		               [](auto& node) { return *node; });
		return ret;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::weights(N const& src, N const& dst) const -> std::vector<E> {
		if ((is_node(src) == false) || (is_node(dst) == false)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node don't "
			                         "exist in the graph");
		}

		auto ret = std::vector<E>();
		auto edge = edges_.find(std::pair{src, dst});
		std::transform(edge->second.begin(),
		               edge->second.end(),
		               std::back_inserter(ret),
		               [](auto& node) { return *node; });
		return ret;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::find(N const& src, N const& dst, E const& weight) const noexcept
	   -> iterator {
		auto edge_iter = edges_.find(std::pair{src, dst});
		if (edge_iter == edges_.end())
			return this->end();

		auto weight_iter = edge_iter->second.find(weight);
		if (weight_iter == edge_iter->second.end())
			return this->end();

		return get_iterator(edge_iter, weight_iter);
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::connections(N const& src) const -> std::vector<N> {
		if (is_node(src) == false)
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist "
			                         "in the graph");

		auto v = std::vector<N>{};
		auto edge_iter = edges_.find(src);

		// traditional for-loop as range unknown before-hand
		for (auto it = edge_iter; it != this->edges_.end(); it++) {
			if (*it->first.first == src) {
				v.push_back(*it->first.second);
			}
			else {
				break;
			}
		}

		return v;
	}

	template<typename N, typename E>
	auto graph<N, E>::insert_node(N const& value) -> bool {
		if (this->nodes_.find(value) != this->nodes_.end())
			return false;
		this->nodes_.insert(std::make_shared<N>(value));
		return true;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::find_weight(E const& weight) const noexcept -> std::shared_ptr<E> {
		// traditional for-loop function to return sooner
		for (auto& edge : this->edges_) {
			auto found = edge.second.find(weight);
			if (found != edge.second.end()) {
				return *found;
			}
		}

		return {};
	}

	template<typename N, typename E>
	auto graph<N, E>::insert_edge(N const& src, N const& dst, E const& weight) -> bool {
		if ((is_node(src) == false) || (is_node(dst) == false)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or "
			                         "dst node does not exist");
		}

		if (this->find(src, dst, weight) != this->end())
			return false;

		auto weight_ptr = this->find_weight(weight);
		if (!weight_ptr)
			weight_ptr = std::make_shared<E>(weight);

		auto edge = this->edges_.find(std::pair{src, dst});
		if (edge == this->edges_.end()) {
			this->edges_.insert(
			   std::pair{std::pair{get_node_ptr(src), get_node_ptr(dst)}, weights_type({weight_ptr})});
		}
		else {
			edge->second.insert(weight_ptr);
		}

		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::extract_edges(N const& value) noexcept -> edges_type {
		auto extracted_edges = edges_type{};
		// To prevent iterator invalidation issues and therefore be able to extract edges in O(e)
		// time, tradition for-loop is used
		for (auto it = this->edges_.begin(), it_end = this->edges_.end(); it != it_end;) {
			auto tmp_it = it++;
			if ((*tmp_it->first.first == value) || (*tmp_it->first.second == value)) {
				extracted_edges.insert(*tmp_it);
				this->edges_.erase(tmp_it);
			}
		}

		return extracted_edges;
	}

	template<typename N, typename E>
	auto graph<N, E>::replace_node(N const& old_data, N const& new_data) -> bool {
		if (is_node(old_data) == false) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
			                         "doesn't exist");
		}

		if (this->nodes_.find(new_data) != this->nodes_.end())
			return false;

		// Remove affected edges so later we can maintain sorted order of graph
		auto edges_removed = extract_edges(old_data);

		auto found_node = this->nodes_.find(old_data);
		auto data_ptr = *found_node;
		this->nodes_.erase(found_node);
		*data_ptr = new_data;

		this->nodes_.insert(data_ptr);
		std::for_each(edges_removed.begin(), edges_removed.end(), [&](auto& edge) {
			this->edges_.insert(edge);
		});

		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::merge_replace_node(N const& old_data, N const& new_data) -> void {
		if ((is_node(old_data) == false) || (is_node(new_data) == false)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new "
			                         "data if they don't exist in the graph");
		}

		if (old_data == new_data)
			return;

		// Remove affected edges so later we can maintain sorted order of graph
		auto edges_removed = extract_edges(old_data);

		auto found_node = this->nodes_.find(old_data);
		auto data_ptr = *found_node;
		this->nodes_.erase(found_node);
		*data_ptr = new_data;

		std::for_each(edges_removed.begin(), edges_removed.end(), [&](auto& edge) {
			auto found = this->edges_.find(edge.first);
			if (found != this->edges_.end()) {
				std::for_each(edge.second.begin(), edge.second.end(), [&](auto& w) {
					found->second.insert(w);
				});
			}
			else {
				this->edges_.insert(edge);
			}
		});
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_node(N const& value) noexcept -> bool {
		if (is_node(value) == false)
			return false;

		extract_edges(value);
		this->nodes_.erase(this->nodes_.find(value));

		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(N const& src, N const& dst, E const& weight) -> bool {
		if ((is_node(src) == false) || (is_node(dst) == false)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they "
			                         "don't exist in the graph");
		}

		auto edge_it = this->edges_.find(std::pair{src, dst});
		if (edge_it == this->edges_.end())
			return false;

		auto weight_set_it = edge_it->second.find(weight);
		if (weight_set_it == edge_it->second.end())
			return false;

		edge_it->second.erase(weight_set_it);
		if (edge_it->second.empty()) {
			this->edges_.erase(edge_it);
		}

		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i) noexcept -> iterator {
		if (i == this->end())
			return this->end();

		auto inner_it = i.get_inner();
		auto const_outer_it = i.get_outer();
		// hack to wipe const to access weight set
		auto outer_it = this->edges_.erase(const_outer_it, const_outer_it);
		++i;

		outer_it->second.erase(inner_it);

		if (outer_it->second.empty()) {
			this->edges_.erase(outer_it);
		}

		return get_iterator(i.get_outer(), i.get_inner());
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i, iterator s) noexcept -> iterator {
		if (i == this->end())
			return this->end();
		if (i == s)
			return s;

		while (i != get_iterator(s.get_outer(), s.get_inner()))
			i = erase_edge(i);

		if (s == this->end()) {
			return this->end();
		}
		else {
			return get_iterator(s.get_outer(), s.get_inner());
		}
	}

	template<typename N, typename E>
	auto graph<N, E>::clear() noexcept -> void {
		while (!this->nodes_.empty())
			this->erase_node(*(*this->nodes_.begin()));
	}

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
