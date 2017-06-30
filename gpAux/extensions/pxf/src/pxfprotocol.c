/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "pxfbridge.h"
#include "access/extprotocol.h"
#include "fmgr.h"

/* define magic module unless run as a part of test cases */
#ifndef UNIT_TESTING
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(pxfprotocol_export);
PG_FUNCTION_INFO_V1(pxfprotocol_import);
PG_FUNCTION_INFO_V1(pxfprotocol_validate_urls);

Datum pxfprotocol_export(PG_FUNCTION_ARGS);
Datum pxfprotocol_import(PG_FUNCTION_ARGS);
Datum pxfprotocol_validate_urls(PG_FUNCTION_ARGS);

void init_context(gphadoop_context **context);
void cleanup_context(gphadoop_context *context);

Datum
pxfprotocol_validate_urls(PG_FUNCTION_ARGS)
{
	elog(INFO, "Dummy PXF protocol validate");
	PG_RETURN_VOID();
}

Datum
pxfprotocol_export(PG_FUNCTION_ARGS)
{
	elog(INFO, "Dummy PXF protocol write");
    PG_RETURN_INT32(0);
}

Datum
pxfprotocol_import(PG_FUNCTION_ARGS)
{
    /* Must be called via the external table format manager */
    if (!CALLED_AS_EXTPROTOCOL(fcinfo))
        elog(ERROR, "extprotocol_import: not called by external protocol manager");

    /* retrieve user context */
    gphadoop_context *context = (gphadoop_context *) EXTPROTOCOL_GET_USER_CTX(fcinfo);

    /* last call -- cleanup */
    if (EXTPROTOCOL_IS_LAST_CALL(fcinfo)) {
        cleanup_context(context);
        EXTPROTOCOL_SET_USER_CTX(fcinfo, NULL);
        PXFLOG("returning from last call");
        PG_RETURN_INT32(0);
    }

    /* first call -- do any desired init */
    if (context == NULL) {
        init_context(&context);
        EXTPROTOCOL_SET_USER_CTX(fcinfo, context);
        gpbridge_import_start(context);
    }

    int bytes_read = gpbridge_read(context, EXTPROTOCOL_GET_DATABUF(fcinfo), EXTPROTOCOL_GET_DATALEN(fcinfo));

    PXFLOG("bytes read %d", bytes_read);
    PG_RETURN_INT32(bytes_read);
}

void
init_context(gphadoop_context **context)
{
    *context = (gphadoop_context *)palloc0(sizeof(gphadoop_context));

    //TODO: remove mock fragment list
    (*context)->current_fragment = palloc0(sizeof(ListCell));
    (*context)->current_fragment->next = NULL;

    //initStringInfo(&context->uri);
    //initStringInfo(&context->write_file_name);
}

void
cleanup_context(gphadoop_context *context)
{
    if (context != NULL) {
        gpbridge_cleanup(context);
        //pfree(context->uri.data);
        //pfree(context->write_file_name.data);
        PXFLOG("ready to free fragment");
        //pfree(context->current_fragment); //TODO remove mock
        PXFLOG("ready to free context");
        pfree(context);
    }
}
