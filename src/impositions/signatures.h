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
// Copyright (C) 2010 by Tom Lechner
//
#ifndef SIGNATURES_H
#define SIGNATURES_H

//#include <lax/anobject.h>
//#include <lax/dump.h>
//#include <lax/refcounted.h>
#include "imposition.h"


//------------------------------------- Fold --------------------------------------

enum FoldDirectionType {
	FOLD_Left_Over_To_Right,
	FOLD_Left_Under_To_Right,
	FOLD_Right_Over_To_Left,
	FOLD_Right_Under_To_Left,
	FOLD_Top_Over_To_Bottom,
	FOLD_Top_Under_To_Bottom,
	FOLD_Bottom_Over_To_Top,
	FOLD_Bottom_Under_To_Top,
};

const char *FoldDirectionName(FoldDirectionType dir, int translate=1);

class Fold
{
 public:
	FoldDirectionType direction; //l over to r, l under to r, rol, rul, tob, tub, bot, but
	int whichfold; //index from the left or top side of completely unfolded paper of which fold to act on
	Fold(FoldDirectionType f, int which) { direction=f; whichfold=which; }
};

//--------------------------------------- FoldedPageInfo ---------------------------------------
class FoldedPageInfo
{
 public:
	int currentrow, currentcol; //where this original is currently
	int y_flipped, x_flipped; //how this one is flipped around in its current location
	int finalindexfront, finalindexback;
	Laxkit::NumStack<int> pages; //what pages are actually there, r,c are pushed

	FoldedPageInfo();
};



//------------------------------------ Signature -----------------------------------------
class Signature : public Laxkit::anObject, public Laxkit::RefCounted, public LaxFiles::DumpUtility
{
 public:
	PaperStyle *paperbox; //optional
	double totalwidth, totalheight;

	int sheetspersignature;
	double insetleft, insetright, insettop, insetbottom;

	double tilegapx, tilegapy;
	int tilex, tiley; //how many partitions per sheet of paper

	double creep;
	double rotation; //in practice, not really sure how this works, 
					 //it is mentioned here and there that minor rotation is sometimes needed
					 //for some printers
	char work_and_turn; //0 for no, 1 work and turn == rotate on axis || to major dim, 2 work and tumble=rot axis || minor dim
	int pilesections; //if tiling, whether to repeat content, or continue content (ignored currently)

	int numhfolds, numvfolds;
	Laxkit::PtrStack<Fold> folds;
	double foldedwidth, foldedheight;

	double trimleft, trimright, trimtop, trimbottom; // number<0 means don't trim that edge (for accordion styles)
	double marginleft, marginright, margintop, marginbottom;

	char up; //which direction is up 'l|r|t|b', ie 'l' means points toward the left
	char binding;    //direction to place binding 'l|r|t|b'
	char positivex;  //direction of the positive x axis: 'l|r|t|b'
	char positivey;  //direction of the positive y axis: 'l|r|t|b'

	 //for easy storing of final arrangement:
	FoldedPageInfo **foldinfo;
	virtual void reallocateFoldinfo();
	virtual int applyFold(FoldedPageInfo **foldinfo, int foldlevel, int fromscratch);

	Signature();
	virtual ~Signature();
	virtual void dump_out(FILE *f,int indent,int what,Laxkit::anObject *context);
	virtual void dump_in_atts(LaxFiles::Attribute *att,int flag,Laxkit::anObject *context);

	virtual int IsVertical();
	virtual unsigned int Validity();
	virtual double PatternHeight();
	virtual double PatternWidth();
	virtual double PageHeight();
	virtual double PageWidth();

	virtual int SetPaper(PaperStyle *p);
};


//------------------------------------ SignatureImposition -----------------------------------------
class SignatureImposition : public Imposition
{
 protected:
	int showwholecover; //whether you see the cover+backcover next to each other, or by themselves
	int autoaddsheets; //whether you increase the num of sheets per sig, or num sigs when adding pages, 
	PaperStyle *papersize;
	Signature *signature;      //folding pattern
	//PaperPartition *partition; //partition to insert folding pattern
 public:
	SignatureImposition();
	virtual ~SignatureImposition();
	virtual void dump_out(FILE *f,int indent,int what,Laxkit::anObject *context);
	virtual void dump_in_atts(LaxFiles::Attribute *att,int flag,Laxkit::anObject *context);

	virtual Style *duplicate(Style *s=NULL);
	virtual int SetPaperSize(PaperStyle *npaper);
	virtual int SetPaperGroup(PaperGroup *ngroup);
	virtual PageStyle *GetPageStyle(int pagenum,int local) = 0;
	virtual Laxkit::DoubleBBox *GoodWorkspaceSize(Laxkit::DoubleBBox *bbox=NULL);
	
	virtual Page **CreatePages() = 0;
	virtual int SyncPageStyles(Document *doc,int start,int n);
	
	virtual LaxInterfaces::SomeData *GetPrinterMarks(int papernum=-1);
	virtual LaxInterfaces::SomeData *GetPageOutline(int pagenum,int local);
	virtual LaxInterfaces::SomeData *GetPageMarginOutline(int pagenum,int local);
	
	virtual Spread *Layout(int layout,int which); 
	virtual int NumLayoutTypes();
	virtual const char *LayoutName(int layout); 
	virtual Spread *SingleLayout(int whichpage); 
	virtual Spread *PageLayout(int whichspread) = 0; 
	virtual Spread *PaperLayout(int whichpaper) = 0;
	virtual Spread *GetLittleSpread(int whichspread);

	virtual int NumSpreads(int layout); 
	virtual int NumSpreads(int layout,int setthismany); 
	virtual int NumPapers();
	virtual int NumPapers(int npapers);
	virtual int NumPages();
	virtual int NumPages(int npages);

	virtual int PaperFromPage(int pagenumber);
	virtual int SpreadFromPage(int layout, int pagenumber);
	virtual int GetPagesNeeded(int npapers);
	virtual int GetPapersNeeded(int npages);
	virtual int GetSpreadsNeeded(int npages);
	virtual int *PrintingPapers(int frompage,int topage);

	virtual int NumPageTypes();
	virtual const char *PageTypeName(int pagetype);
	virtual int PageType(int page);
	virtual int SpreadType(int spread);
};



#endif
