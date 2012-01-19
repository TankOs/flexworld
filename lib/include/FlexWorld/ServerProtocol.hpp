#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/TemplateUtils.hpp>

namespace flex {

typedef tpl::Typelist<msg::OpenLogin, tpl::Typelist<msg::ServerInfo> > ServerMessageList;
typedef Protocol<ServerMessageList> ServerProtocol;

}
