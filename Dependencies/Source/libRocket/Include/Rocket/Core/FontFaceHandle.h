/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef ROCKETCOREFONTFACEHANDLE_H
#define ROCKETCOREFONTFACEHANDLE_H

#include <Rocket/Core/Header.h>
#include <Rocket/Core/ReferenceCountable.h>
#include <Rocket/Core/UnicodeRange.h>
#include <Rocket/Core/Font.h>
#include <Rocket/Core/FontEffect.h>
#include <Rocket/Core/FontGlyph.h>
#include <Rocket/Core/Geometry.h>
#include <Rocket/Core/String.h>
#include <Rocket/Core/Texture.h>

namespace Rocket {
namespace Core {

class FontFaceLayer;

/**
	@author Peter Curry
 */

class ROCKETCORE_API FontFaceHandle : public ReferenceCountable
{
public:
	FontFaceHandle();
	virtual ~FontFaceHandle();

	/// Returns the average advance of all glyphs in this font face.
	/// @return An approximate width of the characters in this font face.
	int GetCharacterWidth() const;

	/// Returns the point size of this font face.
	/// @return The face's point size.
	int GetSize() const;
	/// Returns the pixel height of a lower-case x in this font face.
	/// @return The height of a lower-case x.
	int GetXHeight() const;
	/// Returns the default height between this font face's baselines.
	/// @return The default line height.
	int GetLineHeight() const;

	/// Returns the font's baseline, as a pixel offset from the bottom of the font.
	/// @return The font's baseline.
	int GetBaseline() const;

	/// Returns the font's glyphs.
	/// @return The font's glyphs.
	const FontGlyphMap& GetGlyphs() const;

	/// Returns the width a string will take up if rendered with this handle.
	/// @param[in] string The string to measure.
	/// @param[in] prior_character The optionally-specified character that immediately precedes the string. This may have an impact on the string width due to kerning.
	/// @return The width, in pixels, this string will occupy if rendered with this handle.
	virtual int GetStringWidth(const WString& string, word prior_character = 0) const;

	/// Generates, if required, the layer configuration for a given array of font effects.
	/// @param[in] font_effects The list of font effects to generate the configuration for.
	/// @return The index to use when generating geometry using this configuration.
	virtual int GenerateLayerConfiguration(FontEffectMap& font_effects) = 0;
	/// Generates the texture data for a layer (for the texture database).
	/// @param[out] texture_data The pointer to be set to the generated texture data.
	/// @param[out] texture_dimensions The dimensions of the texture.
	/// @param[in] layer_id The id of the layer to request the texture data from.
	/// @param[in] texture_id The index of the texture within the layer to generate.
	virtual bool GenerateLayerTexture(const byte*& texture_data, Vector2i& texture_dimensions, FontEffect* layer_id, int texture_id) = 0;

	/// Generates the geometry required to render a single line of text.
	/// @param[out] geometry An array of geometries to generate the geometry into.
	/// @param[in] string The string to render.
	/// @param[in] position The position of the baseline of the first character to render.
	/// @param[in] colour The colour to render the text.
	/// @return The width, in pixels, of the string geometry.
	virtual int GenerateString(GeometryList& geometry, const WString& string, const Vector2f& position, const Colourb& colour, int layer_configuration = 0) const = 0;
	/// Generates the geometry required to render a line above, below or through a line of text.
	/// @param[out] geometry The geometry to append the newly created geometry into.
	/// @param[in] position The position of the baseline of the lined text.
	/// @param[in] width The width of the string to line.
	/// @param[in] height The height to render the line at.
	/// @param[in] colour The colour to draw the line in.
	virtual void GenerateLine(Geometry* geometry, const Vector2f& position, int width, Font::Line height, const Colourb& colour) const = 0;

	/// Returns the font face's raw charset (the charset range as a string).
	/// @return The font face's charset.
	const String& GetRawCharset() const;
	/// Returns the font face's charset.
	/// @return The font face's charset.
	const UnicodeRangeList& GetCharset() const;

protected:

	/// Destroys the handle.
	virtual void OnReferenceDeactivate();

	typedef Container::map< word, int >::Type GlyphKerningMap;
	typedef Container::map< word, GlyphKerningMap >::Type FontKerningMap;

	FontGlyphMap glyphs;
	FontKerningMap kerning;
    
	int GetKerning(word lhs, word rhs) const;

    // Generates (or shares) a layer derived from a font effect.
    virtual FontFaceLayer* GenerateLayer(FontEffect* font_effect);

	typedef Container::map< const FontEffect*, FontFaceLayer* >::Type FontLayerMap;
	typedef Container::map< String, FontFaceLayer* >::Type FontLayerCache;
	typedef Container::vector< FontFaceLayer* >::Type LayerConfiguration;
	typedef Container::vector< LayerConfiguration >::Type LayerConfigurationList;

	// The list of all font layers, index by the effect that instanced them.
	FontFaceLayer* base_layer;
	FontLayerMap layers;
	// Each font layer that generated geometry or textures, indexed by the respective generation
	// key.
	FontLayerCache layer_cache;

	// All configurations currently in use on this handle. New configurations will be generated as
	// required.
	LayerConfigurationList layer_configurations;

	// The average advance (in pixels) of all of this face's glyphs.
	int average_advance;

	int size;
	int x_height;
	int line_height;
	int baseline;

	float underline_position;
	float underline_thickness;

	String raw_charset;
	UnicodeRangeList charset;
};

}
}

#endif
