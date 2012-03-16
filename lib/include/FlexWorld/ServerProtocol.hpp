#pragma once

#include <FlexWorld/Protocol.hpp>
#include <FlexWorld/Message.hpp>
#include <FlexWorld/Messages/OpenLogin.hpp>
#include <FlexWorld/Messages/ServerInfo.hpp>
#include <FlexWorld/Messages/LoginOK.hpp>
#include <FlexWorld/Messages/Ready.hpp>
#include <FlexWorld/Messages/Beam.hpp>
#include <FlexWorld/Messages/RequestChunk.hpp>
#include <FlexWorld/Messages/ChunkUnchanged.hpp>
#include <FlexWorld/Messages/CreateEntity.hpp>
#include <FlexWorld/TemplateUtils.hpp>

namespace flex {

typedef tpl::Typelist<msg::OpenLogin, tpl::Typelist<msg::ServerInfo, tpl::Typelist<msg::LoginOK, tpl::Typelist<msg::Ready, tpl::Typelist<msg::Beam, tpl::Typelist<msg::RequestChunk, tpl::Typelist<msg::ChunkUnchanged, tpl::Typelist<msg::CreateEntity>>>>>>>> ServerMessageList;
typedef Protocol<ServerMessageList> ServerProtocol;

}
