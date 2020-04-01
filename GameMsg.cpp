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

	case GAME_MSG_LOGON_SYNCPID:          //ͬ�����id������
	{
		m_poGameMsg = new SyncPid;
		break;
	}
	case GAME_MSG_TALK_CONTENT:             //������Ϣ
	{
		m_poGameMsg = new Talk;
		break;
	}
	case GAME_MSG_NEW_POSTION:              //ͬ�����λ��
	{
		m_poGameMsg = new Position;
		break;
	}
	case GAME_MSG_SKILL_TRIGGER:             //���ܴ���
	{
		m_poGameMsg = new SkillTrigger;
		break;
	}
	case GAME_MSG_SKILL_CONTACT:             //��������
	{
		m_poGameMsg = new SkillContact;
		break;
	}
	case GAME_MSG_CHANGE_WORLD:             //�����л�
	{
		m_poGameMsg = new ChangeWorldRequest;
		break;
	}
	case GAME_MSG_BROADCAST:               //��ͨ�㲥��Ϣ
	{
		m_poGameMsg = new BroadCast;
		break;
	}
	case GAME_MSG_LOGOFF_SYNCPID:          //���������Ϣ
	{
		m_poGameMsg = new SyncPid;
		break;
	}
	case GAME_MSG_SUR_PLAYER:              //ͬ���ܱ������Ϣ
	{
		m_poGameMsg = new SyncPlayers;
		break;
	}
	case GAME_MSG_SKILL_BROAD:           //���ܴ����㲥
	{
		m_poGameMsg = new SkillTrigger;
		break;
	}
	case GAME_MSG_SKILL_CONTACT_BROAD:     //�������й㲥
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


