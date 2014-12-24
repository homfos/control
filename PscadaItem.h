#ifndef PSCADA_ITEM_H_
#define PSCADA_ITEM_H_

//#define CHOOSE_ON_SUCCESS (7 || 10)
//#define CHOOSE_ON_FAILURE 9
//
//#define CHOOSE_OFF_SUCCESS 7
//#define CHOOSE_OFF_FAILURE 9
//
//

//
//#define GET_REMOTE_SIGNAL 0
//
//#define SERIAL_CONTROL_TYPE 1
//#define PARALLEL_CONTROL_TYPE 2
//
//#define TWOPOINT_CONTROL 2
//#define SINGLE_POINT_CONTROL 3
//#define SINGLE_POINT_READ_CONTROL 1
//
//#define IFIX_PDB_OPERATION 1
//#define ICV_PDB_OPERATION 2
//
//#define MAX_DATA_SIZE 1024 
//
//#define SCADA_NODE_NAME "CWJ"
//
//#define REMOTE 1
//#define LOCAL 0
//
//#define OPERATION_AREA_TAG "._C04"
//#define SUBSYSTEM_CODE "POWX"
//
//
//#define CARD_35KV "35kV"
//#define CARD_400V "400V"
//#define CARD_1500V "1500V"
//#define CARD_PARALLEL "Parallel"
//
//#define SERIAL 0
//#define PARALL 1

/*enum Operation_status {
	operation_area_check_start,
	operatoin_area_check_end,
	opeartion_area_check_err,
	operatoin_area_check_ok,
	handon_check_start,
	handon_check_end,
	handon_check_err,
	handon_check_ok,
	close_logic_check_err,
	close_logic_check_ok,
	command_execute_ok,
	command_execute_err,
	execute_finish,
	fix_error
}*/;
#define ACTION_OK 0
#define ACTION_ERR -1
#define ACTION_CHOOSE_DEVICE_TIMEOUT -2
#define ACTION_DEVICE_TIMEOUT -3
#define ACTION_RETURN_VALUE_ERROR -4
#define OPERATION_AREA_OK 1
#define HANG_ON_OK 0

#define CHOOSE_COMMAND_ON 130
#define EXECUTE_ON 2
#define EXECUTE_OFF 1
#define OPERATION_TIMEOUT 1
#define TWO_POINT_OFFSET 1
#define CHOOSE_COMMAND_OFFSET 129
#define CHOOSE_FEEDBACK_OFFSET 7
#define EXECUTE_FEEDBACK_OFFSET 7
#define OPERATION_AREA "_107"
#define REMOTE_CONTROL "_205"
#define REMOTE_FEEDBACK "_105"
#define REPAIRE "_901"
#define STOP_USE "_902"
#define TOUCH_GROUND "_903"

#define ACTIONID_ 0
#define ACTIONNAME_ 1
#define DEVACTTAG_ 2
#define DEVADDTAG_ 3
#define DEVLOCTAG_ 4
#define ACT_CLASS_ 5
#define ACT_AVALUE_ 6
#define ACTIONGROUP_ 7
#define ACTIONSEQID_ 8
#define INTERLOCK_ID_ 9
#define STA_ALIAS_ 10
#define DEV_CLASS_ID_ 11

#define ALL_LINE_ON 201
#define ALL_LINE_Off 200
#define UP_MOVE_ON 205
#define UP_MOVE_OFF 204
#define DOWN_MOVE_ON 203
#define DOWN_MOVE_OFF 202

#define START_CARD_EXECUTE 1
#define STOP_CARD_EXECUTE 2
#define CHECK_CARD_REQUIRE 3

#define CHECK_OK 1
#define OPERATION_AREA_ERROR 2
#define HAND_ON_ERROR 3

#define HAVE_READ 0
#define HAVE_DONE 999
#define HAVE_RUN 100
#define HAVE_RUN_CARD 666
#define HAVE_CONCEL 101
#define PDB_INIT_VALUE -100

#define COMMAND_ACTION_OK 1
#define COMMAND_ACTOIN_ERROR 2

#define SINGLE_STEP_OK 5
#define SINGLE_STEP_ERROR 6
#define DEVICE_REJECT_ACTION 71
#define BASE_VERSION "1.2.0"

#endif