//---------------------------------------------------------------------------
// csved_join.h
//
// relational join on two csv sources for CSVED
//
// Copyright (C) 2009 Neil Butterworth
//---------------------------------------------------------------------------

#ifndef INC_CSVED_JOIN_H
#define INC_CSVED_JOIN_H

#include "a_base.h"
#include "a_csv.h"
#include <map>
#include "csved_command.h"

namespace CSVED {

//---------------------------------------------------------------------------

class JoinCommand : public Command {

	public:

		JoinCommand( const std::string & name,
						const std::string & desc );

		~JoinCommand();

		int Execute( ALib::CommandLine & cmd );


	private:

		void BuildRowMap( ALib::CSVStreamParser * p );
		void BuildJoinSpecs( const std::string & s );
		void ResolveLeft( const ALib::CSVStreamParser * p );
		void ResolveRight( const ALib::CSVStreamParser * p );
		void Clear();
		bool IsJoinCol( unsigned int i ) const;
		std::string MakeKey( const CSVRow & row, bool first );
		void WriteJoinRows( IOManager & io, const CSVRow & row );

		bool mOuterJoin;

		typedef std::vector <std::pair <int,int> > JoinSpecType;
		JoinSpecType mJoinSpecs;

		// join specs given by header name, resolved per stream into mJoinSpecs
		typedef std::vector <std::pair <std::string,std::string> > NamePairType;
		NamePairType mNamePairs;
		bool mHasNames;

		typedef std::multimap <std::string, CSVRow> MapType;
		MapType mRowMap;
		bool mInvert, mIgnoreCase, mKeep;
};

//---------------------------------------------------------------------------

} // end namespace

#endif
