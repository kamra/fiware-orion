/*
*
* Copyright 2018 Telefonica Investigacion y Desarrollo, S.A.U
*
* This file is part of Orion Context Broker.
*
* Orion Context Broker is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Orion Context Broker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
* General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* iot_support at tid dot es
*
* Author: Ken Zangelin
*/
extern "C"
{
#include "kjson/KjNode.h"                                   // KjNode
#include "kjson/kjBuilder.h"                                // kjString
}

#include "logMsg/logMsg.h"                                  // LM_*
#include "logMsg/traceLevels.h"                             // Lmt*

#include "rest/ConnectionInfo.h"                            // ConnectionInfo
#include "orionld/common/orionldErrorResponse.h"            // Own interface



// -----------------------------------------------------------------------------
//
// errorTypeStringV -
//
static const char* errorTypeStringV[] =
{
  "http://uri.etsiu.org/ngsi-ld/errors/InvalidRequest",
  "http://uri.etsiu.org/ngsi-ld/errors/BadRequestData",
  "http://uri.etsiu.org/ngsi-ld/errors/AlreadyExists",
  "http://uri.etsiu.org/ngsi-ld/errors/OperationNotSupported",
  "http://uri.etsiu.org/ngsi-ld/errors/ResourceNotFound",
  "http://uri.etsiu.org/ngsi-ld/errors/InternalError"
};



// ----------------------------------------------------------------------------
//
// orionldErrorResponseCreate -
//
// NOTE
//   Only service routines should use this function.
//   Lower level functions should just return the 'details' string.
//
void orionldErrorResponseCreate(ConnectionInfo* ciP, OrionldResponseErrorType errorType, const char* title, const char* details)
{
  LM_T(LmtErrorResponse, ("Creating error response: %s (%s)", title, details));

  KjNode* typeP     = kjString(ciP->kjsonP, "type",    errorTypeStringV[errorType]);
  KjNode* titleP    = kjString(ciP->kjsonP, "title",   title);
  KjNode* detailsP;

  if (details != NULL)
    detailsP = kjString(ciP->kjsonP, "details", details);
  else
    detailsP = kjString(ciP->kjsonP, "details", "no details");

  ciP->responseTree = kjObject(ciP->kjsonP, NULL);

  kjChildAdd(ciP->responseTree, typeP);
  kjChildAdd(ciP->responseTree, titleP);
  kjChildAdd(ciP->responseTree, detailsP);
}