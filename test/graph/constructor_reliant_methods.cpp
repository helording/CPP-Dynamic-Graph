#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

using vt = typename gdwg::graph<int, int>::value_type;

TEST_CASE("nodes() test") {
	SECTION("nodes() on non-empty graph test") {
		auto g = gdwg::graph<int, int>{1, 2, 3};

		CHECK(g.nodes() == std::vector<int>{1, 2, 3});
	}

	SECTION("nodes() output is in ascending order test") {
		auto g = gdwg::graph<int, int>{4, 5, 6, 1};

		CHECK(g.nodes() == std::vector<int>{1, 4, 5, 6});
	}

	SECTION("nodes() on empty graph test test") {
		auto g = gdwg::graph<int, int>{};

		CHECK(g.nodes() == std::vector<int>{});
	}
}

TEST_CASE("insert_edge() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("insert_edge() throws exception test") {
		CHECK_THROWS_MATCHES(g.insert_edge(4, 5, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::insert_edge when either "
		                                    "src or dst node does not exist"));
		CHECK_THROWS_MATCHES(g.insert_edge(1, 5, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::insert_edge when either "
		                                    "src or dst node does not exist"));
		CHECK_THROWS_MATCHES(g.insert_edge(4, 1, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::insert_edge when either "
		                                    "src or dst node does not exist"));
	}

	SECTION("insert_edge() successfully adding edge returns true otherwise false test") {
		CHECK(g.insert_edge(1, 2, 1) == true);
		CHECK(g.insert_edge(1, 2, 1) == false);
	}

	SECTION("insert_edge() adding an existing node fails to insert into graph test") {
		CHECK(g.insert_edge(1, 2, 1) == true);
		CHECK(g.insert_edge(2, 3, 1) == true);
		CHECK(g.insert_edge(1, 2, 1) == false);
		CHECK(g.insert_edge(2, 3, 1) == false);
		CHECK(g.insert_edge(3, 3, 2) == true);

		auto it = g.begin();
		CHECK(vt(1, 2, 1) == *it);
		CHECK(vt(2, 3, 1) == *++it);
		CHECK(vt(3, 3, 2) == *++it);
		CHECK(g.end() == ++it);
	}

	SECTION("insert_edge() creating reverse edges for two nodes test") {
		g.insert_edge(1, 2, 1);
		g.insert_edge(2, 1, 2);

		auto it = g.begin();
		CHECK(vt(1, 2, 1) == *it);
		CHECK(vt(2, 1, 2) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.find(2, 3, 1) == g.end());
		CHECK(g.find(1, 2, 2) == g.end());
		CHECK(g.find(2, 1, 1) == g.end());
		CHECK(g.find(1, 1, 1) == g.end());
	}

	SECTION("insert_edge() creating edges of src to src test") {
		g.insert_edge(1, 1, 1);
		g.insert_edge(3, 3, 1);
		g.insert_edge(2, 2, 1);

		auto it = g.begin();
		CHECK(vt(1, 1, 1) == *it);
		CHECK(vt(2, 2, 1) == *++it);
		CHECK(vt(3, 3, 1) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.find(2, 2, 2) == g.end());
		CHECK(g.find(1, 2, 1) == g.end());
	}

	SECTION("multiple insert_edge() between the same src and dest nodes test") {
		g.insert_edge(1, 1, 1);
		g.insert_edge(1, 1, 2);
		g.insert_edge(1, 2, 1);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 1);
		g.insert_edge(1, 3, 2);
		g.insert_edge(2, 1, 1);
		g.insert_edge(2, 1, 2);

		auto it = g.begin();
		CHECK(vt(1, 1, 1) == *it);
		CHECK(vt(1, 1, 2) == *++it);
		CHECK(vt(1, 2, 1) == *++it);
		CHECK(vt(1, 2, 5) == *++it);
		CHECK(vt(1, 3, 1) == *++it);
		CHECK(vt(1, 3, 2) == *++it);
		CHECK(vt(2, 1, 1) == *++it);
		CHECK(vt(2, 1, 2) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.find(2, 3, 1) == g.end());
		CHECK(g.find(1, 2, 2) == g.end());
		CHECK(g.find(1, 1, 3) == g.end());
	}
}

TEST_CASE("find() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	auto empty = gdwg::graph<int, int>{};

	SECTION("find() on empty graph") {
		CHECK(empty.find(1, 1, 1) == empty.end());
	}

	SECTION("find() on graph with single weight test") {
		g.insert_edge(1, 2, 5);

		CHECK(g.find(1, 2, 5) == g.begin());
		CHECK(g.find(1, 2, 4) == g.end());
	}

	SECTION("find() on graph with multiple weights between same src and dest test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		auto it = g.begin();
		CHECK(g.find(1, 2, 5) == it);
		CHECK(g.find(1, 2, 6) == ++it);
		CHECK(g.find(1, 2, 7) == ++it);
		CHECK(g.find(1, 2, 4) == g.end());
	}

	SECTION("find() on graph with single weights between different src and dest test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 3, 6);
		g.insert_edge(2, 3, 7);

		auto it = g.begin();
		CHECK(g.find(1, 2, 5) == it);
		CHECK(g.find(1, 3, 6) == ++it);
		CHECK(g.find(2, 3, 7) == ++it);
		CHECK(g.find(1, 2, 4) == g.end());
	}

	SECTION("find() on graph with multiple edges between different src and dest nodes") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(2, 3, 11);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);

		auto it = g.begin();
		CHECK(g.find(1, 2, 5) == it);
		CHECK(g.find(1, 2, 6) == ++it);
		CHECK(g.find(1, 2, 7) == ++it);
		CHECK(g.find(2, 1, 7) == ++it);
		CHECK(g.find(2, 3, 11) == ++it);
		CHECK(g.find(3, 1, 7) == ++it);
		CHECK(g.find(3, 2, 7) == ++it);
		CHECK(g.find(3, 2, 8) == ++it);

		CHECK(g.find(1, 2, 4) == g.end());
		CHECK(g.find(2, 1, 5) == g.end());
		CHECK(g.find(2, 3, 7) == g.end());
	}
}

TEST_CASE("replace_node() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6};

	SECTION("replace_node() throws exception test") {
		auto empty = gdwg::graph<int, int>{};
		CHECK_THROWS_MATCHES(empty.replace_node(7, 8),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::replace_node on a node "
		                                    "that doesn't exist"));
		CHECK_THROWS_MATCHES(empty.replace_node(7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::replace_node on a node "
		                                    "that doesn't exist"));
		CHECK_THROWS_MATCHES(g.replace_node(7, 8),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::replace_node on a node "
		                                    "that doesn't exist"));
		CHECK_THROWS_MATCHES(g.replace_node(7, 1),
		                     std::runtime_error,
		                     Catch::Message("Cannot call gdwg::graph<N, E>::replace_node on a node "
		                                    "that doesn't exist"));
	}

	SECTION("replace_node() correctly returns and replaces node in graph with no edges test") {
		CHECK(g.replace_node(1, 2) == false);

		CHECK(g.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});

		CHECK(g.replace_node(1, 1) == false);

		CHECK(g.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});

		CHECK(g.replace_node(3, 6) == false);

		CHECK(g.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});

		CHECK(g.replace_node(1, 7) == true);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6, 7});

		CHECK(g.replace_node(2, 8) == true);

		CHECK(g.nodes() == std::vector<int>{3, 4, 5, 6, 7, 8});

		CHECK(g.replace_node(7, 9) == true);

		CHECK(g.nodes() == std::vector<int>{3, 4, 5, 6, 8, 9});

		CHECK(g.replace_node(9, 9) == false);

		CHECK(g.nodes() == std::vector<int>{3, 4, 5, 6, 8, 9});
	}

	SECTION("replace_node() on src node having weights from src to dst test") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		CHECK(g.replace_node(1, 7) == true);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6, 7});

		auto it = g.begin();
		CHECK(*it == vt(7, 2, 5));
		CHECK(*++it == vt(7, 2, 6));
		CHECK(*++it == vt(7, 2, 7));
		CHECK(++it == g.end());
	}

	SECTION("replace_node() on src node having weights from src to dst and to self test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 2);

		CHECK(g.replace_node(1, 7) == true);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6, 7});

		auto it = g.begin();
		CHECK(*it == vt(7, 2, 5));
		CHECK(*++it == vt(7, 2, 6));
		CHECK(*++it == vt(7, 2, 7));
		CHECK(*++it == vt(7, 7, 2));
		CHECK(*++it == vt(7, 7, 5));
		CHECK(++it == g.end());
	}

	SECTION("replace_node() on src node having outgoing and incoming edges test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 2);
		g.insert_edge(3, 1, 13);
		g.insert_edge(3, 1, 11);
		g.insert_edge(3, 2, 11);
		g.insert_edge(3, 3, 11);
		g.insert_edge(5, 1, 2);

		CHECK(g.replace_node(1, 7) == true);

		CHECK(g.nodes() == std::vector<int>{2, 3, 4, 5, 6, 7});

		auto it = g.begin();
		CHECK(*it == vt(3, 2, 11));
		CHECK(*++it == vt(3, 3, 11));
		CHECK(*++it == vt(3, 7, 11));
		CHECK(*++it == vt(3, 7, 13));
		CHECK(*++it == vt(5, 7, 2));
		CHECK(*++it == vt(7, 2, 5));
		CHECK(*++it == vt(7, 2, 6));
		CHECK(*++it == vt(7, 2, 7));
		CHECK(*++it == vt(7, 7, 2));
		CHECK(*++it == vt(7, 7, 5));
		CHECK(++it == g.end());
	}

	SECTION("replace_node() node twice consecutively test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 2);
		g.insert_edge(3, 1, 13);
		g.insert_edge(3, 1, 11);
		g.insert_edge(3, 2, 11);
		g.insert_edge(3, 3, 11);
		g.insert_edge(5, 1, 2);

		CHECK(g.replace_node(1, 7) == true);
		CHECK(g.replace_node(7, 0) == true);

		CHECK(g.nodes() == std::vector<int>{0, 2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(*it == vt(0, 0, 2));
		CHECK(*++it == vt(0, 0, 5));
		CHECK(*++it == vt(0, 2, 5));
		CHECK(*++it == vt(0, 2, 6));
		CHECK(*++it == vt(0, 2, 7));
		CHECK(*++it == vt(3, 0, 11));
		CHECK(*++it == vt(3, 0, 13));
		CHECK(*++it == vt(3, 2, 11));
		CHECK(*++it == vt(3, 3, 11));
		CHECK(*++it == vt(5, 0, 2));
		CHECK(++it == g.end());
	}

	SECTION("replace_node() node then replace again by self test") {
		g.insert_edge(1, 2, 7);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 2);
		g.insert_edge(3, 1, 13);
		g.insert_edge(3, 1, 11);
		g.insert_edge(3, 2, 11);
		g.insert_edge(3, 3, 11);
		g.insert_edge(5, 1, 2);

		CHECK(g.replace_node(1, 7) == true);
		CHECK(g.replace_node(7, 1) == true);

		CHECK(g.nodes() == std::vector<int>{1, 2, 3, 4, 5, 6});

		auto it = g.begin();
		CHECK(*it == vt(1, 1, 2));
		CHECK(*++it == vt(1, 1, 5));
		CHECK(*++it == vt(1, 2, 5));
		CHECK(*++it == vt(1, 2, 6));
		CHECK(*++it == vt(1, 2, 7));
		CHECK(*++it == vt(3, 1, 11));
		CHECK(*++it == vt(3, 1, 13));
		CHECK(*++it == vt(3, 2, 11));
		CHECK(*++it == vt(3, 3, 11));
		CHECK(*++it == vt(5, 1, 2));
		CHECK(++it == g.end());
	}
}

TEST_CASE("operator==() test") {
	SECTION("operator==() on empty graph test") {
		auto g1 = gdwg::graph<int, int>{};
		auto g2 = gdwg::graph<int, int>{};
		auto g3 = gdwg::graph<int, int>{1};

		CHECK(g1 == g1);
		CHECK(g1 == g2);
		CHECK(g2 != g3);
	}

	SECTION("operator==() on non-empty graph test with no edges") {
		auto g1 = gdwg::graph<int, int>{1, 2, 3};
		auto g2 = gdwg::graph<int, int>{1, 2, 3};
		auto g3 = gdwg::graph<int, int>{1, 2, 4};
		auto g4 = gdwg::graph<int, int>{1, 2, 3, 4};
		auto g5 = gdwg::graph<int, int>{1};
		auto g6 = gdwg::graph<int, int>{1, 1};

		CHECK(g1 == gdwg::graph<int, int>{1, 2, 3});
		CHECK(g1 == g1);
		CHECK(g5 == g5);
		CHECK(g6 == g6);
		CHECK(g1 != g3);
		CHECK(g1 != g4);
		CHECK(g3 != g4);
		CHECK(g5 == g6);
	}

	SECTION("operator==() on non-empty graph test with edges") {
		auto g1 = gdwg::graph<int, int>{1, 2, 3};

		auto g2 = gdwg::graph<int, int>{1, 2, 3};
		g2.insert_edge(1, 2, 1);
		g2.insert_edge(1, 2, 1);

		auto g3 = gdwg::graph<int, int>{1, 2, 3};
		g3.insert_edge(1, 2, 1);

		auto g4 = gdwg::graph<int, int>{1, 2, 3};
		g4.insert_edge(2, 1, 1);

		auto g5 = gdwg::graph<int, int>{1, 2, 3};
		g5.insert_edge(1, 2, 1);
		g5.insert_edge(2, 1, 1);

		auto g6 = gdwg::graph<int, int>{1, 2, 3};
		g6.insert_edge(1, 2, 1);
		g6.insert_edge(2, 1, 1);

		auto g7 = gdwg::graph<int, int>{1, 2, 3, 4};
		g7.insert_edge(1, 2, 1);
		g7.insert_edge(2, 1, 1);

		CHECK(g1 == gdwg::graph<int, int>{1, 2, 3});
		CHECK(g2 == g3);
		CHECK(g1 != g2);
		CHECK(g2 != g4);
		CHECK(g2 != g5);
		CHECK(g4 != g5);
		CHECK(g6 != g7);
	}
}
