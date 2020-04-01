#pragma once
#include "zinx.h"

class GameChannel;
class GameRole;

class GameProtocol :
	public Iprotocol
{
public:
	GameProtocol();
	virtual ~GameProtocol();

	virtual UserData *raw2request(std::string _szInput)override;

	virtual std::string *response2raw(UserData &_oUserData)override;

	GameChannel *mChannel;
	GameRole *mRole;

protected:
	virtual Irole *GetMsgProcessor(UserDataMsg &_oUserDataMsg)override;

	virtual Ichannel *GetMsgSender(BytesMsg &_oBytes)override;

	std::string mLastBuff;
};


