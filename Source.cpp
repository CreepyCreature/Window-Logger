#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_set>
#include <future>

std::string GetActiveWindowTitle()
{
	HWND hwnd = GetForegroundWindow();
	size_t title_length = GetWindowTextLength(hwnd);
	if (title_length < 1) { return "[Empty Title]"; }

	auto window_text = std::unique_ptr<char>{ new char[title_length + 1] };
	GetWindowText(hwnd, window_text.get(), title_length + 1);

	return window_text.get();
}

std::unordered_set<std::string> LoadTitleSet()
{
	std::ifstream in("titles.txt");
	if (!in.is_open()) { return std::unordered_set<std::string>(); }

	std::unordered_set<std::string> title_set;

	for (std::string title; std::getline(in, title);)
	{
		title_set.insert(title);
	}
	
	in.close();

	return title_set;
}

int main()
{
	using namespace std::chrono_literals;

	std::string title;
	std::unordered_set<std::string> title_set = LoadTitleSet();
	std::ofstream out;
	std::future<void> fut;

	while (true)
	{
		title = GetActiveWindowTitle();
		if (title_set.find(title) == title_set.end())
		{
			if (fut.valid()) { fut.get(); }
			
			fut = std::async(std::launch::async, [&] {
				out.open("titles.txt", std::ios::app);

				out << title << std::endl;
				title_set.insert(title);

				out.close();
			});
		}
		
		std::this_thread::sleep_for(100ms);
	}

	return 0;
}