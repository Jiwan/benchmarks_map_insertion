#include <benchmark/benchmark.h>

#include <algorithm>
#include <memory>
#include <map>
#include <string>
#include <type_traits>
#include <utility>

#include <cstdlib>

template<class T>
struct lazy_convert_construct
{
	using result_type = std::invoke_result_t<T>;
	
	constexpr lazy_convert_construct(T&& t)
		: t_(std::move(t))
	{

	}

	constexpr inline operator result_type() const noexcept(noexcept(std::declval<T>()())) 
	{
		return t_();	
	}
	
	T t_;
};

struct a_costly_type
{
	a_costly_type(int)
	{
		std::generate(std::begin(x), std::end(x), [](){ return std::rand(); });
	}

	int x[1024];
};

const std::string key = "yolololololololololololololololololololoyolololololololololololololololololololo";

template <class T>
void fill_map(std::map<std::string, std::unique_ptr<T>>& m)
{
	auto x = 0;

	for (int i = 0; i < 100000; ++i) {
		x += i;
		auto new_key = key;
		new_key[x % 33] = 'a' + (x % 20);
		m[new_key] = nullptr;	
	}
}

void insertion_double_lookup(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<int>> m;
	fill_map(m);

	for (auto _ : state) {
		if (m.find(key) == m.end()) {
			m[key] = std::make_unique<int>(42);
		}

		state.PauseTiming();
		m.erase(key);
		state.ResumeTiming();
	}
}

BENCHMARK(insertion_double_lookup);

void insertion_construct_before_emplace(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<int>> m;
	fill_map(m);

	for (auto _ : state) {
		m.try_emplace(key, std::make_unique<int>(42)); 
		state.PauseTiming();
		m.erase(key);
		state.ResumeTiming();
	}
}

BENCHMARK(insertion_construct_before_emplace);

void insertion_lazy_convert_try_emplace(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<int>> m;
	fill_map(m);

	for (auto _ : state) {
		m.try_emplace(key, lazy_convert_construct([]{ return std::make_unique<int>(42);} )); 
		state.PauseTiming();
		m.erase(key);
		state.ResumeTiming();
	}
}

BENCHMARK(insertion_lazy_convert_try_emplace);

void no_insertion_double_lookup(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<a_costly_type>> m;
	
	fill_map(m);
	m[key] = nullptr;

	for (auto _ : state) {
		if (m.find(key) == m.end()) {
			m[key] = std::make_unique<a_costly_type>(42);
		}
	}
}

BENCHMARK(no_insertion_double_lookup);

void no_insertion_construct_before_emplace(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<a_costly_type>> m;
	
	fill_map(m);
	m[key] = nullptr;

	for (auto _ : state) {
		m.try_emplace(key, std::make_unique<a_costly_type>(42)); 
	}
}

BENCHMARK(no_insertion_construct_before_emplace);

void no_insertion_lazy_convert_try_emplace(benchmark::State& state) {
	std::map<std::string, std::unique_ptr<a_costly_type>> m;
	
	fill_map(m);
	m[key] = nullptr;

	for (auto _ : state) {
		m.try_emplace(key, lazy_convert_construct([]{ return std::make_unique<a_costly_type>(42);} )); 
	}
}

BENCHMARK(no_insertion_lazy_convert_try_emplace);



BENCHMARK_MAIN();
