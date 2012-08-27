#pragma once

#include <SFML/System/Vector3.hpp>
#include <cstdint>

namespace fw {

typedef sf::Vector3f PlanetCoordinate; ///< Planet coordinate type.
typedef uint16_t PlanetSizeType; ///< Planet size element type.
typedef sf::Vector3<PlanetSizeType> PlanetSize; ///< Planet size type.

typedef sf::Vector3<PlanetSizeType> ChunkVector; ///< Chunk position vector type.
typedef uint8_t ChunkSizeType; ///< Chunk size type.
typedef sf::Vector3<ChunkSizeType> ChunkSize; ///< Chunk size type.

typedef sf::Vector3<ChunkSizeType> BlockVector; ///< Block position type.

}
