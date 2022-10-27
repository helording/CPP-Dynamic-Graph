#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

using vt = typename gdwg::graph<int, int>::value_type;

TEST_CASE("empty() test") {
	auto g = gdwg::graph<int, int>{};

	CHECK(g.empty() == true);
}

TEST_CASE("is_node() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	CHECK(g.is_node(1) == true);
	CHECK(g.is_node(2) == true);
	CHECK(g.is_node(3) == true);
	CHECK(g.is_node(4) != true);
}

TEST_CASE("is_connected() test") {
	SECTION("is_connected() throws exception test") {
		auto g = gdwg::graph<int, int>{};

		CHECK_THROWS_MATCHES(g.is_connected(1, 2),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::is_connected if src or "
		                                    "dst node don't exist in the graph"));
	}

	SECTION("is_connected() on non-empty graph test") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(2, 3, 5);

		CHECK_THROWS_MATCHES(g.is_connected(1, 4),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::is_connected if src or "
		                                    "dst node don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.is_connected(2, -1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::is_connected if src or "
		                                    "dst node don't exist in the graph"));

		CHECK(g.is_connected(1, 3) == false);
		CHECK(g.is_connected(3, 1) == false);

		CHECK(g.is_connected(2, 3) == true);
	}
}

TEST_CASE("weights() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("weights() throws exception test") {
		auto empty = gdwg::graph<int, int>{};
		CHECK_THROWS_MATCHES(empty.weights(1, 2),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::weights if src or dst "
		                                    "node don't exist in the graph"));
	}

	SECTION("weights() on edge with single weight test") {
		g.insert_edge(1, 2, 5);

		CHECK(g.weights(1, 2) == std::vector<int>{5});
	}

	SECTION("weights() on edge with multiple weights test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		CHECK(g.weights(1, 2) == std::vector<int>{5, 6, 7});
	}

	SECTION("weights() output is sorted test") {
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);

		CHECK(g.weights(1, 2) == std::vector<int>{5, 6, 7});
	}

	SECTION("weights() on graph with edges between different src and dest nodes") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(2, 3, 11);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);

		CHECK(g.weights(1, 2) == std::vector<int>{5, 6, 7});
		CHECK(g.weights(2, 1) == std::vector<int>{7});
		CHECK(g.weights(2, 3) == std::vector<int>{11});
		CHECK(g.weights(3, 1) == std::vector<int>{7});
		CHECK(g.weights(3, 2) == std::vector<int>{7, 8});
	}
}

TEST_CASE("connections() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};

	SECTION("connections() throws exception test") {
		auto empty = gdwg::graph<int, int>{};
		CHECK_THROWS_MATCHES(empty.connections(1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::connections if src "
		                                    "doesn't exist in the graph"));
		CHECK_THROWS_MATCHES(g.connections(99),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::connections if src "
		                                    "doesn't exist in the graph"));
	}

	SECTION("connections() on graph having no weights test") {
		CHECK(g.connections(1) == std::vector<int>{});
		CHECK(g.connections(2) == std::vector<int>{});
		CHECK(g.connections(3) == std::vector<int>{});
	}

	SECTION("connections() on graph having single weight test") {
		g.insert_edge(1, 2, 5);
		CHECK(g.connections(1) == std::vector<int>{2});
		CHECK(g.connections(2) == std::vector<int>{});
		CHECK(g.connections(5) == std::vector<int>{});
	}

	SECTION("connections() on graph having multiple weights between same src and dest test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		CHECK(g.connections(1) == std::vector<int>{2});
	}

	SECTION("connections() on graph with src having multiple outgoing edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 6);
		g.insert_edge(1, 4, 2);

		CHECK(g.connections(1) == std::vector<int>{2, 3, 4});
	}

	SECTION("connections() on graph with node having edge to self test") {
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 6);
		g.insert_edge(1, 4, 2);

		CHECK(g.connections(1) == std::vector<int>{1, 2, 3, 4});
	}

	SECTION("connections() outputs result in ascending order test") {
		g.insert_edge(1, 4, 2);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 6);

		CHECK(g.connections(1) == std::vector<int>{2, 3, 4});
	}

	SECTION("connections() on graph with node having both outgoing and incoming edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 6);
		g.insert_edge(1, 4, 2);

		g.insert_edge(4, 1, 5);
		g.insert_edge(6, 1, 6);
		g.insert_edge(6, 1, 2);
		g.insert_edge(5, 1, 2);

		CHECK(g.connections(1) == std::vector<int>{2, 3, 4});
		CHECK(g.connections(2) == std::vector<int>{});
		CHECK(g.connections(3) == std::vector<int>{});
		CHECK(g.connections(4) == std::vector<int>{1});
		CHECK(g.connections(5) == std::vector<int>{1});
		CHECK(g.connections(6) == std::vector<int>{1});
	}

	SECTION("connections() on graph with multiple nodes with outgoing and incoming edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(2, 2, 2);
		g.insert_edge(2, 3, 11);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);
		g.insert_edge(4, 1, 5);
		g.insert_edge(5, 1, 2);
		g.insert_edge(6, 1, 6);
		g.insert_edge(6, 6, 2);
		g.insert_edge(6, 5, 2);
		g.insert_edge(6, 4, 2);
		g.insert_edge(6, 3, 2);
		g.insert_edge(6, 2, 2);
		g.insert_edge(6, 1, 2);

		CHECK(g.connections(1) == std::vector<int>{2});
		CHECK(g.connections(2) == std::vector<int>{1, 2, 3});
		CHECK(g.connections(3) == std::vector<int>{1, 2});
		CHECK(g.connections(4) == std::vector<int>{1});
		CHECK(g.connections(5) == std::vector<int>{1});
		CHECK(g.connections(6) == std::vector<int>{1, 2, 3, 4, 5, 6});
	}
}