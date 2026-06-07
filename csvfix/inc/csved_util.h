//---------------------------------------------------------------------------
// csved_util.h
//
// utilities for CSVED
//
// Copyright (C) 2009 Neil Butterworth
//---------------------------------------------------------------------------


#ifndef INC_CSVED_UTIL_H
#define INC_CSVED_UTIL_H

#include "a_base.h"
#include "a_csv.h"
#include "a_env.h"
#include "a_str.h"
#include "a_env.h"

#include "csved_types.h"
#include "csved_except.h"

namespace CSVED {

class IOManager;	// defined in csved_ioman.h

//---------------------------------------------------------------------------
// IOWatcher is notified when a new CSV input stream is opened. It is declared
// here (rather than in csved_ioman.h) so that shared helpers such as
// FieldSpec can derive from it without a circular include.
//---------------------------------------------------------------------------

class IOWatcher {

	public:

		virtual ~IOWatcher();
		virtual void OnNewCSVStream( const std::string & filename,
										const ALib::CSVStreamParser * p ) = 0;
};

//---------------------------------------------------------------------------
// Convert comma list to vector of ints to be used as column index
//---------------------------------------------------------------------------

void CommaListToIndex( const ALib::CommaList & cl,
						FieldList & idx );

//---------------------------------------------------------------------------
// A FieldSpec is a -f style field specification that may mix one-based
// numeric indexes, inclusive ranges (n:m) and header field names. Names are
// matched case-insensitively against the input header and, because the header
// is not available until input is read, are resolved lazily via the IOWatcher
// mechanism. Numeric-only specs are resolved immediately.
//
// The resolved zero-based indexes are written into a FieldList supplied by
// the owning command via Bind(), so existing field-processing code that uses
// that FieldList needs no change.
//---------------------------------------------------------------------------

class FieldSpec : public IOWatcher {

	public:

		FieldSpec();

		void Bind( FieldList & target );
		void Set( const std::string & spec );

		// Read a numeric field flag (default -f) and/or a field-name flag
		// (default -fn) from the command line into the bound FieldList. The
		// two flags are mutually exclusive; a name given to the numeric flag
		// is an error. Numeric specs are resolved immediately; name specs are
		// resolved later (see Wire / ResolveOnce). defNum is the default value
		// used when neither flag is present.
		void ReadFlags( const ALib::CommandLine & cmd,
						const std::string & defNum = "",
						const char * numFlag = 0,
						const char * nameFlag = 0 );

		// If this spec uses names, register it as a watcher on io so the names
		// are resolved from each input stream's header as it is opened. Use
		// this for commands that read via IOManager::ReadCSV().
		void Wire( IOManager & io );

		bool HasNames() const;
		bool Empty() const;

		// Resolve tokens into the bound FieldList. The parser supplies the
		// header name map and may be null when the spec is numeric-only.
		void Resolve( const ALib::CSVStreamParser * p );

		// Resolve only the first time it is called - used by commands that
		// read via CreateStreamParser() and so do not receive the watcher
		// event, but have a colmap-bearing parser available once its header
		// row has been parsed.
		void ResolveOnce( const ALib::CSVStreamParser * p );

		void OnNewCSVStream( const std::string & filename,
								const ALib::CSVStreamParser * p );

	private:

		ALib::CommaList mTokens;
		FieldList * mTarget;
		bool mHasNames;
		bool mResolved;
};

//---------------------------------------------------------------------------
// Compare two CSV rows - return as for strcmp. If field list is provided,
// compare oonly fields in list.
//----------------------------------------------------------------------------

int CmpRow( const CSVRow & a, const CSVRow & b,
					const FieldList & f = FieldList  () );

//----------------------------------------------------------------------------
// Get field or empty string if field does not exist
//----------------------------------------------------------------------------

std::string GetField( const CSVRow & row, unsigned int  i );


//----------------------------------------------------------------------------
// Check that not both of two flags are specified, and throw if they are.
//----------------------------------------------------------------------------

enum class ReqOp {
	Required, Optional
};

void NotBoth( const ALib::CommandLine & cmd, const std::string & a,
				const std::string & b, ReqOp ro = ReqOp::Optional );


}	// end namespace


#endif

