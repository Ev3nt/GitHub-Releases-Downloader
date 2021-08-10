#pragma once

#include <Windows.h>
#include <string>
#include <vector>

#pragma comment(lib, "urlmon.lib")

namespace GitHub {
	typedef struct {
		std::string version;
		std::string comment;
		HANDLE parent;

		void Download(LPCSTR AssetName, LPCSTR FileName);
	} release;

	class Releases {
	public:
		Releases(LPCSTR RepositoryURL);
		~Releases();

		release operator[](size_t n);

		size_t GetSize();
		std::string GetUrl();
	private:
		std::vector<release> releases;
		std::string url;
	};
}