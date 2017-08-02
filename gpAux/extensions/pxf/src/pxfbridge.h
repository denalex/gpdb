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

#ifndef GPDB_PXFBRIDGE_H
#define GPDB_PXFBRIDGE_H

#include "postgres.h"
#include "cdb/cdbvars.h"
#include "libchurl.h"
#include "nodes/pg_list.h"
#include "pxfuriparser.h"


typedef struct
{
    CHURL_HEADERS churl_headers;
    CHURL_HANDLE churl_handle;
    GPHDUri *gphd_uri;
    StringInfoData uri;
    ListCell *current_fragment;
    StringInfoData write_file_name;
    Relation relation;
} gphadoop_context;

void gpbridge_cleanup(gphadoop_context *context);
void gpbridge_import_start(gphadoop_context *context);
int  gpbridge_read(gphadoop_context *context, char *databuf, int datalen);

/* helpers for dev debugging */

#define PXF_DEBUG 0

#ifdef UNIT_TESTING
#define TESTING 1
#else
#define TESTING 0
#endif

#define PXFLOG(message, ...) \
    if (PXF_DEBUG) { \
        char _logbuff[sizeof(message) + 7] = message;\
        memmove(_logbuff + 7, _logbuff, sizeof(_logbuff) - 7); \
        memcpy(_logbuff, "seg%d: ", 7); \
        if (TESTING) { \
            char _msg[100]; \
            snprintf(_msg, 100, _logbuff, GpIdentity.segindex, ##__VA_ARGS__); \
            printf("%s\n", _msg); \
        } \
        else \
            elog(INFO, _logbuff, GpIdentity.segindex, ##__VA_ARGS__); \
    }

#endif //GPDB_PXFBRIDGE_H
