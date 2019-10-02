#include "DiabloUI/text_draw.h"

#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/text.h"
#include "DiabloUI/ui_item.h"

namespace dvl {

extern SDL_Surface *pal_surface;

namespace {

int AlignXOffset(int flags, const SDL_Rect &dest, int w)
{
	if (flags & UIS_CENTER)
		return (dest.w - w) / 2;
	if (flags & UIS_RIGHT)
		return dest.w - w;
	return 0;
}

} // namespace

void DrawTTF(const char *text, const SDL_Rect &rect, int flags,
    const SDL_Color &text_color, const SDL_Color &shadow_color,
    TtfSurfaceCache **render_cache)
{
	if (*render_cache == nullptr) {
		*render_cache = new TtfSurfaceCache();
		(*render_cache)->text = TTF_RenderUTF8_Solid(font, text, text_color);
		(*render_cache)->shadow = TTF_RenderUTF8_Solid(font, text, shadow_color);
	}
	SDL_Surface *text_surface = (*render_cache)->text;
	SDL_Surface *shadow_surface = (*render_cache)->shadow;

	SDL_Rect dest_rect = rect;
	const int x_offset = AlignXOffset(flags, rect, text_surface->w);
	const int y_offset = (flags & UIS_VCENTER) ? (rect.h - text_surface->h) / 2 : 0;
	dest_rect.x += static_cast<decltype(SDL_Rect().x)>(SCREEN_X + x_offset);
	dest_rect.y += static_cast<decltype(SDL_Rect().y)>(SCREEN_Y + y_offset);

	SDL_Rect shadow_rect = dest_rect;
	shadow_rect.x += 2;
	if (SDL_BlitSurface(shadow_surface, nullptr, pal_surface, &shadow_rect) <= -1)
		SDL_Log(SDL_GetError());
	if (SDL_BlitSurface(text_surface, nullptr, pal_surface, &dest_rect) <= -1)
		SDL_Log(SDL_GetError());
}

void DrawArtStr(const char *text, const SDL_Rect &rect, int flags, bool drawTextCursor)
{
	_artFontTables size = AFT_SMALL;
	_artFontColors color = flags & UIS_GOLD ? AFC_GOLD : AFC_SILVER;

	if (flags & UIS_MED)
		size = AFT_MED;
	else if (flags & UIS_BIG)
		size = AFT_BIG;
	else if (flags & UIS_HUGE)
		size = AFT_HUGE;

	const int x = rect.x + AlignXOffset(flags, rect, GetArtStrWidth(text, size));
	const int y = rect.y + ((flags & UIS_VCENTER) ? (rect.h - ArtFonts[size][color].h()) / 2 : 0);

	int sx = x, sy = y;
	for (size_t i = 0, n = strlen(text); i < n; i++) {
		if (text[i] == '\n') {
			sx = x;
			sy += ArtFonts[size][color].h();
			continue;
		}
		BYTE w = FontTables[size][*(BYTE *)&text[i] + 2] ? FontTables[size][*(BYTE *)&text[i] + 2] : FontTables[size][0];
		DrawArt(sx, sy, &ArtFonts[size][color], *(BYTE *)&text[i], w);
		sx += w;
	}
	if (drawTextCursor && GetAnimationFrame(2, 500)) {
		DrawArt(sx, sy, &ArtFonts[size][color], '|');
	}
}

} // namespace dvl
