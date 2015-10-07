#ifndef CONVERT_H
#define CONVERT_H


#include "catalog/objectaddress.h"
#include "nodes/params.h"
#include "nodes/parsenodes.h"
#include "tcop/dest.h"

extern void convert(ConvertStmt *stmt);

#endif /* CONVERT_H */
