#include "dormer.h"
#include <u.h>
#include <libc.h>
#include <draw.h>
#include <event.h>

struct Dormer dm;
static Event e;
static Image *bg;

void
eresized(int new)
{
	dm.etype = Eresize;
	if(new && getwindow(display, Refmesg) < 0)
		sysfatal("eresized: can't reattach window");
	draw(screen, screen->r, bg, nil, ZP);
	flushimage(display, 1);
}

void
initdm(int *dim, char *title)
{
	if(initdraw(0, 0, title) < 0)
		sysfatal("initdraw: %r");
	bg = allocimagemix(display, DDarkgreen, DPaleBlue);
	if(bg == nil)
		sysfatal("allocimagemix: %r");
	eresized(0);
	einit(Emouse | Ekeyboard);
}

void
handle(ulong type)
{
	switch(type){
	case Ekeyboard:
		dm.etype = Ekbd;
		dm.key = e.kbdc;
		break;
	case Emouse:
		if(e.mouse.buttons){
			dm.etype = Eptr;
			dm.btn = e.mouse.buttons;
		}else{
			dm.etype = Ebtn;
			dm.x = e.mouse.xy.x;
			dm.y = e.mouse.xy.y;
		}
		break;
	};
}

void
readev()
{
	if(ecanread(Emouse | Ekeyboard))
		handle(eread(Emouse | Ekeyboard, &e));
	else
		dm.etype = 0;
}