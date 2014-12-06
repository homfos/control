#ifndef PSCADA_ITEM_H_
#define PSCADA_ITEM_H_

#define CHOOSE_ON_SUCCESS (7 || 10)
#define CHOOSE_ON_FAILURE 9

#define CHOOSE_OFF_SUCCESS 7
#define CHOOSE_OFF_FAILURE 9


#define ACTION_OK 0
#define ACTION_ERR -1
#define ACTION_CHOOSE_DEVICE_TIMEOUT -2
#define ACTION_DEVICE_TIMEOUT -3
#define ACTION_RETURN_VALUE_ERROR -4

#define GET_REMOTE_SIGNAL 0

#define SERIAL_CONTROL_TYPE 1
#define PARALLEL_CONTROL_TYPE 2

#define TWOPOINT_CONTROL 2
#define SINGLE_POINT_CONTROL 3
#define SINGLE_POINT_READ_CONTROL 1

#define IFIX_PDB_OPERATION 1
#define ICV_PDB_OPERATION 2

#define MAX_DATA_SIZE 1024 

#define SCADA_NODE_NAME "CWJ"

#define REMOTE 1
#define LOCAL 0

#define OPERATION_AREA_TAG "._C04"
#define OPERATION_AREA_OK 1
#define SUBSYSTEM_CODE "POWX"

#define HANG_ON_OK 0

#define CARD_35KV "35kV"
#define CARD_400V "400V"
#define CARD_1500V "1500V"
#define CARD_PARALLEL "Parallel"

#define SERIAL 0
#define PARALL 1

enum Operation_status {
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
};


#define CHOOSE_COMMAND_ON 130
//#define CHOOSE_COMMAND_OFF 129
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


#endif