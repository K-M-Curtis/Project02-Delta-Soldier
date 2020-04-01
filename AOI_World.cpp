#include "AOI_World.h"

using namespace std;

//AOI_World *AOI_World::pxWorld = nullptr;
//AOI_World * AOI_World::GetWorld()
//{
//	if (pxWorld == nullptr)
//	{
//		pxWorld = new AOI_World(85, 410, 75, 400, 10, 20);
//	}
//	return pxWorld;
//}

AOI_World::AOI_World(int _minx, int _maxx, int _miny, int _maxy, int _xcnt, int _ycnt)
{
	this->maxX = _maxx;
	this->minX = _minx;
	this->maxY = _maxy;
	this->minY = _miny;
	this->Xcnt = _xcnt;
	this->Ycnt = _ycnt;

	this->m_grids.reserve(Xcnt * Ycnt);
	for (auto i = 0; i < Xcnt * Ycnt; ++i)
	{
		m_grids.push_back(new AOI_Grid(i));
	}

}


AOI_World::~AOI_World()
{
	for (auto g : this->m_grids)
	{
		delete g;
	}
}

std::list<AOI_Player*> AOI_World::GetSurPlayers(AOI_Player * _player)
{
	int row = (_player->GetY() - minY) / ((maxY - minY) / Ycnt);
	int col = (_player->GetX() - minX) / ((maxX - minX) / Xcnt);
	pair<int, int>row_col[] =
	{
		make_pair(row - 1, col - 1),
		make_pair(row - 1, col),
		make_pair(row - 1, col + 1),
		make_pair(row, col - 1),
		make_pair(row, col),
		make_pair(row, col + 1),
		make_pair(row + 1, col - 1),
		make_pair(row + 1, col),
		make_pair(row + 1, col + 1),

	};

	std::list<AOI_Player*>retPlayers;
	for (auto &p : row_col)
	{
		if (p.first < 0 || p.first >= Ycnt)
		{
			continue;
		}
		if (p.second < 0 || p.second >= Xcnt)
		{
			continue;
		}
		int idx = p.first * Xcnt + p.second;
		for (auto &i : m_grids[idx]->m_players)
		{
			retPlayers.push_back(i);
		}
	}
	return retPlayers;
}

void AOI_World::AddPlayer(AOI_Player * _player)
{
	int idx = Calculate_grid_idx(_player->GetX(), _player->GetY());
	cout << "Player add to gird: " << idx << endl;
	if (idx >= 0 && idx < m_grids.size())
	{
		m_grids[idx]->m_players.push_back(_player);
	}
}

void AOI_World::DelPlayer(AOI_Player * _player)
{
	int idx = Calculate_grid_idx(_player->GetX(), _player->GetY());
	if (idx >= 0 && idx < m_grids.size())
	{
		m_grids[idx]->m_players.remove(_player);
	}
}


bool AOI_World::GridChanged(AOI_Player * _player, int _newX, int _newY)
{
	//判断当前玩家有没有改变格子
	//_player中存储的还是旧的坐标,  新的坐标是 _newX _newY
	int oldIdx = Calculate_grid_idx(_player->GetX(), _player->GetY());
	int newIdx = Calculate_grid_idx(_newX, _newY);

	return oldIdx != newIdx;
}

int AOI_World::Calculate_grid_idx(int x, int y)
{
	int row = (y - minY) / ((maxY - minY) / Ycnt);
	int col = (x - minX) / ((maxX - minX) / Xcnt);
	int index = row * Xcnt + col;

	return index;
}
