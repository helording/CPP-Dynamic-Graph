#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

using vt = typename gdwg::graph<int, int>::value_type;

TEST_CASE("insert_node() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	CHECK(g.insert_node(1) == false);
	CHECK(g.insert_node(2) == false);
	CHECK(g.insert_node(3) == false);
	CHECK(g.insert_node(4) == true);
	CHECK(g.insert_node(4) == false);
	CHECK(g.insert_node(5) == true);
	CHECK(g.insert_node(6) == true);
}

TEST_CASE("erase_node() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};
	auto empty = gdwg::graph<int, int>{};

	SECTION("erase_node() returns false when value is not in graph test") {
		CHECK(empty.erase_node(1) == false);
		CHECK(g.erase_node(8) == false);
	}

	SECTION("erase_node() erases node on graph with no weights test") {
		CHECK(g.erase_node(1) == true);
		CHECK(g.erase_node(1) == false);
		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		CHECK(g.erase_node(5) == true);
		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 6});
	}

	SECTION("erase_node() erases node have outgoing edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 3, 9);

		CHECK(g.erase_node(1) == true);
		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(2, 3, 9) == *it);
		CHECK(g.end() == ++it);

		CHECK(g.erase_node(2) == true);
		CHECK(g.nodes() == std::vector<int>{3, 4, 5, 6});

		it = g.begin();
		CHECK(g.end() == it);
	}

	SECTION("erase_node() erases node have outgoing and incoming edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 1, 7);
		g.insert_edge(2, 1, 5);
		g.insert_edge(2, 2, 9);
		g.insert_edge(2, 3, 9);
		g.insert_edge(3, 1, 9);
		g.insert_edge(6, 1, 1);
		g.insert_edge(6, 2, 1);

		CHECK(g.erase_node(1) == true);
		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(2, 2, 9) == *it);
		CHECK(vt(2, 3, 9) == *++it);
		CHECK(vt(6, 2, 1) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.erase_node(3) == true);
		CHECK(g.nodes() == std::vector<int>{2, 4, 5, 6});

		it = g.begin();
		CHECK(vt(2, 2, 9) == *it);
		CHECK(vt(6, 2, 1) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.erase_node(2) == true);
		CHECK(g.nodes() == std::vector<int>{4, 5, 6});

		it = g.begin();
		CHECK(g.end() == it);
	}
}

TEST_CASE("erase_edge(N const&, N const&, E const&) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};
	auto empty = gdwg::graph<int, int>{};

	SECTION("erase_edge(N, N, E) throw exception correctly test") {
		CHECK_THROWS_MATCHES(empty.erase_edge(6, 3, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
		                                    "if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.erase_edge(0, 7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
		                                    "if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.erase_edge(6, 7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
		                                    "if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.erase_edge(7, 2, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::erase_edge on src or dst "
		                                    "if they don't exist in the graph"));
	}

	SECTION("erase_edge(N, N, E) returns false correctly test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 9);
		CHECK(g.erase_edge(1, 2, 9) == false);
		CHECK(g.erase_edge(2, 1, 7) == false);
		CHECK(g.erase_edge(3, 2, 6) == false);
		CHECK(g.erase_edge(1, 4, 6) == false);
		CHECK(g.erase_edge(4, 4, 6) == false);
		CHECK(g.erase_edge(1, 1, 7) == false);
		CHECK(g.erase_edge(4, 5, 6) == false);
	}

	SECTION("erase_edge(N, N, E) erases edge in graph with single weight test") {
		g.insert_edge(1, 2, 7);
		CHECK(g.erase_edge(1, 2, 6) == false);

		auto it = g.begin();
		CHECK(vt(1, 2, 7) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);

		CHECK(g.erase_edge(1, 2, 7) == true);

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(N, N, E) erases edge from node to itself test") {
		g.insert_edge(1, 1, 7);
		CHECK(g.erase_edge(1, 1, 6) == false);

		auto it = g.begin();
		CHECK(vt(1, 1, 7) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 1) == true);

		CHECK(g.erase_edge(1, 1, 7) == true);

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 1) == false);
	}

	SECTION("erase_edge(N, N, E) cannot erase edge twice test") {
		g.insert_edge(1, 2, 7);

		CHECK(g.erase_edge(1, 2, 7) == true);
		CHECK(g.erase_edge(1, 2, 7) == false);

		auto it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(N, N, E) erases edge between two nodes with multiple weights test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		CHECK(g.erase_edge(1, 2, 6) == false);

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 7) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);

		CHECK(g.erase_edge(1, 2, 7) == true);
		CHECK(g.erase_edge(1, 2, 7) == false);

		it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);

		CHECK(g.erase_edge(1, 2, 5) == true);

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(N, N, E) erases edge between two nodes with reverse edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 1, 4);
		g.insert_edge(2, 1, 5);
		CHECK(g.erase_edge(1, 2, 4) == false);
		CHECK(g.erase_edge(1, 2, 9) == false);
		CHECK(g.erase_edge(2, 1, 7) == false);
		CHECK(g.erase_edge(2, 1, 9) == false);

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 7) == *++it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(vt(2, 1, 5) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		CHECK(g.erase_edge(1, 2, 7) == true);

		it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(vt(2, 1, 5) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		CHECK(g.erase_edge(2, 1, 5) == true);

		it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		CHECK(g.erase_edge(1, 2, 5) == true);
		CHECK(g.is_connected(1, 2) == false);

		CHECK(g.erase_edge(2, 1, 4) == true);

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(2, 1) == false);
	}
}

TEST_CASE("erase_edge(iterator) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};
	auto empty = gdwg::graph<int, int>{};

	SECTION("erase_edge(iterator) on empty graph test") {
		auto it = empty.erase_edge(empty.begin());
		CHECK(empty.end() == it);
	}

	SECTION("erase_edge(iterator) pass end() to a non-empty graph test") {
		g.insert_edge(1, 2, 7);
		CHECK(g.erase_edge(g.end()) == g.end());

		auto it = g.begin();
		CHECK(vt(1, 2, 7) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
	}

	SECTION("erase_edge(iterator) erases edge in graph with single weight test") {
		g.insert_edge(1, 2, 7);

		CHECK(g.erase_edge(g.find(1, 2, 7)) == g.end());

		auto it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(iterator) erases edge from node to itself test") {
		g.insert_edge(1, 1, 7);

		CHECK(g.erase_edge(g.find(1, 1, 7)) == g.end());

		auto it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 1) == false);
	}

	SECTION("erase_edge(iterator) erases edge between two nodes with multiple weights test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);

		CHECK(g.erase_edge(g.find(1, 2, 5)) == g.find(1, 2, 7));

		auto it = g.begin();
		CHECK(vt(1, 2, 7) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);

		CHECK(g.erase_edge(g.find(1, 2, 7)) == g.end());

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(iterator) erases edge between two nodes with edges to each other test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 1, 4);
		g.insert_edge(2, 1, 5);

		CHECK(g.erase_edge(g.find(1, 2, 7)) == g.find(2, 1, 4));

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(vt(2, 1, 5) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		CHECK(g.erase_edge(g.find(2, 1, 5)) == g.end());

		it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		CHECK(g.erase_edge(g.find(1, 2, 5)) == g.find(2, 1, 4));
		CHECK(g.is_connected(1, 2) == false);

		CHECK(g.erase_edge(g.find(2, 1, 4)) == g.end());

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(2, 1) == false);
	}
}

TEST_CASE("erase_edge(iterator, iterator) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};
	auto empty = gdwg::graph<int, int>{};

	SECTION("erase_edge(iterator, iterator) on empty graph test") {
		CHECK(empty.erase_edge(empty.begin(), empty.end()) == empty.end());
	}

	SECTION("erase_edge(iterator, iterator) erases edge in graph with single weight test") {
		g.insert_edge(1, 2, 7);
		CHECK(g.erase_edge(g.find(1, 2, 7), g.end()) == g.end());

		auto it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);

		g.insert_edge(1, 2, 7);

		it = g.erase_edge(g.begin(), g.end());
		CHECK(g.end() == it);

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
	}

	SECTION("erase_edge(iterator, iterator) erases edge from node to itself test") {
		g.insert_edge(1, 1, 7);

		CHECK(g.erase_edge(g.find(1, 1, 7), ++g.find(1, 1, 7)) == g.end());

		auto it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 1) == false);
	}

	SECTION("erase_edge(iterator, iterator) erases edge between two nodes with multiple weights "
	        "test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 8);

		CHECK(g.erase_edge(g.find(1, 2, 5), g.find(1, 2, 8)) == g.find(1, 2, 8));

		auto it = g.begin();
		CHECK(vt(1, 2, 8) == *it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(1, 1) == false);

		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 7);

		CHECK(g.erase_edge(g.find(1, 2, 5), g.end()) == g.end());

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
		CHECK(g.is_connected(1, 1) == false);

		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 7);
	}

	SECTION("erase_edge(iterator, iterator) erases edges between two nodes with edges to each other "
	        "test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 1, 4);
		g.insert_edge(2, 1, 5);

		CHECK(g.erase_edge(g.find(1, 2, 7), g.find(2, 1, 5)) == g.find(2, 1, 5));

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 5) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 4);

		CHECK(g.erase_edge(g.find(1, 2, 7), g.find(2, 1, 4)) == g.find(2, 1, 4));

		it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(2, 1, 4) == *++it);
		CHECK(vt(2, 1, 5) == *++it);
		CHECK(g.end() == ++it);
		CHECK(g.is_connected(1, 2) == true);
		CHECK(g.is_connected(2, 1) == true);

		g.insert_edge(1, 2, 5);

		CHECK(g.erase_edge(g.begin(), g.end()) == g.end());

		it = g.begin();
		CHECK(g.end() == it);
		CHECK(g.is_connected(1, 2) == false);
		CHECK(g.is_connected(2, 1) == false);
	}
}

TEST_CASE("clear() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};
	auto empty = gdwg::graph<int, int>{};

	SECTION("clear() on empty graph test") {
		empty.clear();
		CHECK(empty.empty() == true);
	}

	SECTION("clear() erases graph with single weight test") {
		g.insert_edge(1, 2, 7);

		g.clear();

		CHECK(g.empty() == true);
		CHECK(g.begin() == g.end());
	}

	SECTION("clear() erases graph with node edge to self test") {
		g.insert_edge(1, 1, 7);

		g.clear();

		CHECK(g.empty() == true);
		CHECK(g.begin() == g.end());
	}

	SECTION("clear() erases graph with node outgoing edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 8);

		g.clear();

		CHECK(g.empty() == true);
		CHECK(g.begin() == g.end());
	}

	SECTION("erase_node() erases node have outgoing and incoming edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 1, 7);
		g.insert_edge(2, 1, 5);
		g.insert_edge(2, 2, 9);
		g.insert_edge(2, 3, 9);
		g.insert_edge(3, 1, 9);
		g.insert_edge(4, 2, 93);
		g.insert_edge(4, 4, 19);
		g.insert_edge(4, 5, 93);
		g.insert_edge(5, 2, 2);
		g.insert_edge(5, 6, 2);
		g.insert_edge(5, 5, 93);
		g.insert_edge(5, 5, 93);
		g.insert_edge(6, 1, 1);
		g.insert_edge(6, 2, 1);

		g.clear();

		CHECK(g.empty() == true);
		CHECK(g.begin() == g.end());
	}
}

TEST_CASE("replace_merge_node() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};

	SECTION("merge_replace_node() throws exception test") {
		auto empty = gdwg::graph<int, int>{};
		CHECK_THROWS_MATCHES(empty.merge_replace_node(7, 8),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::merge_replace_node on "
		                                    "old or new data if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(empty.merge_replace_node(7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::merge_replace_node on "
		                                    "old or new data if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.merge_replace_node(7, 8),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::merge_replace_node on "
		                                    "old or new data if they don't exist in the graph"));
		CHECK_THROWS_MATCHES(g.merge_replace_node(7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::merge_replace_node on "
		                                    "old or new data if they don't exist in the graph"));
	}

	SECTION("merge_replace_node() replaces node in graph with no edges correctly test") {
		g.merge_replace_node(1, 2);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		CHECK_THROWS_MATCHES(g.merge_replace_node(1, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::merge_replace_node on "
		                                    "old or new data if they don't exist in the graph"));
		g.merge_replace_node(3, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		g.merge_replace_node(3, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		g.merge_replace_node(3, 6);

		CHECK(g.nodes() == std::vector<int>{2, 4, 5, 6});
	}

	SECTION("merge_replace_node() replaces node with outgoing edges with anothers node with no "
	        "edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		g.merge_replace_node(1, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(3, 2, 5) == *it);
		CHECK(vt(3, 2, 6) == *++it);
		CHECK(vt(3, 2, 7) == *++it);
		CHECK(g.end() == ++it);
	}

	SECTION("merge_replace_node() replaces node with incoming edges with anothers node with no "
	        "edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		g.merge_replace_node(2, 3);

		CHECK(g.nodes() == std::vector<int>{1, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(1, 3, 5) == *it);
		CHECK(vt(1, 3, 6) == *++it);
		CHECK(vt(1, 3, 7) == *++it);
		CHECK(g.end() == ++it);
	}

	SECTION("merge_replace_node() replaces node with incoming and outgoing edges with anothers node "
	        "with no edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		g.insert_edge(2, 1, 3);
		g.insert_edge(2, 1, 2);
		g.insert_edge(2, 1, 1);

		g.merge_replace_node(1, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(2, 3, 1) == *it);
		CHECK(vt(2, 3, 2) == *++it);
		CHECK(vt(2, 3, 3) == *++it);
		CHECK(vt(3, 2, 5) == *++it);
		CHECK(vt(3, 2, 6) == *++it);
		CHECK(vt(3, 2, 7) == *++it);
		CHECK(g.end() == ++it);
	}

	SECTION("merge_replace_node() replaces node with outgoing and incoming edges with anothers node "
	        "with duplicated outgoing and incoming edges test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		g.insert_edge(2, 1, 3);
		g.insert_edge(2, 1, 8);
		g.insert_edge(2, 1, 1);

		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 6);
		g.insert_edge(3, 2, 1);

		g.insert_edge(2, 3, 2);
		g.insert_edge(2, 3, 3);
		g.insert_edge(2, 3, 8);

		g.merge_replace_node(1, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(2, 3, 1) == *it);
		CHECK(vt(2, 3, 2) == *++it);
		CHECK(vt(2, 3, 3) == *++it);
		CHECK(vt(2, 3, 8) == *++it);
		CHECK(vt(3, 2, 1) == *++it);
		CHECK(vt(3, 2, 5) == *++it);
		CHECK(vt(3, 2, 6) == *++it);
		CHECK(vt(3, 2, 7) == *++it);
		CHECK(g.end() == ++it);
	}

	SECTION("merge_replace_node() replaces node with anothers node with edges to self and outgoing "
	        "and incoming edges test") {
		g.insert_edge(1, 1, 7);
		g.insert_edge(1, 1, 4);

		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		g.insert_edge(2, 1, 3);
		g.insert_edge(2, 1, 8);
		g.insert_edge(2, 1, 1);

		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 6);
		g.insert_edge(3, 2, 1);

		g.insert_edge(2, 3, 2);
		g.insert_edge(2, 3, 3);
		g.insert_edge(2, 3, 8);

		g.insert_edge(3, 3, 4);
		g.insert_edge(3, 3, 8);

		g.merge_replace_node(1, 3);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(vt(2, 3, 1) == *it);
		CHECK(vt(2, 3, 2) == *++it);
		CHECK(vt(2, 3, 3) == *++it);
		CHECK(vt(2, 3, 8) == *++it);
		CHECK(vt(3, 2, 1) == *++it);
		CHECK(vt(3, 2, 5) == *++it);
		CHECK(vt(3, 2, 6) == *++it);
		CHECK(vt(3, 2, 7) == *++it);
		CHECK(vt(3, 3, 4) == *++it);
		CHECK(vt(3, 3, 7) == *++it);
		CHECK(vt(3, 3, 8) == *++it);
		CHECK(g.end() == ++it);
	}
}