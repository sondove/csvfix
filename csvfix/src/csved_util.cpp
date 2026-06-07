//---------------------------------------------------------------------------
// csved_util.cpp
//
// utilities for CSVED
//
// Copyright (C) 2009 Neil Butterworth
//---------------------------------------------------------------------------

#include "a_base.h"
#include "a_env.h"
#include "a_str.h"
#include "a_collect.h"
#include "csved_util.h"
#include "csved_ioman.h"
#include "csved_strings.h"
#include "csved_except.h"

using std::string;
using std::vector;

namespace CSVED {

//---------------------------------------------------------------------------
// Resolve a single field-spec endpoint to a zero-based column index. The
// token is either a one-based integer index or, when a stream parser with a
// header map is supplied, a (case-insensitive) header field name.
//---------------------------------------------------------------------------

static unsigned int ResolveEndpoint( const string & tok,
										const ALib::CSVStreamParser * p ) {
	if ( ALib::IsInteger( tok ) ) {
		int n = ALib::ToInteger( tok );
		if ( n < 1 ) {
			CSVTHROW( "Index must be greater than zero, not '" << tok << "'" );
		}
		return (unsigned int) ( n - 1 );	// convert to zero-based
	}
	if ( p == 0 ) {
		CSVTHROW( "Field name '" << tok
					<< "' used but input has no field name header" );
	}
	return p->ColIndexFromName( tok, true );	// case-insensitive
}

//---------------------------------------------------------------------------
// Convert comma list to vector of ints to be used as col index. We allow
// ranges in the form n1:n2. Endpoints may be numeric indexes or, when a
// stream parser is supplied, header field names.
//---------------------------------------------------------------------------

static void ResolveCommaList( const ALib::CommaList & cl,
								const ALib::CSVStreamParser * p,
								FieldList & idx ) {
	idx.clear();
	for ( unsigned int i = 0; i < cl.Size(); i++ ) {
		string cle = cl.At(i);
		vector <string> fl;
		ALib::Split( cle, ':', fl );
		if ( fl.size() > 2 ) {
			CSVTHROW( "Invalid field: " << cle );
		}
		else if ( fl.size() == 2 ) {
			unsigned int n1 = ResolveEndpoint( fl[0], p );
			unsigned int n2 = ResolveEndpoint( fl[1], p );
			if ( n1 <= n2 ) {
				for ( unsigned int x = n1; x <= n2; x++ ) {
					idx.push_back( x );
				}
			}
			else {
				for ( unsigned int x = n1; ; x-- ) {
					idx.push_back( x );
					if ( x == n2 ) {
						break;
					}
				}
			}
		}
		else {
			idx.push_back( ResolveEndpoint( cle, p ) );
		}
	}
}

void CommaListToIndex( const ALib::CommaList & cl,
						FieldList & idx ) {
	ResolveCommaList( cl, 0, idx );
}

//---------------------------------------------------------------------------
// FieldSpec - a -f style specification that may contain numeric indexes,
// ranges and/or header field names. See csved_util.h for details.
//---------------------------------------------------------------------------

IOWatcher :: ~IOWatcher() {
	// nothing
}

FieldSpec :: FieldSpec()
	: mTarget( 0 ), mHasNames( false ), mResolved( false ) {
}

void FieldSpec :: Bind( FieldList & target ) {
	mTarget = & target;
}

void FieldSpec :: Set( const string & spec ) {
	mTokens = ALib::CommaList( spec );
	mHasNames = false;
	mResolved = false;
	for ( unsigned int i = 0; i < mTokens.Size(); i++ ) {
		vector <string> fl;
		ALib::Split( mTokens.At(i), ':', fl );
		for ( unsigned int j = 0; j < fl.size(); j++ ) {
			if ( ! ALib::IsInteger( fl[j] ) ) {
				mHasNames = true;
			}
		}
	}
}

bool FieldSpec :: HasNames() const {
	return mHasNames;
}

bool FieldSpec :: Empty() const {
	return mTokens.Size() == 0;
}

void FieldSpec :: Resolve( const ALib::CSVStreamParser * p ) {
	if ( mTarget == 0 ) {
		CSVTHROW( "FieldSpec resolved without a bound field list" );
	}
	ResolveCommaList( mTokens, p, * mTarget );
	mResolved = true;
}

void FieldSpec :: ResolveOnce( const ALib::CSVStreamParser * p ) {
	if ( mHasNames && ! mResolved ) {
		Resolve( p );
	}
}

void FieldSpec :: ReadFlags( const ALib::CommandLine & cmd,
								const string & defNum,
								const char * numFlag,
								const char * nameFlag ) {
	string nf = numFlag ? numFlag : FLAG_COLS;
	string fn = nameFlag ? nameFlag : FLAG_FNAMES;

	bool hasNum  = cmd.HasFlag( nf );
	bool hasName = cmd.HasFlag( fn );

	if ( hasNum && hasName ) {
		CSVTHROW( "Cannot specify both " << nf << " and " << fn << " options" );
	}

	if ( hasName ) {
		Set( cmd.GetValue( fn ) );		// fields by header name
		if ( ! mHasNames ) {
			Resolve( 0 );				// numeric value given to name flag
		}
		// otherwise resolution is deferred until a header is read
	}
	else {
		Set( cmd.GetValue( nf, defNum ) );	// fields by numeric index
		if ( mHasNames ) {
			CSVTHROW( "Field names require the " << fn
						<< " option, not " << nf );
		}
		Resolve( 0 );
	}
}

void FieldSpec :: Wire( IOManager & io ) {
	if ( mHasNames ) {
		io.AddWatcher( * this );
	}
}

void FieldSpec :: OnNewCSVStream( const string &,
									const ALib::CSVStreamParser * p ) {
	Resolve( p );
}


//---------------------------------------------------------------------------
// Compare two CSV rows - return as for strcmp. If field list is provided,
// compare only fields in list.
//----------------------------------------------------------------------------

int CmpRow( const CSVRow & a, const CSVRow & b, const FieldList & f ) {
	unsigned int n = std::max( a.size(), b.size() );
	for ( unsigned int i = 0; i < n; i++ ) {

		if ( f.size() && ! ALib::Contains( f, i ) ) {
			continue;
		}

		string fa = GetField( a, i );
		string fb = GetField( b, i );

		if ( fa == fb ) {
			continue;
		}
		else if ( fa < fb ) {
			return -1;
		}
		else {
			return 1;
		}
	}
	return 0;
}

//----------------------------------------------------------------------------
// Get field or empty string if field does not exist
//----------------------------------------------------------------------------

std::string GetField( const CSVRow & row, unsigned int  i ) {
	return i >= row.size() ? "" : row[i];
}

//----------------------------------------------------------------------------
// Check that not both of two flags are specified, and throw if they are.
//----------------------------------------------------------------------------

void NotBoth( const ALib::CommandLine & cmd, const std::string & a,
				const std::string & b, ReqOp  r  ) {

	if ( cmd.HasFlag( a ) && cmd.HasFlag( b ) ) {
		CSVTHROW( "Cannot specify both " << a << " and " << b << " options");
	}

	if ( r == ReqOp::Required  && ! ( cmd.HasFlag( a ) || cmd.HasFlag(b) ) ) {
		CSVTHROW( "Need one of  " << a << " or " << b  << " options" );
	}
}


//---------------------------------------------------------------------------

}	// end namespace

// end


