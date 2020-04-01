#pragma once
#include <deque>
#include <string>

class RandomName
{
public:
	RandomName();
	~RandomName();

	static RandomName &GetInstance();
	void LoadFile();
	std::string GetName();
	void ReleaseName(std::string szName);

private:
	static RandomName smInstance;
	std::deque<std::string>m_names;

};

