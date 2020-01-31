#!/usr/bin/env python
import roslib; roslib.load_manifest('smach'); roslib.load_manifest('smach_ros')
import rospy
import smach
import smach_ros
import sys
from State_Standby import Standby 
from State_EngineStop import EngineStop
from State_CheckMobileAppConnection import CheckMobileAppConnection
from State_CheckSystemHealth import CheckSystemHealth
from State_ReadData import ReadData
from State_SendData import SendData
from State_LogsAndProtocolData import LogsAndProtocolData
from State_CheckIfBlockedDirection import CheckIfBlockedDirection
from State_CreateOutputInfoLogs import CreateOutputInfoLogs
from State_AutonomousControlTransition import AutonomousControlTransition
from State_IsTaskChoosen import IsTaskChosen
from State_ChooseTask import ChooseTask
from State_AutonomousTaskMonitor import AutonomousTaskMonitor
from State_TaskCompletionRaport import TaskCompletionRaport
from State_SafeState_Start import SafeStateStart
from State_ManualControl_Start import ManualControlStart
from State_MobileAppRoutine_Start import MobileAppRoutineStart
from State_AutonomousControl_Start import AutonomousControlStart

def main():
    rospy.init_node('agribot_state_machine')

    # Create an agribot SMACH state machine
    sm_agribot = smach.StateMachine(outcomes=['agribot_offline'])

    sm_agribot.userdata.ag_safe_state_reason_in='system_power_up'
    sm_agribot.userdata.ag_safe_state_reason_out='app_input_received'

    sm_agribot.userdata.ag_engine_stop_reason='Normal'

    sm_agribot.userdata.ag_man_ctrl_reason_in='application_input_received'
    sm_agribot.userdata.ag_man_ctrl_reason_out='automatic_control_prepared'

    sm_agribot.userdata.ag_auto_ctrl_reason_in='app_input_received'
    sm_agribot.userdata.ag_auto_ctrl_reason_out='task_conf_error'

    # Open the container
    with sm_agribot:

        # SAFE STATE
        # Create the sub SMACH state machine 
        sm_safe_state = smach.StateMachine( outcomes=['application_input_received','system_repaired','system_power_is_off'], 
                                            input_keys=['sm_safe_state_reason_in','sm_safe_state_engine_stop_reason'],
                                            output_keys=['sm_safe_state_reason_out'])

        #sm_safe_state.userdata.sm_safe_state_reason_in='system_power_up'

        # Open the container
        with sm_safe_state:

            # Add states to the container
            smach.StateMachine.add('SAFE_STATE_START', SafeStateStart(), 
                                   transitions={'in_system_power_up':'STANDBY',
                                                'in_return_to_safe_state':'ENGINE_STOP'},
                                   remapping={'safe_state_reason':'sm_safe_state_reason_in'})

            smach.StateMachine.add('STANDBY', Standby(), 
                                   transitions={'app_input_received_standby':'application_input_received', 
                                                'default_on_standby':'STANDBY'},
                                    remapping={'safe_state_reason_out':'sm_safe_state_reason_out'})

            smach.StateMachine.add('ENGINE_STOP', EngineStop(), 
                                   transitions={'engine_stopped_normal':'STANDBY',
                                                'engine_stop_failure_reaction':'system_power_is_off'},
                                   remapping={'engine_stop_reason':'sm_safe_state_engine_stop_reason'})

        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('SAFE_STATE', sm_safe_state,
                               transitions={'application_input_received':'MANUAL_CONTROL_STATE',
                               'system_repaired':'agribot_offline',
                               'system_power_is_off':'agribot_offline'},
                                remapping={'sm_safe_state_reason_in':'ag_safe_state_reason_in',
                                'sm_safe_state_engine_stop_reason':'ag_engine_stop_reason',
                                'sm_safe_state_reason_out':'ag_safe_state_reason_out'})

        #MANUAL CONTROL STATE
        # Create the sub SMACH state machine
        sm_man_ctrl= smach.StateMachine(outcomes=['auto_ctrl_prepared'],
                                        input_keys=['sm_manual_control_reason_in'],
                                        output_keys=['sm_manual_control_reason_out'])
        #sm_man_ctrl.userdata.sm_manual_control_reason_in='application_input_received'

        with sm_man_ctrl:

             # Add states to the container
            smach.StateMachine.add('MANUAL_CONTROL_START', ManualControlStart(), 
                                   transitions={'task_config_error':'MOBILE_APP_CONTROL_ROUTINE',
                                                'app_input_received':'MOBILE_APP_CONTROL_ROUTINE'},
                                   remapping={'manual_control_state_reason':'sm_manual_control_reason_in',
                                            'manual_control_state_app_reason':'sm_mobile_app_reason_in'})

            #MOBILE APP CONTROL ROUTINE
            # Create the sub SMACH state machine 
            sm_mobile_app_control_routine=smach.StateMachine(outcomes=['automatic_control_command','received_only_move_commands'],
                                                            input_keys=['sm_mobile_app_reason_in'],
                                                            output_keys=['sm_mobile_app_reason_out'])

            #sm_mobile_app_control_routine.userdata.sm_mobile_app_reason_in='app_input_received'
            sm_mobile_app_control_routine.userdata.sm_mobile_app_reason_out='automatic_control_command'

            with sm_mobile_app_control_routine:

                # Add states to the container
                smach.StateMachine.add('MOBILE_APP_CONTROL_START', MobileAppRoutineStart(), 
                                        transitions={'task_config_error':'CHECK_MOBILE_APP_CONNECTION',
                                                    'app_input_received':'CHECK_MOBILE_APP_CONNECTION'},
                                        remapping={'mobile_app_routine_reason_in':'sm_mobile_app_reason_in'})

                smach.StateMachine.add('CHECK_MOBILE_APP_CONNECTION', CheckMobileAppConnection(), 
                                   transitions={'connection_is_stable':'CHECK_SYSTEM_HEALTH'})

                smach.StateMachine.add('CHECK_SYSTEM_HEALTH', CheckSystemHealth(), 
                                   transitions={'system_is_healthy':'READ_DATA'})

                smach.StateMachine.add('READ_DATA', ReadData(), 
                                   transitions={'send_data_def_event':'SEND_DATA',
                                   'auto_ctrl_command':'automatic_control_command',
                                   'rec_only_move_commands':'received_only_move_commands'},
                                   remapping={'read_data_command_out':'sm_mobile_app_reason_out'})

                smach.StateMachine.add('SEND_DATA', SendData(), 
                                   transitions={'data_sent':'LOGS_AND_PROTOCOL_DATA'})

                smach.StateMachine.add('LOGS_AND_PROTOCOL_DATA', LogsAndProtocolData(), 
                                   transitions={'logs_and_protocol_data_sent':'CHECK_MOBILE_APP_CONNECTION'})

            #Finish state machine transitions and add it to agribot_state_machine
            smach.StateMachine.add('MOBILE_APP_CONTROL_ROUTINE', sm_mobile_app_control_routine,
                               transitions={'automatic_control_command':'AUTONOMOUS_CONTROL_TRANSITION',
                               'received_only_move_commands':'EXECUTE_MOVE_COMMANDS'})
            
            #EXECUTE MOVE COMMANDS ROUTINE
            # Create the sub SMACH state machine 
            sm_execute_move_commands=smach.StateMachine(outcomes=['executed_move_commands'])

            with sm_execute_move_commands:
                # Add states to the container
                smach.StateMachine.add('CHECK_IF_BLOCKED_DIRECTION', CheckIfBlockedDirection(), 
                                   transitions={'no_obstacles':'CREATE_INFO_LOGS'})

                smach.StateMachine.add('CREATE_INFO_LOGS', CreateOutputInfoLogs(), 
                                   transitions={'executed_outputs':'executed_move_commands'})

            #Finish state machine transitions and add it to agribot_state_machine
            smach.StateMachine.add('EXECUTE_MOVE_COMMANDS', sm_execute_move_commands,
                               transitions={'executed_move_commands':'MOBILE_APP_CONTROL_ROUTINE'})

            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_TRANSITION', AutonomousControlTransition(), 
                                transitions={'auto_mode_failed':'MOBILE_APP_CONTROL_ROUTINE',
                                'auto_control_prepared':'auto_ctrl_prepared'},
                                remapping={'auto_control_transition_out':'sm_manual_control_reason_out'})            
        
        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('MANUAL_CONTROL_STATE', sm_man_ctrl,
                               transitions={'auto_ctrl_prepared':'AUTONOMOUS_CONTROL_STATE'},
                                remapping={'sm_manual_control_reason_in':'ag_man_ctrl_reason_in',
                                            'sm_manual_control_reason_out':'ag_man_ctrl_reason_out'})
        
        #AUTONOMOUS CONTROL STATE
        # Create the sub SMACH state machine
        sm_auto_ctrl = smach.StateMachine(outcomes=['task_configuration_failed', 'return_to_save_state'],
                                        input_keys=['sm_auto_control_reason_in'],
                                        output_keys=['sm_auto_control_reason_out'])

        with sm_auto_ctrl:

            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_START', AutonomousControlStart(), 
                                    transitions={'auto_control_prepared':'IS_TASK_CHOSEN',
                                                'sys_repaired':'return_to_save_state'},
                                    remapping={'autonomous_control_state_reason':'sm_auto_control_reason_in'})

            smach.StateMachine.add('IS_TASK_CHOSEN', IsTaskChosen(), 
                                   transitions={'task_is_active':'AUTONOMOUS_TASK_MONITOR',
                                   'no_active_task':'CHOOSE_TASK'})

            smach.StateMachine.add('CHOOSE_TASK', ChooseTask(), 
                                   transitions={'task_is_active':'AUTONOMOUS_TASK_MONITOR',
                                   'task_conf_error':'task_configuration_failed'},
                                   remapping={'choose_task_output':'sm_auto_control_reason_out'})

            smach.StateMachine.add('AUTONOMOUS_TASK_MONITOR', AutonomousTaskMonitor(), 
                                   transitions={'succes':'TASK_COMPLETION_RAPORT'})                                   

            smach.StateMachine.add('TASK_COMPLETION_RAPORT', TaskCompletionRaport(), 
                                   transitions={'complete':'return_to_save_state'})   

         #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('AUTONOMOUS_CONTROL_STATE', sm_auto_ctrl,
                               transitions={'task_configuration_failed':'MANUAL_CONTROL_STATE',
                               'return_to_save_state':'SAFE_STATE'},
                               remapping={'sm_auto_control_reason_in':'ag_man_ctrl_reason_out',
                                            'sm_auto_control_reason_out':'ag_auto_ctrl_reason_out'})                          

    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('agribot_state_machine_intro_server', sm_agribot, '/SM_ROOT')
    sis.start()

    # Execute SMACH plan
    outcome = sm_agribot.execute()

    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()


if __name__ == '__main__':
    main()