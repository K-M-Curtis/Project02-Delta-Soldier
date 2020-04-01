#include "GameMsg.h"
using namespace pb;
using namespace std;

GameSingleTLV::GameSingleTLV(GameMsgType _Type, google::protobuf::Message * _poGameMsg)
{
	this->m_MsgType = _Type;
	this->m_poGameMsg = _poGameMsg;
}

GameSingleTLV::GameSingleTLV(GameMsgType _Type, std::string _szInputData)
{
	switch (_Type)
	{

	case GAME_MSG_LOGON_SYNCPID:          //同步玩家id和名字
	{
		m_poGameMsg = new SyncPid;
		break;
	}
	case GAME_MSG_TALK_CONTENT:             //聊天信息
	{
		m_poGameMsg = new Talk;
		break;
	}
	case GAME_MSG_NEW_POSTION:              //同步玩家位置
	{
		m_poGameMsg = new Position;
		break;
	}
	case GAME_MSG_SKILL_TRIGGER:             //技能触发
	{
		m_poGameMsg = new SkillTrigger;
		break;
	}
	case GAME_MSG_SKILL_CONTACT:             //技能命中
	{
		m_poGameMsg = new SkillContact;
		break;
	}
	case GAME_MSG_CHANGE_WORLD:             //场景切换
	{
		m_poGameMsg = new ChangeWorldRequest;
		break;
	}
	case GAME_MSG_BROADCAST:               //普通广播消息
	{
		m_poGameMsg = new BroadCast;
		break;
	}
	case GAME_MSG_LOGOFF_SYNCPID:          //玩家离线消息
	{
		m_poGameMsg = new SyncPid;
		break;
	}
	case GAME_MSG_SUR_PLAYER:              //同步周边玩家消息
	{
		m_poGameMsg = new SyncPlayers;
		break;
	}
	case GAME_MSG_SKILL_BROAD:           //技能触发广播
	{
		m_poGameMsg = new SkillTrigger;
		break;
	}
	case GAME_MSG_SKILL_CONTACT_BROAD:     //技能命中广播
	{
		m_poGameMsg = new SkillContact;
		break;
	}
	case GAME_MSG_CHANGE_WORLD_RESPONSE:
	{
		m_poGameMsg = new ChangeWorldResponse;
		break;
	}
	default:
		break;
	}

	if (m_poGameMsg != nullptr)
	{
		m_poGameMsg->ParseFromString(_szInputData);
		m_MsgType = _Type;
	}

}

GameSingleTLV::~GameSingleTLV()
{
	if (m_poGameMsg != nullptr)
	{
		delete m_poGameMsg;
	}
}

std::string GameSingleTLV::Serialize()
{
	if (m_poGameMsg)
	{
		string poRet;
		m_poGameMsg->SerializeToString(&poRet);
		return poRet;
	}
	return std::string();
}

GameMsg::GameMsg()
{
}


GameMsg::~GameMsg()
{
	for (auto msg : m_MsgList)
	{
		delete msg;
	}
}


