//---------------------------------------------------------------------------
// csved_join.cpp
//
// relational join on two csv sources for CSVED
//
// Copyright (C) 2009 Neil Butterworth
//---------------------------------------------------------------------------

#include "a_str.h"
#include "a_collect.h"
#include "csved_except.h"
#include "csved_cli.h"
#include "csved_join.h"
#include "csved_strings.h"

using std::string;
using std::vector;

namespace CSVED {

//---------------------------------------------------------------------------
// Register join command
//---------------------------------------------------------------------------

static RegisterCommand <JoinCommand> rc1_(
	CMD_JOIN,
	"perform relational join"
);

//----------------------------------------------------------------------------
// Help text
//----------------------------------------------------------------------------

const char * const JOIN_HELP = {
	"perform relational join between two input streams\n"
	"streams are joined in turn against the last stream on the command line\n"
	"usage: csvfix join  [flags] [file ...]\n"
	"where flags are:\n"
	"  -f jlist\tlist of fields to perform join on, by numeric index (e.g. 1:2)\n"
	"  -fn jlist\tlist of fields to perform join on, by header name (e.g. id:custid)\n"
	"  -oj\t\tperform outer join\n"
	"  -inv\t\tinvert sense of join to exclude matching rows\n"
	"  -ic\t\tignore character case in join columns\n"
	"  -k\tkeep join fields in output\n"
	"#ALL"
};

//---------------------------------------------------------------------------
// Usual ctor stuff
//---------------------------------------------------------------------------

JoinCommand :: JoinCommand( const string & name,
							const string & desc )
			: Command( name, desc, JOIN_HELP),
					mOuterJoin( false ), mHasNames( false ),
					mIgnoreCase( false ), mKeep( false ) {

	AddFlag( ALib::CommandLineFlag( FLAG_COLS, false, 1 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_FNAMES, false, 1 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_OUTERJ, false, 0 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_INVERT, false, 0 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_ICASE, false, 0 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_KEEP, false, 0 ) );
}

//---------------------------------------------------------------------------
// Usual dtor stuff
//---------------------------------------------------------------------------

JoinCommand :: ~JoinCommand() {
	Clear();
}

//---------------------------------------------------------------------------
// Trash specs and row lookup map
//---------------------------------------------------------------------------

void JoinCommand :: Clear() {
	mJoinSpecs.clear();
	mNamePairs.clear();
	mRowMap.clear();
}

//---------------------------------------------------------------------------
// Do a join btween all data sources but the last to the last.
//---------------------------------------------------------------------------

int JoinCommand :: Execute( ALib::CommandLine & cmd ) {

	Clear();

    mKeep = cmd.HasFlag( FLAG_KEEP );
	mOuterJoin = cmd.HasFlag( FLAG_OUTERJ );
	mInvert = cmd.HasFlag( FLAG_INVERT );
	mIgnoreCase = cmd.HasFlag( FLAG_ICASE );
	if ( mOuterJoin && mInvert ) {
		CSVTHROW( "Cannot have both " << FLAG_OUTERJ
					<< " and " << FLAG_INVERT << " flags" );
	}
	if ( cmd.HasFlag( FLAG_COLS ) && cmd.HasFlag( FLAG_FNAMES ) ) {
		CSVTHROW( "Cannot specify both " << FLAG_COLS
					<< " and " << FLAG_FNAMES << " options" );
	}
	mHasNames = cmd.HasFlag( FLAG_FNAMES );
	string js = cmd.GetValue( mHasNames ? FLAG_FNAMES : FLAG_COLS );
	BuildJoinSpecs( js );

	IOManager io( cmd, mHasNames );
	unsigned int scount = io.InStreamCount();
	if (  scount < 2 ) {
		CSVTHROW( "Need at least two input streams" );
	}

	BuildRowMap( io.CreateStreamParser( scount - 1 ) );

	CSVRow row;
	for ( unsigned int i = 0; i < scount - 1; i++ ) {
		std::unique_ptr <ALib::CSVStreamParser> p( io.CreateStreamParser( i ) );
		bool resolved = false;
		while( p->ParseNext( row ) ) {
			if ( mHasNames && ! resolved ) {
				ResolveLeft( p.get() );		// LHS names from this stream's header
				resolved = true;
			}
			WriteJoinRows( io, row );
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
// This does the actual join by looking up the key columns in each input
// row with the key columns saved in the map.
//---------------------------------------------------------------------------

void JoinCommand :: WriteJoinRows( IOManager & io, const CSVRow & row ) {

	string key = MakeKey( row, true );

	MapType::const_iterator pos = mRowMap.lower_bound( key );
	MapType::const_iterator last = mRowMap.upper_bound( key );

	if ( mOuterJoin && pos == last ) {
		io.WriteRow( row );
	}
	else if ( mInvert ) {
		if ( pos == last ) {
			io.WriteRow( row );
		}
		return;
	}
	else {
		while( pos != last ) {
			const CSVRow & jr = pos->second;
			CSVRow newrow = row;
			for ( unsigned int i = 0; i < jr.size(); i++ ) {
				newrow.push_back( jr[i] );
			}
			io.WriteRow( newrow );
			++pos;
		}
	}
}

//---------------------------------------------------------------------------
// Joins are specified like this :
//
//		-f 1:2,4:4
//
// which means join fields 1 -> 2 and 4 -> 4. This handles parsing
// and validation of the join specs.
//---------------------------------------------------------------------------

void JoinCommand :: BuildJoinSpecs( const string & js ) {

	if ( js == "" ) {
		CSVTHROW( "No join specified with " << FLAG_COLS << " flag" );
	}

	ALib::CommaList cl( js );
	for ( unsigned int i = 0; i < cl.Size(); i++ ) {
		vector <string> cols;
		if ( ALib::Split( cl.At(i), ':', cols ) != 2 ) {
			CSVTHROW( "Invalid join specification: " << cl.At(i) );
		}
		if ( mHasNames ) {
			// left:right header names, resolved per stream from the header
			mNamePairs.push_back( std::make_pair( cols[0], cols[1] ) );
			mJoinSpecs.push_back( std::make_pair( -1, -1 ) );	// placeholder
		}
		else {
			int c1 = ALib::ToInteger( cols[0], "Invalid column: " + cols[0] );
			int c2 = ALib::ToInteger( cols[1], "Invalid column: " + cols[1] );
			if ( c1 < 1 || c2 < 1 ) {
				CSVTHROW( "Invalid join specfication: " << cl.At(i) );
			}
			mJoinSpecs.push_back( std::make_pair( c1 - 1, c2 - 1 ) );
		}
	}
}

//---------------------------------------------------------------------------
// Resolve the left/right header names of the join specs to column indexes
// using the supplied stream's header (case-insensitive).
//---------------------------------------------------------------------------

void JoinCommand :: ResolveLeft( const ALib::CSVStreamParser * p ) {
	for ( unsigned int i = 0; i < mNamePairs.size(); i++ ) {
		mJoinSpecs[i].first =
			(int) p->ColIndexFromName( mNamePairs[i].first, true );
	}
}

void JoinCommand :: ResolveRight( const ALib::CSVStreamParser * p ) {
	for ( unsigned int i = 0; i < mNamePairs.size(); i++ ) {
		mJoinSpecs[i].second =
			(int) p->ColIndexFromName( mNamePairs[i].second, true );
	}
}

//---------------------------------------------------------------------------
// See if a particular column index partakes in a join.
//---------------------------------------------------------------------------

bool JoinCommand :: IsJoinCol( unsigned int ci ) const {
	for ( unsigned int i = 0; i < mJoinSpecs.size(); i++ ) {
		if ( (int) ci == mJoinSpecs[i].second ) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
// Construct a key from a row. The columns used to construct the key are
// taken from the join specs. The 'first' parameter specifies if we are
// taking the first pair value (the LHS key) or the second (RHS). If
// the user specified the -ic option, keys are converted to uppercase.
//---------------------------------------------------------------------------

string JoinCommand :: MakeKey( const CSVRow & row, bool first ) {
	string key;
	for ( unsigned int i = 0; i < mJoinSpecs.size(); i++ ) {
		unsigned int col = first ? mJoinSpecs[i].first
								 : mJoinSpecs[i].second;
		if ( col >= row.size() ) {
			continue;
		}
		key += mIgnoreCase ? ALib::Upper( row[col] ) : row[col];
		key += '\0';		// key element separator
	}
	return key;
}

//---------------------------------------------------------------------------
// Build a key to row map from inputs. The map is a multimap, as we
// may have duplicate keys.
//---------------------------------------------------------------------------

void JoinCommand :: BuildRowMap( ALib::CSVStreamParser * sp ) {
	string line;
	std::unique_ptr <ALib::CSVStreamParser> p( sp );
	CSVRow row;
	bool resolved = false;
	while( p->ParseNext( row ) ) {
		if ( mHasNames && ! resolved ) {
			ResolveRight( p.get() );	// RHS names from build stream's header
			resolved = true;
		}
		string key = MakeKey( row, false );
		CSVRow jrow;
		for ( unsigned int i = 0; i < row.size(); i++ ) {
			if ( (! IsJoinCol( i )) || mKeep  ) {
				jrow.push_back( row[i] );
			}
		}
		mRowMap.insert( std::make_pair( key, jrow ) );
	}
}

//---------------------------------------------------------------------------

}	// namespace


// end

