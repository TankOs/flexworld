#include "UserInterface.hpp"

#include <FlexWorld/Config.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <Rocket/Core.h>

UserInterface::UserInterface( const sf::RenderTarget& render_target ) :
	m_render_target( render_target ),
	m_context( 
		Rocket::Core::CreateContext(
			"default",
			Rocket::Core::Vector2i(
				render_target.getSize().x,
				render_target.getSize().y
			)
		)
	),
	m_chat_window_document( nullptr )
{
	// Load fonts.
	Rocket::Core::FontDatabase::LoadFontFace(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/Economica-Bold.ttf" )).c_str(),
		"MenuFont",
		Rocket::Core::Font::STYLE_NORMAL,
		Rocket::Core::Font::WEIGHT_NORMAL
	);

	// Load chat window document.
	m_chat_window_document = m_context->LoadDocument(
		(flex::ROOT_DATA_DIRECTORY + std::string( "local/gui/chat_window.rml" )).c_str()
	);

	m_chat_window_document->Show();
}

UserInterface::~UserInterface() {
	m_chat_window_document->RemoveReference();
	m_context->RemoveReference();
}

void UserInterface::render() const {
	m_context->Render();
}
