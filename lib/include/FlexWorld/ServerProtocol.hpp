#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/TemplateUtils.hpp>

namespace flex {

typedef tpl::Typelist<msg::OpenLogin, tpl::Typelist<msg::ServerInfo, tpl::Typelist<msg::LoginOK> > > ServerMessageList;
typedef Protocol<ServerMessageList> ServerProtocol;

}
