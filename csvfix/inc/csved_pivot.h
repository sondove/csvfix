//---------------------------------------------------------------------------
// csved_pivot.h
//
// Simple pivot table stuff
//
// Copyright (C) 2014 Neil Butterworth
//---------------------------------------------------------------------------

#ifndef INC_CSVED_PIVOT_H
#define INC_CSVED_PIVOT_H

#include "a_base.h"
#include "csved_command.h"
#include "csved_ioman.h"

#include <utility>
#include <string>
#include <map>
#include <set>

namespace CSVED {

//---------------------------------------------------------------------------

class PivotCommand : public Command, public IOWatcher {

	public:

		PivotCommand( const std::string & name,
						const std::string & desc );

		int Execute( ALib::CommandLine & cmd );

		// resolve row/col/fact columns given by header name
		void OnNewCSVStream( const std::string & filename,
								const ALib::CSVStreamParser * p );

        enum class Action {
            Sum, Count, Average
        };


	private:

        struct SumCount {

            double mSum;
            unsigned int mCount;

            SumCount() : mSum(0), mCount(0) {}

            void Update( double val ) {
                mSum += val;
                mCount++;
            }
        };

        struct ColRow{

            std::string mCol;
            std::string mRow;

            ColRow( const std::string & c, const std::string & r )
                    : mCol(c), mRow(r) {}


            bool operator < ( const ColRow & r ) const {
                return std::tie( mCol, mRow ) < std::tie( r.mCol, r.mRow );
            }
        };

        void ProcessFlags( const ALib::CommandLine & cmd );
        ColRow MakeColRow( const CSVRow & row ) const;
        std::string GetFact( const CSVRow & row ) const;
        void AddFact( const ColRow & cr, const std::string & fact );
        void OutputPivot( IOManager & io );

        typedef std::map <ColRow, SumCount> MapType;
        MapType mColRowValMap;
        std::set <std::string> mCols, mRows;

        Action mAction;
        unsigned int mCol, mRow, mFact;
        // header names for col/row/fact when given by name (empty if numeric);
        // resolved from the header at stream-open time
        std::string mColName, mRowName, mFactName;
        bool mHasNames;

};


//------------------------------------------------------------------------

}	// end namespace

#endif

