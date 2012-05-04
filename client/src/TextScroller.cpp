#include "TextScroller.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

TextScroller::TextScroller( const sf::Vector2f& bottom_left ) :
	m_bottom_left( bottom_left ),
	m_sustain_duration( sf::seconds( 5 ) ),
	m_fade_duration( sf::milliseconds( 800 ) ),
	m_max_num_lines( 10 ),
	m_line_spacing( 5.0f ),
	m_font_size( 12 ),
	m_font( &sf::Font::getDefaultFont() )
{
}

void TextScroller::set_sustain_duration( const sf::Time& duration ) {
	m_sustain_duration = duration;
}

void TextScroller::set_fade_duration( const sf::Time& fade_duration ) {
	m_fade_duration = fade_duration;
}

void TextScroller::set_max_num_lines( std::size_t num ) {
	m_max_num_lines = num;
}

void TextScroller::update() {
	LineList::iterator line_iter = m_lines.begin();
	sf::Time elapsed_time = m_timer.getElapsedTime();
	
	while( line_iter != m_lines.end() ) {
		Line& line = *line_iter;

		// Line expired?
		if( line.creation_time + m_sustain_duration + m_fade_duration <= elapsed_time ) {
			line_iter = m_lines.erase( line_iter );
			continue;
		}
		else if( line.creation_time + m_sustain_duration <= elapsed_time ) { // Fade out?
			sf::Color color = line.text.getColor();

			color.a = static_cast<sf::Uint8>( (line.creation_time + m_sustain_duration + m_fade_duration - elapsed_time).asSeconds() / m_fade_duration.asSeconds() * 255.0f );
			line.text.setColor( color );

			color = line.shadow.getColor();
			color.a = static_cast<sf::Uint8>( (line.creation_time + m_sustain_duration + m_fade_duration - elapsed_time).asSeconds() / m_fade_duration.asSeconds() * 120.0f );
			line.shadow.setColor( color );
		}

		++line_iter;
	}
}

void TextScroller::render( sf::RenderTarget& target ) const {
	LineList::const_iterator line_iter( m_lines.begin() );
	LineList::const_iterator line_iter_end( m_lines.end() );
	
	for( ; line_iter != line_iter_end; ++line_iter ) {
		target.draw( line_iter->shadow );
		target.draw( line_iter->text );
	}
}

void TextScroller::add_text( const sf::String& text ) {
	m_lines.push_back( Line() );

	Line& line = m_lines.back();

	line.creation_time = m_timer.getElapsedTime();
	line.text.setString( text );

	line.shadow = line.text;
	line.shadow.setColor( sf::Color( 0, 0, 0, 120 ) );

	invalidate();
}

void TextScroller::invalidate() {
	// Update positions and fonts.
	LineList::reverse_iterator r_line_iter = m_lines.rbegin();
	LineList::reverse_iterator r_line_iter_end = m_lines.rend();

	sf::Vector2f position = m_bottom_left;

	for( ; r_line_iter != r_line_iter_end; ++r_line_iter ) {
		Line& line = *r_line_iter;

		// Reset font + character size.
		line.text.setFont( *m_font );
		line.text.setCharacterSize( m_font_size );

		line.shadow.setFont( *m_font );
		line.shadow.setCharacterSize( m_font_size );

		// Update position.
		position.y -= line.text.getGlobalBounds().height;
		line.text.setPosition( position );
		line.shadow.setPosition( position + sf::Vector2f( 1, 1 ) );

		// Apply line spacing for next line.
		position.y -= m_line_spacing;
	}
}
