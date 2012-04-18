#pragma once

/** Texture filter.
 */
enum TextureFilter {
	TRILINEAR_FILTER = 0, ///< Trilinear (mipmaps).
	BILINEAR_FILTER, ///< Bilinear.
	NEAREST_FILTER, ///< Nearest.
	NUM_FILTERS ///< Number of filters.
};
