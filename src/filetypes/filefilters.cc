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
// Copyright (C) 2007 by Tom Lechner
//

#include <lax/strmanip.h>

#include "../language.h"
#include "filefilters.h"
#include "../laidout.h"


using namespace LaxFiles;

//---------------------------- DocumentExportConfig ------------------------------
/*! \class DocumentExportConfig
 * \brief Holds basic settings for exporting a document.
 *
 * If filename==NULL and tofiles!=NULL, then write out one spread per file, and tofiles
 * must be a file name template.
 *
 * \todo On exporting, should have option to collect for out to a directory. This includes
 *   possibly rasterizing certain portions of a document, and making references to it in
 *   the exported document. If this happens, also output an output log of warnings/errors
 *   and a list of applied work arounds to the output directory. Finally, collect for out should 
 *   have option to zip up the output directory?
 */
/*! \var int DocumentExportConfig::target
 * 
 * 0 for filename,
 * 1 for tofiles: 1 spread per file,
 * 2 for command.
 */

DocumentExportConfig::DocumentExportConfig()
{
	target=0;
	filename=NULL;
	tofiles=NULL;
	start=end=0;
	layout=0;
	doc=NULL;
	filter=NULL;
}

/*! Increments count on ndoc if it exists.
 */
DocumentExportConfig::DocumentExportConfig(Document *ndoc, const char *file, const char *to,int l,int s,int e)
{
	target=0;
	filename=newstr(file);
	tofiles=newstr(to);
	start=s;
	end=e;
	layout=l;
	doc=ndoc;
	filter=NULL;
	if (doc) doc->inc_count();
}

/*! Decrements doc if it exists.
 */
DocumentExportConfig::~DocumentExportConfig()
{
	if (filename) delete[] filename;
	if (tofiles)  delete[] tofiles;
	if (doc) doc->dec_count();
}

void DocumentExportConfig::dump_out(FILE *f,int indent,int what)
{
	char spc[indent+1]; memset(spc,' ',indent); spc[indent]='\0';
	if (what==-1) {
		fprintf(f,"%stofile /file/to/export/to \n",spc);
		fprintf(f,"%stofiles  \"/files/like###.this\"  #the # section is replaced with the page index\n",spc);
		fprintf(f,"%s                                #Only one of filename or tofiles should be present\n",spc);
		fprintf(f,"%sformat  \"SVG 1.0\"    #the format to export as\n",spc);
		fprintf(f,"%simposition  Booklet  #the imposition used. This is set automatically when exporting a document\n",spc);
		fprintf(f,"%slayout pages         #this is particular to the imposition used by the document\n",spc);
		fprintf(f,"%sstart 3              #the starting index to export, counting from 0\n",spc);
		fprintf(f,"%send   5              #the ending index to export, counting from 0\n",spc);
		return;
	}
	if (filename) fprintf(f,"%stofile %s\n",spc,filename);
	if (tofiles) fprintf(f,"%stofiles  \"%s\"\n",spc,tofiles);
	if (filter) fprintf(f,"%sformat  \"%s\"\n",spc,filter->VersionName());
	if (doc && doc->docstyle && doc->docstyle->imposition) {
		fprintf(f,"%simposition \"%s\"\n",spc,doc->docstyle->imposition->whattype());
		fprintf(f,"%slayout \"%s\"\n",spc,doc->docstyle->imposition->LayoutName(layout));
	}
	fprintf(f,"%sstart %d\n",spc,start);
	fprintf(f,"%send   %d\n\n",spc,end);
}

void DocumentExportConfig::dump_in_atts(Attribute *att,int flag)
{
	char *name,*value;
	int c,c2;
	start=end=-1;
	for (c=0; c<att->attributes.n; c++)  {
		name=att->attributes.e[c]->name;
		value=att->attributes.e[c]->value;
		if (!strcmp(name,"tofile")) {
			makestr(filename,value);
		} else if (!strcmp(name,"tofiles")) {
			makestr(tofiles,value);
		} else if (!strcmp(name,"format")) {
			filter=NULL;
			 //search for exact format match first
			for (c2=0; c2<laidout->exportfilters.n; c2++) {
				if (!strcmp(laidout->exportfilters.e[c2]->VersionName(),value)) {
					filter=laidout->exportfilters.e[c2];
					break;
				}
			}
			 //if no match, search for first case insensitive match
			if (filter==NULL) {
				for (c2=0; c2<laidout->exportfilters.n; c2++) {
					if (!strncasecmp(laidout->exportfilters.e[c2]->VersionName(),value,strlen(value))) {
						filter=laidout->exportfilters.e[c2];
						break;
					}
				}
			}
		} else if (!strcmp(name,"imposition")) {
			//***
		} else if (!strcmp(name,"layout")) {
			//***
		} else if (!strcmp(name,"start")) {
			IntAttribute(value,&start);
		} else if (!strcmp(name,"end")) {
			IntAttribute(value,&end);
		}
	}
	if (start<0) start=0;
	if (end<0) end=1000000000;
}

//------------------------------------- FileFilter -----------------------------------
/*! \class FileFilter
 * \brief Abstract base class of input and output file filters.
 *
 * These filters act on whole files, not on chunks of them. They are for exporting
 * to an entire postscript file, for instance, or importing an entire svg file to 
 * an object or document context.
 *
 * Descendent classes of this class will have a whattype() of "FileInputFilter" or 
 * "FileOutputFilter". Other distinguishing tags are accessible through the proper
 * filter info functions, not through the whattype() function.
 */


/*! \var Plugin *FileFilter::plugin;
 * \brief Which plugin, if any, the filter came from. NULL if is built in.
 * \todo *** implement plugins!!
 */
/*! \fn ~FileFilter()
 * \brief Empty virtual destructor.
 */
/*! \fn const char *FileFilter::Author()
 * \brief Return who made this filter. For default ones, this is "Laidout".
 */
/*! \fn const char *FileFilter::FilterVersion()
 * \brief Return a string representing the filter version.
 */
/*! \fn const char *FileFilter::Format()
 * \brief Return the general file format the filter deals with.
 *
 * For instance, this would be "Postscript", "Svg", etc.
 */
/*! \fn const char *FileFilter::DefaultExtension()
 * \brief Return default file extension, something like "eps" or "svg".
 */
/*! \fn const char *FileFilter::Version()
 * \brief The version of Format() that the filter knows how to deal with.
 *
 * For PDF, for instance, this might be "1.4" or "1.3".
 * For SVG, this could even be broken down into "1.0", "1.1", or "1.0-inkscape", for instance.
 */
/*! \fn const char *FileFilter::VersionName()
 * \brief A name for the format and version for a screen dialog.
 *
 * This might be "Postscript LL3", or "Svg, version 1.1". In the latter case, the
 * format was "SVG" and the version was "1.1", but the name is something composite, which
 * is open to translations.
 */
/*! \fn const char *FileFilter::FilterClass()
 * \brief What the filter dumps from or to.
 *
 * \todo These can currently be "document", "object", "image" (a raster image),
 * or resources such as "gradient", "palette", or "net".
 *
 * LaidoutApp keeps a catalog of filters grouped by FilterClass(), and then by whether
 * they are for input or output.
 */
/*! \fn Laxkit::anXWindow *ConfigDialog()
 * \brief Return a configuration dialog for the filter.
 *
 * Default is to return NULL.
 *
 * \todo *** implement this feature!
 */

FileFilter::FileFilter()
{
	plugin=NULL; 
	flags=0;
}

//------------------------------------- FileInputFilter -----------------------------------
/*! \class FileInputFilter
 * \brief Abstract base class of input file filters.
 */


/*! \fn const char *FileInputFilter::FileType(const char *first100bytes)
 * \brief Return the version of the filter's format that the file seems to be, or NULL if not recognized.
 */
/*! \fn int FileInputFilter::Out(const char *file, Laxkit::anObject *context, char **error_ret)
 * \brief The function that outputs the stuff.
 *
 * If file!=NULL, then output to that single file, and ignore the files in context.
 *
 * context must be a configuration object that the filter understands. For instance, this
 * might be a DocumentExportConfig object.
 *
 * On success, return 0. If there are any warnings they are put in error_ret.
 * On failure, return nonzero, and put error messages in error_ret.
 */

//------------------------------------- FileOutputFilter -----------------------------------
/*! \class FileOutputFilter
 * \brief Abstract base class of input file filters.
 */
/*! \fn int FileOutputFilter::Verify(Laxkit::anObject *context)
 * \brief Preflight checker.
 *
 * This feature is not thought out enough to even have decent documentation. Default just returns 1.
 *
 * \todo Ideally, this function should return some sort of set of objects that cannot be transfered
 *   in the given format, and other objects that can be transfered, but only in a lossless way.
 */
/*! \fn int FileOutputFilter::Out(const char *file, Laxkit::anObject *context, char **error_ret)
 * \brief The function that outputs the stuff.
 *
 * context must be a configuration object that the filter understands. For instance, this
 * might be a DocumentExportConfig object.
 *
 * On success, return 0. If there are any warnings they are put in error_ret.
 * On failure, return nonzero, and put error messages in error_ret.
 */
	



