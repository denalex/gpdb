//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 Greenplum, Inc.
//
//	@filename:
//		CGPOptimizer.h
//
//	@doc:
//		Entry point to GP optimizer
//
//	@test:
//
//
//---------------------------------------------------------------------------
#ifndef CGPOptimizer_H
#define CGPOptimizer_H

#include "postgres.h"
#include "nodes/params.h"
#include "nodes/plannodes.h"
#include "nodes/parsenodes.h"

class CGPOptimizer
{
	public:

		// optimize given query using GP optimizer
		static
		PlannedStmt *GPOPTOptimizedPlan
			(
			Query *query,
			bool *had_unexpected_failure // output : set to true if optimizer unexpectedly failed to produce plan
			);

		// serialize planned statement into DXL
		static
		char *SerializeDXLPlan(Query *query);

    // gpopt initialize and terminate
    static
    void InitGPOPT();

    static
    void TerminateGPOPT();
};

#endif // CGPOptimizer_H
