#ifndef __CIAS__FACTORY_TEST_H__
#define __CIAS__FACTORY_TEST_H__

#include <malloc.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include <stdbool.h>
#include "status_share.h"
#include "cias_aiot_protocol.h"

bool cias_factory_test_init(void);
#endif   //__CIAS__FACTORY_TEST_H__