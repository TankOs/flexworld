#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/Login.hpp>
#include <FlexWorld/Messages/Chat.hpp>
#include <FlexWorld/TemplateUtils.hpp>

#include <string>

namespace flex {

typedef tpl::Typelist<msg::Login, tpl::Typelist<msg::Chat> > ServerMessageList;
typedef Protocol<ServerMessageList, uint8_t, Message::Buffer::value_type> ServerProtocol;

}
