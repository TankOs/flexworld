#pragma once

#include <SFML/System/Vector3.hpp>

/** Camera.
 */
class Camera {
	public:
		/** Ctor.
		 */
		Camera();

		/** Set field of view.
		 * @param fov FOV (degrees).
		 */
		void set_fov( float fov );

		/** Get field of view.
		 * @return FOV (degrees).
		 */
		float get_fov() const;

		/** Set aspect ratio.
		 * @param aspect Aspect ratio (width/height).
		 */
		void set_aspect_ratio( float aspect );

		/** Get aspect ratio.
		 * @return Aspect ratio (width/height).
		 */
		float get_aspect_ratio() const;

		/** Set position.
		 * @param pos Position.
		 */
		void set_position( const sf::Vector3f& pos );

		/** Get position.
		 * @return Position.
		 */
		const sf::Vector3f& get_position() const;

		/** Set rotation.
		 * @param rotation Rotation (degrees).
		 */
		void set_rotation( const sf::Vector3f& rotation );

		/** Get rotation.
		 * @return Rotation (degrees).
		 */
		const sf::Vector3f& get_rotation() const;

		/** Turn camera.
		 * @param angle Angle.
		 */
		void turn( const sf::Vector3f& angle );

		/** Clamp pitch.
		 * @param clamp Clamp (degrees).
		 */
		void set_pitch_clamp( float clamp );

		/** Walk (along local -z axis).
		 * @param distance Distance.
		 */
		void walk( float distance );

		/** Strafe (along local +x axis).
		 * @param distance Distance.
		 */
		void strafe( float distance );

		/** Fly (along local +y axis).
		 * @param distance Distance.
		 */
		void fly( float distance );

		/** Set eye offset.
		 * @param offset Offset.
		 */
		void set_eye_offset( const sf::Vector3f& offset );

		/** Get eye offset.
		 * @return Offset.
		 */
		const sf::Vector3f& get_eye_offset() const;

	private:
		void apply_clamp();

		sf::Vector3f m_position;
		sf::Vector3f m_rotation;
		sf::Vector3f m_eye_offset;
		float m_fov;
		float m_aspect;
		float m_pitch_clamp;
};
