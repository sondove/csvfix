//---------------------------------------------------------------------------
// csved_sort.h
//
// sort command for CSVED
//
// Copyright (C) 2009 Neil Butterworth
//---------------------------------------------------------------------------

#ifndef INC_CSVED_SORT_H
#define INC_CSVED_SORT_H

#include "a_base.h"
#include "a_sort.h"
#include "csved_command.h"
#include "csved_ioman.h"

namespace CSVED {

//---------------------------------------------------------------------------

class SortCommand : public Command, public IOWatcher {

	public:

		SortCommand( const std::string & name,
						const std::string & desc );

		int Execute( ALib::CommandLine & cmd );

		// resolve any field-name sort specs from the input header
		void OnNewCSVStream( const std::string & filename,
								const ALib::CSVStreamParser * p );

	private:

		void BuildFieldSpecs( const ALib::CommandLine & cmd );
		void Sort( std::vector <CSVRow> & rows );

		// a sort field specified by header name, with its sort parameters
		struct NameField {
			std::string mName;
			ALib::SortField::Direction mDir;
			ALib::SortField::CmpType mType;
		};

		std::vector <ALib::SortField> mFields;
		std::vector <NameField> mNameFields;
		bool mHasNames;

};

//------------------------------------------------------------------------

}	// end namespace

#endif

