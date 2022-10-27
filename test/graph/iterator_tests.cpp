#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

using vt = typename gdwg::graph<int, int>::value_type;

TEST_CASE("Creating iterators through begin and end graph functions") {
	SECTION("Construct begin and end iterator correctly on empty graph test") {
		auto g = gdwg::graph<int, std::string>{};

		auto it = g.begin();
		auto end = g.end();
		CHECK(it == end);
	}

	SECTION("Construct begin and end iterator correctly on graph with nodes test") {
		auto g = gdwg::graph<int, int>{1, 2, 3};

		auto it = g.begin();
		auto end = g.end();
		CHECK(it == end);
	}

	SECTION("Construct begin and end iterator correctly on graph with edges test") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(2, 1, 7);

		auto it = g.begin();
		auto end = g.end();

		CHECK(it != end);
		CHECK(*it == vt(1, 2, 5));
		CHECK(*--end == vt(2, 1, 7));
	}
}

TEST_CASE("Graph iterator operator== test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);

	auto it = g.begin();
	auto end = g.end();
	CHECK(it == --end);

	g = gdwg::graph<int, int>{};
	CHECK(g.begin() == g.end());
}

TEST_CASE("Graph iterator dereference assignment test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	g.insert_edge(1, 2, 5);
	g.insert_edge(1, 2, 6);
	g.insert_edge(2, 1, 7);

	auto it = g.begin();
	CHECK(*it == vt(1, 2, 5));
	CHECK(*++it == vt(1, 2, 6));
	CHECK(*++it == vt(2, 1, 7));
}

TEST_CASE("Graph iterator operator++() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("operator++() test on graph with one edge") {
		g.insert_edge(1, 2, 5);

		auto it = g.begin();
		CHECK(*it == vt(1, 2, 5));
		CHECK(++it == g.end());
	}

	SECTION("operator++() test on graph with multiple edges from the same src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		auto it = g.begin();
		CHECK(*it == vt(1, 2, 5));
		CHECK(*++it == vt(1, 2, 6));
		CHECK(*++it == vt(1, 2, 7));
		CHECK(++it == g.end());
	}

	SECTION("operator++() test on graph with multiple edges from the different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 6);
		g.insert_edge(3, 2, 7);

		auto it = g.begin();
		CHECK(*it == vt(1, 2, 5));
		CHECK(*++it == vt(2, 2, 6));
		CHECK(*++it == vt(3, 2, 7));
		CHECK(++it == g.end());
	}

	SECTION("operator++() test on graph with multiple edges to the same and different src and dst") {
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);

		auto it = g.begin();
		CHECK(*it == vt(1, 2, 5));
		CHECK(*++it == vt(1, 2, 6));
		CHECK(*++it == vt(1, 2, 7));
		CHECK(*++it == vt(2, 1, 7));
		CHECK(*++it == vt(3, 1, 7));
		CHECK(*++it == vt(3, 2, 7));
		CHECK(*++it == vt(3, 2, 8));
		CHECK(++it == g.end());
	}
}

TEST_CASE("Graph iterator operator++(int) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("operator++(int) test on graph with one edge") {
		g.insert_edge(1, 2, 5);

		auto it = g.begin();
		CHECK(*it++ == vt(1, 2, 5));
		CHECK(it == g.end());
	}

	SECTION("operator++(int) test on graph with multiple edges from the same src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		auto it = g.begin();
		CHECK(*it++ == vt(1, 2, 5));
		CHECK(*it++ == vt(1, 2, 6));
		CHECK(*it++ == vt(1, 2, 7));
		CHECK(it == g.end());
	}

	SECTION("operator++(int) test on graph with multiple edges from the different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 6);
		g.insert_edge(3, 2, 7);

		auto it = g.begin();
		CHECK(*it++ == vt(1, 2, 5));
		CHECK(*it++ == vt(2, 2, 6));
		CHECK(*it++ == vt(3, 2, 7));
		CHECK(it == g.end());
	}

	SECTION("operator++(int) test on graph with multiple edges to the same and different src and "
	        "dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);

		auto it = g.begin();
		CHECK(*it++ == vt(1, 2, 5));
		CHECK(*it++ == vt(1, 2, 6));
		CHECK(*it++ == vt(1, 2, 7));
		CHECK(*it++ == vt(2, 1, 7));
		CHECK(*it++ == vt(3, 1, 7));
		CHECK(*it++ == vt(3, 2, 7));
		CHECK(*it++ == vt(3, 2, 8));
		CHECK(it == g.end());
	}
}

TEST_CASE("Graph iterator operator--() test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("operator--() test on graph with one edge") {
		g.insert_edge(1, 2, 5);

		auto it = g.end();
		CHECK(*--it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges from the same src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		auto it = g.end();
		CHECK(*--it == vt(1, 2, 7));
		CHECK(*--it == vt(1, 2, 6));
		CHECK(*--it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges from the different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 6);
		g.insert_edge(3, 2, 7);

		auto it = g.end();
		CHECK(*--it == vt(3, 2, 7));
		CHECK(*--it == vt(2, 2, 6));
		CHECK(*--it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges to the same and different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);

		auto it = g.end();
		CHECK(*--it == vt(3, 2, 8));
		CHECK(*--it == vt(3, 2, 7));
		CHECK(*--it == vt(3, 1, 7));
		CHECK(*--it == vt(2, 1, 7));
		CHECK(*--it == vt(1, 2, 7));
		CHECK(*--it == vt(1, 2, 6));
		CHECK(*--it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}
}

TEST_CASE("Graph iterator operator--(int) test") {
	auto g = gdwg::graph<int, int>{1, 2, 3};

	SECTION("operator--() test on graph with one edge") {
		g.insert_edge(1, 2, 5);

		auto it = g.end();
		it--;
		CHECK(*it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges from the same src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);

		auto it = g.end();
		it--;
		CHECK(*it-- == vt(1, 2, 7));
		CHECK(*it-- == vt(1, 2, 6));
		CHECK(*it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges from the different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(2, 2, 6);
		g.insert_edge(3, 2, 7);

		auto it = g.end();
		it--;
		CHECK(*it-- == vt(3, 2, 7));
		CHECK(*it-- == vt(2, 2, 6));
		CHECK(*it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}

	SECTION("operator--() test on graph with multiple edges to the same and different src and dst") {
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 6);
		g.insert_edge(1, 2, 7);
		g.insert_edge(2, 1, 7);
		g.insert_edge(3, 1, 7);
		g.insert_edge(3, 2, 7);
		g.insert_edge(3, 2, 8);

		auto it = g.end();
		it--;
		CHECK(*it-- == vt(3, 2, 8));
		CHECK(*it-- == vt(3, 2, 7));
		CHECK(*it-- == vt(3, 1, 7));
		CHECK(*it-- == vt(2, 1, 7));
		CHECK(*it-- == vt(1, 2, 7));
		CHECK(*it-- == vt(1, 2, 6));
		CHECK(*it == vt(1, 2, 5));
		CHECK(it == g.begin());
	}
}
