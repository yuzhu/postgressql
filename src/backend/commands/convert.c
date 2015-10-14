#include "postgres.h"

#include "access/reloptions.h"
#include "access/htup_details.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "access/xlog.h"
#include "catalog/namespace.h"
#include "catalog/pg_depend.h"
#include "catalog/dependency.h"
#include "catalog/toasting.h"
#include "commands/convert.h"
#include "commands/createas.h"
#include "commands/matview.h"
#include "commands/prepare.h"
#include "commands/tablecmds.h"
#include "commands/view.h"
#include "miscadmin.h"
#include "parser/parse_clause.h"
#include "rewrite/rewriteHandler.h"
#include "storage/smgr.h"
#include "tcop/tcopprot.h"
#include "utils/builtins.h"
#include "utils/lsyscache.h"
#include "utils/rel.h"
#include "utils/rls.h"
#include "utils/snapmgr.h"

static void convertInternal(Oid objid);

static void
convertInternal(Oid objid) {
  Relation targetrelation;
  Relation  depRel;
	// ObjectAddresses *targetObjects;
  ObjectAddress obj;

  obj.classId = RelationRelationId;
  obj.objectId = objid;
  obj.objectSubId = 0;


	targetrelation = heap_open(objid, AccessExclusiveLock);
  if (targetrelation->rd_rel->relkind != RELKIND_MATVIEW) {
    elog(ERROR, "Can not convert anything other than MATVIEW %s ", 
          targetrelation->rd_rel->relname.data);
  }

	heap_close(targetrelation, AccessExclusiveLock);

  /* Delete the dependency between the MATVIEW and its underlying tables */
  
  depRel = heap_open(DependRelationId, RowExclusiveLock);

  /* Delete the view part of matview */
  /* Convert it into a table */
  performDeletion(&obj, DROP_RESTRICT, 0);
/*
	for (i = 0; i < targetObjects->numrefs; i++){
		const ObjectAddress *thisobj = objects->refs + i;
    elog(LOG, "targetlist %d ", thisobj->objectId);
  }

*/
	heap_close(depRel, RowExclusiveLock);
}


void convert(ConvertStmt *stmt) {
  Oid   relid; 
	elog(LOG, "Convert Stmt for %s", stmt->relation->relname);
  // missing not ok
  // reusing RangeVarCallbackForAlterRelation to check permissions
	relid = RangeVarGetRelidExtended(stmt->relation, AccessExclusiveLock,
									 false, false,
									 0, 0);
	if (!OidIsValid(relid))
	{
		ereport(NOTICE,
				(errmsg("relation \"%s\" does not exist, skipping",
						stmt->relation->relname)));
	}

  convertInternal(relid);

}
