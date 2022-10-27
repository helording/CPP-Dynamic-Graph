#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <forward_list>
#include <list>

using vt = typename gdwg::graph<int, int>::value_type;

/*
The approach taken to testing was to test functions in order of reliance.
At the highest level, the constructors and assignment operators correct implementation are relied
upon for all other member, friend, and non-member functions and so are tested first.

Since graph access and modification are needed to test constructors and assignment operators,
the graphs iterator, and a couple of basic accessors and modifiers as well as the equality operator
are tested next. These accessors and modifiers are namely the nodes(), find(), insert_edge() and
replace_node() methods. Within these functions the iterator is tested first and as it is replied
upon the most for constructor testing and by insert_edge(), which in turn is later used for find()
and replace_node() testing.

Next, the remainder of the accessors are tested. They are tested before modifiers
as their correction can be asserted without the use of any other modifiers not tested, and,
accessors are likely either to be used modifiers functions (e.g. for testing assertions)
or likely to make testing modifier functions simpler.

Next, the modifiers are tested, apart from insert_edge() which was tested beforehand.

The last two groups of function tests are friend and non-member functions tests. These were tested
later as these functions were likely to use member functions of the class.

Lastly, after all functions have been tested, the genericness of the graph structure is asserted.
These tests will not go into as great detail on the logical correction of functions as at this point
the functions have already been asserted to be logically correct. Rather a couple of template graphs
are constructed with more complex data types and each member function is called so that it can be
asserted that the graph variables and methods comply with these data types.

On a more micro level, the testing of each function begins with simple use cases and scenarios and
gradually tests more complex scenarios. Every increase in complexity is demarcated with a catch2
SECTION so that it can be asserted exactly why the test has failed.
*/

TEST_CASE("graph() default constructor test") {
	auto g = gdwg::graph<int, int>{};

	CHECK(g.empty() == true);

	auto g2 = gdwg::graph<int, int>();

	CHECK(g2.empty() == true);
}

TEST_CASE("graph(std::initializer_list<N>) constructor test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	CHECK(g.empty() == false);

	auto nodes = g.nodes();
	auto nodes_it = nodes.begin();
	CHECK(*(nodes_it++) == 1);
	CHECK(*(nodes_it++) == 2);
	CHECK(*(nodes_it++) == 3);
	CHECK(nodes_it == nodes.end());
}

TEST_CASE("graph(InputIt , InputIt) constructor test") {
	auto v = std::vector<int>{1, 2, 3, 4};
	auto l = std::list<int>{1, 2};
	auto fl = std::forward_list<int>{3, 1};

	SECTION("constructed with unaltered begin and end iterators test") {
		auto g = gdwg::graph<int, int>(v.begin(), v.end());

		CHECK(g.empty() == false);

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(*(nodes_it++) == 4);
		CHECK(nodes_it == nodes.end());
	}

	SECTION("constructed with altered begin and end vector iterators test") {
		auto g = gdwg::graph<int, int>(v.begin() + 1, v.end() - 1);

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());
	}

	SECTION("constructing with one altered vector iterator test") {
		auto g = gdwg::graph<int, int>(v.begin(), v.end() - 2);

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(nodes_it == nodes.end());
	}

	SECTION("constructing with equal vector iterator test") {
		auto g = gdwg::graph<int, int>(v.begin() + 2, v.begin() + 2);

		CHECK(g.empty() == true);
	}

	SECTION("constructing from list container test") {
		auto g = gdwg::graph<int, int>(l.begin(), l.end());

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(nodes_it == nodes.end());
	}

	SECTION("constructing from forward_list container test") {
		auto g = gdwg::graph<int, int>(fl.begin(), fl.end());

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());
	}
}

TEST_CASE("graph(graph&) constructor test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 2, 6);
	g.insert_edge(2, 1, 7);

	auto g2 = gdwg::graph<int, int>{6, 4};
	g2.insert_edge(6, 4, 2);

	auto default_ = gdwg::graph<int, int>{};

	SECTION("graph copy constructor on unintialised variable from multi-node graph with edges") {
		auto copy = gdwg::graph<int, int>(g);

		CHECK(copy == g);

		auto nodes = copy.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g.end() == ++it);

		// ensure deep-copy
		g.replace_node(1, 4);
		CHECK(g.is_node(4) == true);
		CHECK(copy.is_node(4) == false);
	}

	SECTION("graph copy constructor on intialised variable from multi-node graph with edges") {
		g2 = gdwg::graph<int, int>(g);

		CHECK(g2 == g);

		auto nodes = g2.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g2.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g2.end() == ++it);

		g.replace_node(1, 4);
		CHECK(g.is_node(4) == true);
		CHECK(g2.is_node(4) == false);
	}

	SECTION("graph copy constructor from default graph") {
		auto copy = gdwg::graph<int, int>(default_);

		CHECK(copy == default_);
		CHECK(copy.empty() == true);
	}

	SECTION("graph copy constructor from self") {
		g = gdwg::graph<int, int>(g);

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.find(1, 2, 6) != g.end());
		CHECK(g.find(2, 1, 7) != g.end());
	}
}

TEST_CASE("graph(graph&&) constructor test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 2, 6);
	g.insert_edge(2, 1, 7);

	auto g2 = gdwg::graph<int, int>{6, 4};
	g2.insert_edge(6, 4, 2);

	auto default_ = gdwg::graph<int, int>{};

	SECTION("graph move constructor on unintialised variable from multi-node graph with edges") {
		auto it = g.begin();
		auto move = gdwg::graph<int, int>(std::move(g));

		auto nodes = move.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);

		it = move.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(move.end() == ++it);

		CHECK(g == gdwg::graph<int, int>());
	}

	SECTION("graph move constructor on intialised variable from multi-node graph with edges") {
		auto it = g.begin();
		g2 = gdwg::graph<int, int>(std::move(g));

		auto nodes = g2.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);

		it = g2.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g2.end() == ++it);

		CHECK(g2.find(6, 4, 2) == g2.end());

		CHECK(g == gdwg::graph<int, int>());
	}

	SECTION("graph move constructor from default graph") {
		auto move = gdwg::graph<int, int>(std::move(default_));

		CHECK(move == gdwg::graph<int, int>());
		CHECK(default_ == gdwg::graph<int, int>());
	}
}

TEST_CASE("operator=(graph&) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 2, 6);
	g.insert_edge(2, 1, 7);

	auto g2 = gdwg::graph<int, int>{6, 4};
	g2.insert_edge(6, 4, 2);

	auto default_ = gdwg::graph<int, int>{};

	SECTION("graph copy assignment on unintialised variable from multi-node graph with edges") {
		auto copy = g;

		CHECK(copy == g);

		auto nodes = copy.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g.end() == ++it);

		// ensure deep-copy
		g.replace_node(1, 4);
		CHECK(g.is_node(4) == true);
		CHECK(copy.is_node(4) == false);
	}

	SECTION("graph copy assignment on intialised variable from multi-node graph with edges") {
		g2 = g;

		CHECK(g2 == g);

		auto nodes = g2.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g2.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g2.end() == ++it);

		g.replace_node(1, 4);
		CHECK(g.is_node(4) == true);
		CHECK(g2.is_node(4) == false);
	}

	SECTION("graph copy assignment from default graph") {
		auto copy = default_;

		CHECK(copy == default_);
		CHECK(copy.empty() == true);
	}

	SECTION("graph copy assignment from self") {
		g = g;

		auto nodes = g.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		auto it = g.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.find(1, 2, 6) != g.end());
		CHECK(g.find(2, 1, 7) != g.end());
	}
}

TEST_CASE("operator=(graph&&) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 2, 6);
	g.insert_edge(2, 1, 7);

	auto g2 = gdwg::graph<int, int>{6, 4};
	g2.insert_edge(6, 4, 2);

	auto default_ = gdwg::graph<int, int>{};

	SECTION("graph move assignment on unintialised variable from multi-node graph with edges") {
		auto it = g.begin();
		auto move = std::move(g);

		auto nodes = move.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);

		it = move.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(move.end() == ++it);

		CHECK(g == gdwg::graph<int, int>());
	}

	SECTION("graph move assignment on intialised variable from multi-node graph with edges") {
		auto it = g.begin();
		g2 = std::move(g);

		auto nodes = g2.nodes();
		auto nodes_it = nodes.begin();
		CHECK(*(nodes_it++) == 1);
		CHECK(*(nodes_it++) == 2);
		CHECK(*(nodes_it++) == 3);
		CHECK(nodes_it == nodes.end());

		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);

		it = g2.begin();
		CHECK(vt(1, 2, 5) == *it);
		CHECK(vt(1, 2, 6) == *++it);
		CHECK(vt(2, 1, 7) == *++it);
		CHECK(g2.end() == ++it);

		CHECK(g2.find(6, 4, 2) == g2.end());

		CHECK(g == gdwg::graph<int, int>());
	}

	SECTION("graph move assignment from default graph") {
		auto move = std::move(default_);

		CHECK(move == gdwg::graph<int, int>());
		CHECK(default_ == gdwg::graph<int, int>());
	}

	SECTION("graph move assignment from self") {
		g = std::move(g);

		CHECK(g == gdwg::graph<int, int>());
	}
}
