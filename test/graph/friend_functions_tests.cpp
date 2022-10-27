#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

#include <istream>
#include <sstream>
#include <string>

TEST_CASE("stream output (<<) operator tests") {
	SECTION("<< operator printing empty graph test") {
		auto g = gdwg::graph<int, int>{};

		std::stringstream test("");
		test << g;
		CHECK(test.str() == "");
	}

	SECTION("<< operator printing printing graph with edges") {
		using graph = gdwg::graph<int, int>;
		auto const v = std::vector<graph::value_type>{
		   {4, 1, -4},
		   {3, 2, 2},
		   {2, 4, 2},
		   {2, 1, 1},
		   {6, 2, 5},
		   {6, 3, 10},
		   {1, 5, -1},
		   {3, 6, -8},
		   {4, 5, 3},
		   {5, 2, 7},
		};

		auto g = graph{};
		for (const auto& [from, to, weight] : v) {
			g.insert_node(from);
			g.insert_node(to);
			g.insert_edge(from, to, weight);
		}

		g.insert_node(64);
		auto out = std::ostringstream{};
		out << g;
		auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");

		CHECK(out.str() == expected_output);
	}
}