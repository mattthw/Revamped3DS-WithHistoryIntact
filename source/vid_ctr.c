/*
Copyright (C) 2015 Felipe Izzo
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "quakedef.h"
#include "d_local.h"

#include <3ds.h>
#include "ctr.h"

viddef_t	vid;				// global video state

int	basewidth, baseheight;

byte	*vid_buffer[2];
short	*zbuffer;
byte	surfcache[1024*1536];
u16 	*left_fb, *right_fb;
float separation_distance;

u16	d_8to16table[256];

void	VID_SetPalette (unsigned char *palette)
{
	int i;
	u8 *pal = palette;
	u16 *table = d_8to16table;
	unsigned r, g, b;
	for(i=0; i<256; i++){
		r = pal[0];
		g = pal[1];
		b = pal[2];
		table[0] = RGB8_to_565(r,g,b);
		table++;
		pal += 3;
	}
}

void	VID_ShiftPalette (unsigned char *palette)
{
	VID_SetPalette(palette);
}

void	VID_Init (unsigned char *palette)
{
	baseheight = 240;
	basewidth = 400;

	vid_buffer[0] = malloc(sizeof(byte) * basewidth * baseheight); //Left buffer
	vid_buffer[1] = malloc(sizeof(byte) * basewidth * baseheight); //Right buffer
	zbuffer = malloc(sizeof(short) * basewidth * baseheight);

	vid.maxwarpwidth = vid.width = vid.conwidth = basewidth;
	vid.maxwarpheight = vid.height = vid.conheight = baseheight;
	vid.aspect = 1.0;
	vid.numpages = 1;
	vid.colormap = host_colormap;
	vid.fullbright = 256 - LittleLong (*((int *)vid.colormap + 2048));
	vid.buffer = vid.conbuffer = vid_buffer[0];
	vid.rowbytes = vid.conrowbytes = basewidth;
	VID_SetPalette(palette);
	d_pzbuffer = zbuffer;
	D_InitCaches (surfcache, sizeof(surfcache));

	left_fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	right_fb = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
}

void	VID_SetBuffer (int buffer)
{
	if(buffer)
		vid.conbuffer = vid.buffer = vid_buffer[1];
	else
		vid.conbuffer = vid.buffer = vid_buffer[0];
}

void	VID_Shutdown (void)
{
	free(vid_buffer[0]);
	free(vid_buffer[1]);
	free(zbuffer);
}

void	VID_Update (vrect_t *rects)
{
	int x,y;
	for(x = rects->x; x < rects->width; x++){
		for(y = rects->x; y < rects->height; y++){
			left_fb[((x*240) + (239 -y))] = d_8to16table[vid_buffer[0][y*basewidth + x]];
			if(separation_distance)
				right_fb[((x*240) + (239 -y))] = d_8to16table[vid_buffer[1][y*basewidth + x]];
		}
	}
}

/*
================
D_BeginDirectRect
================
*/
void D_BeginDirectRect (int x, int y, byte *pbitmap, int width, int height)
{
}


/*
================
D_EndDirectRect
================
*/
void D_EndDirectRect (int x, int y, int width, int height)
{
}
