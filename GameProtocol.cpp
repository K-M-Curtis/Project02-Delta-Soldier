#include "GameProtocol.h"
#include <iostream>
#include "GameChannel.h"
#include "GameRole.h"
#include "GameMsg.h"

using namespace std;
//在protocol这一层来做到自己释放
class GoogleGlobalGarbo
{
public:
	~GoogleGlobalGarbo()
	{
		google::protobuf::ShutdownProtobufLibrary();
	}
}gGoogleGarbo;


GameProtocol::GameProtocol()
{
}


GameProtocol::~GameProtocol()
{
}

UserData * GameProtocol::raw2request(std::string _szInput)
{
	GameMsg *retMsg = nullptr;
	mLastBuff.append(_szInput);
	while (mLastBuff.size() >= 8)
	{
		int messageLen = mLastBuff[0] |
			mLastBuff[1] << 8 |
			mLastBuff[2] << 16 |
			mLastBuff[3] << 24;

		int messageId = mLastBuff[4] |
			mLastBuff[5] << 8 |
			mLastBuff[6] << 16 |
			mLastBuff[7] << 24;

		if (mLastBuff.size() >= messageLen + 8)
		{
			string msgContent = mLastBuff.substr(8, messageLen);
			mLastBuff.erase(0, 8 + messageLen);
			cout << "MessageLen: " << messageLen << " MessageId: " << messageId << endl;

			GameSingleTLV *tlv = new GameSingleTLV((GameSingleTLV::GameMsgType)messageId, msgContent);
			if (retMsg == nullptr)
			{
				retMsg = new GameMsg;
			}
			retMsg->m_MsgList.push_back(tlv);
		}
		else
		{
			break;
		}
	}

	return retMsg;
}

std::string * GameProtocol::response2raw(UserData & _oUserData)
{
	GameMsg &msg = dynamic_cast<GameMsg&>(_oUserData);
	string *retBuf = new string;
	for (auto &single : msg.m_MsgList)
	{
		auto protobufString = single->Serialize();
		int msgLen = protobufString.size();

		int msgId = single->m_MsgType;

		retBuf->push_back((char)(msgLen & 0xff));
		retBuf->push_back((char)(msgLen >> 8 & 0xff));
		retBuf->push_back((char)(msgLen >> 16 & 0xff));
		retBuf->push_back((char)(msgLen >> 24 & 0xff));

		retBuf->push_back((char)(msgId & 0xff));
		retBuf->push_back((char)(msgId >> 8 & 0xff));
		retBuf->push_back((char)(msgId >> 16 & 0xff));
		retBuf->push_back((char)(msgId >> 24 & 0xff));

		retBuf->append(protobufString);

	}
	return retBuf;
}

Irole * GameProtocol::GetMsgProcessor(UserDataMsg & _oUserDataMsg)
{
	return mRole;
}

Ichannel * GameProtocol::GetMsgSender(BytesMsg & _oBytes)
{
	return mChannel;
}
