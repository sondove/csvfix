//---------------------------------------------------------------------------
// csved_inter.cpp
//
// interleave fields from two csv sources
//
// Copyright (C) 2010 Neil Butterworth
//---------------------------------------------------------------------------

#include "a_base.h"
#include "a_collect.h"
#include "csved_cli.h"
#include "csved_inter.h"
#include "csved_strings.h"

using std::string;
using std::vector;

namespace CSVED {

//---------------------------------------------------------------------------
// Register inter  command
//---------------------------------------------------------------------------

static RegisterCommand <InterCommand> rc1_(
	CMD_INTER,
	"interleave fields from two CSV sources"
);

//----------------------------------------------------------------------------
// Source-stream prefixes used in field specs (L=first input, R=second)
//----------------------------------------------------------------------------

const char SRC0 = 'L';
const char SRC1 = 'R';

//----------------------------------------------------------------------------
// Help text
//----------------------------------------------------------------------------

const char * const INTER_HELP = {
	"interleave fields from two CSV sources"
	"usage: csvfix inter [flags] [files ...]\n"
	"where flags are:\n"
	"  -f fields\tspecify fields to interleave by index, e.g. L1,R2\n"
	"  -fn fields\tspecify fields to interleave by header name, e.g. Lid,Rname\n"
	"#SMQ,SEP,IBL,IFN,OFL"
};

//------------------------------------------------------------------------
// Standard command ctor
//---------------------------------------------------------------------------

InterCommand :: InterCommand( const string & name,
								const string & desc )
		: Command( name, desc, INTER_HELP ), mHasNames( false ) {

	AddFlag( ALib::CommandLineFlag( FLAG_COLS, false, 1 ) );
	AddFlag( ALib::CommandLineFlag( FLAG_FNAMES, false, 1 ) );
}

//---------------------------------------------------------------------------
// Read until first stream exhausted - streams don't have to have same
// number of records. Can't use normal IOMan stuff for this as we deal
// with two input streams at same time.
//---------------------------------------------------------------------------

int InterCommand :: Execute( ALib::CommandLine & cmd ) {

	ProcessFlags( cmd );

	IOManager io( cmd, mHasNames );
	if ( io.InStreamCount() != 2 ) {
		CSVTHROW( "Command requires exactly two input streams" );
	}

	std::unique_ptr<ALib::CSVStreamParser> p0( io.CreateStreamParser( 0 ) );
	std::unique_ptr<ALib::CSVStreamParser> p1( io.CreateStreamParser( 1 ) );

	CSVRow row0, row1;
	bool resolved = false;

	while( p0->ParseNext( row0 ) ) {
		if ( ! p1->ParseNext( row1 )) {
			row1.clear();
		}
		if ( mHasNames && ! resolved ) {
			// both headers have now been parsed - resolve names to indexes
			ResolveNames( p0.get(), p1.get() );
			resolved = true;
		}
		CSVRow out = Interleave( row0, row1 );
		io.WriteRow( out );
	}

	return 0;
}

//----------------------------------------------------------------------------
// Resolve field-name specs (-fn) to indexes using each stream's header.
// L fields resolve against the first stream, R fields against the second.
//----------------------------------------------------------------------------

void InterCommand :: ResolveNames( const ALib::CSVStreamParser * p0,
									const ALib::CSVStreamParser * p1 ) {
	mFields.clear();
	for ( unsigned int i = 0; i < mNameFields.size(); i++ ) {
		const ALib::CSVStreamParser * p = mNameFields[i].mSrc == 0 ? p0 : p1;
		unsigned int idx = p->ColIndexFromName( mNameFields[i].mName, true );
		mFields.push_back( FieldSpec( mNameFields[i].mSrc, idx ) );
	}
}

//----------------------------------------------------------------------------
// Create a field-name spec from a string of the form L<name> or R<name>.
//----------------------------------------------------------------------------

InterCommand::NameField InterCommand :: MakeNameField( const string & f ) const {
	if ( f.size() < 2 ) {
		CSVTHROW( "Invalid field spec " << f );
	}
	char src = std::toupper( f[0] );
	if ( src != SRC0 && src != SRC1 ) {
		CSVTHROW( "Invalid source spec in field spec " << f );
	}
	unsigned int si = src == SRC0 ? 0 : 1;
	return NameField( si, f.substr( 1 ) );
}

//----------------------------------------------------------------------------
// Create field index from field spec string
//----------------------------------------------------------------------------

InterCommand::FieldSpec InterCommand :: MakeField( const string &  f ) const {
	if ( f.size() < 2 ) {
		CSVTHROW( "Invalid field spec " << f );
	}

	char src = std::toupper( f[0] );
	if ( src != SRC0 && src != SRC1 ) {
		CSVTHROW( "Invalid source spec in field spec " << f );
	}

	unsigned int si = src == SRC0 ? 0 : 1;
	if ( ! ALib::IsInteger( f.substr( 1 ))) {
		CSVTHROW( "Field index not integer in field " << f );
	}

	int fi = ALib::ToInteger( f.substr(1));
	if ( fi <= 0 ) {
		CSVTHROW( "Field index must be 1 or greater in field " << f );
	}

	return FieldSpec( si, fi - 1 );
}

//---------------------------------------------------------------------------
// Handle all user options with error checking
//---------------------------------------------------------------------------

void InterCommand :: ProcessFlags( const ALib::CommandLine & cmd ) {

	if ( cmd.HasFlag( FLAG_COLS ) && cmd.HasFlag( FLAG_FNAMES ) ) {
		CSVTHROW( "Cannot specify both " << FLAG_COLS
					<< " and " << FLAG_FNAMES << " options" );
	}

	mFields.clear();
	mNameFields.clear();
	mHasNames = cmd.HasFlag( FLAG_FNAMES );

	if ( mHasNames ) {
		ALib::CommaList cl( cmd.GetValue( FLAG_FNAMES, "" ) );
		for ( unsigned int i = 0; i < cl.Size(); i++ ) {
			mNameFields.push_back( MakeNameField( cl.At(i) ) );
		}
	}
	else {
		string fields = cmd.GetValue( FLAG_COLS, "" );
		if ( ! ALib::IsEmpty( fields ) ) {
			ALib::CommaList cl( fields );
			for ( unsigned int i = 0; i < cl.Size(); i++ ) {
				mFields.push_back( MakeField( cl.At(i) ) );
			}
		}
	}
}

//----------------------------------------------------------------------------
// Gert field from first or second input row, depending on field spec
//----------------------------------------------------------------------------

string InterCommand :: GetField( const FieldSpec & f,
									const CSVRow & r1 ,
									const CSVRow & r2 ) const {

	const CSVRow & r = f.mSrc == 0 ? r1 : r2;
	string val = f.mField >= r.size() ? "" : r[f.mField];
	return val;
}

//----------------------------------------------------------------------------
// Given two rows, interleave using field specs.
//----------------------------------------------------------------------------

CSVRow InterCommand :: Interleave( const CSVRow & r1 ,
									const CSVRow & r2 ) const {
	if ( mFields.size() == 0 ) {
		CSVRow r( r1 );
		if ( r2.size() ) {
			ALib::Append( r, r2 );
		}
		return r;
	}
	else {
		CSVRow r;
		for ( unsigned int i = 0; i < mFields.size(); i++ ) {
			r.push_back( GetField( mFields[i], r1, r2 ));
		}
		return r;
	}
}


} // end namespace

// end

