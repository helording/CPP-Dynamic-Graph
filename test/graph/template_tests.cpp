#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

using vt = typename gdwg::graph<std::string, std::string>::value_type;
using vt2 = typename gdwg::graph<std::vector<int>, std::vector<std::string>>::value_type;

TEST_CASE("template with std::strings test") {
	auto g = gdwg::graph<std::string, std::string>{};

	SECTION("empty(), insert_node(), insert_edge(), is_node() and iterator works test") {
		CHECK(g.empty() == true);

		CHECK(g.insert_node("fly") == true);
		CHECK(g.insert_node("fly") == false);
		CHECK(g.insert_node("hi") == true);
		CHECK(g.insert_node("bye") == true);
		CHECK(g.is_node("bye") == true);

		CHECK(g.insert_edge("hi", "bye", "sly") == true);
		CHECK(g.insert_edge("hi", "bye", "sly") == false);
		CHECK(g.insert_edge("fly", "bye", "aa") == true);

		auto it = g.begin();
		CHECK(vt("fly", "bye", "aa") == *it);
		CHECK(vt("hi", "bye", "sly") == *++it);
		CHECK(g.end() == ++it);
	}

	g = gdwg::graph<std::string, std::string>{"fly", "hi", "bye"};

	CHECK(g.insert_edge("hi", "bye", "sly") == true);
	CHECK(g.insert_edge("fly", "bye", "aa") == true);

	SECTION("is_connected(), nodes(), weights(), find() and connections() works test") {
		CHECK(g.is_connected("fly", "bye") == true);
		CHECK(g.nodes() == std::vector<std::string>{"bye", "fly", "hi"});
		CHECK(g.weights("hi", "bye") == std::vector<std::string>{"sly"});
		CHECK(g.find("hi", "bye", "sly") != g.end());
		CHECK(g.connections("hi") == std::vector<std::string>{"bye"});
	}

	SECTION("equality operator works test") {
		auto g2 = gdwg::graph<std::string, std::string>{};
		CHECK(g != g2);

		CHECK(g2.insert_node("fly") == true);
		CHECK(g2.insert_node("hi") == true);

		CHECK(g != g2);

		CHECK(g2.insert_node("bye") == true);

		CHECK(g != g2);

		CHECK(g2.insert_edge("hi", "bye", "sly") == true);

		CHECK(g != g2);

		CHECK(g2.insert_edge("fly", "bye", "aa") == true);

		CHECK(g == g2);
	}

	SECTION("is_connected(), nodes(), weights(), find() and connections() works test") {
		CHECK(g.replace_node("fly", "die") == true);
		g.merge_replace_node("die", "hi");

		auto it = g.begin();
		CHECK(g.find("hi", "bye", "aa") == it);
		CHECK(g.find("hi", "bye", "sly") == ++it);
		CHECK(g.end() == ++it);

		CHECK(g.erase_node("hi") == true);

		CHECK(g.insert_node("fly") == true);
		CHECK(g.insert_node("hi") == true);

		CHECK(g.insert_edge("hi", "bye", "sly") == true);
		CHECK(g.insert_edge("fly", "bye", "aa") == true);

		CHECK(g.erase_edge("hi", "bye", "sly") == true);

		auto b = g.begin();
		CHECK(g.erase_edge(b) == g.find("hi", "bye", "sly"));

		CHECK(g.insert_edge("fly", "bye", "aa") == true);
		CHECK(g.insert_edge("fly", "bye", "zz") == true);

		b = g.begin();
		auto e = g.end();
		CHECK(g.erase_edge(b, --e) == --g.end());

		g.clear();
		CHECK(g == gdwg::graph<std::string, std::string>{});
	}
}

TEST_CASE("template with std::vector test") {
	auto g = gdwg::graph<std::vector<int>, std::vector<std::string>>{};

	SECTION("empty(), insert_node(), insert_edge(), is_node() and iterator works test") {
		CHECK(g.empty() == true);

		CHECK(g.insert_node(std::vector{1}) == true);
		CHECK(g.insert_node(std::vector{1}) == false);
		CHECK(g.insert_node(std::vector{2}) == true);
		CHECK(g.insert_node(std::vector{3}) == true);
		CHECK(g.is_node(std::vector{2}) == true);

		CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);
		CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == false);
		CHECK(g.insert_edge(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == true);

		auto it = g.begin();
		CHECK(vt2(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == *it);
		CHECK(vt2(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == *++it);
		CHECK(g.end() == ++it);
	}

	g = gdwg::graph<std::vector<int>, std::vector<std::string>>{std::vector{1},
	                                                            std::vector{2},
	                                                            std::vector{3}};

	CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);
	CHECK(g.insert_edge(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == true);

	SECTION("is_connected(), nodes(), weights(), find() and connections() works test") {
		CHECK(g.is_connected(std::vector{1}, std::vector{2}) == true);
		CHECK(g.nodes()
		      == std::vector<std::vector<int>>{std::vector{1}, std::vector{2}, std::vector{3}});
		CHECK(g.weights(std::vector{1}, std::vector{2})
		      == std::vector<std::vector<std::string>>{std::vector{std::string("a")}});
		CHECK(g.find(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) != g.end());
		CHECK(g.connections(std::vector{1}) == std::vector<std::vector<int>>{std::vector{2}});
	}

	SECTION("equality operator works test") {
		auto g2 = gdwg::graph<std::vector<int>, std::vector<std::string>>{};
		CHECK(g != g2);

		CHECK(g2.insert_node(std::vector{1}) == true);
		CHECK(g2.insert_node(std::vector{2}) == true);

		CHECK(g != g2);

		CHECK(g2.insert_node(std::vector{3}) == true);

		CHECK(g != g2);

		CHECK(g2.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);

		CHECK(g != g2);

		CHECK(g2.insert_edge(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == true);

		CHECK(g == g2);
	}

	SECTION("is_connected(), nodes(), weights(), find() and connections() works test") {
		CHECK(g.replace_node(std::vector{1}, std::vector{4}) == true);
		g.merge_replace_node(std::vector{4}, std::vector{2});

		auto it = g.begin();
		CHECK(vt2(std::vector{2}, std::vector{2}, std::vector{std::string("a")}) == *it);
		CHECK(vt2(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == *++it);
		CHECK(g.end() == ++it);

		CHECK(g.erase_node(std::vector{2}) == true);

		CHECK(g.insert_node(std::vector{1}) == true);
		CHECK(g.insert_node(std::vector{2}) == true);

		CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);
		CHECK(g.insert_edge(std::vector{2}, std::vector{3}, std::vector{std::string("c")}) == true);

		CHECK(g.erase_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);

		CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);

		auto b = g.begin();
		CHECK(g.erase_edge(b) == g.find(std::vector{2}, std::vector{3}, std::vector{std::string("c")}));

		CHECK(g.insert_edge(std::vector{1}, std::vector{2}, std::vector{std::string("a")}) == true);

		b = g.begin();
		auto e = g.end();
		CHECK(g.erase_edge(b, --e) == --g.end());

		g.clear();
		CHECK(g == gdwg::graph<std::vector<int>, std::vector<std::string>>{});
	}
}