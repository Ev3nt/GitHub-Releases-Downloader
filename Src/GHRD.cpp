#include "GHRD.h"

#include <sstream>

namespace GitHub {
	Releases::Releases(LPCSTR RepositoryURL)
	{
		url = RepositoryURL;

		IStream* stream;
		URLOpenBlockingStream(NULL, (url + std::string("/releases")).c_str(), &stream, NULL, NULL);

		STATSTG stats;
		stream->Stat(&stats, STATFLAG_DEFAULT);
		SIZE_T size = (SIZE_T)stats.cbSize.QuadPart;

		LPSTR buffer = new char[size];
		stream->Read(buffer, size, NULL);
		std::istringstream ss(buffer);
		delete[] buffer;

		bool release_block = false;

		release* info = NULL;

		while (!ss.eof())
		{
			char main_buffer[2048];
			ZeroMemory(main_buffer, sizeof(main_buffer));
			ss.getline(main_buffer, sizeof(main_buffer));
			std::string main_line(main_buffer);

			if (release_block)
			{
				if (strstr(main_line.c_str(), "class=\"css-truncate-target\""))
					info->version = main_line.substr(main_line.find('>') + 1, main_line.find('<', main_line.find('>')) - main_line.find('>') - 1);
				else if (strstr(main_line.c_str(), "class=\"markdown-body\""))
				{
					std::string comment = "";

					bool description_end = false;

					while (!description_end)
					{
						char buffer[2048];
						ZeroMemory(buffer, sizeof(buffer));
						ss.getline(buffer, sizeof(buffer));
						std::string line = buffer;

						while (line[0] == ' ')
							line.erase(0, 1);

						int i;
						while ((i = line.find('<')) != -1)
						{
							std::string element = line.substr(i, line.find('>') - i + 1);

							if (!element.compare("<p>"))
								line.erase(i, element.size());
							else if (!element.compare("<br>"))
								line.replace(i, element.size(), "\n");
							else if (!element.compare("</p>"))
								line.replace(i, element.size(), "\n\n");
							else if (!element.compare("</div>"))
							{
								line.erase(i, element.size());

								description_end = true;
							}
						}

						comment += line;
					}

					comment.erase(comment.size() - 2, 2);

					info->comment = comment;
				}
				else if (strstr(main_line.c_str(), "<!-- /.release -->"))
				{
					release_block = false;
					info->parent = this;
					releases.push_back(*info);
					delete info;
				}
			}
			else
				if (strstr(main_line.c_str(), "class=\"release-entry\""))
				{
					release_block = true;
					info = new release;
				}

		}

		ss.clear();
	}

	Releases::~Releases()
	{
		releases.~vector();
	}

	release Releases::operator[](size_t n)
	{
		return releases[n];
	}

	size_t Releases::GetSize()
	{
		return releases.size();
	}

	std::string Releases::GetUrl()
	{
		return url;
	}

	void release::Download(LPCSTR AssetName, LPCSTR FileName)
	{
		Releases* releases = (Releases*)parent;
		URLDownloadToFile(NULL, (releases->GetUrl() + std::string("/releases/download/") + version + std::string("/") + AssetName).c_str(), FileName, NULL, NULL);
	}
}