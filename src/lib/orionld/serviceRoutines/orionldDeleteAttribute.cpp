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
#include "logMsg/logMsg.h"                                     // LM_*
#include "logMsg/traceLevels.h"                                // Lmt*

#include "rest/ConnectionInfo.h"                               // ConnectionInfo
#include "ngsi10/UpdateContextRequest.h"                       // UpdateContextRequest
#include "ngsi10/UpdateContextResponse.h"                      // UpdateContextResponse
#include "mongoBackend/mongoUpdateContext.h"                   // mongoUpdateContext

#include "orionld/common/orionldErrorResponse.h"               // orionldErrorResponseCreate
#include "orionld/common/httpStatusCodeToOrionldErrorType.h"   // httpStatusCodeToOrionldErrorType
#include "orionld/context/orionldUriExpand.h"                  // orionldUriExpand
#include "orionld/serviceRoutines/orionldDeleteAttribute.h"    // Own Interface



// ----------------------------------------------------------------------------
//
// orionldDeleteAttribute -
//
bool orionldDeleteAttribute(ConnectionInfo* ciP)
{
  Entity*      eP;
  char*        type = (char*) ((ciP->uriParam["type"] != "")? ciP->uriParam["type"].c_str() : NULL);
  char         longName[256];
  char*        details;

  // Get the long name of the Context Attribute name
  if (orionldUriExpand(ciP->contextP, ciP->wildcard[1], longName, sizeof(longName), &details) == false)
  {
    orionldErrorResponseCreate(ciP, OrionldBadRequestData, details, type, OrionldDetailsAttribute);
    return false;
  }
  
  // Create and fill in the entity
  eP       = new Entity();
  eP->id   = ciP->wildcard[0];

  // Create and fill in the attribute
  ContextAttribute* caP = new ContextAttribute;

  caP->name = longName;
  eP->attributeVector.push_back(caP);
  
  LM_T(LmtServiceRoutine, ("Deleting attribute '%s' of entity '%s'", ciP->wildcard[1], ciP->wildcard[0]));

  UpdateContextRequest  ucr;
  UpdateContextResponse ucResponse;

  ucr.fill(eP, ActionTypeDelete);
  ciP->httpStatusCode = mongoUpdateContext(&ucr,
                                           &ucResponse,
                                           ciP->tenant,
                                           ciP->servicePathV,
                                           ciP->uriParam,
                                           ciP->httpHeaders.xauthToken,
                                           ciP->httpHeaders.correlator,
                                           ciP->httpHeaders.ngsiv2AttrsFormat,
                                           ciP->apiVersion,
                                           NGSIV2_NO_FLAVOUR);

  LM_TMP(("KZ: ciP->httpStatusCode: %d", (int) ciP->httpStatusCode));
  if (ciP->httpStatusCode != SccOk)
  {
    orionldErrorResponseCreate(ciP, httpStatusCodeToOrionldErrorType(ciP->httpStatusCode), "DELETE /ngsi-ld/v1/entities/*/attrs/*", ciP->wildcard[0], OrionldDetailsString);
    ucr.release();
    return false;
  }

  ucr.release();
  ciP->httpStatusCode = SccNoContent;
  return true;
}