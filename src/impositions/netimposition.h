//
// $Id$
//	
// Laidout, for laying out
// Please consult http://www.laidout.org about where to send any
// correspondence about this software.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// For more details, consult the COPYING file in the top directory.
//
// Copyright (C) 2004-2007 by Tom Lechner
//
#ifndef NETIMPOSITION_H
#define NETIMPOSITION_H

#include "imposition.h"
#include "nets.h"
#include "poly.h"

#define SINGLES_WITH_ADJACENT_LAYOUT 3

class NetImposition : public Imposition
{
 public:
	PageStyle *pagestyle;
	AbstractNet *abstractnet;
	Laxkit::RefPtrStack<Net> nets;
	int maptoabstractnet;
	int printnet;
	int netisbuiltin;

	Polyhedron *polyhedron;

	NetImposition(Net *newnet=NULL);
	virtual ~NetImposition();
	virtual StyleDef *makeStyleDef();
	virtual Style *duplicate(Style *s=NULL);
	
	virtual int SetPaperSize(PaperStyle *npaper);
	virtual PageStyle *GetPageStyle(int pagenum,int local);
	virtual Page **CreatePages();

	virtual LaxInterfaces::SomeData *GetPageOutline(int pagenum,int local);

	virtual Spread *Layout(int layout,int which); 
	virtual int NumLayouts();
	virtual const char *LayoutName(int layout); 
	virtual Spread *GenerateSpread(Spread *spread, Net *net, int pageoffset);
	//----------
	virtual Spread *SingleLayout(int whichpage); 
	virtual Spread *SingleLayoutWithAdjacent(int whichpage); 
	virtual Spread *PageLayout(int whichspread); 
	virtual Spread *PaperLayout(int whichpaper);

	virtual int NumSpreads(int layout); 
	virtual int PaperFromPage(int pagenumber);
	virtual int SpreadFromPage(int pagenumber);
	virtual int SpreadFromPage(int layout, int pagenumber);
	virtual int GetPagesNeeded(int npapers);
	virtual int GetPapersNeeded(int npages);
	virtual int GetSpreadsNeeded(int npages);
	virtual int *PrintingPapers(int frompage,int topage);

	virtual int PageType(int page);
	virtual int SpreadType(int spread);

	virtual void dump_out(FILE *f,int indent,int what,Laxkit::anObject *context);
	virtual void dump_in_atts(LaxFiles::Attribute *att,int flag,Laxkit::anObject *context);
	virtual AbstractNet *AbstractNetFromFile(const char *filename);
	
	 //new for this class:
	virtual int SetNet(const char *nettype);
	virtual int SetNet(Net *newnet);
	virtual void setPage();
	virtual int numActiveFaces();
	virtual int numActiveNets();
};

#endif

